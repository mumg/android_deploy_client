#ifndef HTTP_I_REQUEST_H
#define HTTP_I_REQUEST_H

#include "mmlICStringStream.h"

#include "mmlIStream.h"



#define MML_HTTP_REQUEST_URI_UUID { 0x7C3D7E17, 0x6C81, 0x1014, 0xBC8F , { 0xC8, 0x2C, 0xA9, 0xB8, 0x5B, 0x65 } }

class mmlIHTTPRequestUri : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_HTTP_REQUEST_URI_UUID)
public:

	virtual mmlBool Load ( mmlICStringInputStream * stream ) = 0;

	virtual void Set ( const mmlChar * uri ) = 0;

	virtual const mmlChar * Get () = 0;

	virtual const mmlChar * GetArgument ( const mmlChar * key ) = 0;

	virtual int Size () = 0;

	virtual mmlBool GetArgument (const mmlInt32 index , mmlICString ** key , mmlICString ** value ) = 0;

	virtual void SetArgument ( const mmlChar * key , const mmlChar * value ) = 0;

	virtual mmlFloat64 GetVersion ()= 0;

	virtual void SetVersion (const mmlFloat64 version) = 0;

};


#define METHOD_OPTIONS "OPTIONS"
#define METHOD_GET "GET"
#define METHOD_HEAD "HEAD"
#define METHOD_POST "POST"
#define METHOD_PUT "PUT"
#define METHOD_DELETE "DELETE"
#define METHOD_TRACE "TRACE"
#define METHOD_CONNECT "CONNECT"


#define MML_HTTP_REQUEST_UUID { 0x8454BAE9, 0x6C81, 0x1014, 0x97F0 , { 0xB8, 0x86, 0xCD, 0xAC, 0xB3, 0x71 } }

class mmlIHTTPRequest : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_HTTP_REQUEST_UUID)
public:

	virtual const mmlChar * GetHeader ( const mmlChar * name ) = 0;

	virtual void SetHeader ( const mmlChar * name , const mmlChar * value ) = 0;
	
	virtual void DeleteHeader ( const mmlChar * name ) = 0;

	virtual mmlIHTTPRequestUri * GetReqURI () = 0;

	virtual void SetReqURI(mmlIHTTPRequestUri * uri) = 0;

	virtual const mmlChar * GetMethod () = 0;

	virtual void SetMethod ( const mmlChar * str ) = 0;

};




#endif //HTTP_I_REQUEST_H
