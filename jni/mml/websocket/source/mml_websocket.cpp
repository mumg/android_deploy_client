#include "mml_components.h"
#include "mmlWebSocketClient.h"


MML_CONSTRUCTOR_IMPL(mmlWebSocketClient)

static mmlObjectFactory mml_websocket_component [] =
{
	{
		MML_WEBSOCKET_CLIENT_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlWebSocketClient)
	}
};


MML_COMPONENT_IMPL(websocket,mml_websocket_component) 

