#ifndef MML_WEBSOCKET_CLIENT_HEADER_INCLUDED
#define MML_WEBSOCKET_CLIENT_HEADER_INCLUDED

#include "mmlIObject.h"
#include "mmlIStream.h"

class mmlIWebSocketClient;

class mmlIWebSocketMessageListener : public mmlIObject
{
public:

	typedef enum
	{
		OK,
		ERROR
	}RESULT_T;

	virtual mmlBool OnMessageResult (const  RESULT_T  result) = 0;

};

class mmlIWebSocketClientListener : public mmlIObject
{
public:

	virtual mmlBool OnReceiveMessage ( mmlIWebSocketClient * client,  mmlIInputStream * message ) = 0;

	virtual mmlBool IsShutdown () = 0;

	virtual mmlBool OnConnect ( mmlIWebSocketClient * client ) = 0;

	virtual void OnDisconnect () = 0;

	virtual mmlBool OnPingReceived () = 0;

	virtual mmlBool OnPingSent () = 0;

};



#define MML_WEBSOCKET_CLIENT_UUID { 0x2E646FEB, 0x6FC2, 0x1014, 0x97F3 , { 0xE5, 0xE5, 0xC4, 0x62, 0x5A, 0x36 } }

class mmlIWebSocketClient : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_WEBSOCKET_CLIENT_UUID)
public:

	virtual mmlBool Connect (mmlIVariantStruct * config,  const mmlChar * url , mmlIWebSocketClientListener * listener ) = 0;

	virtual mmlBool SendMessage (  const mmlBool binary, mmlIInputStream * message , mmlIWebSocketMessageListener * listener ) = 0;

};


#endif //MML_WEBSOCKET_CLIENT_HEADER_INCLUDED
