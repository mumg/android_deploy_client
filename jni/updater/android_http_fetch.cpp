#include "mmlIHTTPClient.h"
#include "mmlIHTTPRequest.h"
#include "mmlIHTTPResponse.h"
#include "mml_components.h"
#include "mml_services.h"

void log_main( const mmlChar * formatter , ... );

static mmlInt32 GetConfigValue ( mmlIVariantStruct * config, const mmlChar * name, const mmlInt32 default_value )
{
	mmlAutoPtr < mmlIVariantInteger > value;
	if ( config->Get(name, mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant >(value)) == mmlFalse )
	{
		return default_value;
	}
	if ( value == mmlNULL )
	{
		return default_value;
	}
	return value->Get();
}


class android_http_fetch_processor : public mmlIHTTPClientProcessorPut
{
	MML_OBJECT_DECL
private:

	mmlInt32 mWriteTimeout;
	mmlInt32 mReadTimeout;
	mmlInt32 mResponseTimeout;
	mmlInt32 mOffset;

public:
	android_http_fetch_processor(const mmlInt32 offset,
		                         mmlIVariantStruct * config,
		                         mmlICString * input_content_type,
		                         mmlIInputStream * input_content,
		                         mmlICString ** output_content_type,
		                         mmlIOutputStream * output_content)
								 :mInputContentType(input_content_type), mInputContent(input_content),
								 mOutputContent(output_content), mOutputContentType(output_content_type),
								 mOffset(offset)
	{
		mWriteTimeout = GetConfigValue(config, "write", 60000);
		mReadTimeout = GetConfigValue(config, "read", 60000);
		mResponseTimeout = GetConfigValue(config, "response", 60000);
	}

	HTTP_PROCESS_RESULT_T OnPrepare ( const mmlChar * scheme, mmlIHTTPRequest * request , mmlBool * use_ssl )
	{
		if ( mmlStrCompare(scheme, "https", MML_CASE_INSENSITIVE) == 0 )
		{
			*use_ssl = mmlTrue;
		}
		if ( mInputContent != mmlNULL )
		{
			if ( mInputContentType != mmlNULL )
			{
				request->SetHeader("Content-type", mInputContentType->Get());
			}
			mmlInt64 size = mInputContent->Size();
			mmlChar content_size_str[64];
			mmlSprintf(content_size_str, sizeof(content_size_str), "%" MML_INT64, size);
			request->SetHeader("Content-Length" , content_size_str);
			request->SetMethod(METHOD_POST);
		}
		else
		{
			request->SetMethod(METHOD_GET);
			if ( mOffset > 0 )
			{
				mmlChar buff[64];
				mmlSprintf(buff, sizeof(buff), "bytes=%d-", mOffset);
				request->SetHeader("Range", buff);
			}
		}
		return HTTP_SUCCESS;
	}


	HTTP_PROCESS_RESULT_T OnReceive ( mmlIHTTPResponse * response,
		                mmlIInputStream * input,
		                mmlIOutputStream * output )
	{
		if ( mOffset != 0  )
		{
			if ( response->GetResponseCode() == HTTP_PARTIAL_CONTENT)
			{
				mmlAutoPtr < mmlIStreamControl > control;
				mOutputContent->GetControl(control.getterAddRef());
				if ( control == mmlNULL )
				{
					return HTTP_ERROR;
				}
				if ( control->Seek(MML_SEEK_SET, mOffset) == mmlFalse )
				{
					return HTTP_ERROR;
				}
			}
		}
		else if ( response->GetResponseCode() != HTTP_OK )
		{
			return HTTP_ERROR;
		}
		const mmlChar * content_type = response->GetHeader("Content-Type");
		const mmlChar * content_length = response->GetHeader("Content-Length");
		if ( content_type != mmlNULL && mOutputContentType != mmlNULL )
		{
			*mOutputContentType = mmlNewCString(content_type);
			MML_ADDREF(*mOutputContentType);
		}
		mmlInt64 size = 0;
		if ( content_length != mmlNULL )
		{
			size = mmlStoD(content_length, mmlNULL);
		}

		log_main("[http] response size %" MML_INT64 "\n", size);
		if (size == 0)
		{
			return HTTP_SUCCESS;
		}

		mmlSharedPtr < mmlIStreamControl > control;
		if ( input->GetControl(control.getterAddRef()) == mmlFalse )
		{
			return HTTP_ERROR;

		}
		control->SetTimeout(mReadTimeout);

		mmlSharedPtr < mmlIStreamCopy > copy = mmlNewObject(MML_OBJECT_UUID(mmlIStreamCopy));
		if ( copy == mmlNULL )
		{
			return HTTP_ERROR;
		}
		if ( copy->Copy(input, mOutputContent, size) == mmlFalse )
		{
			return HTTP_ABORTED;
		}
		return HTTP_SUCCESS;
	}

	HTTP_PROCESS_RESULT_T OnSend ( mmlIOutputStream * output )
	{
		if ( mInputContent != mmlNULL )
		{
			mmlSharedPtr < mmlIStreamControl > control;
			if ( output->GetControl(control.getterAddRef()) == mmlFalse )
			{
				return HTTP_ERROR;
			
			}
			control->SetTimeout(mWriteTimeout);
			mmlSharedPtr < mmlIStreamCopy > copy = mmlNewObject(MML_OBJECT_UUID(mmlIStreamCopy));
			if ( copy == mmlNULL )
			{
				return HTTP_ERROR;
			}
			if ( copy->Copy(mInputContent, output) == mmlFalse )
			{
				return HTTP_ABORTED;
			}
		}
		return HTTP_SUCCESS;
	}
private:

	mmlICString * mInputContentType;
	mmlIInputStream * mInputContent;
	mmlICString ** mOutputContentType;
	mmlIOutputStream * mOutputContent;
};

MML_OBJECT_IMPL2(android_http_fetch_processor, mmlIHTTPClientProcessorPut, mmlIHTTPClientProcessorGet)

HTTP_PROCESS_RESULT_T android_http_fetch (const mmlInt32 offset, mmlIVariantStruct * config,
	                         mmlICString * url,
                             mmlICString * input_content_type,
							 mmlIInputStream * input_content,
							 mmlICString ** output_content_type,
	                         mmlIOutputStream * output_content)
{
	mmlSharedPtr < mmlIHTTPClient > client = mmlNewObject(MML_OBJECT_UUID(mmlIHTTPClient));
	if ( client == mmlNULL )
	{
		return HTTP_ERROR;
	}
	mmlAutoPtr < android_http_fetch_processor > processor = new android_http_fetch_processor(offset, config, input_content_type, input_content, output_content_type, output_content);
	return client->Process(config, url->Get(), processor);
}
