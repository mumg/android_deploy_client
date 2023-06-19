#include "mmlBridgeStreamServer.h"
#include "mml_components.h"

void log_bridge(const mmlChar * formatter, ...);

mmlBool mmlBridgeStreamServer::Start(mmlIVariantStruct * config, mmlIStreamServerHandler * handler)
{
	if (config->Get("client", mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant >(mClientConfig)) == mmlFalse)
	{
		return mmlFalse;
	}
	if (mClientConfig == mmlNULL)
	{
		return mmlFalse;
	}
	mmlAutoPtr < mmlIVariantString > type;
	if (mClientConfig->Get("type", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(type)) == mmlFalse)
	{
		return mmlFalse;
	}
	if (type == mmlNULL)
	{
		return mmlFalse;
	}
	if (mmlGetObjectUUID(type->Get()->Get(), mClientUUID) == mmlFalse)
	{
		return mmlFalse;
	}
	mmlAutoPtr < mmlIVariantInteger > reconnect;
	if (config->Get("reconnect", mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant >(reconnect)) == mmlFalse)
	{
		return mmlFalse;
	}
	if (reconnect == mmlNULL)
	{
		return mmlFalse;
	}
	mReconnectTimeout = reconnect->Get();
	mThread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
	mHandler = handler;
	return mThread->Create(new mmlThreadFunctionProxy <mmlBridgeStreamServer>(this), PriorityNormal, MML_DEFAULT_STACK_SIZE);
}

mmlBool mmlBridgeStreamServer::Stop()
{
	if (mThread != mmlNULL)
	{
		mThread->Join();
	}
	return mmlTrue;
}

class mmlBridgeThread : public mmlIThreadFunction
{
	MML_OBJECT_DECL
	MML_THREAD_DECLARE_SHUTDOWN_FLAG
private:
	mmlAutoPtr < mmlIInputStream > mInput;
	mmlAutoPtr < mmlIOutputStream > mOutput;
	mmlInt32 mChunkSize;
public:
	mmlBridgeThread(mmlIInputStream * input, mmlIOutputStream * output, const mmlInt32 chunk_size)
		:mInput(input), mOutput(output), mChunkSize(chunk_size)
	{

	}

	void Execute(mmlIThread * thread)
	{
		mmlAutoPtr < mmlIStreamControl > control;
		mInput->GetControl(control.getterAddRef());
		if (control != mmlNULL)
		{
			control->SetTimeout(100);
		}
		void * buffer = mmlAlloc(mChunkSize);
		while (IsShutdown() == mmlFalse)
		{
			mmlInt64 rd = mInput->Read(mChunkSize, buffer);
			if (rd < 0)
			{
				break;
			}
			if (rd > 0)
			{
				if (mOutput->Write(rd, buffer) != rd)
				{
					break;
				}
			}
		}
		mmlFree(buffer);
	}
};

MML_OBJECT_IMPL0(mmlBridgeThread)

void mmlBridgeStreamServer::Execute(mmlIThread * thread)
{
	while (IsShutdown() != mmlTrue)
	{
		mmlSharedPtr < mmlIStreamClient > client = mmlNewObject(mClientUUID);
		if (client == mmlNULL)
		{
			break;
		}
		mmlAutoPtr < mmlIInputStream > client_input;
		mmlAutoPtr < mmlIOutputStream > client_output;
		log_bridge("Connecting to bridge source\n");
		if (client->Connect(mClientConfig, client_input.getterAddRef(), client_output.getterAddRef()) == NSC_OK)
		{
			mmlInt32 chunk_size = 4096;
			mmlAutoPtr < mmlIInputStream > input;
			mmlAutoPtr < mmlIOutputStream > output;

			if (mHandler->OnConnect(mmlNULL, mmlNULL, &chunk_size, output.getterAddRef(), input.getterAddRef()) == mmlFalse)
			{
				continue;
			}
			mmlSharedPtr < mmlIThread > bridge1 = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
			bridge1->Create(new mmlBridgeThread(client_input, output, chunk_size), PriorityNormal, MML_DEFAULT_STACK_SIZE);
			mmlSharedPtr < mmlIThread > bridge2 = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
			bridge2->Create(new mmlBridgeThread(input, client_output, chunk_size), PriorityNormal, MML_DEFAULT_STACK_SIZE);
			log_bridge("Bridge started %d\n", chunk_size);
			while (client->IsConnected() == mmlTrue && IsShutdown() == mmlFalse )
			{
				mml_sleep(1000);
			}
			log_bridge("Bridge stopped\n");
			bridge1->Join();
			bridge2->Join();
		}
		else
		{
			log_bridge("Connect to bridge failed\n");
			if (mReconnectTimeout < 1000)
			{
				mml_sleep(mReconnectTimeout);
			}
			else
			{
				for (mmlInt32 tick = 0; tick < mReconnectTimeout / 1000 && IsShutdown() == mmlFalse; tick++)
				{
					mml_sleep(1000);
				}
			}
		}
	}
}

MML_OBJECT_IMPL2(mmlBridgeStreamServer, mmlIBridgeStreamServer, mmlIStreamServer)
