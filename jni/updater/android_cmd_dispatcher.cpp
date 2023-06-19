#include "android_cmd_dispatcher.h"
#include "android_utils.h"
#include "android_updater_database.h"
#include "mml_services.h"
#include "mmlIThread.h"
#include "mmlIObjectQueue.h"
#include "mml_components.h"
#include <map>
#include "mmlIVariantSerializer.h"
#include "mmlIVariantDeserializer.h"
#include "android_http_fetch.h"
#include "android_reboot.h"

void log_main(const mmlChar * formatter, ...);

#define MML_BINARY_VARIANT_SERIALIZER_UUID { 0x6392D37F, 0x6C6E, 0x1014, 0xAE5B , { 0xDC, 0x81, 0x31, 0xDC, 0xEA, 0xA1 } }

#define MML_BINARY_VARIANT_DESERIALIZER_UUID { 0x6E1257B1, 0x6C6E, 0x1014, 0x8067 , { 0x8F, 0x47, 0x3C, 0xAD, 0xC9, 0x8C } }

#define MML_JSON_VARIANT_SERIALIZER_UUID { 0x407065FF, 0x6C70, 0x1014, 0xBDA0 , { 0xF9, 0xC2, 0xF4, 0x90, 0x1F, 0xFA } }

static const mmlUUID mml_json_variant_serializer_uuid = MML_JSON_VARIANT_SERIALIZER_UUID;

#define MML_JSON_VARIANT_DESERIALIZER_UUID { 0x4383190F, 0x6C70, 0x1014, 0x9A99 , { 0x9C, 0xA2, 0x29, 0x60, 0xE2, 0xE9 } }

static const mmlUUID mml_json_variant_deserializer_uuid = MML_JSON_VARIANT_DESERIALIZER_UUID;

#define ANDROID_CMD_DISPATCHER_UUID { 0x5DB5ACEE, 0x7C27, 0x1014, 0x82D6 , { 0xB5, 0xDB, 0xFC, 0xCC, 0xDF, 0x5C } }

#define FETCH_REQUEUE_DELAY (5 * 60 * 1000)
#define CONFIRM_REQUEUE_DELAY (5 * 60 * 1000)
#define RUNNER_TTL ( 24 * 60 * 60 * 1000)

static std::list < const CMD_MODULE_T * > modules;

class android_cmd_runner;

class android_cmd_dispatcher : public mmlIObject
{
	MML_OBJECT_DECL
		MML_OBJECT_UUID_DECL(ANDROID_CMD_DISPATCHER_UUID)
private:
	mmlSharedPtr < mmlIObjectQueue > mQueue;
public:

	void Enqueue(mmlICString * url);
	void Enqueue(android_cmd_runner * runner, const mmlInt32 delay);
	void Run(mmlBool * terminate);
	mmlBool IsIdle()
	{
		return mQueue->Size() == 0 ? mmlTrue : mmlFalse;
	}

};

MML_OBJECT_IMPL1(android_cmd_dispatcher, android_cmd_dispatcher)



#define ANDROID_CMD_RUNNER_UUID  { 0x62be396e, 0xbcc0, 0x43ea, 0x866c, { 0x8c, 0x7b, 0x88, 0x37, 0x85, 0x20} }

class android_cmd_runner : public mmlIObject
{
	MML_OBJECT_UUID_DECL(ANDROID_CMD_RUNNER_UUID)
private:
	mmlInt64 mId;
	mml_timespec mTimestamp;
	android_cmd_runner * _Save(mmlUUID runnerId)
	{
		mmlSharedPtr < mmlIMemoryOutputStream > blob = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));
		mmlAutoPtr < mmlIVariant > data;
		_Write(data.getterAddRef());

		static mmlUUID binary_serializer_uuid = MML_BINARY_VARIANT_SERIALIZER_UUID;
		mmlSharedPtr < mmlIVariantSerializer > serializer = mmlNewObject(binary_serializer_uuid);
		if (serializer == mmlNULL)
		{
			return mmlNULL;
		}
		if (serializer->Write(blob, data) == mmlFalse)
		{
			return mmlNULL;
		}	
		blob->Close();
		mmlAutoPtr < mmlIBuffer > blob_data;
		blob->GetData(blob_data.getterAddRef());
		mmlAutoPtr < mmlIDatabase > db;
		android_database_get(db.getterAddRef());
		mmlChar runnerIdStr[64];
		mmlUUIDToStr(runnerId, runnerIdStr, sizeof(runnerIdStr));
		if (db->Query("INSERT INTO runners (timestamp, uuid, data) VALUES(?,?,?)", mmlNULL, 
			mmlNewVariantArrayFill(
				mmlNewVariantInteger(mTimestamp.tv_sec),
				mmlNewVariantString(mmlNewCString(runnerIdStr)), 
				mmlNewVariantRaw(blob_data)
			), &mId) == mmlFalse)
		{
			log_main("Could not insert runner data\n");
			return mmlNULL;
		}
		return this;
	}

	android_cmd_runner * _Load(const mmlInt64 id, const mmlInt64 timestamp, mmlIVariantRaw * data)
	{
		mId = id;
		mTimestamp.tv_sec = timestamp;
		mTimestamp.tv_nsec = 0;
		mmlAutoPtr < mmlIBuffer > blob;
		data->Get(blob.getterAddRef());
		mmlSharedPtr < mmlIInputStream > stream = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), blob);
		static mmlUUID binary_deserializer = MML_BINARY_VARIANT_DESERIALIZER_UUID;
		mmlSharedPtr < mmlIVariantDeserializer > deserializer = mmlNewObject(binary_deserializer);
		mmlAutoPtr < mmlIVariant > _data;
		if (deserializer->Read(stream, _data.getterAddRef()) == mmlFalse)
		{
			_Delete();
			return mmlNULL;
		}
		if (_Read(_data) == mmlFalse)
		{
			_Delete();
			return mmlNULL;
		}
		return this;
	}

protected:
	friend class android_cmd_dispatcher;
	virtual void _Write(mmlIVariant ** data) = 0;
	virtual mmlBool _Read(mmlIVariant * data) = 0;
	virtual void _Run() = 0;
	virtual void _Dump() = 0;

	mmlBool _IsExpired(const mmlInt32 ttl)
	{
		mml_timespec cts;
		mml_clock_gettime_realtime(&cts);
		if (mml_clock_diff(&cts, &mTimestamp) >= ttl)
		{
			return mmlTrue;
		}
		return mmlFalse;
	}
public:
	void _Delete()
	{
		mmlAutoPtr < mmlIDatabase > db;
		android_database_get(db.getterAddRef());
		db->Query("DELETE FROM runners WHERE id = ? ", mmlNULL, mmlNewVariantInteger(mId), mmlNULL);
	}
	void _Requeue(const mmlInt32 delay = 0)
	{
		mmlSharedPtr < android_cmd_dispatcher > dispatcher = mmlGetService(MML_OBJECT_UUID(android_cmd_dispatcher));
		if (dispatcher != mmlNULL)
		{
			dispatcher->Enqueue(this, delay);
		}
	}
public:

	android_cmd_runner()
		:mId(0)
	{
		mml_clock_gettime_realtime(&mTimestamp);
	}
	

	static android_cmd_runner * Create(mmlUUID runnerId, MML_OBJECT_VARGS)
	{
		android_cmd_runner * instance = (android_cmd_runner*)mmlNewObject(runnerId, MML_OBJECT_VARGS_FORWARD);
		if (instance == mmlNULL)
		{
			return mmlNULL;
		}
		return instance->_Save(runnerId);
	}

	static void Load(mmlIObjectQueue * queue)
	{
		mmlAutoPtr < mmlIDatabase > db;
		android_database_get(db.getterAddRef());
		class stored_runners : public mmlIDatabaseQueryHandler
		{
			MML_OBJECT_STATIC_DECL
		private:
			mmlAutoPtr < mmlIObjectQueue > mQueue;
		public:
			stored_runners(mmlIObjectQueue * queue)
				:mQueue(queue)
			{
			}

			mmlBool OnRow(const mmlInt32 index, mmlIDatabaseRow * row)
			{
				mmlAutoPtr < mmlIVariantInteger > runner_id;
				mmlAutoPtr < mmlIVariantInteger > runner_timestamp;
				mmlAutoPtr < mmlIVariantString > runner_uuid_str;
				mmlAutoPtr < mmlIVariantRaw > runner_data;
				if (row->GetValue(0, mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant >(runner_id)) == mmlFalse)
				{
					return mmlTrue;
				}
				if (row->GetValue(1, mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant >(runner_timestamp)) == mmlFalse)
				{
					return mmlTrue;
				}
				if (row->GetValue(2, mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(runner_uuid_str)) == mmlFalse)
				{
					return mmlTrue;
				}
				if (row->GetValue(3, mmlRelativePtrAddRef < mmlIVariantRaw, mmlIVariant >(runner_data)) == mmlFalse)
				{
					return mmlTrue;
				}
				
				mmlUUID runner_uuid;
				mmlStrToUUID(runner_uuid_str->Get()->Get(), runner_uuid);
				
				mmlSharedPtr < android_cmd_runner > runner = mmlNewObject(runner_uuid);
				if (runner != mmlNULL)
				{
					runner = runner->_Load(runner_id->Get(), runner_timestamp->Get(), runner_data);
					if (runner != mmlNULL)
					{
						mQueue->Post(runner);
					}
				}
				return mmlTrue;
			}
		}runners_handler(queue);
		db->Query("SELECT id, timestamp, uuid, data FROM runners", &runners_handler, mmlNULL, mmlNULL);
	}

};


#define  ANDROID_RUNNER_ASSERT_ABORT(c) \
	if ( !(c)){ \
		_Delete(); \
		return; \
	}

#define ANDROID_RUNNER_ASSERT_REQUEUE(c, delay) \
	if ( !(c) ){ \
		_Requeue(delay); \
		return; \
	}

#define ANDROID_RUNNER_COMPLETE() \
	_Delete(); \
	return;



#define ANDROID_CMD_CONFIRM_RUNNER_UUID { 0x2e638aab, 0xaf07, 0x419c, 0xa5ce, { 0x6a, 0x8e, 0x78, 0xe5, 0x69, 0x7a }}

class android_cmd_confirm_runner : public android_cmd_runner
{
	MML_OBJECT_DECL
	MML_OBJECT_UUID_DECL(ANDROID_CMD_CONFIRM_RUNNER_UUID)
private:
	mmlAutoPtr < mmlICString > mConfirmURL;
	mmlAutoPtr < mmlIVariantStruct > mData;

protected:
	void _Write(mmlIVariant ** data)
	{
		mmlAutoPtr < mmlIVariantStruct > str = mmlNewVariantStruct();
		str->Set("url", mmlNewVariantString(mConfirmURL));
		str->Set("data", mData);
		*data = str;
		MML_ADDREF(*data);
	}
	mmlBool _Read(mmlIVariant * data)
	{
		mmlSharedPtr < mmlIVariantStruct > str = data;
		if (str == mmlNULL)
		{
			return mmlFalse;
		}
		mmlAutoPtr < mmlIVariantString > url;
		str->Get("url", mmlRelativePtrAddRef <mmlIVariantString,  mmlIVariant>(url));
		if (url == mmlNULL)
		{
			return mmlFalse;
		}
		mConfirmURL = url->Get();
		str->Get("data", mmlRelativePtrAddRef <mmlIVariantStruct, mmlIVariant>(mData));
		if (mData == mmlNULL)
		{
			return mmlFalse;
		}
		return mmlTrue;
	}
	void _Dump()
	{
		log_main("confirm runner, url=%s\n", mConfirmURL->Get());
	}
	void _Run()
	{
		mmlSharedPtr < mmlIVariantStruct > config = mmlGetService(MML_GLOBAL_CONFIG_UUID());
		mmlAutoPtr < mmlIVariantStruct > query_config;
		ANDROID_RUNNER_ASSERT_ABORT(config->Get("query", mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant>(query_config)) == mmlTrue);
		ANDROID_RUNNER_ASSERT_ABORT(query_config != mmlNULL);
		ANDROID_RUNNER_ASSERT_REQUEUE(android_query_server(mConfirmURL, query_config, mData, mmlNULL) == mmlTrue, CONFIRM_REQUEUE_DELAY);
		ANDROID_RUNNER_COMPLETE();
	}
public:
	mmlBool Construct(mmlICString * url, mmlIVariantStruct * data)
	{
		mConfirmURL = url;
		mData = data;
		return mmlTrue;
	}
};

MML_OBJECT_IMPL1(android_cmd_confirm_runner, android_cmd_runner)
MML_CONSTRUCTOR_IMPL(android_cmd_confirm_runner)
MML_CONSTRUCTOR_IMPL2(android_cmd_confirm_runner, mmlICString, mmlIVariantStruct)

class android_cmd_request_impl : public android_cmd_request
{
	MML_OBJECT_DECL
private:
	mmlAutoPtr < android_cmd_runner > mRunner;
	mmlAutoPtr < mmlIVariantStruct > mRequest;
	mmlAutoPtr < mmlICString > mConfirmUrl;

public:
	android_cmd_request_impl(android_cmd_runner * runner, mmlIVariantStruct * request, mmlICString * confirmURL)
		:mRunner(runner), mRequest(request), mConfirmUrl(confirmURL)
	{

	}


	void GetRequest(mmlIVariantStruct ** request)
	{
		*request = mRequest;
		MML_ADDREF(*request);
	}

	void Respond(mmlIVariantStruct * response)
	{
		mRunner->_Delete();
		if (mConfirmUrl == mmlNULL)
		{
			return;
		}
		mmlSharedPtr < mmlIVariantStruct > config = mmlGetService(MML_GLOBAL_CONFIG_UUID());
		mmlAutoPtr < mmlICString > deviceId;
		android_get_device_id(deviceId.getterAddRef());
		response->Set("deviceId", mmlNewVariantString(deviceId));
		ADD_PROPERTY(config, "customer", "customer", response);
		android_cmd_runner * runner = android_cmd_runner::Create(MML_OBJECT_UUID(android_cmd_confirm_runner), mConfirmUrl, response);
		if (runner != mmlNULL)
		{
			mmlSharedPtr < android_cmd_dispatcher > dispatcher = mmlGetService(MML_OBJECT_UUID(android_cmd_dispatcher));
			if (dispatcher != mmlNULL)
			{
				dispatcher->Enqueue(runner, 0);
			}
		}
	}

	void RespondOK(mmlIVariantStruct * data)
	{
		if (data == mmlNULL) data = mmlNewVariantStruct();
		data->Set("result", mmlNewVariantInteger(0));
		Respond(data);
	}
	void RespondError(mmlInt32 errorCode, const mmlChar * errorStr, mmlIVariantStruct * data)
	{
		if (data == mmlNULL) data = mmlNewVariantStruct();
		data->Set("result", mmlNewVariantInteger(errorCode));
		data->Set("resultStr", mmlNewVariantString(mmlNewCString(errorStr)));
		Respond(data);
	}
};

MML_OBJECT_IMPL1(android_cmd_request_impl, android_cmd_request)

#define ANDROID_CMD_TASK_RUNNER_UUID { 0x5d672608, 0xec59, 0x4c99, 0x869f, {0x62, 0x7f, 0x08, 0xbf, 0x9c, 0x84 } }

class android_cmd_task_runner : public android_cmd_runner
{
	MML_OBJECT_DECL
	MML_OBJECT_UUID_DECL(ANDROID_CMD_TASK_RUNNER_UUID)
private:
	mmlSharedPtr < mmlIVariantStruct > mData;

protected:
	void _Write(mmlIVariant ** data)
	{
		*data = mData;
		MML_ADDREF(*data);
	}
	mmlBool _Read(mmlIVariant * data)
	{
		mData = data;
		if (mData == mmlNULL)
		{
			return mmlFalse;
		}
		return mmlTrue;
	}
	void _Dump()
	{
		log_main("task runner\n");
		mData->Dump(2, mmlNULL, log_variant_log_func);
	}
	void _Run()
	{
		mmlAutoPtr < mmlIVariantString > _t_type;
		ANDROID_RUNNER_ASSERT_ABORT(mData->Get("type", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant>(_t_type)) == mmlTrue);
		ANDROID_RUNNER_ASSERT_ABORT(_t_type != mmlNULL);
		mmlAutoPtr < mmlIVariantString > _t_confirmURL;
		mData->Get("confirmURL", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant>(_t_confirmURL));
		mmlAutoPtr < mmlIVariantStruct > _t_data;
		mData->Get("data", mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant>(_t_data));

		mmlAutoPtr < android_cmd_request > request = new android_cmd_request_impl(this, _t_data, _t_confirmURL == mmlNULL ? mmlNULL : _t_confirmURL->Get());
		for (std::list < const CMD_MODULE_T * >::iterator cmd = modules.begin(); cmd != modules.end(); cmd++)
		{
			if (mmlStrNCompare((*cmd)->event, _t_type->Get()->Get(), INT_MAX) == 0)
			{
				(*cmd)->handler(request);
				return;
			}
		}
		request->RespondError(-1, "Handler not found", mmlNULL);
		ANDROID_RUNNER_COMPLETE();
	}
public:
	mmlBool Construct(mmlIVariantStruct * data)
	{
		mData = data;
		return mmlTrue;
	}
};

MML_OBJECT_IMPL1(android_cmd_task_runner, android_cmd_runner)
MML_CONSTRUCTOR_IMPL(android_cmd_task_runner)
MML_CONSTRUCTOR_IMPL1(android_cmd_task_runner, mmlIVariantStruct)

#define ANDROID_CMD_FETCH_RUNNER_UUID { 0x21c818c1, 0xfc26, 0x4ce9, 0x80ba, { 0x80, 0x4a, 0x10, 0x18, 0x7b, 0x44 } }

class android_cmd_fetch_runner : public android_cmd_runner
{
	MML_OBJECT_DECL
	MML_OBJECT_UUID_DECL(ANDROID_CMD_FETCH_RUNNER_UUID)
private:
	mmlAutoPtr < mmlICString > mUrl;

protected:
	void _Write(mmlIVariant ** data)
	{
		*data = mmlNewVariantString(mUrl);
		MML_ADDREF(*data);
	}
	mmlBool _Read(mmlIVariant * data)
	{
		mmlSharedPtr < mmlIVariantString > str = data;
		if (str == mmlNULL)
		{
			return mmlFalse;
		}
		mUrl = str->Get();
		return mmlTrue;
	}
	void _Dump()
	{
		log_main("fetch runner, %s\n", mUrl->Get());
	}
	void _Run()
	{
		mmlSharedPtr < mmlIVariantStruct > config = mmlGetService(MML_GLOBAL_CONFIG_UUID());
		mmlAutoPtr < mmlIVariantStruct > file_config;
		ANDROID_RUNNER_ASSERT_ABORT(config->Get("query", mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant>(file_config)) == mmlTrue);
		ANDROID_RUNNER_ASSERT_ABORT(file_config != mmlNULL);
		mmlSharedPtr < mmlIMemoryOutputStream > data = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));
		HTTP_PROCESS_RESULT_T res = android_http_fetch(0, file_config, mUrl, mmlNULL, mmlNULL, mmlNULL, data);
		ANDROID_RUNNER_ASSERT_REQUEUE(res != HTTP_ABORTED, FETCH_REQUEUE_DELAY);
		data->Close();
		mmlAutoPtr < mmlIBuffer > buffer;
		data->GetData(buffer.getterAddRef());

		mmlAutoPtr < mmlIVariantStruct > _t_task;
		android_json_deserialize(buffer, mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant>(_t_task));
		ANDROID_RUNNER_ASSERT_ABORT(_t_task != mmlNULL);
		android_cmd_runner * runner = android_cmd_runner::Create(MML_OBJECT_UUID(android_cmd_task_runner), _t_task);
		if (runner != mmlNULL)
		{
			mmlSharedPtr < android_cmd_dispatcher > dispatcher = mmlGetService(MML_OBJECT_UUID(android_cmd_dispatcher));
			if (dispatcher != mmlNULL)
			{
				dispatcher->Enqueue(runner, 0);
			}
		}
		ANDROID_RUNNER_COMPLETE();
	}
public:
	mmlBool Construct(mmlICString * url)
	{
		mUrl = url;
		return mmlTrue;
	}
};

MML_OBJECT_IMPL1(android_cmd_fetch_runner, android_cmd_runner)
MML_CONSTRUCTOR_IMPL(android_cmd_fetch_runner)
MML_CONSTRUCTOR_IMPL1(android_cmd_fetch_runner, mmlICString)





static mmlObjectFactory android_cmd_runners[] =
{
	{
		MML_OBJECT_UUID(android_cmd_fetch_runner),
		mmlNULL,
		MML_CONSTRUCTOR(android_cmd_fetch_runner)
	},
	{
		MML_OBJECT_UUID(android_cmd_fetch_runner),
		mmlNULL,
		MML_CONSTRUCTOR1(android_cmd_fetch_runner, mmlICString)
	},
	{
		MML_OBJECT_UUID(android_cmd_task_runner),
		mmlNULL,
		MML_CONSTRUCTOR(android_cmd_task_runner)
	},
	{
		MML_OBJECT_UUID(android_cmd_task_runner),
		mmlNULL,
		MML_CONSTRUCTOR1(android_cmd_task_runner, mmlIVariantStruct)
	},
	{
		MML_OBJECT_UUID(android_cmd_confirm_runner),
		mmlNULL,
		MML_CONSTRUCTOR(android_cmd_confirm_runner)
	},
	{
		MML_OBJECT_UUID(android_cmd_confirm_runner),
		mmlNULL,
		MML_CONSTRUCTOR2(android_cmd_confirm_runner, mmlICString , mmlIVariantStruct)
	}
};



void android_cmd_dispatcher::Enqueue(mmlICString * url)
{
	android_cmd_runner * runner = android_cmd_runner::Create(MML_OBJECT_UUID(android_cmd_fetch_runner), url);
	if (runner != mmlNULL)
	{
		mQueue->Post(runner);
	}
}
void android_cmd_dispatcher::Enqueue(android_cmd_runner * runner, const mmlInt32 delay)
{
	mQueue->Post(runner, delay);
}
void android_cmd_dispatcher::Run(mmlBool * terminate)
{
	mQueue = mmlNewObject(MML_OBJECT_UUID(mmlIObjectQueue));
	android_cmd_runner::Load(mQueue);
	while (*terminate == mmlFalse)
	{
		mmlAutoPtr < android_cmd_runner > runner;
		if (mQueue->Get(1000, runner.getterAddRef()) == mmlTrue)
		{
			if (runner != mmlNULL)
			{
				log_main("begin\n");
				runner->_Dump();
				if (runner->_IsExpired(RUNNER_TTL))
				{
					log_main("runner expired\n");
					runner->_Delete();
				}
				else
				{
					runner->_Run();
				}
				log_main("end\n");
			}
		}
	}
}


void android_cmd(mmlICString * url)
{
	mmlSharedPtr < android_cmd_dispatcher > dispatcher = mmlGetService(MML_OBJECT_UUID(android_cmd_dispatcher));
	if (dispatcher != mmlNULL)
	{
		dispatcher->Enqueue(url);
	}
}

void android_register_cmd(CMD_MODULE_T * module)
{
	modules.push_back(module);
}

mmlBool android_cmd_idle()
{
	mmlSharedPtr < android_cmd_dispatcher > dispatcher = mmlGetService(MML_OBJECT_UUID(android_cmd_dispatcher));
	if (dispatcher != mmlNULL)
	{
		return dispatcher->IsIdle();
	}
	return mmlTrue;
}

void android_cmd_dispatcher_run(mmlBool * terminate)
{
	log_main("cmd dispatcher stated\n");
	android_updater_reboot_register_checker(android_cmd_idle);
	for (mmlInt32 index = 0; index < sizeof(android_cmd_runners) / sizeof(mmlObjectFactory); index++)
	{
		mmlRegisterComponentFactory(&android_cmd_runners[index]);
	}
	for (std::list < const CMD_MODULE_T * >::iterator module = modules.begin(); module != modules.end(); module++)
	{
		if ((*module)->init != mmlNULL)
		{
			if ((*module)->init() == mmlFalse)
			{
				return;
			}
		}
	}
	mmlAutoPtr < android_cmd_dispatcher > dispatcher = new android_cmd_dispatcher();
	mmlSetService(MML_OBJECT_UUID(android_cmd_dispatcher), dispatcher);
	dispatcher->Run(terminate);
	mmlSetService(MML_OBJECT_UUID(android_cmd_dispatcher), mmlNULL);
	for (std::list < const CMD_MODULE_T * >::iterator module = modules.begin(); module != modules.end(); module++)
	{
		if ((*module)->destroy != mmlNULL)
		{
			(*module)->destroy();
		}
	}
	log_main("cmd dispatcher stopped\n");
}