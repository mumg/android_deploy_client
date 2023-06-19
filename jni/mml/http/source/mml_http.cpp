#include "mml_components.h"
#include "mmlHTTPRequest.h"
#include "mmlHTTPResponse.h"
#include "mmlHTTPServer.h"
#include "mmlHTTPClient.h"

MML_CONSTRUCTOR_IMPL(mmlHTTPRequestUri);
MML_CONSTRUCTOR_IMPL(mmlHTTPRequest);
MML_CONSTRUCTOR_IMPL(mmlHTTPResponse);
MML_CONSTRUCTOR_IMPL(mmlHTTPServer);
MML_CONSTRUCTOR_IMPL(mmlHTTPClient);

static mmlObjectFactory mml_http_component [] =
{
	{
		MML_HTTP_REQUEST_URI_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlHTTPRequestUri)
	},
	{
		MML_HTTP_REQUEST_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlHTTPRequest)
	},
	{
		MML_HTTP_SERVER_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlHTTPServer)
	},
	{
		MML_HTTP_RESPONSE_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlHTTPResponse)
	},
	{
		MML_HTTP_CLIENT_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlHTTPClient)
	}
};

MML_COMPONENT_IMPL(http,mml_http_component) 

