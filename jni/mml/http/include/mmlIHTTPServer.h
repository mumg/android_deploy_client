#ifndef HTTP_I_SERVER_H
#define HTTP_I_SERVER_H

#include "mmlIHTTPResponse.h"
#include "mmlIHTTPRequest.h"
#include "mmlIStream.h"

class mmlIHTTPContentFactory;

#define MML_HTTP_SERVER_CONTEXT_UUID { 0x6944173B, 0x6C82, 0x1014, 0x9909 , { 0xE7, 0x43, 0x17, 0x0E, 0x14, 0xB1 } }

class mmlIHTTPServerContext : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_HTTP_SERVER_CONTEXT_UUID)
public:

	virtual mmlBool Response (mmlIHTTPResponse * response, mmlIInputStream * content) = 0;

	virtual mmlBool Response ( const mmlInt32 error ) = 0;

	virtual mmlBool IsActive () = 0;

	virtual mmlBool GetSource ( mmlICString ** source ) = 0;
};


class mmlIHTTPServerContentHandler : public mmlIObject
{
public:
	virtual mmlBool PrepareContent ( mmlIHTTPRequest * request, mmlIOutputStream ** content_output, mmlIInputStream ** content_input ) = 0;
};



class mmlIHTTPServerHandler : public mmlIObject
{
public:
	virtual mmlBool Process ( mmlIHTTPServerContext * socket , mmlIHTTPRequest * request , mmlIInputStream * content) = 0;
};

#define MML_HTTP_SERVER_UUID { 0x6FBB660E, 0x6C82, 0x1014, 0x8C38 , { 0xB0, 0x01, 0x29, 0xB2, 0x14, 0xA9 } }

class mmlIHTTPServer : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_HTTP_SERVER_UUID)
public:
	virtual mmlBool Start (mmlIVariantStruct * config , mmlIHTTPServerHandler * server_handler, mmlIHTTPServerContentHandler * content_handler = mmlNULL) = 0;
	virtual void Stop () = 0;
};


#endif 
