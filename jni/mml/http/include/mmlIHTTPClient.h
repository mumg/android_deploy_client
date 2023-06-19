#ifndef MML_HTTP_CLIENT_HEADER_INCLUDED
#define MML_HTTP_CLIENT_HEADER_INCLUDED

#include "mmlIObject.h"
#include "mmlICString.h"
#include "mmlIHTTPRequest.h"
#include "mmlIHTTPResponse.h"
#include "mmlIStream.h"
#include "mml_time.h"
#include "mmlIVariant.h"

typedef enum
{
	HTTP_SUCCESS,
	HTTP_ABORTED,
	HTTP_ERROR
}HTTP_PROCESS_RESULT_T;

#define MML_HTTP_CLIENT_PROCESSOR_GET_UUID { 0xD5165890, 0x6BF4, 0x1014, 0xAF50 , { 0xC5, 0xF3, 0xED, 0xD4, 0x70, 0xF7 } }

class mmlIHTTPClientProcessorGet : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_HTTP_CLIENT_PROCESSOR_GET_UUID)
public:
	virtual HTTP_PROCESS_RESULT_T OnPrepare ( const mmlChar * scheme, mmlIHTTPRequest * request , mmlBool * use_ssl ) = 0;


	virtual HTTP_PROCESS_RESULT_T OnReceive ( mmlIHTTPResponse * headers,
		                        mmlIInputStream * input,
							    mmlIOutputStream * output ) = 0;

};

#define MML_HTTP_CLIENT_PROCESSOR_PUT_UUID { 0xD5165890, 0x6BF4, 0x1014, 0xAF50 , { 0xC5, 0xF3, 0xED, 0xD4, 0x70, 0xF8 } }


class mmlIHTTPClientProcessorPut : public mmlIHTTPClientProcessorGet
{
	MML_OBJECT_UUID_DECL(MML_HTTP_CLIENT_PROCESSOR_PUT_UUID)
public:
	virtual HTTP_PROCESS_RESULT_T OnSend ( mmlIOutputStream * output ) = 0;

};

#define MML_HTTP_CLIENT_UUID { 0xD5165890, 0x6BF4, 0x1014, 0xAF50 , { 0xC5, 0xF3, 0xED, 0xD4, 0x70, 0xF6 } }

class mmlIHTTPClient : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_HTTP_CLIENT_UUID)
public:

	virtual HTTP_PROCESS_RESULT_T Process ( 
		mmlIVariantStruct * config,
		const mmlChar * url,
		mmlIHTTPClientProcessorGet * processor
						    ) = 0;
};


#endif //MML_HTTP_CLIENT_HEADER_INCLUDED
