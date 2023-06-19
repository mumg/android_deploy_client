#ifndef MML_HTTP_CLIENT_IMPL_HEADER_INCLUDED
#define MML_HTTP_CLIENT_IMPL_HEADER_INCLUDED

#include "mmlIHTTPClient.h"

class mmlHTTPClient : public mmlIHTTPClient
{
	MML_OBJECT_DECL
public:
	HTTP_PROCESS_RESULT_T Process ( 
					  mmlIVariantStruct * config,
		              const mmlChar * url,
		              mmlIHTTPClientProcessorGet * processor
					);
};


#endif //MML_HTTP_CLIENT_IMPL_HEADER_INCLUDED
