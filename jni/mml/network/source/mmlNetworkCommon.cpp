#ifdef MML_WIN
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#endif

#include "mmlNetworkCommon.h"
#ifdef MML_SSL

#include <mbedtls/ssl.h>
#include <mbedtls/certs.h>
#include <mbedtls/x509.h>

#include <mbedtls/error.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/sha256.h>

#endif

#ifdef MML_WIN
#pragma comment(lib, "Ws2_32.lib")
#define MSG_NOSIGNAL 0
static mmlBool mml_network_init_flag = mmlFalse;

#endif

mmlBool mml_network_init ()
{
#ifdef MML_WIN
	if ( mml_network_init_flag == mmlFalse )
	{
		mml_network_init_flag = mmlTrue;
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2,2), &wsaData);
	}
#endif
	return mmlTrue;
}

static mmlBool mml_network_init_ssl_flag = mmlFalse;

mmlBool mml_network_ssl_init ()
{
	if ( mml_network_init_ssl_flag == mmlFalse )
	{
#ifdef MML_SSL
#endif
		mml_network_init_ssl_flag = mmlTrue;
	}
	return mmlTrue;
}


mmlNetworkSocketPoll::mmlNetworkSocketPoll()
{
	Reset();
}

mmlNetworkSocketPoll::~mmlNetworkSocketPoll()
{

}

void mmlNetworkSocketPoll::Reset ()
{
#ifdef MML_WIN
	mReadCount = 0;
	mWriteCount = 0;
	mErrorCount = 0;
	FD_ZERO(&mReadFd);
	FD_ZERO(&mWriteFd);
	FD_ZERO(&mErrorFd);
#else
	mCount = 0;
#endif
}

mmlBool mmlNetworkSocketPoll::Add (const mmlInt32 sid, const mmlBool track_read, const mmlBool track_write)
{
#ifdef MML_WIN
	if ( mReadCount < MAX_SOCKETS )
	{
		if ( track_read == mmlTrue )
		{
			mReadSIDs[mReadCount] = sid;
			mReadCount ++;
			FD_SET(sid , &mReadFd);
		}
		if ( track_write == mmlTrue )
		{
			mWriteSIDs[mWriteCount] = sid;
			mWriteCount ++;
			FD_SET(sid , &mWriteFd);
		}
		mErrorSIDs[mErrorCount] = sid;
		mErrorCount ++;
		FD_SET(sid , &mErrorFd);
		return mmlTrue;
	}
	return mmlFalse;
#else
	if ( mCount < MAX_SOCKETS )
	{
		mFd[mCount].fd = sid;
		mFd[mCount].events = (track_read == mmlTrue ? POLLIN : 0) | (track_write == mmlTrue ? POLLOUT : 0 ) | POLLERR;
		mFd[mCount].revents = 0;
		mCount ++;
		return mmlTrue;
	}
	return mmlFalse;
#endif
}

mmlBool mmlNetworkSocketPoll::Wait 
	( 
		const mmlInt32 timeout , 
		mmlInt32 * fds_read   , mmlInt32 * count_read,
		mmlInt32 * fds_write  , mmlInt32 * count_write,
		mmlInt32 * fds_invalid, mmlInt32 * count_invalid
	)
{
	if ( count_read != mmlNULL )*count_read = 0;
	if ( count_write != mmlNULL )*count_write = 0;
	if ( count_invalid != mmlNULL )*count_invalid = 0;
#ifdef MML_WIN
	if ( mReadCount > 0 && (fds_read == mmlNULL || count_read == mmlNULL ) )
	{
		return mmlFalse;
	}
	if ( mWriteCount > 0 && (fds_write == mmlNULL || count_write == mmlNULL))
	{
		return mmlFalse;
	}
	timeval tm;
	tm.tv_sec =  timeout / 1000;
	tm.tv_usec = (timeout % 1000) * 1000;
	mmlInt32 signaled = select ( 0 , mReadCount > 0 ? &mReadFd : mmlNULL , mWriteCount > 0 ? &mWriteFd : mmlNULL , &mErrorFd , &tm );

	if ( signaled > 0 )
	{

		for ( mmlInt32 index = 0 ; index < mReadCount; index ++ )
		{
			if ( FD_ISSET(mReadSIDs[index] , &mReadFd) )
			{
				fds_read[*count_read] = mReadSIDs[index];
				(*count_read)++;
			}
		}
		for ( mmlInt32 index = 0 ; index < mErrorCount; index ++ )
		{
			if ( FD_ISSET(mErrorSIDs[index] , &mErrorFd) )
			{
				fds_invalid[*count_invalid] = mErrorSIDs[index];
				(*count_invalid)++;
			}
		}
		for ( mmlInt32 index = 0 ; index < mWriteCount; index ++ )
		{
			if ( FD_ISSET(mWriteSIDs[index] , &mWriteFd) )
			{
				fds_write[*count_write] = mWriteSIDs[index];
				(*count_write)++;
			}
		}
		return mmlTrue;
	}
	return mmlFalse;
#else
	mmlBool result = mmlFalse;
	mmlInt32 rc = poll(mFd, mCount , timeout);
		if ( rc > 0 )
	{
		for ( mmlInt32 index = 0 ; index < mCount; index ++ )
		{
			if ( (mFd[index].revents & POLLIN)  != 0 ||
				(mFd[index].revents & POLLPRI) != 0)
			{
				fds_read[*count_read] = mFd[index].fd;
				(*count_read)++;
				result = mmlTrue;
			}
			if ( (mFd[index].revents & POLLOUT) != 0 )
			{
				fds_write[*count_write] = mFd[index].fd;
				(*count_write)++;
				result = mmlTrue;
			}
			if (
#if defined(POLLRDHUP)				
				(mFd[index].revents & POLLRDHUP ) != 0 ||
#endif
				(mFd[index].revents & POLLERR ) != 0 ||
				(mFd[index].revents & POLLHUP ) != 0 ||
				(mFd[index].revents & POLLNVAL) != 0
				)
			{
				fds_invalid[*count_invalid] = mFd[index].fd;
				(*count_invalid)++;
				result = mmlTrue;
			}
		}
	}
	return result;
#endif
}

mmlBool mmlNetworkSocketPoll::IsEmpty ()
{
#ifdef MML_WIN
	return (mReadCount == 0 && mWriteCount == 0) ? mmlTrue : mmlFalse;
#else
	return mCount == 0 ? mmlTrue : mmlFalse;
#endif
}

