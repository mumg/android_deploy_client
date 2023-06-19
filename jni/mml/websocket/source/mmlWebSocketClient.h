#ifndef MML_WEBSOCKET_CLIENT_IMPL_HEADER_INCLUDED
#define MML_WEBSOCKET_CLIENT_IMPL_HEADER_INCLUDED

#include "mmlIWebSocketClient.h"
#include "mmlIThread.h"
#include "mmlIObjectQueue.h"


class mmlWebSocketClient : public mmlIWebSocketClient
{
	MML_OBJECT_DECL
public:
	mmlBool Connect (mmlIVariantStruct * config, const mmlChar * url , mmlIWebSocketClientListener * listener );

	mmlBool SendMessage ( const mmlBool binary, mmlIInputStream * message , mmlIWebSocketMessageListener * listener );

private:

	mmlSharedPtr < mmlIObjectQueue > mQueue;
};

#endif //MML_WEBSOCKET_CLIENT_IMPL_HEADER_INCLUDED
