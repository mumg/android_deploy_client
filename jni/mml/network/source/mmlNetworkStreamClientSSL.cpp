#include "mmlNetworkStreamClientSSL.h"
#include "mmlNetworkCommon.h"
#include "mmlICString.h"
#include <mbedtls/debug.h>

void log_network(const mmlChar * formatter , ...);

static void my_debug( void *ctx, int level,
	const char *file, int line,
	const char *str )
{
	log_network("%s:%04d: %s", file, line, str );

}
mmlNetworkClientSocketSSLContext::mmlNetworkClientSocketSSLContext()
{
	mbedtls_net_init(&(mFD));
	mbedtls_ssl_init(&(mSSL));
	mbedtls_ssl_config_init(&(mConf));
	mbedtls_x509_crt_init(&(mCaCert));
	mbedtls_x509_crt_init(&(mClientCert));
	mbedtls_pk_init(&(mClientKey));
	mbedtls_entropy_init(&(mEntropy));
	mbedtls_ctr_drbg_init(&mDrbgCtx);
	mbedtls_ssl_conf_dbg( &mConf, my_debug, NULL );
}

mmlNetworkClientSocketSSLContext::~mmlNetworkClientSocketSSLContext()
{
	int ret = 0;
	do {
		ret = mbedtls_ssl_close_notify(&mSSL);
	} while(ret == MBEDTLS_ERR_SSL_WANT_WRITE);

	mbedtls_net_free(&(mFD));

	mbedtls_x509_crt_free(&(mClientCert));
	mbedtls_x509_crt_free(&(mCaCert));
	mbedtls_pk_free(&(mClientKey));
	mbedtls_ssl_free(&(mSSL));
	mbedtls_ssl_config_free(&(mConf));
	mbedtls_entropy_free(&(mEntropy));
	mbedtls_ctr_drbg_free(&mDrbgCtx);
}

MML_OBJECT_IMPL0(mmlNetworkClientSocketSSLContext)

mmlNetworkClientSocketSSL::mmlNetworkClientSocketSSL()
:mInSecure(mmlFalse), mClientCert(mmlFalse)
{
	mml_network_init();
	mMutex = mmlNewObject(MML_OBJECT_UUID(mmlIMutex));
}

mmlNetworkClientSocketSSL::~mmlNetworkClientSocketSSL()
{
	Close();
}

mmlBool mmlNetworkClientSocketSSL::Close()
{
	mContext = mmlNULL;
	return mmlTrue;
}

NSC_RESULT_T mmlNetworkClientSocketSSL::Connect (mmlIVariantStruct * config )
{
	mContext = new mmlNetworkClientSocketSSLContext();

	mmlAutoPtr < mmlIVariantString > ca;
	mmlAutoPtr < mmlIVariantString > key;
	if ( config->Get("ca", mmlRelativePtrAddRef < mmlIVariantString , mmlIVariant > (ca)) == mmlTrue &&
		 config->Get("privatekey", mmlRelativePtrAddRef < mmlIVariantString , mmlIVariant > (key)) == mmlTrue )
	{
		if ( _ConfigureSSL_CERT(ca->Get()->Get(), key->Get()->Get()) == mmlFalse )
		{
			log_network("SSL conf failed\n");
			return NSC_ERROR;
		}
	}
	else if ( _ConfigureDefault() == mmlFalse )
	{
		log_network("SSL def conf failed\n");
		return NSC_ERROR;
	}

	mmlAutoPtr < mmlIVariantInteger > insecure;
	config->Get("insecure", mmlRelativePtrAddRef < mmlIVariantInteger , mmlIVariant >(insecure));
	if ( insecure != mmlNULL && insecure->Get() == 1 )
	{
		mInSecure = mmlTrue;
	}

	mmlAutoPtr < mmlIVariantString > host;
	mmlAutoPtr < mmlIVariantInteger > port;

	if ( config->Get("host", mmlRelativePtrAddRef < mmlIVariantString , mmlIVariant > (host)) == mmlFalse ||
		 config->Get("port", mmlRelativePtrAddRef < mmlIVariantInteger , mmlIVariant > (port)) == mmlFalse )
	{
		log_network("No host and port in config\n");
		return NSC_ERROR;
	}

	mmlChar port_str[16];
	mmlSprintf(port_str, sizeof(port_str), "%d", (mmlInt32)port->Get());
	log_network("ssl connect %s:%s\n", host->Get()->Get(), port_str);
	int ret = mbedtls_net_connect(mContext->GetFD(), host->Get()->Get(), port_str,
		MBEDTLS_NET_PROTO_TCP);
	if (ret == MBEDTLS_ERR_NET_UNKNOWN_HOST) 
	{
		log_network("Could not connect\n");
		return NSC_REFUSED;
	}
	else if ( ret == MBEDTLS_ERR_NET_RECV_FAILED ||
              ret == MBEDTLS_ERR_NET_SEND_FAILED ||
              ret == MBEDTLS_ERR_NET_CONN_RESET )
	{
		return NSC_REFUSED;
	}
	else if ( ret != 0 )
	{
		return NSC_ERROR;
	}
	ret = mbedtls_ssl_config_defaults(mContext->GetConfig(), MBEDTLS_SSL_IS_CLIENT,
		MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
	if (ret != 0 ) 
	{
		log_network("SLL config failed\n");
		return NSC_ERROR;
	}
	if ( mInSecure == mmlTrue )
	{
		mbedtls_ssl_conf_authmode(mContext->GetConfig(), MBEDTLS_SSL_VERIFY_OPTIONAL);
	}
	else
	{
		mbedtls_ssl_conf_authmode(mContext->GetConfig(), MBEDTLS_SSL_VERIFY_REQUIRED);
	}
	if( ( ret = mbedtls_ctr_drbg_seed( mContext->GetDrbgCtx(), mbedtls_entropy_func,mContext->GetEntropy(),
		(const unsigned char *) "tNQ2Uy2i",
		8 ) ) != 0 )
	{
		log_network("drbg_seed failed\n");
		return NSC_ERROR;
	}
	mbedtls_ssl_conf_rng(mContext->GetConfig(), mbedtls_ctr_drbg_random,
		mContext->GetDrbgCtx());

	mmlAutoPtr < mmlIVariantInteger > timeout;
	config->Get("connect", mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant >(timeout));

	mbedtls_ssl_conf_read_timeout(mContext->GetConfig(), timeout == mmlNULL ? 0 : timeout->Get());

	if ( mClientCert == mmlTrue )
	{
		ret = mbedtls_ssl_conf_own_cert(mContext->GetConfig(), mContext->GetClientCert(),
			mContext->GetClientKey());
		if (ret != 0) 
		{
			return NSC_ERROR;
		}
	}
	//mbedtls_debug_set_threshold(4);
	ret = mbedtls_ssl_setup(mContext->GetSSL(), mContext->GetConfig());
	if (ret != 0) 
	{
		log_network("ssl_setup failed\n");
		return NSC_ERROR;
	}


	ret = mbedtls_ssl_set_hostname(mContext->GetSSL(), host->Get()->Get());
	if (ret != 0) 
	{
		log_network("set_hostname failed\n");
		return NSC_ERROR;
	}


	mbedtls_ssl_set_bio(mContext->GetSSL(), mContext->GetFD(), mbedtls_net_send, NULL,
		mbedtls_net_recv_timeout);


	while ((ret = mbedtls_ssl_handshake(mContext->GetSSL())) != 0) 
	{
		if (ret != MBEDTLS_ERR_SSL_WANT_READ
			&& ret != MBEDTLS_ERR_SSL_WANT_WRITE) 
		{
			if ( ret == MBEDTLS_ERR_SSL_TIMEOUT )
			{
				return NSC_REFUSED;
			}
			log_network("handshake failed\n");
			return NSC_ERROR;
		}
	}

	mbedtls_ssl_get_record_expansion(mContext->GetSSL());
	ret = mbedtls_ssl_get_verify_result(mContext->GetSSL());
	if (ret != 0 && mInSecure == mmlFalse) {
		return NSC_ERROR;
	}
	log_network("ssl connected\n");
	return NSC_OK;
}


mmlBool mmlNetworkClientSocketSSL::_ConfigureDefault ()
{
	return mmlTrue;
}


mmlBool mmlNetworkClientSocketSSL::_ConfigureSSL_CERT(const mmlChar * ca, const mmlChar * key)
{
	int ret;
	ret = mbedtls_x509_crt_parse(mContext->GetClientCert(),
		(const unsigned char *) ca,
		mmlStrLength(ca) + 1);
	if (ret < 0) 
	{
		return mmlFalse;
	}

	ret = mbedtls_pk_parse_key(mContext->GetClientKey(),
		(const unsigned char *) key,
		mmlStrLength(key) + 1, NULL, 0);
	if (ret != 0) 
	{
		return mmlFalse;
	}
	mClientCert = mmlTrue;
	return mmlTrue;
}

mmlInt32 mmlNetworkClientSocketSSL::Write ( const mmlInt32 timeout, const mmlInt32 size, void * data )
{
	if ( mContext == mmlNULL )
	{
		return -1;
	}
	int ret;
	while( ( ret = mbedtls_ssl_write( mContext->GetSSL(), (const unsigned char*)data, size ) ) <= 0 )
	{
		if( ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE )
		{
			mContext = mmlNULL;
			return -1;
		}
	}
	return size;
}

mmlInt32 mmlNetworkClientSocketSSL::Read (const mmlInt32 timeout, const mmlInt32 size, void * data )
{
	if ( mContext == mmlNULL )
	{
		return -1;
	}
	int ret;
	for (;;)
	{
		mbedtls_ssl_conf_read_timeout(mContext->GetConfig(), timeout);

		ret = mbedtls_ssl_read( mContext->GetSSL(), (unsigned char*)data, size );
		if ( ret == MBEDTLS_ERR_SSL_TIMEOUT )
		{
			return 0;
		}
		if( ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE )
		{
			continue;
		}
		if( ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY )
		{
			mContext = mmlNULL;
			return -1;
		}
		if ( ret < 0 )
		{
			mContext = mmlNULL;
			return -1;
		}
		return ret;
	}
}


MML_OBJECT_IMPL0(mmlNetworkClientSocketSSL)

NSC_RESULT_T mmlNetworkStreamClientSSL::Connect  (mmlIVariantStruct * config, mmlIInputStream ** input, mmlIOutputStream ** output )
{
	mSocket = new mmlNetworkClientSocketSSL();
	NSC_RESULT_T res;
	if ( (res = mSocket->Connect(config)) != NSC_OK )
	{
		return res;
	}
	if ( input != mmlNULL )
	{
		*input = new mmlNetworkInputStream(mSocket);
		MML_ADDREF(*input);
	}
	if ( output != mmlNULL )
	{
		*output = new mmlNetworkOutputStream(mSocket);
		MML_ADDREF(*output);
	}
	return res;
}

mmlBool mmlNetworkStreamClientSSL::IsConnected()
{
	if ( mSocket != mmlNULL ) return mSocket->IsConnected();
	return mmlFalse;
}

MML_OBJECT_IMPL3(mmlNetworkStreamClientSSL, mmlINetworkStreamClientSSL, mmlINetworkStreamClient, mmlIStreamClient)