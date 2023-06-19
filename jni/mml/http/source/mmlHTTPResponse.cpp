#include "mmlHTTPResponse.h"
#include "mml_components.h"
#include "mmlICStringUtils.h"
#include <list>


static struct  
{
	mmlInt32 code;
	const mmlChar * reason;
}http_codes_map [] =
{
	{ HTTP_CONTINUE,                         "Continue" },
	{ HTTP_SWITCHING_PROTOCOL,               "Switching Protocol" },
	{ HTTP_OK ,                              "Ok" },
	{ HTTP_CREATED ,                         "Created" },
	{ HTTP_ACCEPTED ,                        "Accepted" },
	{ HTTP_NON_AUTHORITATIVE_INFORMATION ,   "Non Authoritative Information" },
	{ HTTP_NO_CONTENT ,                      "No Content" },
	{ HTTP_RESET_CONTENT ,                   "Reset Content"} ,
	{ HTTP_PARTIAL_CONTENT ,                 "Partial Content"},
	{ HTTP_MULTIPLE_CHOICES ,                "Multiple Choices"},
	{ HTTP_MOVED_PERMANENTLY ,               "Moved permanently" },
	{ HTTP_FOUND ,                           "Found" },
	{ HTTP_SEE_OTHER ,                       "See Other"},
	{ HTTP_NOT_MODIFIED ,                    "Not Modified"},
	{ HTTP_BAD_REQUEST ,                     "Bad Request"},
	{ HTTP_UNAUTHORIZED ,                    "Unauthorized"},
	{ HTTP_PAYMENT_REQUIRED ,                "Payment Required"},
	{ HTTP_FORBIDDEN ,                       "Forbidden"},
	{ HTTP_NOT_FOUND ,                       "Not Found"},
	{ HTTP_METHOD_NOT_ALLOWED ,              "Method Not Allowed"},
	{ HTTP_NOT_ACCEPTABLE ,                  "Not Acceptable"},
	{ HTTP_PROXY_AUTHENTICATION_REQUIRED ,   "Proxy Authentication Required"},
	{ HTTP_REQUEST_TIMEOUT ,                 "Requested Timeout"},
	{ HTTP_CONFLICT ,                        "Conflict" },
	{ HTTP_GONE ,                            "Gone"},
	{ HTTP_LENGTH_REQUIRED ,                 "Length Required"},
	{ HTTP_PRECONDITION_FAILED ,             "Precondition Failed"},
	{ HTTP_REQUEST_ENTITY_TOO_LARGE ,        "Request Entity Too Large"},
	{ HTTP_REQUEST_URI_TOO_LONG ,            "Request URI Too Long"},
	{ HTTP_UNSUPPORTED_MEDIA_TYPE ,          "Unsupported Media Type"},
	{ HTTP_REQUESTED_RANGE_NOT_SATISFIABLE , "Request Range Not Satisfiable"},
	{ HTTP_EXPECTATION_FAILED ,              "Expectation Failed"},
	{ HTTP_INTERNAL_SERVER_ERROR ,           "Internal Server Error"},
	{ HTTP_NOT_IMPLEMENTED ,                 "Not Implemented"},
	{ HTTP_BAD_GATEWAY ,                     "Bad Gateway"},
	{ HTTP_SERVICE_UNAVAILABLE ,             "Service Unavailable"},
	{ HTTP_GATEWAY_TIMEOUT ,                 "Gateway Timeout"},
	{ HTTP_VERSION_NOT_SUPPORTED ,           "Version Not Supported"},
	{ 0,                                                    NULL }
};

extern mmlInt32 http_characters_safe_map [];

static const mmlChar * http_get_str ( const mmlInt32 code )
{
	for ( mmlInt32 index = 0 ; http_codes_map[index].reason != NULL ; index ++ )
	{
		if ( http_codes_map[index].code == code )
		{
			return http_codes_map[index].reason;
		}
	}
	return NULL;
}

mmlHTTPResponse::mmlHTTPResponse (const mmlInt32 code, mmlICString * str , const mmlFloat64 version)
:mVersion(version), mCode(code), mStr(str)
{
}

mmlHTTPResponse::mmlHTTPResponse()
:mVersion(1.0) , mCode(HTTP_OK)
{
}

mmlBool mmlHTTPResponse::Load ( void * data , const mmlInt32 data_len, mmlInt32 * data_decoded, mmlIHTTPResponse ** response)
{
	std::list < mmlSharedPtr < mmlICStringInputStream > > lines;
	mmlChar * buffer = (mmlChar*)data;
	mmlInt32 begin = 0;
	mmlInt32 index = 0;
	mmlBool delimiter_found = mmlFalse;
	for ( index = 0 ; index < data_len; index ++ )
	{
		if ( index + 1 < data_len &&
			(( buffer[index] == '\n' && buffer[index + 1] == '\r' ) ||
			( buffer[index] == '\r' && buffer[index + 1] == '\n' )) )
		{
			if ( index - begin == 0 )
			{
				break;
			}

			mmlSharedPtr < mmlICStringInputStream > input_stream = mmlNewObject(MML_OBJECT_UUID(mmlICStringInputStream), 
				                                                                mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream) , mmlNewStaticBuffer(buffer + begin , index + 1 - begin)));
			lines.push_back(input_stream);
			begin = index + 2;
			index ++;

			if ( index + 2 <= data_len)
			{
				if ( buffer[index + 1] == '\r' &&
					buffer[index + 2] == '\n')
				{
					delimiter_found = mmlTrue;
					index += 3;
					break;
				}
			}
		}
		else if ( http_characters_safe_map[(mmlInt32)buffer[index]] == 0 )
		{
			return mmlFalse;
		}
	}
	if (delimiter_found == mmlFalse )
	{
		return mmlFalse;
	}
	if ( lines.size() > 0 )
	{
		*data_decoded = index;
		mmlInt32 skipped;
		mmlSharedPtr < mmlICStringInputStream > status_line = lines.front(); lines.pop_front();
		mmlSharedPtr < mmlICStringUtils > utils = mmlNewObject(MML_OBJECT_UUID(mmlICStringUtils));
		mmlInt32 status_code;
		mmlAutoPtr < mmlICString > status_str;
		mmlFloat64 version;
		mmlAutoPtr < mmlICString > protocol;
		mmlBool equal;
		if ( status_line->ReadUntil("/ \r\n" , NULL , NULL , NULL , protocol.getterAddRef()) == mmlFalse || protocol->CompareStr(MML_EQUAL , "HTTP", equal) == mmlFalse || equal == mmlFalse ) return mmlFalse;
		if ( status_line->Skip("/" , &skipped) == mmlFalse || skipped != 1) return mmlFalse;
		if ( status_line->ReadFloat(&version) == mmlFalse ) return mmlFalse;
		status_line->Skip(" " , NULL);
		if ( status_line->ReadInteger32(&status_code) == mmlFalse || status_code < 100 || status_code >= 600) return mmlFalse;
		if ( status_line->ReadUntil("\r\n" , " " , " " , NULL , status_str.getterAddRef()) == mmlFalse ) return mmlFalse;
		mmlAutoPtr < mmlHTTPResponse > rsp = new mmlHTTPResponse(status_code , status_str , version);
		for ( std::list < mmlSharedPtr < mmlICStringInputStream > > ::iterator line = lines.begin(); line != lines.end(); line ++ )
		{
			mmlAutoPtr < mmlICString > key;
			mmlAutoPtr < mmlICString > value;
			if ( (*line)->ReadToken(":" , mmlTrue , key.getterAddRef()) == mmlFalse || (*line)->Skip(":" , &skipped) == mmlFalse || skipped != 1) return mmlFalse;
			if ( (*line)->ReadUntil("\r\n" , " " , " " , NULL , value.getterAddRef()) == mmlFalse ) return mmlFalse;
			rsp->mHeaders.push_back(std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > > (key , value));
		}
		*response = rsp;
		MML_ADDREF(*response);
		return mmlTrue;
	}
	return mmlFalse;
}


void mmlHTTPResponse::Serialize ( mmlICStringOutputStream * stream )
{
	stream->WriteFormatted("HTTP/%.1f %d %s\r\n", mVersion , mCode, GetResponseStr());
	if ( mHeaders.size() > 0 )
	{
		for ( std::list <  std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > > >::iterator hdr = mHeaders.begin(); hdr != mHeaders.end(); hdr ++ )
		{
			stream->WriteFormatted("%s: %s\r\n" , (*hdr).first->Get() , (*hdr).second->Get());
		}
	}
}

void mmlHTTPResponse::SetResponseCode ( const mmlInt32 response_code )
{
	mCode = response_code;
}

mmlInt32 mmlHTTPResponse::GetResponseCode ()
{
	return mCode;
}

void mmlHTTPResponse::SetResponseStr ( const mmlChar * response_str )
{
	mStr = mmlNewCString(response_str);
}

const mmlChar * mmlHTTPResponse::GetResponseStr ()
{
	if ( mStr == mmlNULL )
	{
		return http_get_str(mCode);
	}
	return mStr->Get();
}

const mmlChar * mmlHTTPResponse::GetHeader ( const mmlChar * name )
{
	for ( std::list <  std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > > >::iterator hdr = mHeaders.begin(); hdr != mHeaders.end(); hdr ++ )
	{
		mmlBool equal;
		if ( hdr->first->CompareStr(MML_EQUAL , name, equal, MML_CASE_INSENSITIVE) == mmlTrue && equal == mmlTrue )
		{
			return hdr->second->Get();
		}
	}
	return mmlNULL;
}

void mmlHTTPResponse::SetHeader ( const mmlChar * name, const mmlChar * value )
{
	mHeaders.push_back(std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > >(mmlNewCString(name), mmlNewCString(value)));
}

void mmlHTTPResponse::SetVersion ( const mmlFloat64 version )
{
	mVersion = version;
}

mmlFloat64 mmlHTTPResponse::GetVersion ()
{
	return mVersion;
}



MML_OBJECT_IMPL2(mmlHTTPResponse,mmlHTTPResponse,mmlIHTTPResponse)
