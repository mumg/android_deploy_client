#ifndef MML_NETWORK_COMMON_HEADER_INCLUDED
#define MML_NETWORK_COMMON_HEADER_INCLUDED

#include "mmlIObject.h"
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
#include <poll.h>
#include <sys/un.h>
#endif
mmlBool mml_network_init ();

mmlBool mml_network_ssl_init ();

#define MAX_SOCKETS FD_SETSIZE

#ifdef MML_WIN
#define MSG_NOSIGNAL 0
#else
#define closesocket close
#endif

class mmlNetworkSocketPoll
{
public:

	mmlNetworkSocketPoll();

	~mmlNetworkSocketPoll();

	void Reset ();

	mmlBool Add 
		(
			const mmlInt32 sid, const mmlBool track_read, const mmlBool track_write
		);

	mmlBool Wait 
		( 
			const mmlInt32 timeout , 
			mmlInt32 * fds_read   , mmlInt32 * count_read,
			mmlInt32 * fds_write  , mmlInt32 * count_write,
			mmlInt32 * fds_invalid, mmlInt32 * count_invalid
		);

	mmlBool IsEmpty ();

private:

#ifdef MML_WIN
	mmlInt32 mReadCount;
	mmlInt32 mWriteCount;
	mmlInt32 mErrorCount;
	mmlInt32 mReadSIDs[MAX_SOCKETS];
	mmlInt32 mWriteSIDs[MAX_SOCKETS];
	mmlInt32 mErrorSIDs[MAX_SOCKETS];
	fd_set mReadFd;
	fd_set mWriteFd;
	fd_set mErrorFd;
#else
	mmlInt32 mCount;
	struct pollfd mFd[MAX_SOCKETS];
#endif
};

#endif
