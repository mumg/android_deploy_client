#ifndef MML_STREAM_CLIENT_HEADER_INCLUDED
#define MML_STREAM_CLIENT_HEADER_INCLUDED

#include "mmlIObject.h"
#include "mmlIStream.h"

#define MML_NETWORK_STREAM_CLIENT_UUID { 0xAE2918E9, 0x6BF7, 0x1014, 0x868C , { 0x8A, 0xC4, 0xD5, 0x4F, 0xF9, 0x65 } }



class mmlINetworkStreamClient : public mmlIStreamClient
{
	MML_OBJECT_UUID_DECL(MML_NETWORK_STREAM_CLIENT_UUID)
public:
	virtual NSC_RESULT_T Connect ( 
		mmlIVariantStruct * config,
		mmlIInputStream ** input,
		mmlIOutputStream ** output
							) = 0;

	virtual mmlBool GetHostName ( mmlICString ** hostname ) = 0;
};

#endif //MML_STREAM_CLIENT_HEADER_INCLUDED
