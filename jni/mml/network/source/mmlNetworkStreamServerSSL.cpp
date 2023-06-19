#include "mmlNetworkStreamServerSSL.h"
#include "mml_components.h"
#include "mmlNetworkCommon.h"
#include <openssl/err.h>


class mmlNetworkSocketSSL : public mmlNetworkSocket
{
	MML_OBJECT_DECL
public:

	mmlNetworkSocketSSL(const mmlInt32 sid, SSL_CTX * context,  const mmlInt32 chunk_size, mmlIInputStream * output, mmlIOutputStream * input)
		:mmlNetworkSocket(sid, chunk_size, output, input), mWantWrite(mmlFalse), mState(SOCKET_SSL_HANDSHAKE), mReadChunk(mmlNULL)
	{
		mHandle = SSL_new(context);
		SSL_set_fd (mHandle, mSID);
		SSL_set_accept_state(mHandle);
		BIO_set_nbio(SSL_get_rbio(mHandle), 1);
		BIO_set_nbio(SSL_get_wbio(mHandle), 1);
		mReadChunk = mmlAlloc(chunk_size);
	}

	~mmlNetworkSocketSSL()
	{
		//log_network("SSL Socket destroy\n");
		if ( mHandle != mmlNULL ) SSL_shutdown ((SSL*)mHandle);
		if ( mHandle != mmlNULL ) SSL_free ((SSL*)mHandle);
		if ( mReadChunk != mmlNULL )
		{
			mmlFree(mReadChunk);
		}
		Close();
	}

	virtual mmlBool IsSending ()
	{
		return mWantWrite;
	}

	virtual void SendComplete ()
	{
		mWantWrite = mmlFalse;
	}

	mmlBool Recv ()
	{
		if ( mState == SOCKET_SSL_HANDSHAKE )
		{
			for(;;)
			{
			mmlInt32 ret = SSL_accept(mHandle); 
			if ( ret ==  1)
			{
				mState = SOCKET_SSL_ACTIVE;
				break;
			}
			mmlInt32 err = SSL_get_error(mHandle, ret);
			if ( err == SSL_ERROR_WANT_READ )
			{
				return mmlTrue;
			}
			else if ( err == SSL_ERROR_WANT_WRITE )
			{
				mWantWrite = mmlTrue;
				return mmlTrue;
			}
			return mmlFalse;
		}
			
		}
		if ( mState == SOCKET_SSL_ACTIVE )
		{
			mmlInt32 ret = 0;
			for (;;)
			{
				ret = SSL_read(mHandle, mReadChunk, mChunkSize);
				if ( ret > 0 )
				{
					if ( mInput->Write(ret, mReadChunk) != ret )
					{
						return mmlFalse;
					}
				}
				else
				{
					break;
				}
			}
			mmlInt32 err = SSL_get_error(mHandle, ret);
			if ( err == SSL_ERROR_ZERO_RETURN )
			{
				return mmlFalse;
			}
			if ( err == SSL_ERROR_WANT_READ )
			{
				return mmlTrue;
			}
			else if ( err == SSL_ERROR_WANT_WRITE )
			{
				mWantWrite = mmlTrue;
				return mmlTrue;
			}
		}
		return mmlFalse;
	}

	mmlBool Send ()
	{
		if ( mState == SOCKET_SSL_HANDSHAKE )
		{
			return mmlTrue;
		}
		if ( mState == SOCKET_SSL_ACTIVE && mWantWrite == mmlFalse )
		{
				mmlInt32 actual = 0;
				if ( ( actual = mOutput->Read(mChunkSize, mChunk) ) < 0 )
				{
					return mmlFalse;
				}
				if ( actual == 0 )
				{
					return mmlTrue;
				}
				mWriteLen = actual;
				mmlInt32 ret = SSL_write(mHandle, mChunk, mWriteLen);
				if ( ret > 0)
				{
					mWantWrite = mmlTrue;
					return mmlTrue;
				}
				mmlInt32 err = SSL_get_error(mHandle, ret);
				if ( err == SSL_ERROR_WANT_READ )
				{
					return mmlTrue;
				}
				else if ( err == SSL_ERROR_WANT_WRITE )
				{
					mWantWrite = mmlTrue;
					return mmlTrue;
				}
		}
		return mmlTrue;
	}
private:

	void * mReadChunk;

	enum
	{
		SOCKET_SSL_HANDSHAKE,
		SOCKET_SSL_ACTIVE
	}
	mState;


	mmlBool mWantWrite;
	
	SSL_CTX * mContext;
	SSL * mHandle;
	mmlInt32 mWriteLen;

};



MML_OBJECT_IMPL0(mmlNetworkSocketSSL)

mmlNetworkStreamServerSSL::mmlNetworkStreamServerSSL()
	:mContext(mmlNULL)
{

}

mmlNetworkStreamServerSSL::~mmlNetworkStreamServerSSL()
{
	mPool.clear();
	if ( mContext != mmlNULL )
	{
		SSL_CTX_free ((SSL_CTX*)mContext);
	}
}

mmlBool mmlNetworkStreamServerSSL::Start ( mmlIConfig * config, mmlIStreamServerHandler * handler )
{
	mmlAutoPtr < mmlICString > privatekey;
	mmlAutoPtr < mmlICString > cafile;
	if ( config->GetValue("privatekey", privatekey.getterAddRef()) == mmlFalse )
	{
		return mmlFalse;
	}
	if ( config->GetValue("ca", cafile.getterAddRef()) == mmlFalse )
	{
		return mmlFalse;
	}
	mml_network_ssl_init();
	if ( mContext != mmlNULL )
	{
		return mmlFalse;
	}
	BIO     *bio;
	X509    *x509;

	mContext = SSL_CTX_new(TLSv1_method());

	if ( mContext == mmlNULL )
	{
		log_network("Could not create SSL context\n");
		return mmlFalse;
	}

	/* client side options */

	SSL_CTX_set_options(mContext, SSL_OP_MICROSOFT_SESS_ID_BUG);
	SSL_CTX_set_options(mContext, SSL_OP_NETSCAPE_CHALLENGE_BUG);

	/* server side options */

	SSL_CTX_set_options(mContext, SSL_OP_NO_SSLv2);

	SSL_CTX_set_options(mContext, SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG);
	SSL_CTX_set_options(mContext, SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER);


	SSL_CTX_set_options(mContext, SSL_OP_SSLEAY_080_CLIENT_DH_BUG);
	SSL_CTX_set_options(mContext, SSL_OP_TLS_D5_BUG);
	SSL_CTX_set_options(mContext, SSL_OP_TLS_BLOCK_PADDING_BUG);

	SSL_CTX_set_options(mContext, SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS);

	SSL_CTX_set_options(mContext, SSL_OP_SINGLE_DH_USE);


#ifdef SSL_OP_NO_COMPRESSION
	SSL_CTX_set_options(mContext, SSL_OP_NO_COMPRESSION);
#endif

#ifdef SSL_MODE_RELEASE_BUFFERS
	SSL_CTX_set_mode(mContext, SSL_MODE_RELEASE_BUFFERS);
#endif

	SSL_CTX_set_read_ahead(mContext, 1);

	bio = BIO_new_mem_buf((void*)cafile->Get() , cafile->Length());
	if (bio == NULL) 
	{
		log_network("Could not open certificate file %s\n", cafile->Get());
		return mmlFalse;
	}

	x509 = PEM_read_bio_X509_AUX(bio, NULL, NULL, NULL);
	if (x509 == NULL) 
	{
		log_network("Could not read certificate file %s\n", cafile->Get());
		BIO_free(bio);
		return mmlFalse;
	}

	if (SSL_CTX_use_certificate(mContext, x509) == 0) 
	{
		log_network("Could not use certificate file %s\n", cafile->Get());
		X509_free(x509);
		BIO_free(bio);
		return mmlFalse;
	}
	X509_free(x509); 
	mmlUInt32 n;
	for ( ;; ) 
	{

		x509 = PEM_read_bio_X509(bio, NULL, NULL, NULL);
		if (x509 == NULL) 
		{
			n = ERR_peek_last_error();

			if (ERR_GET_LIB(n) == ERR_LIB_PEM
				&& ERR_GET_REASON(n) == PEM_R_NO_START_LINE)
			{
				ERR_clear_error();
				break;
			}

			log_network("Could not read certificate file (bundle) %s\n", cafile->Get());
			BIO_free(bio);
			return mmlFalse;
		}

		if (SSL_CTX_add_extra_chain_cert(mContext, x509) == 0) 
		{
			log_network("Could not add extra chain for certificate file %s", cafile->Get());
			X509_free(x509);
			BIO_free(bio);
			return mmlFalse;
		}
	}
	BIO_free(bio); 

	BIO * pkey_bio = BIO_new_mem_buf((void*)privatekey->Get(), privatekey->Length());

	if ( pkey_bio == NULL )
	{
		return mmlFalse;
	}

	EVP_PKEY * pkey = PEM_read_bio_PrivateKey(pkey_bio, NULL, NULL, NULL);

	BIO_free(pkey_bio);
	if ( pkey == NULL )
	{
		return mmlFalse;
	}
	if (SSL_CTX_use_PrivateKey(mContext, pkey) == 0 )
	{
		log_network("Could not use private key %s\n", privatekey->Get());
		return mmlFalse;
	}

	mmlAutoPtr < mmlICString > client;
	if ( config->GetValue("caclient", client.getterAddRef()) == mmlTrue)
	{
		if ( client != mmlNULL )
		{
			bio = BIO_new_mem_buf((void*)client->Get() , client->Length());
			if (bio == NULL) 
			{
				log_network("Could not open certificate file %s\n", client->Get());
				return mmlFalse;
			}

			x509 = PEM_read_bio_X509_AUX(bio, NULL, NULL, NULL);
			if (x509 == NULL) 
			{
				log_network("Could not read certificate file %s\n", client->Get());
				BIO_free(bio);
				return mmlFalse;
			}
			if ( !SSL_CTX_add_client_CA(mContext, x509))
			{
				X509_free(x509);
				BIO_free(bio);
				return mmlFalse;
			}
			X509_free(x509);
			BIO_free(bio);
		}
	}




	if ( mContext == mmlNULL )
	{
		log_network("SSL context is not configured\n");
		return mmlFalse;
	}
	return mmlNetworkServerBase< mmlINetworkStreamServerSSL >::Start(config, handler);
}


mmlNetworkSocket * mmlNetworkStreamServerSSL::_Create (mmlInt32 sid, const mmlInt32 chunk_size, mmlIInputStream * output, mmlIOutputStream * input)
{
	return new mmlNetworkSocketSSL(sid, mContext, chunk_size, output, input);
}

MML_OBJECT_IMPL2(mmlNetworkStreamServerSSL, mmlINetworkStreamServerSSL, mmlIStreamServer)
