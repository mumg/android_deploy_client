#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "mml_time.h"
#include "mmlIObjectQueue.h"
#include "mmlINetworkStreamServer.h"
#include "mmlIHTTPServer.h"
#include "mmlIThread.h"
#include "mmlIMutex.h"
#include <list>
#include <map>

class mmlHTTPServer : public mmlIHTTPServer
{
	MML_OBJECT_DECL;
public:
	mmlHTTPServer();

	mmlBool Start (mmlIVariantStruct * config , mmlIHTTPServerHandler * server_handler, mmlIHTTPServerContentHandler * content_handler);
	void Stop ();


private:

	mmlSharedPtr < mmlIStreamServer > mServer;
};


#endif 
