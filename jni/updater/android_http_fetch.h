#ifndef ANDROID_DOWNLOADER_HEADER_INCLUDED
#define ANDROID_DOWNLOADER_HEADER_INCLUDED

#include "mmlICString.h"
#include "mmlIStream.h"
#include "mmlIHTTPClient.h"

HTTP_PROCESS_RESULT_T android_http_fetch ( const mmlInt32 offset,
	                         mmlIVariantStruct * config,
	                         mmlICString * url,
	                         mmlICString * input_content_type,
	                         mmlIInputStream * input_content,
	                         mmlICString ** output_content_type,
	                         mmlIOutputStream * output_content);

#endif // ANDROID_DOWNLOADER_HEADER_INCLUDED
