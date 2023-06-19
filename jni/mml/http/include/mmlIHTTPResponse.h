#ifndef HTTP_I_RESPONSE_H
#define HTTP_I_RESPONSE_H

#include "mmlIHTTPRequest.h"

enum
{
	HTTP_CONTINUE = 100,
	HTTP_SWITCHING_PROTOCOL = 101,
	HTTP_OK = 200,
	HTTP_CREATED = 201,
	HTTP_ACCEPTED = 202,
	HTTP_NON_AUTHORITATIVE_INFORMATION = 203,
	HTTP_NO_CONTENT = 204,
	HTTP_RESET_CONTENT = 205,
	HTTP_PARTIAL_CONTENT = 206,
	HTTP_MULTIPLE_CHOICES = 300,
	HTTP_MOVED_PERMANENTLY = 301,
	HTTP_FOUND = 302,
	HTTP_SEE_OTHER = 303,
	HTTP_NOT_MODIFIED = 304,
	HTTP_USE_PROXY = 305,
	HTTP_BAD_REQUEST = 400,
	HTTP_UNAUTHORIZED = 401,
	HTTP_PAYMENT_REQUIRED = 402,
	HTTP_FORBIDDEN = 403,
	HTTP_NOT_FOUND = 404,
	HTTP_METHOD_NOT_ALLOWED = 405,
	HTTP_NOT_ACCEPTABLE = 406,
	HTTP_PROXY_AUTHENTICATION_REQUIRED = 407,
	HTTP_REQUEST_TIMEOUT = 408,
	HTTP_CONFLICT = 409,
	HTTP_GONE = 410,
	HTTP_LENGTH_REQUIRED = 411,
	HTTP_PRECONDITION_FAILED = 412,
	HTTP_REQUEST_ENTITY_TOO_LARGE = 413,
	HTTP_REQUEST_URI_TOO_LONG = 414,
	HTTP_UNSUPPORTED_MEDIA_TYPE = 415,
	HTTP_REQUESTED_RANGE_NOT_SATISFIABLE = 416,
	HTTP_EXPECTATION_FAILED = 417,
	HTTP_INTERNAL_SERVER_ERROR = 500,
	HTTP_NOT_IMPLEMENTED = 501,
	HTTP_BAD_GATEWAY = 502,
	HTTP_SERVICE_UNAVAILABLE = 503,
	HTTP_GATEWAY_TIMEOUT = 504,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

#define MML_HTTP_RESPONSE_UUID { 0xD05ED5AE, 0x6CA7, 0x1014, 0x8F4A , { 0xB3, 0x32, 0x7E, 0x7D, 0xE3, 0x19 } }

class mmlIHTTPResponse : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_HTTP_RESPONSE_UUID)
public:

	virtual void SetResponseCode ( const mmlInt32 response_code ) = 0;

	virtual mmlInt32 GetResponseCode () = 0;

	virtual void SetResponseStr ( const mmlChar * response_str ) = 0;

	virtual const mmlChar * GetResponseStr () = 0;

	virtual const mmlChar * GetHeader ( const mmlChar * name ) = 0;

	virtual void SetHeader ( const mmlChar * name , const mmlChar * value ) = 0;

	virtual void SetVersion ( const mmlFloat64 version ) = 0;

	virtual mmlFloat64 GetVersion () = 0;

};

#endif