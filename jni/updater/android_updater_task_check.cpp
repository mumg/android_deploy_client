#include "mmlIScheduler.h"
#include "mmlIFileSystem.h"
#include "mmlIVariantSerializer.h"
#include "mmlIVariantDeserializer.h"
#include "android_http_update.h"
#include "android_http_fetch.h"
#include "mml_components.h"
#include "android_utils.h"
#include "mml_services.h"

void log_main( const mmlChar * formatter , ... );

extern const mmlChar * updater_version;

#define _ADD_PROPERTY(source, prop, name) \
	{ \
		mmlAutoPtr < mmlIVariant > var; \
		if (source->Get(prop, var.getterAddRef()) == mmlFalse) \
		{ \
			return mmlISchedulerRule::TASK_SUCCESS; \
		} \
		request->Set(name, var); \
	}


class AndroidCheckTask : public mmlISchedulerTask
{
	MML_OBJECT_DECL
public:

	mmlISchedulerRule::TASK_RESULT_T Run (mmlIVariantStruct * _config)
	{
		log_main("check for update\n");
		mmlSharedPtr < mmlIVariantStruct > config = mmlGetService(MML_GLOBAL_CONFIG_UUID());
		mmlAutoPtr < mmlIVariantStruct > android;
		if (config->Get("android", mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant >(android)) == mmlFalse)
		{
			return mmlISchedulerRule::TASK_SUCCESS;
		}
		mmlAutoPtr < mmlIVariantStruct > bootstrap;
		if (config->Get("bootstrap", mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant >(bootstrap)) == mmlFalse)
		{
			return mmlISchedulerRule::TASK_SUCCESS;
		}
		mmlAutoPtr < mmlIVariantStruct > request = mmlNewVariantStruct();
		request->Set("version", mmlNewVariantString(mmlNewCString(updater_version)));
		_ADD_PROPERTY(config, "customer", "customer_id");
		_ADD_PROPERTY(android, "cpu", "cpu");
		_ADD_PROPERTY(android, "sdk", "sdk");
		_ADD_PROPERTY(android, "android_id", "id");
		request->Set("product", mmlNewVariantString(mmlNewCString("56683628-0f0d-11e8-ba89-0ed5f89f718b")));
		mmlAutoPtr < mmlIVariantStruct > response;
		{
			mmlAutoPtr < mmlIVariantString > url;
			if (bootstrap->Get("url", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(url)) == mmlFalse)
			{
				return mmlISchedulerRule::TASK_SUCCESS;
			}
			log_main("Query server for update\n");
			request->Dump(0, mmlNULL, log_variant_log_func);
			if (android_query_server(url->Get(), bootstrap, request, mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant >(response)) == mmlFalse)
			{
				log_main("Query failed\n");
				return mmlISchedulerRule::TASK_SUCCESS;
			}

			if (response == mmlNULL)
			{
				log_main("Response is empty\n");
				return mmlISchedulerRule::TASK_SUCCESS;
			}
			log_main("Response from update server\n");
			response->Dump(0, mmlNULL, log_variant_log_func);
		}
		mmlAutoPtr < mmlIVariantString > remote_version;
		response->Get("version", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(remote_version));
		if (remote_version == mmlNULL)
		{
			return mmlISchedulerRule::TASK_SUCCESS;
		}
		mmlBool eq;
		remote_version->Get()->CompareStr(MML_EQUAL, updater_version, eq);
		if (eq == mmlTrue)
		{
			log_main("Self version is equal\n");
			return mmlISchedulerRule::TASK_SUCCESS;
		}
		log_main("Newest version is available, exiting...\n");
		exit(0);
		return mmlISchedulerRule::TASK_SUCCESS;
	}

};

MML_OBJECT_IMPL1(AndroidCheckTask, mmlISchedulerTask)

#include "mml_components.h"

MML_CONSTRUCTOR_IMPL(AndroidCheckTask);

static mmlObjectFactory android_check_task=
{
	{ 0x543A4904, 0x6FC0, 0x1014, 0x9EE8 , { 0xAE, 0x9B, 0xD1, 0xF9, 0xC5, 0xE5 } },
	"mml/scheduler/tasks/check",
	MML_CONSTRUCTOR(AndroidCheckTask)
};

mmlBool register_android_check_task ()
{
	log_main("REGISTER CHECK TASK\n");
	mmlRegisterComponentFactory(&android_check_task);
	return mmlTrue;
}

ANDROID_INIT_MODULE_I(register_android_check_task);
