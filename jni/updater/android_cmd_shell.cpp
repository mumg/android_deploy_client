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

#ifdef MML_WIN
const mmlChar * shell_executable = "cmd.exe";
#elif MML_ANDROID
const mmlChar * shell_executable = "/system/bin/sh";
#else
const mmlChar * shell_executable = "/bin/bash";
#endif

void log_main( const mmlChar * formatter , ... );

#define MML_JSON_VARIANT_SERIALIZER_UUID { 0x407065FF, 0x6C70, 0x1014, 0xBDA0 , { 0xF9, 0xC2, 0xF4, 0x90, 0x1F, 0xFA } }

#define MML_JSON_VARIANT_DESERIALIZER_UUID { 0x4383190F, 0x6C70, 0x1014, 0x9A99 , { 0x9C, 0xA2, 0x29, 0x60, 0xE2, 0xE9 } }

class android_shell_input_stream  : public mmlIInputStream
{
	MML_OBJECT_DECL
public:
	android_shell_input_stream(const mmlInt32 timeout, mmlIObjectQueue *queue);

	mmlBool GetControl ( mmlIStreamControl ** control );

	mmlInt64 Read ( const mmlInt64 size , void * data );

	mmlBool IsEmpty ();

	mmlInt64 Size ();

	mmlBool Close ();

private:

	mmlInt32 mTimeout;

	mmlAutoPtr < mmlIInputStream > mCurrent;

	mmlAutoPtr < mmlIObjectQueue > mQueue;
};

android_shell_input_stream::android_shell_input_stream(const mmlInt32 timeout, mmlIObjectQueue *queue)
	:mQueue(queue), mTimeout(timeout)
{

}

mmlBool android_shell_input_stream::GetControl ( mmlIStreamControl ** control )
{
	return mmlFalse;
}

mmlInt64 android_shell_input_stream::Read ( const mmlInt64 size , void * data )
{
	if ( mQueue == mmlNULL )
	{
		return -1;
	}
	for (;;)
	{
		if ( mCurrent != mmlNULL )
		{
			if ( mCurrent->IsEmpty() == mmlFalse )
			{
				mmlUInt8 * ptr = (mmlUInt8*)data;
				mmlInt32 sz = 0;
				for ( mmlInt32 index = 0 ; index < size ; index ++ )
				{
					mmlUInt8 d;
					if ( mCurrent->Read(1, &d) == 1 )
					{
						if ( d == 0xFF )
						{
							mmlUInt8 d2[2];
							if ( mCurrent->Read(2, &d2) != 2 )
							{
								mCurrent = mmlNULL;
								return sz;
							}
						}
						else
						{
							*ptr = d;
							ptr ++;
							sz ++;
						}
					}
					else
					{
						mCurrent = mmlNULL;
						return index;
					}
				}
			}
			else
			{
				mCurrent = mmlNULL;
			}
		}
		if ( mTimeout == 0 && mQueue->Size() == 0 )
		{
			return 0;
		}
		else
		{
			if ( mQueue->Get(mTimeout, mCurrent.getterAddRef()) == mmlTrue )
			{
				if ( mCurrent == mmlNULL )
				{
					mQueue = mmlNULL;
					return -1;
				}
			}
			else
			{
				return 0;
			}
		}
	}
}

mmlBool android_shell_input_stream::IsEmpty ()
{
	if ( mQueue != mmlNULL )
	{
		return mmlFalse;
	}
	return mmlTrue;
}

mmlInt64 android_shell_input_stream::Size ()
{
	return -1;
}

mmlBool android_shell_input_stream::Close ()
{
	return mmlTrue;
}

MML_OBJECT_IMPL1(android_shell_input_stream, mmlIInputStream)


class android_shell_output_stream : public mmlIOutputStream
{
	MML_OBJECT_DECL
public:
	android_shell_output_stream ( mmlIWebSocketClient * client )
		:mClient(client)
	{

	}

	mmlInt64 Write ( const mmlInt64 size , const void * data )
	{
		if ( mStream == mmlNULL )
		{
			mStream = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));
		}
		return mStream->Write(size, data);
	}

	mmlBool IsFull (){ return mmlFalse; }

	mmlBool GetControl ( mmlIStreamControl ** control ) { return mmlFalse; }

	mmlBool Close () { return mmlTrue; }

	mmlInt64 Size () { return -1; }

	mmlBool Flush ()
	{
		if ( mStream != mmlNULL )
		{
			mStream->Close();
			mmlAutoPtr < mmlIBuffer > data;
			mStream->GetData(data.getterAddRef());
			if ( data != mmlNULL && data->Size() > 0 )
			{
				mmlSharedPtr < mmlIInputStream > msg = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), data);
				if ( msg != mmlNULL )
				{
					mStream = mmlNULL;
					mClient->SendMessage(mmlTrue, msg, mmlNULL);
				}
			}
		}
		return mmlTrue;
	}

private:

	mmlIWebSocketClient * mClient;

	mmlSharedPtr < mmlIMemoryOutputStream > mStream;
};

MML_OBJECT_IMPL1(android_shell_output_stream, mmlIOutputStream)


class android_shell_websocket_listener : public mmlIWebSocketClientListener
{
	MML_OBJECT_DECL

	mmlSharedPtr < mmlIProcess > mShell;

	mmlSharedPtr < mmlIObjectQueue > mProcessInput;

	mmlBool * mShutdown;
public:

	android_shell_websocket_listener( mmlBool * shutdown)
		:  mShutdown(shutdown)
	{
	}

	mmlBool OnPingReceived() { return mmlTrue; }

	mmlBool OnPingSent () { return mmlTrue; }

	mmlBool OnReceiveMessage ( mmlIWebSocketClient * client, mmlIInputStream * message )
	{
		return mProcessInput->Post(message);
	}

	mmlBool IsShutdown ()
	{
		if ( *mShutdown == mmlTrue ) 
		{
				return mmlTrue;
		}
		if ( mShell == mmlNULL ) return mmlFalse;
		mmlBool finished = mmlFalse;
		if ( mShell->Wait(1, &finished, mmlNULL) == mmlTrue )
		{
			if ( finished == mmlTrue )
			{
				int i = 0;
			}
			return finished;
		}
		return mmlFalse;
	}

	mmlBool OnConnect ( mmlIWebSocketClient * client )
	{
		log_main("shell connected\n");
		mProcessInput = mmlNewObject(MML_OBJECT_UUID(mmlIObjectQueue));
		mShell = mmlNewObject(MML_OBJECT_UUID(mmlIProcessPty));
		if (mShell == mmlNULL)
		{
			return mmlFalse;
		}
		log_main("Executing shell %s\n", shell_executable);
		if (mShell->Exec(mmlCStaticString(shell_executable), mmlNULL, 0, mmlNULL, new android_shell_input_stream(100, mProcessInput), new android_shell_output_stream(client), mmlNULL) == mmlFalse)
		{
			return mmlFalse;
		}
		return mmlTrue;
	}

	void OnDisconnect ()
	{
		if ( mShell != mmlNULL )
		{
			mShell->Shutdown(mmlTrue);
		}
	}
};

MML_OBJECT_IMPL0(android_shell_websocket_listener)

#include "mmlIThread.h"
#include "mmlIMutex.h"

class android_shell_websocket_thread : public mmlIThreadFunction
{
	MML_OBJECT_DECL
private:
	mmlBool mFinished;
	mmlBool mShutdown;
	mmlAutoPtr < mmlIVariantString > mUrl;
	mmlSharedPtr < mmlIThread > mThread;
public:
	android_shell_websocket_thread(mmlIVariantString * url)
		:mFinished(mmlFalse), mShutdown(mmlFalse), mUrl(url)
	{
		mThread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
		mThread->Create(new mmlThreadFunctionProxy < android_shell_websocket_thread >(this), PriorityNormal, MML_DEFAULT_STACK_SIZE);
	}

	~android_shell_websocket_thread()
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
				websocket->Connect(websocket_config, mUrl->Get()->Get(), new android_shell_websocket_listener(&mShutdown));
			}
		}
		mFinished = mmlTrue;
	}
};

MML_OBJECT_IMPL0(android_shell_websocket_thread)


#define ANDROID_SHELL_WEBSOCKET_MANAGER_UUID { 0x5CB59CEE, 0x6C27, 0x1014, 0x82D6 , { 0xB5, 0xDB, 0xFC, 0xCC, 0xDF, 0x5C } }

class android_shell_websocket_manager : public mmlIThreadFunction
{
	MML_OBJECT_UUID_DECL(ANDROID_SHELL_WEBSOCKET_MANAGER_UUID)
	MML_OBJECT_DECL
private:
	MML_THREAD_DECLARE_SHUTDOWN_FLAG
	mmlSharedPtr < mmlIThread > mThread;
	std::list < mmlAutoPtr < android_shell_websocket_thread > > mSessions;
	mmlSharedPtr < mmlIMutex > mMutex;
public:
	android_shell_websocket_manager()
	{
		mMutex = mmlNewObject(MML_OBJECT_UUID(mmlIMutex));
		mThread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
		mThread->Create(new mmlThreadFunctionProxy<android_shell_websocket_manager>(this), PriorityNormal, MML_DEFAULT_STACK_SIZE);
	}

	~android_shell_websocket_manager()
	{
	}

	mmlBool Start ( mmlIVariantString * url )
	{
		mmlMutexGuard g(mMutex);
		mSessions.push_back(new android_shell_websocket_thread(url));
		return mmlTrue;
	}

	void Execute (mmlIThread * thread)
	{
		while (IsShutdown() == mmlFalse )
		{
			mml_sleep(1000);
			mmlMutexGuard g(mMutex);
			for ( std::list < mmlAutoPtr < android_shell_websocket_thread > >::iterator session = mSessions.begin(); session != mSessions.end();)
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

MML_OBJECT_IMPL1(android_shell_websocket_manager,android_shell_websocket_manager)

CMD_HANDLER_BEGIN(cmd_shell)
{
	mmlSharedPtr < mmlIVariantStruct > shell_request;
	request->GetRequest(shell_request.getterAddRef());
    CMD_HANDLER_ASSERT(shell_request != mmlNULL, -1, "Could not cast request");
	mmlAutoPtr < mmlIVariantString > url;
	CMD_HANDLER_ASSERT(shell_request->Get("url", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant>(url)) == mmlTrue, -1, "Could not get url");
	log_main("Received shell command url=%s\n", url->Get()->Get());
	mmlSharedPtr < android_shell_websocket_manager > manager = mmlGetService(MML_OBJECT_UUID(android_shell_websocket_manager));
	CMD_HANDLER_ASSERT(manager->Start(url) == mmlTrue, -1, "Could not start session");
	request->RespondOK(mmlNULL);
}
CMD_HANDLER_END()

mmlBool cmd_shell_init ()
{
	mmlSetService(MML_OBJECT_UUID(android_shell_websocket_manager), new android_shell_websocket_manager());
	return mmlTrue;
}
void cmd_shell_destroy()
{
	mmlSharedPtr < android_shell_websocket_manager > manager = mmlGetService(MML_OBJECT_UUID(android_shell_websocket_manager));
	manager->Stop();
}



ANDROID_CMD_MODULE_ID(shell, "io.appservice.module.SHELL", cmd_shell, cmd_shell_init, cmd_shell_destroy);