#include "mmlNetworkStreamClient.h"
#include "mmlIThread.h"
#include "mmlICString.h"
#include "mml_components.h"
#ifndef MML_WIN
#include <errno.h>
#include <sys/ioctl.h>
#endif

void log_network(const mmlChar * formatter , ...);

NSC_RESULT_T mmlNetworkClientSocket::Connect (mmlIVariantStruct * config)
{
	mmlInt32 socket_type = SOCK_STREAM;
	mmlInt32 socket_family = AF_INET;
	mmlAutoPtr < mmlIVariantString > sock;
	mmlBool abstr = mmlFalse;
	if ( config->Get("socket", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant > (sock)) == mmlTrue )
	{
		mmlBool eq;
		if (sock->Get()->CompareStr(MML_EQUAL, "unix", eq, MML_CASE_INSENSITIVE) == mmlTrue &&
			eq == mmlTrue )
		{
			socket_family = AF_UNIX;
#ifndef MML_WIN
			mmlAutoPtr < mmlIVariantString > fs;
			config->Get("fs", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(fs));
			if (fs != mmlNULL)
			{
				fs->Get()->CompareStr(MML_EQUAL, "abstract", abstr);
			}
			if (abstr == mmlTrue)
			{
				socket_family = AF_LOCAL;
			}
#endif
		}
		if (sock->Get()->CompareStr(MML_EQUAL, "udp", eq, MML_CASE_INSENSITIVE) == mmlTrue &&
			eq == mmlTrue )
		{
			socket_type = SOCK_DGRAM;
		}
	}
	mmlAutoPtr < mmlIVariantInteger > timeout;
	config->Get("connect", mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant >(timeout));
	if ( socket_family == AF_INET )
	{
		mmlAutoPtr < mmlIVariantString > host;
		if ( config->Get("host", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant > (host)) == mmlFalse )
		{
			return NSC_ERROR;
		}
		mmlAutoPtr < mmlIVariantInteger > port;
		if ( config->Get("port", mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant >(port)) == mmlFalse )
		{
			return NSC_ERROR;
		}

		//log_network("STREAM: Connecting to %s:%d\n" , host->Get()->Get(), port->Get());
		mml_network_init();
		mSID = socket(AF_INET, socket_type , 0);

#ifdef MML_WIN
		u_long one = 1;
		ioctlsocket(mSID , FIONBIO, &one);
#else
		int one = 1;
		ioctl (mSID, FIONBIO, &one);
#endif

		sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port->Get());
		addr.sin_addr.s_addr = inet_addr(host->Get()->Get());
#ifdef MML_WIN
		struct hostent *remoteHost = gethostbyname(host->Get()->Get());
		if ( remoteHost == mmlNULL )
		{
			if ( WSAHOST_NOT_FOUND == WSAGetLastError() )
			{
				return NSC_ERROR;
			}
			else
			{
				return NSC_REFUSED;
			}
		}
#else
		mmlInt32 err;
		mmlChar buf[2048] = {0};
		struct hostent hbuf = {0};
		struct hostent *remoteHost = NULL;
		mmlInt32 rc = gethostbyname_r(host->Get()->Get(), &hbuf, buf, sizeof(buf), &remoteHost, &err);
		if ( rc != 0 )
		{
			if ( rc == HOST_NOT_FOUND )
			{
				return NSC_ERROR;
			}
			else
			{
				return NSC_REFUSED;
			}
			remoteHost = NULL;
		}
#endif
		if ( remoteHost != NULL )
		{
			addr.sin_addr.s_addr = *(u_long *)remoteHost->h_addr_list[0];
		}
		if ( addr.sin_addr.s_addr == INADDR_NONE )
		{
			log_network("Could not resolve %s\n", host->Get()->Get());
			return NSC_ERROR;
		}
		if ( connect(mSID, (sockaddr*)&addr , sizeof(addr)) == 0 )
		{
			//log_network("STREAM: connected\n");
			return NSC_OK;
		}
	}
	else if ( socket_type == AF_UNIX 
#ifndef MML_WIN
		    || socket_type == AF_LOCAL
#endif
		)
	{
#ifdef MML_WIN
		return NSC_ERROR;
#else
		mmlAutoPtr < mmlIVariantString >  path;
		if ( config->Get("path", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant>(path)) == mmlFalse )
		{
			return NSC_ERROR;
		}

		//log_network("STREAM: Connecting to %s\n" ,path->Get()->Get());
		mml_network_init();
		mSID = socket(socket_type, SOCK_STREAM , 0);

		int one = 1;
		ioctl (mSID, FIONBIO, &one);

		setsockopt(mSID, SOL_SOCKET, SO_REUSEADDR, (const char*)&one, sizeof(one));
		sockaddr_un sock_addr = { 0 };
		int sock_len = 0;
		if (socket_type == AF_LOCAL)
		{
			sock_addr.sun_family = AF_LOCAL;
			sock_addr.sun_path[0] = 0;
			mmlStrCopy(sock_addr.sun_path + 1, path->Get()->Get(), sizeof(sock_addr.sun_path));
			sock_len = mmlStrLength(path->Get()->Get()) + offsetof(struct sockaddr_un, sun_path) + 1;
			//log_network("AF_LOCAL sock_len=%d\n", sock_len);
		}
		else
		{
			sock_addr.sun_family = AF_UNIX;
			mmlStrCopy(sock_addr.sun_path, path->Get()->Get(), sizeof(sock_addr.sun_path));
			sock_len = mmlStrLength(path->Get()->Get()) + offsetof(struct sockaddr_un, sun_path);
			//log_network("AF_UNIX sock_len=%d\n", sock_len);
		}

		mSID = socket(sock_addr.sun_family, SOCK_STREAM, 0);
		if (mSID < 0)
		{
			log_network("could not create unixsocket\n");
			return NSC_ERROR;
		}


		if ( connect(mSID, (sockaddr*)&sock_addr, sock_len) == 0 )
		{
			log_network("STREAM: connected\n");
			return NSC_OK;
		}
#endif
	}
#ifdef MML_WIN
	if ( WSAGetLastError() != WSAEWOULDBLOCK )
	{
		return NSC_REFUSED;
	}
#else
	if ( errno != EINPROGRESS )
	{
		log_network("STREAM: error %d\n", errno);
		return NSC_REFUSED;
	}
#endif
	while ( timeout == 0 )
	{
		mmlNetworkSocketPoll poll;
		poll.Add(mSID, mmlFalse, mmlTrue);
		mmlInt32 w[1];
		mmlInt32 wc;
		mmlInt32 e[1];
		mmlInt32 ec;
		if ( poll.Wait(100, mmlNULL, mmlNULL , w, &wc, e, &ec) == mmlTrue )
		{
			if ( ec > 0 )
			{
				log_network("STREAM: couldn't connect\n");
				return NSC_REFUSED;
			}
			if ( wc > 0 )
			{
				log_network("STREAM: connected\n");
				return NSC_OK;
			}
		}
	}
	mmlNetworkSocketPoll poll;
	poll.Add(mSID, mmlFalse, mmlTrue);
	mmlInt32 w[1];
	mmlInt32 wc;
	mmlInt32 e[1];
	mmlInt32 ec;
	if ( poll.Wait(timeout == mmlNULL ? 0 : timeout->Get(), mmlNULL, mmlNULL , w, &wc, e, &ec) == mmlTrue )
	{
		if ( ec > 0 )
		{
			log_network("STREAM: couldn't connect\n");
			return NSC_REFUSED;
		}
		if ( wc > 0 )
		{
			log_network("STREAM: connected\n");
			return NSC_OK;
		}
	}
	log_network("STREAM: couldn't connect\n");

	return NSC_REFUSED;
}

mmlBool mmlNetworkClientSocket::Close ()
{
	if ( mSID != 0)
	{
#ifdef MML_WIN
		closesocket(mSID);
#else
		close(mSID);
#endif
		mSID = 0;
	}
	return mmlTrue;
}

mmlInt32 mmlNetworkClientSocket::Read (const mmlInt32 timeout, const mmlInt32 size, void * data )
{
	mmlNetworkSocketPoll poll;
	poll.Add(mSID, mmlTrue, mmlFalse);
	mmlInt32 r[1];
	mmlInt32 rc;
	mmlInt32 e[1];
	mmlInt32 ec;
	if ( poll.Wait(timeout, r, &rc, mmlNULL, mmlNULL, e, &ec) == mmlTrue )
	{
		if ( ec > 0 )
		{
			Close();
			return -1;
		}
		if ( rc > 0 )
		{
			mmlInt32 rd = recv(mSID, (char*)data, size, MSG_NOSIGNAL);
			if ( rd <= 0 )
			{
				Close();
				return -1;
			}
			return rd;
		}
	}
	return 0;
}

mmlInt32 mmlNetworkClientSocket::Write ( const mmlInt32 timeout, const mmlInt32 size, void * data )
{
	if (mSID <= 0)
	{
		return -1;
	}
	mmlInt32 res = send(mSID, (char*)data, size, MSG_NOSIGNAL);

	mmlNetworkSocketPoll poll;
	poll.Add(mSID, mmlFalse, mmlTrue);
	mmlInt32 w[1];
	mmlInt32 wc;
	mmlInt32 e[1];
	mmlInt32 ec;
	if ( poll.Wait(timeout, mmlNULL, mmlNULL , w, &wc, e, &ec)  == mmlFalse )
	{
		return -1;
	}
	if ( ec > 0 )
	{
		return -1;
	}
	if ( wc > 0 )
	{
		return size;
	}
	return -1;
}

MML_OBJECT_IMPL0(mmlNetworkClientSocket)

MML_OBJECT_IMPL1(mmlNetworkInputStreamControl, mmlIStreamControl)

mmlNetworkInputStream::mmlNetworkInputStream(mmlINetworkClientSocket * sock)
:mSocket(sock), mControl(new mmlNetworkInputStreamControl)
{

}

mmlNetworkInputStream::~mmlNetworkInputStream()
{
	if ( mSocket != mmlNULL )
	{
		mSocket = mmlNULL;
	}
}

mmlBool mmlNetworkInputStream::GetControl ( mmlIStreamControl ** control )
{
	*control = mControl;
	MML_ADDREF(*control);
	return mmlTrue;
}

mmlInt64 mmlNetworkInputStream::Read ( const mmlInt64 size , void * data )
{
	if ( mSocket == mmlNULL )
	{
		return -1;
	}
	while ( mControl->timeout == 0 )
	{
		mmlInt32 rd = mSocket->Read(100, size, data);
		if ( rd > 0 )
		{
			return rd;
		}
		else if ( rd == 0 )
		{
			continue;
		}
		else
		{
			return -1;
		}
	}
	return mSocket->Read(mControl->timeout, size, data);
}

mmlBool mmlNetworkInputStream::IsEmpty ()
{
	return mmlFalse;
}

mmlInt64 mmlNetworkInputStream::Size ()
{
	return 0;
}

mmlBool mmlNetworkInputStream::Close ()
{
	if ( mSocket != mmlNULL )
	{
		mSocket = mmlNULL;
	}
	return mmlTrue;
}

MML_OBJECT_IMPL1(mmlNetworkInputStream, mmlIInputStream)

MML_OBJECT_IMPL2(mmlNetworkOutputStreamControl, mmlIOutputStreamControl, mmlIStreamControl)

mmlNetworkOutputStream::mmlNetworkOutputStream(mmlINetworkClientSocket * sock)
:mSocket(sock), mControl(new mmlNetworkOutputStreamControl)
{

}

mmlNetworkOutputStream::~mmlNetworkOutputStream()
{

}

mmlInt64 mmlNetworkOutputStream::Write ( const mmlInt64 size , const void * data )
{
	if ( mSocket == mmlNULL )
	{
		return -1;
	}
	return mSocket->Write(mControl->timeout, size, (void*)data);
}

mmlBool mmlNetworkOutputStream::IsFull ()
{
	if ( mControl->max_size != 0 && mControl->current_size >= mControl->max_size )
	{
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlNetworkOutputStream::GetControl ( mmlIStreamControl ** control )
{
	*control = mControl;
	MML_ADDREF(*control);
	return mmlTrue;
}

mmlBool mmlNetworkOutputStream::Close ()
{
	if ( mSocket != mmlNULL )
	{
		mSocket = mmlNULL;
	}
	return mmlTrue;
}

mmlInt64 mmlNetworkOutputStream::Size ()
{
	return 0;
}

MML_OBJECT_IMPL1(mmlNetworkOutputStream, mmlIOutputStream)

NSC_RESULT_T mmlNetworkStreamClient::Connect  ( mmlIVariantStruct * config, mmlIInputStream ** input, mmlIOutputStream ** output )
{
	mSocket = new mmlNetworkClientSocket();
	NSC_RESULT_T res = mSocket->Connect(config);
	if ( res  != NSC_OK )
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

mmlBool mmlNetworkStreamClient::IsConnected()
{
	if ( mSocket != mmlNULL ) return mSocket->IsConnected();
	return mmlFalse;
}

MML_OBJECT_IMPL2(mmlNetworkStreamClient, mmlINetworkStreamClient, mmlIStreamClient)
