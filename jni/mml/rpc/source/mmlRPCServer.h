#ifndef MML_RPC_SERVER_IMPL_HEADER_INCLUDED
#define MML_RPC_SERVER_IMPL_HEADER_INCLUDED

#include "mmlIRCPServer.h"
#include "mmlIThread.h"
#include "mmlIObjectQueue.h"
#include <list>
#include <map>
#include <string>

class mml_rpc_cmd_worker : public mmlIThreadFunction
{
	MML_THREAD_DECLARE_SHUTDOWN_FLAG
	MML_OBJECT_DECL
private:
	mmlSharedPtr < mmlIObjectQueue > mQueue;
	mmlSharedPtr < mmlIThread > mThread;
public:

	mml_rpc_cmd_worker(mmlIObjectQueue * queue);

	~mml_rpc_cmd_worker();

	void Execute(mmlIThread * thread);
};

class mmlRPCServerStreamHandler : public mmlIRPCServerStreamHandler
{
	MML_OBJECT_DECL
private:
	std::list < mmlAutoPtr < mml_rpc_cmd_worker > > mWorkers;
	std::map < std::string, mmlAutoPtr < mmlIRPCServerFunction > > mFunctions;
	mmlSharedPtr < mmlIObjectQueue > mRequestQueue;
	mmlAutoPtr < mmlIRPCServerAuthenticator > mAuthenticator;
public:
	mmlRPCServerStreamHandler();

	mmlBool Construct(mmlIRPCServerAuthenticator * authenticator);

	void Add(const mmlChar * name, mmlIRPCServerFunction * function);

	mmlIRPCServerFunction * Get(const mmlChar * name);

	void SetWorkers(const mmlInt32 count);

	mmlBool OnConnect(mmlICString * iface, mmlICString * source, mmlInt32 * chunk_size, mmlIOutputStream ** input, mmlIInputStream ** output);
};

#endif //MML_RPC_SERVER_IMPL_HEADER_INCLUDED
