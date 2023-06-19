#ifndef MML_NETWORK_CLIENT_IMPL_HEADER_INCLUDED
#define MML_NETWORK_CLIENT_IMPL_HEADER_INCLUDED

#include "mmlIThread.h"
#include "mmlPtr.h"
#include "mmlINetworkStreamClient.h"
#include "mmlICondition.h"
#include "mmlNetworkCommon.h"
#include "mml_time.h"
#include "mml_components.h"
#include "mmlIStream.h"
#include "mmlICString.h"
#include <string>
#ifdef MML_POSIX
#include <sys/ioctl.h>
#endif


class mmlINetworkClientSocket : public mmlIObject
{
public:
	virtual NSC_RESULT_T Connect (mmlIVariantStruct * config) = 0;

	virtual mmlBool Close () = 0;

	virtual mmlInt32 Read (const mmlInt32 timeout, const mmlInt32 size, void * data ) = 0;

	virtual mmlInt32 Write (const mmlInt32 timeout,  const mmlInt32 size, void * data ) = 0;

	virtual mmlBool IsConnected () = 0;

};


class mmlNetworkClientSocket : public mmlINetworkClientSocket
{
	MML_OBJECT_DECL
public:
	mmlNetworkClientSocket()
		:mSID(0)
	{

	}

	~mmlNetworkClientSocket()
	{
		Close ();
	}

	virtual NSC_RESULT_T Connect (mmlIVariantStruct * config);

	virtual mmlBool Close ();

	virtual mmlInt32 Read (const mmlInt32 timeout, const mmlInt32 size, void * data );

	virtual mmlInt32 Write (const mmlInt32 timeout,  const mmlInt32 size, void * data );

	mmlBool IsConnected () { return mSID != 0 ? mmlTrue : mmlFalse; }

protected:

	mmlInt32 mSID;

};

class mmlNetworkInputStreamControl : public mmlIStreamControl
{
	MML_OBJECT_DECL
public:

	mmlInt32 timeout;

	mmlNetworkInputStreamControl()
		:timeout(0)
	{

	}

	mmlBool SetSignal(mmlIStreamSignal * signal)
	{
		return mmlFalse;
	}

	mmlBool SetTimeout ( const mmlInt32 _timeout )
	{
		timeout = _timeout;
		return mmlTrue;
	}

	mmlBool GetTimeout ( mmlInt32 * _timeout )
	{
		*_timeout = timeout;
		return mmlTrue;
	}

	mmlInt64 GetPosition ()
	{
		return 0;
	}

	mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
	{
		return mmlFalse;
	}
};

class mmlNetworkInputStream : public mmlIInputStream
{
	MML_OBJECT_DECL
public:

	mmlNetworkInputStream(mmlINetworkClientSocket * sock);

	~mmlNetworkInputStream();

	mmlBool GetControl ( mmlIStreamControl ** control );

	mmlInt64 Read ( const mmlInt64 size , void * data );

	mmlBool IsEmpty ();

	mmlInt64 Size ();

	mmlBool Close ();

private:

	mmlAutoPtr < mmlINetworkClientSocket > mSocket; 
	mmlAutoPtr < mmlNetworkInputStreamControl > mControl;
};

class mmlNetworkOutputStreamControl : public mmlIOutputStreamControl
{
	MML_OBJECT_DECL
public:
	mmlInt64 max_size;

	mmlInt64 current_size;

	mmlInt32 timeout;

	mmlNetworkOutputStreamControl()
		:max_size(0), current_size(0), timeout(0)
	{

	}

	mmlBool SetSignal(mmlIStreamSignal* signal)
	{
		return mmlFalse;
	}

	mmlBool SetTimeout ( const mmlInt32 _timeout )
	{
		timeout = _timeout;
		return mmlTrue;
	}

	mmlBool GetTimeout ( mmlInt32 * _timeout )
	{
		*_timeout = timeout;
		return mmlTrue;
	}

	mmlInt64 GetPosition ()
	{
		return 0;
	}

	mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
	{
		return mmlFalse;
	}

	mmlBool SetMaxSize ( const mmlInt64 size )
	{
		max_size = size;
		return mmlTrue;
	}

	mmlInt64 GetMaxSize ()
	{
		return max_size;
	}
};


class mmlNetworkOutputStream : public mmlIOutputStream
{
	MML_OBJECT_DECL
public:

	mmlNetworkOutputStream(mmlINetworkClientSocket * sock);

	~mmlNetworkOutputStream();

	mmlInt64 Write ( const mmlInt64 size , const void * data );

	mmlBool IsFull ();

	mmlBool GetControl ( mmlIStreamControl ** control );

	mmlBool Close ();

	mmlInt64 Size ();

	mmlBool Flush () { return mmlTrue; }
private:
	mmlAutoPtr < mmlNetworkOutputStreamControl > mControl;
	mmlAutoPtr < mmlINetworkClientSocket > mSocket; 
};


class mmlNetworkStreamClient : public mmlINetworkStreamClient
{
	MML_OBJECT_DECL
public:

	mmlNetworkStreamClient()
	{
		mml_network_init();
	}

	~mmlNetworkStreamClient()
	{
	}

	virtual NSC_RESULT_T Connect 
		            ( 
		              mmlIVariantStruct * config,
					  mmlIInputStream ** input,
					  mmlIOutputStream ** output
		            );

	mmlBool IsConnected();

	mmlBool GetHostName ( mmlICString ** hostname )
	{
		mmlChar hostname_buf[128+1] = {0};
		gethostname(hostname_buf, 128);
		*hostname = mmlNewCString(hostname_buf);
		MML_ADDREF(*hostname);
		return mmlTrue;
	}

private:

	mmlAutoPtr < mmlNetworkClientSocket > mSocket;
};


#endif //MML_NETWORK_CLIENT_IMPL_HEADER_INCLUDED
