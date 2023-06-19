#ifndef MML_NETWORK_SERVER_SSL_IMPL_HEADER_INCLUDED
#define MML_NETWORK_SERVER_SSL_IMPL_HEADER_INCLUDED

#include "mmlINetworkStreamServerSSL.h"
#include "mmlNetworkStreamServer.h"

class mmlNetworkStreamServerSSL : public mmlNetworkServerBase < mmlINetworkStreamServerSSL >
{
	MML_OBJECT_DECL
public:
	mmlNetworkStreamServerSSL();
	~mmlNetworkStreamServerSSL();
	mmlBool Start ( mmlIConfig * config, mmlIStreamServerHandler * handler );
private:
	mmlNetworkSocket * _Create (mmlInt32 sid, const mmlInt32 chunk_size, mmlIInputStream * output, mmlIOutputStream * input);
	SSL_CTX * mContext;
};


#endif //MML_NETWORK_SERVER_SSL_IMPL_HEADER_INCLUDED
