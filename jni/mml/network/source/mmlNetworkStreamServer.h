#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#ifdef MML_WIN
#include <winsock2.h>
#include <windows.h>
#include <ws2ipdef.h>
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/un.h>
#endif

#include "mml_components.h"
#include "mmlIThread.h"
#include "mmlIMutex.h"
#include "mmlINetworkStreamServer.h"
#include "mmlICondition.h"
#include "mmlIBuffer.h"
#include "mmlPtr.h"
#include "mmlICString.h"
#include <list>
#include <map>

void log_network(const mmlChar * formatter , ...);

#ifndef MML_WIN
#define SOCKET mmlInt32
#endif

#include "mmlNetworkCommon.h"

class mmlNetworkSocketSignal : public mmlIStreamSignal
{
	MML_OBJECT_DECL
private:
	mmlInt32 mNotifySID;
	mmlInt32 mSID;
	mmlBool mSending;
	mmlBool mNotified;
public:

	mmlNetworkSocketSignal(mmlInt32 notify_sid, mmlInt32 sid)
		:mSending(mmlFalse), mNotifySID(notify_sid), mSID(sid), mNotified(mmlFalse)
	{
		
	}

	void Signal()
	{
		if ( mSending == mmlFalse )
		{
			if ( mNotified == mmlFalse && mNotifySID != 0)
			{
				mNotified = mmlTrue;
				send(mNotifySID, (char*)&mSID, sizeof(mSID), MSG_NOSIGNAL);
			}
		}
	}

	void SetSending(const mmlBool is_sending)
	{
		mSending = is_sending;
		mNotified = mmlFalse;
	}

	mmlBool IsSending()
	{
		return mSending;
	}
};

class mmlNetworkSocket : public mmlIObject
{
	MML_OBJECT_DECL;
public:
	mmlNetworkSocket (mmlInt32 sid , const mmlInt32 chunk_size, mmlIInputStream * output, mmlIOutputStream * input );
	
	virtual ~mmlNetworkSocket ();

	void SetSignal(mmlInt32 notify_sid);

	void Close ();

	mmlBool IsActive ();

	virtual mmlBool Recv ();

	virtual mmlBool Send ();

	virtual mmlBool IsSending () { return mSignal->IsSending(); }

	virtual void SendComplete () { mSignal->SetSending( mmlFalse); }



protected:

	mmlAutoPtr < mmlNetworkSocketSignal > mSignal;

	mmlInt64 mSent;

	mmlAutoPtr < mmlIInputStream > mOutput;

	mmlAutoPtr < mmlIOutputStream > mInput;

	mmlSharedPtr < mmlIMutex > mMutex;

	void * mChunk;
	mmlInt32 mChunkSize;
	mmlInt32 mSize;

	friend class mmlNetworkSocketPool;
	mmlInt32 mSID;
};


class mmlNetworkSocketPool : public mmlIThreadFunction
{
	MML_OBJECT_DECL;
	MML_THREAD_DECLARE_SHUTDOWN_FLAG;
public:

	mmlNetworkSocketPool();
	
	~mmlNetworkSocketPool();
	
	mmlBool Add ( mmlInt32 sid , mmlNetworkSocket * socket);
	
	float Score ();

	void Enumerate (void * arg);
	
	void Execute(mmlIThread *);

private:

	
	mmlInt32 mNotifyPIPE[2];
	
	mmlSharedPtr < mmlIMutex > mMutex;
	
	std::list < std::pair < mmlInt32 ,  mmlAutoPtr < mmlNetworkSocket > > > mPendingSockets;
	
	std::map < mmlInt32 ,  mmlAutoPtr < mmlNetworkSocket > > mSockets;
	
	mmlSharedPtr < mmlIThread > mThread;

	mmlSharedPtr < mmlICondition > mCondition;

};






#ifdef MML_WIN
#define socklen_t mmlInt32
#endif

static mmlBool pool_predicate(mmlAutoPtr < mmlNetworkSocketPool > & h1 ,
	                          mmlAutoPtr < mmlNetworkSocketPool > & h2)
{
	return h1->Score() < h2->Score() ? mmlTrue : mmlFalse;
}

template < class T >
class mmlNetworkServerBase : public T
{
	MML_THREAD_DECLARE_SHUTDOWN_FLAG;
public:

	mmlNetworkServerBase()
	{
		mml_network_init();
	}
	
	~mmlNetworkServerBase()
	{
		Stop ();
	}

	mmlBool Start ( mmlIVariantStruct * config, mmlIStreamServerHandler * handler )
	{
		mHandler = handler;
		mmlInt32 socket_type = AF_INET;
		mmlAutoPtr < mmlIVariantString > sock;
		if ( config->Get("socket", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant>(sock)) == mmlTrue )
		{
			mmlBool eq;
			if (sock->Get()->CompareStr(MML_EQUAL, "unix", eq, MML_CASE_INSENSITIVE) == mmlTrue &&
				eq == mmlTrue )
			{
				socket_type = AF_UNIX;
			}
		}

		if ( socket_type == AF_INET )
		{
			mmlAutoPtr < mmlIVariantString > host;
			if ( config->Get("host", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(host)) == mmlFalse )
			{
				return mmlFalse;
			}
			mmlAutoPtr < mmlIVariantInteger> port;
			if ( config->Get("port", mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant>(port)) == mmlFalse )
			{
				return mmlFalse;
			}

			mSID = (SOCKET)socket(AF_INET , SOCK_STREAM , 0);
			if ( mSID < 0 )
			{
				return mmlFalse;
			}
			mmlInt32 optval = 1;
			setsockopt(mSID, SOL_SOCKET, SO_REUSEADDR,(const char*)&optval , sizeof(optval));
			sockaddr_in sock_addr;
			sock_addr.sin_family = AF_INET;
			sock_addr.sin_port = htons(port->Get());

			if ( host )
			{
				sock_addr.sin_addr.s_addr = inet_addr(host->Get()->Get());
			}
			else
			{
				sock_addr.sin_addr.s_addr = INADDR_ANY;
			}
			if (bind ( mSID , (sockaddr*)&sock_addr , sizeof(sock_addr)) != 0)
			{
#ifdef MML_WIN
				closesocket(mSID); mSID = 0;
#else
				close(mSID); mSID = 0;
#endif
				return mmlFalse;
			}

		}
		else
		{
#ifdef MML_WIN
			return mmlFalse;
#else
			mmlAutoPtr < mmlIVariantString > path;
			if ( config->Get("path", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant>(path)) == mmlFalse )
			{
				return mmlFalse;
			}
			mmlBool abstr = mmlFalse;
			mmlAutoPtr < mmlIVariantString > fs;
			config->Get("fs", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant>(fs));
			if ( fs != mmlNULL )
			{
				fs->Get()->CompareStr(MML_EQUAL, "abstract", abstr);
			}
			mmlInt32 optval = 1;
			setsockopt(mSID, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));
			sockaddr_un sock_addr;
			if ( abstr == mmlTrue )
			{
				sock_addr.sun_family = AF_LOCAL;
			}
			else
			{
				sock_addr.sun_family = AF_UNIX;
			}
			if (abstr == mmlTrue)
			{
				sock_addr.sun_path[0] = 0;
				mmlStrCopy(sock_addr.sun_path + 1, path->Get()->Get(), sizeof(sock_addr.sun_path));
			}
			else
			{
				unlink(path->Get()->Get());
				mmlStrCopy(sock_addr.sun_path, path->Get()->Get(), sizeof(sock_addr.sun_path));
			}
			mSID = (SOCKET)socket(sock_addr.sun_family, SOCK_STREAM , 0);
			if ( mSID < 0 )
			{
				return mmlFalse;
			}


			if (bind ( mSID , (sockaddr*)&sock_addr , mmlStrLength(path->Get()->Get()) + offsetof(struct sockaddr_un, sun_path) + 1) != 0)
			{
				close(mSID); mSID = 0;
				return mmlFalse;
			}
#endif
		}
		if ( listen(mSID , 5) != 0 )
		{
#ifdef MML_WIN
			closesocket(mSID); mSID = 0;
#else
			close(mSID); mSID = 0;
#endif
			return mmlFalse;
		}
		mMutex = mmlNewObject(MML_OBJECT_UUID(mmlIMutex));
		mThread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
		return mThread->Create(new mmlThreadFunctionProxy < mmlNetworkServerBase > (this) , PriorityNormal , MML_DEFAULT_STACK_SIZE);
	}
	
	mmlBool Stop ()
	{
		for ( std::list < mmlAutoPtr < mmlNetworkSocketPool > > ::iterator pool = mPool.begin(); pool != mPool.end() ; pool ++ )
		{
			(*pool)->Shutdown();
		}
		if ( mThread != mmlNULL ) mThread->Join ();
		if ( mSID != 0 )
		{
#ifdef MML_WIN
			closesocket(mSID);
#else
			close(mSID);
#endif
		}
		return mmlTrue;
	}

	void Execute (mmlIThread *)
	{
		timeval tm;
		tm.tv_sec = 1;
		tm.tv_usec = 0;
		while (IsShutdown() == mmlFalse)
		{
			fd_set fd;
			FD_ZERO(&fd);
			FD_SET(mSID, &fd);
			tm.tv_sec = 0;
			tm.tv_usec = 10000;
			mmlInt32 res = select ( (mmlInt32)mSID + 1 , &fd , 0 , 0 , &tm );
			if ( res > 0)
			{
				sockaddr_in remote_host;
				socklen_t remote_host_len = sizeof(remote_host);
				mmlInt32 new_sid = (SOCKET)(accept(mSID , (sockaddr*)&remote_host , &remote_host_len));
				if ( new_sid > 0 )
				{
					_Add(new_sid , &remote_host);
				}
			}
		}
	}


protected:

	virtual mmlNetworkSocket * _Create (mmlInt32 sid, const mmlInt32 chunk_size, mmlIInputStream * output, mmlIOutputStream * input)
	{
		return new mmlNetworkSocket(sid, chunk_size, output, input);
	}

private:

	mmlBool _Add ( mmlInt32 sid , sockaddr_in * remote )
	{
		mmlMutexGuard guard(mMutex);
		mmlBool added = mmlFalse;
		mmlInt32 count = 0;
		mmlChar remote_host[INET_ADDRSTRLEN] = {0};
		union
		{
			mmlUInt8 ip_digits[4];
			mmlUInt32 ip;
		}u;
		u.ip = remote->sin_addr.s_addr;
		mmlSprintf(remote_host, INET_ADDRSTRLEN, "%d.%d.%d.%d", (mmlUInt32)u.ip_digits[0], (mmlUInt32)u.ip_digits[1], (mmlUInt32)u.ip_digits[2], (mmlUInt32)u.ip_digits[3]);
		mmlAutoPtr < mmlICString > remote_host_str = mmlNewCString(remote_host);

		mmlInt32 chunk_size = 4096;
		mmlAutoPtr < mmlIInputStream > output;
		mmlAutoPtr < mmlIOutputStream > input;
		if ( mHandler->OnConnect(mIface, remote_host_str, &chunk_size, input.getterAddRef(), output.getterAddRef() ) == mmlFalse )
		{
#ifdef MML_WIN
			closesocket(sid);
#else
			close(sid);
#endif
		}
		else
		{
			mmlAutoPtr < mmlNetworkSocket > sock = _Create(sid, chunk_size, output, input);
			for ( std::list < mmlAutoPtr < mmlNetworkSocketPool > > ::iterator pool = mPool.begin(); pool != mPool.end() ; pool ++ , count ++)
			{
				if ( (*pool)->Add(sid , sock) == mmlTrue )
				{
					added = mmlTrue;
					break;
				}
			}
			if ( added == mmlFalse )
			{
				mmlNetworkSocketPool * pool = new mmlNetworkSocketPool();
				mPool.push_front(pool);
				return pool->Add(sid , sock);
			}
		}
		return mmlFalse;
	}

protected:

	std::list < mmlAutoPtr < mmlNetworkSocketPool > > mPool;

private:

	mmlInt32 mSID;

	mmlAutoPtr < mmlICString > mIface;
	
	mmlSharedPtr < mmlIMutex > mMutex;

	mmlSharedPtr < mmlIThread > mThread;

	mmlAutoPtr < mmlIStreamServerHandler > mHandler;

};

class mmlNetworkStreamServer : public mmlNetworkServerBase < mmlINetworkStreamServer >
{
	MML_OBJECT_DECL
};


#endif //TCP_SERVER_H
