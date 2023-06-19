#ifndef MML_BRIDGE_SERVER_IMPL_HEADER_INCLUDED
#define MML_BRIDGE_SERVER_IMPL_HEADER_INCLUDED

#include "mmlIBridgeStreamServer.h"
#include "mmlIThread.h"

class mmlBridgeStreamServer : public mmlIBridgeStreamServer
{
	MML_OBJECT_DECL
	MML_THREAD_DECLARE_SHUTDOWN_FLAG

public:

	mmlBool Start(mmlIVariantStruct * config, mmlIStreamServerHandler * handler);

	mmlBool Stop();

	void Execute(mmlIThread * thread);
private:

	mmlInt32 mReconnectTimeout;

	mmlAutoPtr < mmlIVariantStruct > mClientConfig;

	mmlUUID mClientUUID;



	mmlAutoPtr < mmlIStreamClient > mClient;

	mmlSharedPtr < mmlIThread > mThread;

	mmlAutoPtr < mmlIStreamServerHandler > mHandler;
};

#endif //MML_BRIDGE_SERVER_IMPL_HEADER_INCLUDED
