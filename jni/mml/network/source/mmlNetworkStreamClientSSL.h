#ifndef MML_NETWORK_CLIENT_SSL_IMPL_HEADER_INCLUDED
#define MML_NETWORK_CLIENT_SSL_IMPL_HEADER_INCLUDED

#include "mmlINetworkStreamClientSSL.h"
#include "mmlNetworkStreamClient.h"
#include "mmlIMutex.h"

#include <mbedtls/ssl.h>
#include <mbedtls/certs.h>
#include <mbedtls/x509.h>

#include <mbedtls/error.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/sha256.h>
#include <mbedtls/net_sockets.h>


class mmlNetworkClientSocketSSLContext : public mmlIObject
{
	MML_OBJECT_DECL
public:


	mmlNetworkClientSocketSSLContext();

	~mmlNetworkClientSocketSSLContext();

	mbedtls_entropy_context * GetEntropy()
	{
		return &mEntropy;
	}

	mbedtls_ssl_context * GetSSL()
	{
		return &mSSL;
	}

	mbedtls_ssl_config * GetConfig ()
	{
		return &mConf;
	}

	mbedtls_x509_crt * GetCaCert ()
	{
		return &mCaCert;
	}

	mbedtls_x509_crt * GetClientCert ()
	{
		return &mClientCert;
	}

	mbedtls_pk_context * GetClientKey()
	{
		return &mClientKey;
	}

	mbedtls_net_context * GetFD()
	{
		return &mFD;
	}

	mbedtls_ctr_drbg_context * GetDrbgCtx()
	{
		return &mDrbgCtx;
	}

private:

	mbedtls_entropy_context mEntropy;
	mbedtls_ssl_context mSSL;
	mbedtls_ssl_config mConf;

	mbedtls_x509_crt mCaCert;
	mbedtls_x509_crt mClientCert;
	mbedtls_pk_context mClientKey;
	mbedtls_net_context mFD;
	mbedtls_ctr_drbg_context mDrbgCtx;
};

class mmlNetworkClientSocketSSL : public mmlINetworkClientSocket
{
	MML_OBJECT_DECL;
public:

	mmlNetworkClientSocketSSL();

	~mmlNetworkClientSocketSSL();

	NSC_RESULT_T Connect (mmlIVariantStruct * config );

	mmlBool Close ();
	
	mmlInt32 Read (const mmlInt32 timeout, const mmlInt32 size, void * data );

	mmlInt32 Write ( const mmlInt32 timeout, const mmlInt32 size, void * data );

	mmlBool IsConnected () { return mContext != mmlNULL ? mmlTrue : mmlFalse; }

private:

	mmlBool _ConfigureDefault ();

	mmlBool _ConfigureSSL_CERT(const mmlChar * ca, const mmlChar * key);

	mmlBool mInSecure;

	mmlAutoPtr < mmlNetworkClientSocketSSLContext > mContext;

	mmlSharedPtr < mmlIMutex > mMutex;

	mmlBool mClientCert;

};


class mmlNetworkStreamClientSSL : public mmlINetworkStreamClientSSL
{
	MML_OBJECT_DECL
public:

	mmlNetworkStreamClientSSL()
	{
	}

	~mmlNetworkStreamClientSSL()
	{
	}

	virtual NSC_RESULT_T Connect 
		( 
		mmlIVariantStruct * config,
		mmlIInputStream ** input,
		mmlIOutputStream ** output
		);

	mmlBool GetHostName ( mmlICString ** hostname )
	{
		mmlChar hostname_buf[128+1] = {0};
		gethostname(hostname_buf, 128);
		*hostname = mmlNewCString(hostname_buf);
		MML_ADDREF(*hostname);
		return mmlTrue;
	}
	mmlBool IsConnected();

private:
	mmlAutoPtr < mmlNetworkClientSocketSSL > mSocket;
};

#endif //MML_TCP_CLIENT_SSL_IMPL_HEADER_INCLUDED
