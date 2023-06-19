#include "mml_components.h"
#include "mmlNetworkStreamServer.h"
#ifdef MML_SSL
//#include "mmlNetworkStreamServerSSL.h"
#endif
#include "mmlNetworkStreamClient.h"
#ifdef MML_SSL
#include "mmlNetworkStreamClientSSL.h"
#endif

#ifdef MML_WIN
#include <Windows.h>
#endif


MML_CONSTRUCTOR_IMPL(mmlNetworkStreamServer);
#ifdef MML_SSL
//MML_CONSTRUCTOR_IMPL(mmlNetworkStreamServerSSL);
#endif
MML_CONSTRUCTOR_IMPL(mmlNetworkStreamClient);
#ifdef MML_SSL
MML_CONSTRUCTOR_IMPL(mmlNetworkStreamClientSSL);
#endif

static mmlObjectFactory mml_network_component [] =
{
	{
		MML_NETWORK_STREAM_SERVER_UUID,
		"@mml/stream/network/server",
		MML_CONSTRUCTOR(mmlNetworkStreamServer)
	},
#ifdef MML_SSL
	/*{
		//MML_NETWORK_STREAM_SERVER_SSL_UUID,
		//"@mml/stream/server/tcp-ssl",
		//MML_CONSTRUCTOR(mmlNetworkStreamServerSSL)
	},*/
#endif
	{
		MML_NETWORK_STREAM_CLIENT_UUID,
		"@mml/stream/network/client",
		MML_CONSTRUCTOR(mmlNetworkStreamClient)
	}
#ifdef MML_SSL
	,{
		MML_NETWORK_STREAM_CLIENT_SSL_UUID,
		"@mml/stream/network/client-ssl",
		MML_CONSTRUCTOR(mmlNetworkStreamClientSSL)
	}
#endif
};

static mmlBool network_constructor ()
{
	return mmlTrue;
}

static void network_destructor()
{
}

MML_COMPONENT_IMPL_CD(network,mml_network_component,network_constructor , network_destructor,  mmlNULL , mmlNULL) 

