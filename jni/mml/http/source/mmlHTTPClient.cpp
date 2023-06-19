#include "mmlHTTPClient.h"
#include "mmlHTTPRequest.h"
#include "mmlHTTPResponse.h"
#include "mml_components.h"
#include "mml_services.h"
#include "mmlINetworkStreamClient.h"
#include "mmlINetworkStreamClientSSL.h"
#include "mmlIMemoryStream.h"
#include "mmlIStream.h"
#include "mmlIFileSystem.h"

#define MAX_HTTP_HEADERS_SIZE 32768
#define HTTP_CHUNK_SIZE 16384
#define HTTP_DEFAULT_IN_MEMORY_SIZE 1024*1024

void log_http(const mmlChar * formatter , ...);

HTTP_PROCESS_RESULT_T mmlHTTPClient::Process (
    mmlIVariantStruct * config,
    const mmlChar * url,
	mmlIHTTPClientProcessorGet * processor
							   )

{
	mmlSharedPtr < mmlICStringInputStream > url_stream = 
		mmlNewObject(MML_OBJECT_UUID(mmlICStringInputStream),
		mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), mmlNewStaticBuffer((void*)url, mmlStrLength(url)) ));

	mmlAutoPtr < mmlICString > scheme;

	if ( url_stream->ReadUntil(":", mmlNULL, mmlNULL, mmlNULL, scheme.getterAddRef()) == mmlFalse )
	{
		return HTTP_ERROR;
	}

	if ( url_stream->Skip(":", mmlNULL) == mmlFalse )
	{
		return HTTP_ERROR;
	}
	mmlInt32 count = 0;
	if ( url_stream->Skip("/", &count) == mmlFalse )
	{
		return HTTP_ERROR;
	}
	if ( count != 2 )
	{
		return HTTP_ERROR;
	}
	mmlAutoPtr < mmlICString > hostname;
	if ( url_stream->ReadUntil("/:", mmlNULL, mmlNULL, mmlNULL, hostname.getterAddRef()) == mmlFalse )
	{
		return HTTP_ERROR;
	}
	count = 0;
	url_stream->Skip(":", &count);
	mmlInt32 port = 0;
	if ( count == 1 )
	{
		mmlAutoPtr < mmlICString > port_str;
		if ( url_stream->ReadUntil("/", mmlNULL, mmlNULL, mmlNULL, port_str.getterAddRef() ) == mmlFalse )
		{
			return HTTP_ERROR;
		}
		port = mmlStoD(port_str->Get(), mmlNULL);
	}
	mmlSharedPtr < mmlIHTTPRequestUri > request_uri = mmlNewObject(MML_OBJECT_UUID(mmlIHTTPRequestUri));
	if ( request_uri == mmlNULL )
	{
		return HTTP_ERROR;
	}
	if ( request_uri->Load(url_stream) == mmlFalse )
	{
		return HTTP_ERROR;
	}
	mmlSharedPtr < mmlIHTTPRequest > request = mmlNewObject(MML_OBJECT_UUID(mmlIHTTPRequest));
	if ( request == mmlNULL )
	{
		return HTTP_ERROR;
	}
	request->SetReqURI(request_uri);
	request->SetHeader("Host", hostname->Get());

	mmlBool use_ssl = mmlFalse;
	HTTP_PROCESS_RESULT_T res;
	if ((res = processor->OnPrepare(scheme->Get(), request, &use_ssl )) != HTTP_SUCCESS)
	{
		return res;
	}
	if ( use_ssl == mmlFalse )
	{
		if ( port == 0 )
		{
			port = 80;
		}
	}
	else
	{
		if ( port == 0 )
		{
			port = 443;
		}
	}


	mmlAutoPtr < mmlIInputStream > http_input_stream;

	mmlAutoPtr < mmlIOutputStream > http_output_stream;

	mmlSharedPtr < mmlINetworkStreamClient > tcp_stream_client;

	if ( use_ssl == mmlTrue )
	{
		tcp_stream_client = mmlNewObject(MML_OBJECT_UUID(mmlINetworkStreamClientSSL));
	}
	else
	{
		tcp_stream_client = mmlNewObject(MML_OBJECT_UUID(mmlINetworkStreamClient));
	}
	if (tcp_stream_client == mmlNULL)
	{
		return HTTP_ERROR;
	}

	mmlAutoPtr < mmlIVariantStruct > tcp_config = mmlNewVariantStruct(config);
	tcp_config->Set("host", mmlNewVariantString(hostname));
	tcp_config->Set("port", mmlNewVariantInteger(port));
	NSC_RESULT_T connect_res;
	if ( (connect_res = tcp_stream_client->Connect(tcp_config, http_input_stream.getterAddRef(), http_output_stream.getterAddRef())) != NSC_OK )
	{
		if ( connect_res == NSC_REFUSED )
		{
			return HTTP_ABORTED;
		}
		return HTTP_ERROR;
	}

	mmlSharedPtr < mmlIInputStream > http_buffered_input_stream = mmlNewObject(MML_OBJECT_UUID(mmlIBufferedInputStream), http_input_stream);
	mmlSharedPtr < mmlIOutputStream > http_buffered_output_stream = mmlNewObject(MML_OBJECT_UUID(mmlIBufferedOutputStream), http_output_stream);

	mmlSharedPtr < mmlHTTPRequest > http_request = request;
	if ( http_request == mmlNULL )
	{
		return HTTP_ERROR;
	}
	mmlSharedPtr < mmlICStringOutputStream > cstring_stream = mmlNewObject(MML_OBJECT_UUID(mmlICStringOutputStream), http_buffered_output_stream);

	if ( http_request->Serialize(cstring_stream) == mmlFalse )
	{
		return HTTP_ERROR;
	}

	if ( cstring_stream->WriteStr("\r\n") != mmlTrue )
	{
		return HTTP_ERROR;
	}
	if ( http_buffered_output_stream->Flush() == mmlFalse )
	{
		return HTTP_ERROR;
	}
	{
		mmlSharedPtr < mmlIHTTPClientProcessorPut > put_processor = processor;
		if ( put_processor != mmlNULL )
		{
			HTTP_PROCESS_RESULT_T res;
			if ( (res = put_processor->OnSend(http_output_stream)) != HTTP_SUCCESS )
			{
				return res;
			}
		}
	}
	if ( http_buffered_output_stream->Flush() == mmlFalse )
	{
		return HTTP_ABORTED;
	}

	mmlChar headers_buffer[MAX_HTTP_HEADERS_SIZE];
	mmlInt32 headers_sz_left = MAX_HTTP_HEADERS_SIZE;
	mmlInt32 of = 0;
	mmlInt32 header_len = 0;


	mmlAutoPtr < mmlIVariantInteger > response_timeout;
	config ->Get("response", mmlRelativePtrAddRef < mmlIVariantInteger , mmlIVariant >(response_timeout));

	mmlAutoPtr < mmlIStreamControl > ctrl;
	if (http_buffered_input_stream->GetControl(ctrl.getterAddRef()) == mmlTrue )
	{
		if ( ctrl != mmlNULL )
		{
			ctrl->SetTimeout(response_timeout == mmlNULL ? 0 : response_timeout->Get());
		}
	}
	mmlChar queue[4] = {0};
	while (headers_sz_left > 0 )
	{
		if (queue[0] != 0 )
		{
			headers_buffer[header_len] = queue[0];
			header_len++;
		}
		if ( header_len == MAX_HTTP_HEADERS_SIZE )
		{
			return HTTP_ERROR;
		}
		queue[0] = queue[1];
		queue[1] = queue[2];
		queue[2] = queue[3];
		mmlInt32 rd = (mmlInt32)http_buffered_input_stream->Read(1 , &queue[3]);
		if ( rd <= 0 )
		{
			return HTTP_ABORTED;
		}
		if ( mmlMemoryCmp(queue, "\r\n\r\n", 4) == mmlTrue ||
			 mmlMemoryCmp(queue, "\n\r\n\r", 4) == mmlTrue )
		{
			mmlMemoryCopy(headers_buffer + header_len , "\r\n\r\n", 4);
			header_len +=4;
			break;
		}
	}
	if ( header_len == MAX_HTTP_HEADERS_SIZE )
	{
		return HTTP_ERROR;
	}
	headers_buffer[header_len] = 0;
	if ( header_len == 0 )
	{
		return HTTP_ERROR;
	}
	mmlInt32 decoded = 0;
	mmlAutoPtr < mmlIHTTPResponse > http_response;
	if ( mmlHTTPResponse::Load(headers_buffer, header_len, &decoded, http_response.getterAddRef()) == mmlFalse )
	{
		return HTTP_ERROR;
	}
	if ( decoded != header_len )
	{
		return HTTP_ERROR;
	}
	return processor->OnReceive(http_response, http_buffered_input_stream, http_buffered_output_stream);
}


MML_OBJECT_IMPL1(mmlHTTPClient ,mmlIHTTPClient)
