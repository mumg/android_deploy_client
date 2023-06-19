#include "mml_components.h"
#include "mmlRPCServer.h"


MML_CONSTRUCTOR_IMPL1(mmlRPCServerStreamHandler, mmlIRPCServerAuthenticator);
MML_CONSTRUCTOR_IMPL(mmlRPCServerStreamHandler);

static mmlObjectFactory mml_rpc_component[] =
{
	{
		MML_RCP_SERVER_STREAM_HANDLER_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlRPCServerStreamHandler, mmlIRPCServerAuthenticator)
	},
	{
		MML_RCP_SERVER_STREAM_HANDLER_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlRPCServerStreamHandler)
	}
};

MML_COMPONENT_IMPL(rpc, mml_rpc_component)