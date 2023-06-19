#include "mmlBridgeStreamServer.h"
#include "mml_library.h"

MML_CONSTRUCTOR_IMPL(mmlBridgeStreamServer)

static mmlObjectFactory mml_bridge_component[] =
{
	{
		MML_BRIDGE_STREAM_SERVER_UUID,
		"@mml/stream/bridge",
		MML_CONSTRUCTOR(mmlBridgeStreamServer)
	}
};

MML_COMPONENT_IMPL(bridge, mml_bridge_component)