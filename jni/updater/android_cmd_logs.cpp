#include "mmlICString.h"
#include "mmlICStringStream.h"
#include "mml_components.h"
#include "mmlIVariant.h"
#include "mmlIProcess.h"
#include "android_utils.h"
#include "android_cmd_dispatcher.h"
#include "mmlIWebSocketClient.h"
#include "mml_components.h"
#include "mmlIVariantSerializer.h"
#include "mmlIVariantDeserializer.h"
#include "mml_services.h"
#include "mmlIProcess.h"
#include "mmlIObjectQueue.h"
#include "mmlIMutex.h"
#include "android_updater_database.h"
#include "mmlIThread.h"

void log_main( const mmlChar * formatter , ... );

#define MML_JSON_VARIANT_SERIALIZER_UUID { 0x407065FF, 0x6C70, 0x1014, 0xBDA0 , { 0xF9, 0xC2, 0xF4, 0x90, 0x1F, 0xFA } }

#define MML_JSON_VARIANT_DESERIALIZER_UUID { 0x4383190F, 0x6C70, 0x1014, 0x9A99 , { 0x9C, 0xA2, 0x29, 0x60, 0xE2, 0xE9 } }


class android_logs_output_stream : public mmlILoggerOutput
{
	MML_OBJECT_DECL
public:
	android_logs_output_stream ( mmlIWebSocketClient * client )
		:mClient(client)
	{
	}

	~android_logs_output_stream()
	{

	}

	void DoLog ( const mmlChar * str )
	{
		mmlSharedPtr < mmlIMemoryOutputStream > output = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));
		mml_timespec ts;
		mml_clock_gettime_realtime(&ts);
		mml_tm tm;
		mml_clock_localtime(&ts, &tm);
		mmlChar buffer[64];
		mmlInt32 len = mmlSprintf(buffer , sizeof(buffer) , 
			"%d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d:%0.3d ",
			tm.tm_year , tm.tm_mon , tm.tm_mday, tm.tm_hour , tm.tm_min , tm.tm_sec , tm.tm_msec);
		output->Write(len, buffer);
		output->Write(mmlStrLength(str), str);
		output->Close();

		mmlAutoPtr < mmlIBuffer > data;
		output->GetData(data.getterAddRef());
		mmlSharedPtr < mmlIInputStream > data_stream = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), data);
		mClient->SendMessage(mmlTrue, data_stream, mmlNULL);
	}

private:

	mmlIWebSocketClient * mClient;

};

MML_OBJECT_IMPL0(android_logs_output_stream)

#define ANDROID_HISTORY_LOGS_OUTPUT_STREAM_UUID { 0xDDB9A64D, 0x6D97, 0x1014, 0x8920 , { 0xCD, 0x3B, 0xBC, 0x15, 0xEC, 0x59 } }




class android_logs_websocket_listener : public mmlIWebSocketClientListener
{
	MML_OBJECT_DECL

	mmlBool * mShutdown;

	mmlAutoPtr < android_logs_output_stream > mLogsWatcher;
	mmlInt32 mLogId;
public:

	android_logs_websocket_listener(mmlBool * shutdown)
		:mShutdown(shutdown), mLogId(0)
	{
	}

	~android_logs_websocket_listener()
	{
		if ( mLogId != 0 )
		{
			mmlSharedPtr < mmlILogger > logger = mmlGetService(MML_OBJECT_UUID(mmlILogger));
			logger->Detach(mLogId);
		}
	}

	mmlBool OnPingReceived() { return mmlTrue; }

	mmlBool OnPingSent () { return mmlTrue; }

	mmlBool OnReceiveMessage ( mmlIWebSocketClient * client, mmlIInputStream * message )
	{
		return mmlTrue;
	}

	mmlBool IsShutdown ()
	{
		if ( *mShutdown == mmlTrue ) 
		{
				return mmlTrue;
		}
		return mmlFalse;
	}

	mmlBool OnConnect ( mmlIWebSocketClient * client )
	{
		mmlAutoPtr < mmlIDatabase > db;
		android_database_get(db.getterAddRef());
		if (db != mmlNULL)
		{
			class db_logs_query : public mmlIDatabaseQueryHandler
			{
				MML_OBJECT_STATIC_DECL
					mmlIWebSocketClient * _client;
			public:
				db_logs_query(mmlIWebSocketClient * client)
					:_client(client)
				{
				}
				mmlBool OnRow(const mmlInt32 index, mmlIDatabaseRow * row)
				{
					mmlAutoPtr < mmlIVariantInteger > timestamp;
					mmlAutoPtr < mmlIVariantString > log;
					if (row->GetValue(0, mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant >(timestamp)) == mmlTrue
						&& row->GetValue(1, mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(log)) == mmlTrue)
					{
						mml_timespec ts;
						ts.tv_sec = timestamp->Get();
						ts.tv_nsec = 0;
						mml_tm tm;
						mml_clock_localtime(&ts, &tm);
						mmlSharedPtr < mmlIMemoryOutputStream > stream = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));
						mmlSharedPtr < mmlICStringOutputStream > output = mmlNewObject(MML_OBJECT_UUID(mmlICStringOutputStream), stream);

						output->WriteFormatted("%d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d:%0.3d ",
							tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, tm.tm_msec);
						output->WriteStr(log->Get()->Get(), log->Get()->Length());
						output->Close();

						mmlAutoPtr < mmlIBuffer > msg_data;
						stream->GetData(msg_data.getterAddRef());
						mmlSharedPtr < mmlIInputStream > msg_stream = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), msg_data);
						_client->SendMessage(mmlTrue, msg_stream, mmlNULL);
					}
					return mmlTrue;
				}
			}lq(client);
			db->Query("SELECT * FROM (SELECT timestamp, data FROM logs ORDER BY timestamp DESC  LIMIT 1000 ) ORDER BY timestamp ASC", &lq, mmlNULL, mmlNULL);

		}

		mLogsWatcher = new android_logs_output_stream(client);
		mmlSharedPtr < mmlILogger > logger = mmlGetService(MML_OBJECT_UUID(mmlILogger));
		logger->Attach(mLogsWatcher, &mLogId);
		return mmlTrue;
	}

	void OnDisconnect ()
	{
		if ( mLogId != 0 )
		{
			mmlSharedPtr < mmlILogger > logger = mmlGetService(MML_OBJECT_UUID(mmlILogger));
			logger->Detach(mLogId);
			mLogId = 0;
		}
	}
};

MML_OBJECT_IMPL0(android_logs_websocket_listener)

#include "mmlIThread.h"
#include "mmlIMutex.h"

class android_logs_websocket_thread : public mmlIThreadFunction
{
	MML_OBJECT_DECL
private:
	mmlBool mFinished;
	mmlBool mShutdown;
	mmlAutoPtr < mmlIVariantString > mUrl;
	mmlSharedPtr < mmlIThread > mThread;
public:
	android_logs_websocket_thread( mmlIVariantString * url)
		:mFinished(mmlFalse), mShutdown(mmlFalse), mUrl(url)
	{
		mThread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
		mThread->Create(new mmlThreadFunctionProxy < android_logs_websocket_thread >(this), PriorityNormal, MML_DEFAULT_STACK_SIZE);
	}

	~android_logs_websocket_thread()
	{
		mThread->Join();
	}

	void Shutdown()
	{
		mShutdown = mmlTrue;
	}

	mmlBool IsFinished()
	{
		return mFinished;
	}

	void Execute(mmlIThread * thread)
	{
		mmlSharedPtr < mmlIWebSocketClient > websocket = mmlNewObject(MML_OBJECT_UUID(mmlIWebSocketClient));
		mmlSharedPtr < mmlIVariantStruct > config = mmlGetService(MML_GLOBAL_CONFIG_UUID());
		mmlAutoPtr < mmlIVariantStruct > websocket_config;
		if ( config->Get("websocket", mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant>(websocket_config)) == mmlTrue )
		{
			if ( websocket_config != mmlNULL)
			{
				websocket->Connect(websocket_config, mUrl->Get()->Get(), new android_logs_websocket_listener(&mShutdown));
			}
		}
		mFinished = mmlTrue;
	}
};

MML_OBJECT_IMPL0(android_logs_websocket_thread)


#define ANDROID_LOGS_WEBSOCKET_MANAGER_UUID { 0x8DFD01A4, 0x6D0E, 0x1014, 0x90CD , { 0x8A, 0xA9, 0x9B, 0xE7, 0x8C, 0x67 } }

class android_logs_websocket_manager : public mmlIThreadFunction
{
	MML_OBJECT_UUID_DECL(ANDROID_LOGS_WEBSOCKET_MANAGER_UUID)
	MML_OBJECT_DECL
private:
	MML_THREAD_DECLARE_SHUTDOWN_FLAG
	mmlSharedPtr < mmlIThread > mThread;
	std::list < mmlAutoPtr < android_logs_websocket_thread > > mSessions;
	mmlSharedPtr < mmlIMutex > mMutex;
public:
	android_logs_websocket_manager()
	{
		mMutex = mmlNewObject(MML_OBJECT_UUID(mmlIMutex));
		mThread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
		mThread->Create(new mmlThreadFunctionProxy<android_logs_websocket_manager>(this), PriorityNormal, MML_DEFAULT_STACK_SIZE);
	}

	~android_logs_websocket_manager()
	{
	}

	mmlBool Start ( mmlIVariantString * url )
	{
		mmlMutexGuard g(mMutex);
		mSessions.push_back(new android_logs_websocket_thread(url));
		return mmlTrue;
	}

	void Execute (mmlIThread * thread)
	{
		while (IsShutdown() == mmlFalse )
		{
			mml_sleep(1000);
			mmlMutexGuard g(mMutex);
			for ( std::list < mmlAutoPtr < android_logs_websocket_thread > >::iterator session = mSessions.begin(); session != mSessions.end();)
			{
				if ( (*session)->IsFinished() == mmlTrue )
				{
					mSessions.erase(session++);
				}
				else
				{
					session++;
				}
			}
		}
	}

	void Stop ()
	{
		mThread->Join();
	}
};

MML_OBJECT_IMPL1(android_logs_websocket_manager,android_logs_websocket_manager)

CMD_HANDLER_BEGIN(cmd_logs)
{
	mmlSharedPtr < mmlIVariantStruct > logs_request;
	request->GetRequest(logs_request.getterAddRef());
	CMD_HANDLER_ASSERT(logs_request != mmlNULL, -1, "Could not cast request");
	mmlAutoPtr < mmlIVariantString > url;
	CMD_HANDLER_ASSERT(logs_request->Get("url", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant>(url)) == mmlTrue, -1, "Could not get url");
	log_main("Received logs command url=%s\n", url->Get()->Get());
	mmlSharedPtr < android_logs_websocket_manager > manager = mmlGetService(MML_OBJECT_UUID(android_logs_websocket_manager));
	CMD_HANDLER_ASSERT(manager->Start( url) == mmlTrue, -1, "Could not start session");
	request->RespondOK(mmlNULL);
}
CMD_HANDLER_END()

mmlBool cmd_logs_init ()
{
	mmlSetService(MML_OBJECT_UUID(android_logs_websocket_manager), new android_logs_websocket_manager());
	return mmlTrue;
}
void cmd_logs_destroy()
{
	mmlSharedPtr < android_logs_websocket_manager > manager = mmlGetService(MML_OBJECT_UUID(android_logs_websocket_manager));
	manager->Stop();
}



ANDROID_CMD_MODULE_ID(logs, "io.appservice.module.LOGS", cmd_logs, cmd_logs_init, cmd_logs_destroy);