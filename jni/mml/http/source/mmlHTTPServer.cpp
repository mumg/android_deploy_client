#include "mmlHTTPServer.h"
#include "mmlHTTPResponse.h"
#include "mmlHTTPRequest.h"
#include "mmlILogger.h"
#include "mml_components.h"
#include "mml_services.h"
#include "mmlIFileSystem.h"
#include "mmlIStateMachine.h"
#include "mmlINetworkStreamServer.h"
#include "mmlINetworkStreamServerSSL.h"

#define HTTP_MAX_HEADER_SIZE 32768
#define HTTP_CHUNK_SIZE 16384
#define HTTP_MAX_IN_MEMORY_DEFAULT 1024*1024

enum
{
	HTTP_INPUT_HEADERS,
	HTTP_INPUT_PROCESS,
	HTTP_INPUT_CONTENT,
	HTTP_OUTPUT_HEADERS,
	HTTP_OUTPUT_CONTENT,
	HTTP_CLOSE
};

enum
{
	HTTP_WRITE,
	HTTP_READ,
	HTTP_RESPONSE,
	HTTP_IS_ACTIVE,
	HTTP_DISCONNECTED
};

class mmlHTTPStreamContext : public mmlIObject
{
	MML_OBJECT_DECL
public:

	mmlBool mKeepAlive;
	mmlAutoPtr < mmlIHTTPServerContentHandler > mContentHandler;
	mmlAutoPtr < mmlIHTTPServerHandler > mRequestHandler;
	mmlInt32 mResponseCode;
	mmlAutoPtr < mmlIHTTPResponse > mResponse;
	mmlInt64 mContentSize;
	mmlAutoPtr < mmlIHTTPRequest > mRequest;

	mmlSharedPtr < mmlIInputStream > mContentInput;
	mmlSharedPtr < mmlIOutputStream > mContentOutput;

	mmlChar mHeaders[HTTP_MAX_HEADER_SIZE];
	mmlInt32 mHeadersOffset;
	mmlInt32 mHeadersLength;
	mmlAutoPtr < mmlICString > mSource;

	mmlHTTPStreamContext(mmlIHTTPServerHandler * request_handler, mmlIHTTPServerContentHandler * content_handler, mmlICString * source)
		:mHeadersOffset(0), mKeepAlive(mmlFalse), mRequestHandler(request_handler), mContentHandler(content_handler), mResponseCode(HTTP_OK), mContentSize(0), mSource(source)
	{

	}

};

MML_OBJECT_IMPL0(mmlHTTPStreamContext)

class mmlHTTPServerContext : public mmlIHTTPServerContext
{
	MML_OBJECT_DECL
public:

	mmlHTTPServerContext( mmlIStateMachine * http_sm , mmlICString * source)
		:mSM(http_sm), mSource(source)
	{

	}
	
	mmlBool Response (mmlIHTTPResponse * response, mmlIInputStream * content)
	{
		mmlSharedPtr < mmlIList > lst = mmlNewObject(MML_OBJECT_UUID(mmlIList));
		lst->PushBack(response);
		lst->PushBack(content);
		return mSM->Process(HTTP_RESPONSE, lst, mmlNULL, 0 , mmlNULL, mmlNULL);
	}

	mmlBool Response ( const mmlInt32 error )
	{
		mmlSharedPtr < mmlIList > lst = mmlNewObject(MML_OBJECT_UUID(mmlIList));
		mmlAutoPtr < mmlHTTPResponse > rsp = new mmlHTTPResponse();
		rsp->SetResponseCode(error);
		lst->PushBack(rsp);
		lst->PushBack(mmlNULL);
		return mSM->Process(HTTP_RESPONSE, lst, mmlNULL, 0 , mmlNULL, mmlNULL);
	}

	mmlBool IsActive ()
	{
		mmlBool is_active = mmlFalse;
		mSM->Process(HTTP_IS_ACTIVE, mmlNULL, &is_active, 0 , mmlNULL, mmlNULL);
		return is_active;
	}

	mmlBool GetSource ( mmlICString ** source )
	{
		*source = mSource;
		MML_ADDREF(*source);
		return mmlTrue;
	}

private:
	mmlAutoPtr < mmlICString > mSource;
	mmlAutoPtr < mmlIStateMachine > mSM;
};

MML_OBJECT_IMPL1(mmlHTTPServerContext, mmlIHTTPServerContext)

MML_SM_STATE_ENTER_FUNC(http_write_header_enter)
{
	mmlHTTPStreamContext * ctx = (mmlHTTPStreamContext*) context;
	ctx->mRequest = mmlNULL;
	ctx->mContentInput = mmlNULL;
	ctx->mContentOutput = mmlNULL;
	ctx->mResponse = mmlNULL;
	ctx->mResponseCode = HTTP_OK;
	ctx->mHeadersLength = 0;
	ctx->mHeadersOffset = 0;
	return mmlTrue;
}

MML_SM_STATE_EVENT_FUNC(http_write_header)
{
	mmlHTTPStreamContext * ctx = (mmlHTTPStreamContext*) context;


	mmlInt32 sz = data_size > HTTP_MAX_HEADER_SIZE - ctx->mHeadersOffset ? HTTP_MAX_HEADER_SIZE - ctx->mHeadersOffset : data_size;
	mmlInt32 of = ctx->mHeadersOffset;

	mmlChar * headers = ctx->mHeaders;

	mmlMemoryCopy(headers + ctx->mHeadersOffset, data, sz);
	ctx->mHeadersOffset += sz;
	mmlInt32 header_len = 0;
	for ( mmlInt32 index = 0 ; ctx->mHeadersOffset > 4 && index <= (ctx->mHeadersOffset - 4) ; index ++ )
	{
		if ( (headers[index + 0] == '\r' &&
			  headers[index + 1] == '\n' &&
			  headers[index + 2] == '\r' &&
			  headers[index + 3] == '\n') ||
			 (headers[index + 0] == '\n' &&
			  headers[index + 1] == '\r' &&
			  headers[index + 2] == '\n' &&
			  headers[index + 3] == '\r') )
		{
			header_len = index + 4;
			break;
		}
	}
	if ( ctx->mHeadersOffset  == HTTP_MAX_HEADER_SIZE && header_len == 0 )
	{
		*next_state = HTTP_CLOSE;
	}
	else
	{
		if ( header_len > 0 )
		{
			mmlInt32 decoded;
			if ( mmlHTTPRequest::Load(headers, header_len, &decoded, ctx->mRequest.getterAddRef()) != mmlTrue )
			{
				*next_state = HTTP_CLOSE;
			}
			else
			{
				*data_processed = (data_size - sz) + header_len - of;
			}
		}
		else
		{
			*data_processed = data_size;
		}
	}
	if ( ctx->mRequest != mmlNULL )
	{
		const mmlChar * content_length_str = ctx->mRequest->GetHeader("Content-Length");
		mmlInt64 content_length = content_length_str != mmlNULL ? mmlStoD(content_length_str, mmlNULL) : 0;
		if ( content_length > 0 )
		{
			if ( ctx->mContentHandler != mmlNULL )
			{
				if ( ctx->mContentHandler->PrepareContent(ctx->mRequest, ctx->mContentOutput.getterAddRef(), ctx->mContentInput.getterAddRef())  == mmlFalse )
				{
					ctx->mResponseCode = HTTP_NOT_ACCEPTABLE;
					*next_state = HTTP_OUTPUT_HEADERS;
				}
				else
				{
					ctx->mContentSize = content_length;
					*next_state = HTTP_INPUT_CONTENT;
				}
			}
			else
			{
				ctx->mContentOutput = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));
				ctx->mContentSize = content_length;
				*next_state = HTTP_INPUT_CONTENT;
			}

		}
		else
		{
			*next_state = HTTP_INPUT_PROCESS;
		}
	}
	return mmlTrue;
}

MML_SM_STATE_EVENT_FUNC(http_write_content)
{
	mmlHTTPStreamContext * ctx = (mmlHTTPStreamContext*) context;
	mmlInt32 sz = data_size > ctx->mContentSize  ? ctx->mContentSize : data_size;
	if ( ctx->mContentOutput->Write(sz, data) != sz )
	{
		*next_state = HTTP_CLOSE;
	}
	else
	{
		*data_processed = sz;
		ctx->mContentSize -= sz;
		if ( ctx->mContentSize == 0 )
		{
			if ( ctx->mContentOutput->Close() == mmlFalse )
			{
				*next_state = HTTP_CLOSE;
			}
			else
			{
				ctx->mContentInput = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), ctx->mContentOutput);
				ctx->mContentOutput = mmlNULL;
				*next_state = HTTP_INPUT_PROCESS;
			}
		}
	}
	return mmlTrue;
}

MML_SM_STATE_EVENT_FUNC(http_read_empty)
{
	*data_processed = 0;
	return mmlTrue;
}

MML_SM_STATE_EVENT_FUNC(http_write_none)
{
	if ( data_size > 0 )
	{
		*next_state = HTTP_CLOSE;
	}
	return mmlTrue;
}

MML_SM_STATE_ENTER_FUNC(http_process_enter)
{
	mmlHTTPStreamContext * ctx = (mmlHTTPStreamContext*) context;
	mmlAutoPtr < mmlHTTPServerContext > server_ctx = new mmlHTTPServerContext(sm, ctx->mSource);
	mmlAutoPtr < mmlIHTTPRequest > rq = ctx->mRequest;
	ctx->mRequest = mmlNULL;
	mmlSharedPtr < mmlIInputStream > cnt = ctx->mContentInput;
	ctx->mContentInput = mmlNULL;
	if ( ctx->mRequestHandler->Process(server_ctx, rq , cnt) == mmlFalse )
	{
		*next_state = HTTP_OUTPUT_HEADERS;
	}
	return mmlTrue;
}

MML_SM_STATE_EVENT_FUNC(http_process_response)
{
	mmlHTTPStreamContext * ctx = (mmlHTTPStreamContext*) context;
	mmlSharedPtr < mmlIList > lst = arg;
	if ( lst == mmlNULL )
	{
		ctx->mResponseCode = HTTP_INTERNAL_SERVER_ERROR;
		*next_state = HTTP_OUTPUT_HEADERS;
	}
	else if ( lst->Length() != 2)
	{
		ctx->mResponseCode = HTTP_INTERNAL_SERVER_ERROR;
		*next_state = HTTP_OUTPUT_HEADERS;
	}
	else
	{
		lst->Front(ctx->mResponse.getterAddRef());
		lst->PopFront();
		lst->Front(ctx->mContentInput.getterAddRef());
		lst->PopFront();
		if ( ctx->mResponse == mmlNULL )
		{
			ctx->mResponseCode = HTTP_INTERNAL_SERVER_ERROR;
		}
		*next_state = HTTP_OUTPUT_HEADERS;
	}
	return mmlTrue;
}


MML_SM_STATE_ENTER_FUNC(http_prepare_header)
{
	mmlHTTPStreamContext * ctx = (mmlHTTPStreamContext*) context;
	if ( ctx->mResponse == mmlNULL )
	{
		ctx->mResponse = new mmlHTTPResponse();
		ctx->mResponse->SetResponseCode(ctx->mResponseCode);
	}
	mmlSharedPtr < mmlHTTPResponse > int_response = (mmlIHTTPResponse*)ctx->mResponse;
	mmlSharedPtr < mmlIMemoryOutputStream > memory_stream =  mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream), mmlNewStaticBuffer(ctx->mHeaders, sizeof(ctx->mHeaders)));
	mmlSharedPtr < mmlICStringOutputStream > response_stream = mmlNewObject(MML_OBJECT_UUID(mmlICStringOutputStream),memory_stream);
	if ( response_stream == mmlNULL )
	{
		*next_state = HTTP_CLOSE;
	}
	else if ( int_response == mmlNULL )
	{
		*next_state = HTTP_CLOSE;
	}
	else
	{
		int_response->Serialize(response_stream);
		if ( ctx->mContentInput != mmlNULL )
		{
			if ( response_stream->WriteFormatted("Content-Length: %" MML_INT64 "\r\n", ctx->mContentInput->Size()) != mmlTrue )
			{
				return mmlFalse;
			}
		}
		else
		{
			if ( response_stream->WriteStr("Content-Length: 0\r\n") != mmlTrue )
			{
				return mmlFalse;
			}
		}
		if ( response_stream->WriteStr("\r\n") != mmlTrue )
		{
			return mmlFalse;
		}
		memory_stream->Close();
		ctx->mHeadersLength = memory_stream->Size();
		ctx->mHeadersOffset = 0;
	}
	return mmlTrue;
}


MML_SM_STATE_EVENT_FUNC(http_read_header)
{
	*data_processed = 0;
	mmlHTTPStreamContext * ctx = (mmlHTTPStreamContext*) context;
	if ( ctx->mHeadersOffset == ctx->mHeadersLength )
	{
		if ( ctx->mContentInput == mmlNULL )
		{
			if ( ctx->mKeepAlive == mmlFalse )
			{
				*next_state = HTTP_CLOSE;
			}
			else
			{
				*next_state = HTTP_INPUT_HEADERS;
			}
		}
		else
		{
			*next_state = HTTP_OUTPUT_CONTENT;
		}
	}
	else
	{
		mmlInt32 sz = ctx->mHeadersLength - ctx->mHeadersOffset > data_size ? data_size : ctx->mHeadersLength - ctx->mHeadersOffset;
		mmlMemoryCopy(data, ctx->mHeaders + ctx->mHeadersOffset, sz);
		*data_processed = sz;
		ctx->mHeadersOffset += sz;
	}
	return mmlTrue;
}


MML_SM_STATE_EVENT_FUNC(http_read_content)
{
	mmlHTTPStreamContext * ctx = (mmlHTTPStreamContext*) context;
	if ( ctx->mContentInput->IsEmpty() == mmlTrue )
	{
		if ( ctx->mKeepAlive == mmlFalse )
		{
			*next_state = HTTP_CLOSE;
		}
		else
		{
			*next_state = HTTP_INPUT_HEADERS;
		}
	}
	else
	{
		*data_processed = ctx->mContentInput->Read(data_size, data);
	}
	return mmlTrue;	
}



MML_SM_STATE_EVENT_FUNC(http_is_active)
{
	mmlBool * is_active = (mmlBool*)data;
	*is_active = mmlTrue;
	return mmlTrue;
}

MML_SM_STATE_EVENT_FUNC(http_is_inactive)
{
	mmlBool * is_active = (mmlBool*)data;
	*is_active = mmlFalse;
	return mmlTrue;
}

MML_SM_STATE_EVENT_FUNC(http_disconnected)
{
	*next_state = HTTP_CLOSE;
	return mmlTrue;
}

MML_SM_MAP_BEGIN(http_stream)
	MML_SM_BEGIN_STATE(HTTP_INPUT_HEADERS)
		MML_SM_STATE_ENTER(http_write_header_enter)
		MML_SM_STATE_EVENT(HTTP_WRITE, http_write_header)
		MML_SM_STATE_EVENT(HTTP_READ, http_read_empty)
		MML_SM_STATE_EVENT(HTTP_IS_ACTIVE, http_is_active)
		MML_SM_STATE_EVENT(HTTP_DISCONNECTED, http_disconnected)
	MML_SM_END_STATE()
	MML_SM_BEGIN_STATE(HTTP_INPUT_CONTENT)
		MML_SM_STATE_EVENT(HTTP_WRITE, http_write_content)
		MML_SM_STATE_EVENT(HTTP_READ, http_read_empty)
		MML_SM_STATE_EVENT(HTTP_IS_ACTIVE, http_is_active)
		MML_SM_STATE_EVENT(HTTP_DISCONNECTED, http_disconnected)
	MML_SM_END_STATE()
	MML_SM_BEGIN_STATE(HTTP_INPUT_PROCESS)
		MML_SM_STATE_ENTER(http_process_enter)
		MML_SM_STATE_EVENT(HTTP_WRITE, http_write_none)
		MML_SM_STATE_EVENT(HTTP_READ, http_read_empty)
		MML_SM_STATE_EVENT(HTTP_RESPONSE, http_process_response)
		MML_SM_STATE_EVENT(HTTP_IS_ACTIVE, http_is_active)
		MML_SM_STATE_EVENT(HTTP_DISCONNECTED, http_disconnected)
	MML_SM_END_STATE()
	MML_SM_BEGIN_STATE(HTTP_OUTPUT_HEADERS)
		MML_SM_STATE_ENTER(http_prepare_header)
		MML_SM_STATE_EVENT(HTTP_WRITE, http_write_none)
		MML_SM_STATE_EVENT(HTTP_READ, http_read_header)
		MML_SM_STATE_EVENT(HTTP_IS_ACTIVE, http_is_active)
		MML_SM_STATE_EVENT(HTTP_DISCONNECTED, http_disconnected)
	MML_SM_END_STATE()
	MML_SM_BEGIN_STATE(HTTP_OUTPUT_CONTENT)
		MML_SM_STATE_EVENT(HTTP_WRITE, http_write_none)
		MML_SM_STATE_EVENT(HTTP_READ, http_read_content)
		MML_SM_STATE_EVENT(HTTP_IS_ACTIVE, http_is_active)
		MML_SM_STATE_EVENT(HTTP_DISCONNECTED, http_disconnected)
	MML_SM_END_STATE()
	MML_SM_BEGIN_STATE(HTTP_CLOSE)
		MML_SM_STATE_EVENT(HTTP_IS_ACTIVE, http_is_inactive)
	MML_SM_END_STATE()
MML_SM_MAP_END(http_stream)

class mmlHTTPInputStream : public mmlIOutputStream
{
	MML_OBJECT_DECL
public:
	mmlHTTPInputStream(mmlIStateMachine * sm)
		:mClosed(mmlFalse), mSM(sm)
	{
		
	}

	mmlInt64 Write ( const mmlInt64 size , const void * data )
	{
		const mmlUInt8 * ptr = (const mmlUInt8*)data;
		mmlInt64 sz = size;
		while ( sz > 0 )
		{
			mmlInt32 processed = 0;
			mmlUInt32 current_state;
			if ( mSM->Process(0, mmlNULL, (void*)ptr, sz, &processed, &current_state ) == mmlFalse )
			{
				mClosed = mmlTrue;
				return -1;
			}
			if ( current_state == HTTP_CLOSE )
			{
				mClosed = mmlTrue;
				return -1;
			}
			if ( processed == 0 )
			{
				mClosed = mmlTrue;
				return -1;
			}
			ptr += processed;
			sz -= processed;
		}
		return size;
	}

	mmlBool IsFull () { return mClosed; }

	mmlBool GetControl ( mmlIStreamControl ** control ) { return mmlFalse; }

	mmlBool Close ()
	{ 
		mSM->Process(HTTP_DISCONNECTED, mmlNULL, mmlNULL, 0, mmlNULL, mmlNULL);
		return mmlTrue; 
	}

	mmlInt64 Size () { return 0; }

	mmlBool Flush () { return mmlTrue; }

private:
	mmlBool mClosed;
	mmlAutoPtr < mmlIStateMachine > mSM;

};

MML_OBJECT_IMPL1(mmlHTTPInputStream, mmlIOutputStream)


class mmlHTTPOutputStream : public mmlIInputStream
{
	MML_OBJECT_DECL
public:

	mmlHTTPOutputStream(mmlIStateMachine * sm)
		:mClosed(mmlFalse), mSM(sm)
	{

	}

	mmlBool GetControl ( mmlIStreamControl ** control )
	{
		return mmlFalse;
	}

	mmlInt64 Read ( const mmlInt64 size , void * data )
	{
		mmlUInt32 current_state;
		mmlInt32 processed;
		if ( mSM->Process(HTTP_READ, mmlNULL, data, size, &processed, &current_state) == mmlFalse )
		{
			mClosed = mmlTrue;
			return -1;
		}
		if ( current_state == HTTP_CLOSE )
		{
			return -1;
		}
		return processed;
	}

	mmlBool IsEmpty () { return mClosed; }

	mmlInt64 Size ()
	{
		return 0;
	}

	mmlBool Close ()
	{
		return mmlTrue;
	}

private:
	mmlBool mClosed;
	mmlAutoPtr < mmlIStateMachine > mSM;
};


MML_OBJECT_IMPL1(mmlHTTPOutputStream, mmlIInputStream)

class mmlHTTPStreamServerHandler : public mmlIStreamServerHandler
{
	MML_OBJECT_DECL
public:
	mmlHTTPStreamServerHandler(mmlIHTTPServerHandler * server_handler, mmlIHTTPServerContentHandler * content_handler)
		:mServerhandler(server_handler), mContentHandler(content_handler)
	{

	}

	mmlBool OnConnect ( mmlICString * iface, mmlICString * source, mmlInt32 * chunk_size, mmlIOutputStream ** input, mmlIInputStream ** output)
	{
		*chunk_size = HTTP_CHUNK_SIZE;
		mmlAutoPtr < mmlHTTPStreamContext > http_context = new mmlHTTPStreamContext(mServerhandler, mContentHandler, source);
		mmlSharedPtr < mmlIStateMachine > http_sm = mmlNewObject(MML_OBJECT_UUID(mmlIStateMachine));
		if ( http_sm == mmlNULL )
		{
			return mmlFalse;
		}
		if ( http_sm->Init(HTTP_INPUT_HEADERS, http_context, MML_SM_MAP(http_stream)) == mmlFalse )
		{
			return mmlFalse;
		}
		*input = new mmlHTTPInputStream(http_sm);
		MML_ADDREF(*input);
		*output = new mmlHTTPOutputStream(http_sm);
		MML_ADDREF(*output);
		return mmlTrue;
	}

private:
	mmlAutoPtr < mmlIHTTPServerHandler > mServerhandler;
	mmlAutoPtr < mmlIHTTPServerContentHandler > mContentHandler;
};

MML_OBJECT_IMPL0(mmlHTTPStreamServerHandler)

mmlHTTPServer::mmlHTTPServer()
{

}

mmlBool mmlHTTPServer::Start (mmlIVariantStruct * config , mmlIHTTPServerHandler * server_handler, mmlIHTTPServerContentHandler * content_handler)
{
	mmlAutoPtr < mmlIVariantInteger > ssl;
	config->Get("ssl", mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant >(ssl));

	

	if ( ssl == mmlNULL ||  ssl->Get() == 0 )
	{
		mServer = mmlNewObject(MML_OBJECT_UUID(mmlINetworkStreamServer));
	}
	else
	{
		mServer = mmlNewObject(MML_OBJECT_UUID(mmlINetworkStreamServerSSL));
	}
	mmlAutoPtr < mmlHTTPStreamServerHandler > http_handler = new mmlHTTPStreamServerHandler(server_handler, content_handler);
	return mServer->Start(config, http_handler);
}

void mmlHTTPServer::Stop ()
{
	if ( mServer != mmlNULL )
	{
		mServer->Stop();
	}
}

MML_OBJECT_IMPL1(mmlHTTPServer, mmlIHTTPServer)