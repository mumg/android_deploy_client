

#include "mmlNetworkStreamServer.h"
#include "mml_components.h"
#include <stdlib.h>
#ifndef MML_WIN
#include <sys/poll.h>
#endif




MML_OBJECT_IMPL0(mmlNetworkSocket)
MML_OBJECT_IMPL0(mmlNetworkSocketSignal)

mmlNetworkSocket::mmlNetworkSocket (int sid , const mmlInt32 chunk_size, mmlIInputStream * output, mmlIOutputStream * input )
: mSID(sid), mChunk(mmlAlloc(chunk_size)), mChunkSize(chunk_size),  mSent(0), mOutput(output), mInput(input)
{
#ifdef MML_WIN
	u_long one = 1;
	ioctlsocket(mSID , FIONBIO, &one);
#else
	int one = 1;
	ioctl (mSID, FIONBIO, &one);
#endif
	mMutex = mmlNewObject(MML_OBJECT_UUID(mmlIMutex));

}

void mmlNetworkSocket::SetSignal(mmlInt32 notify_sid)
{
	mSignal = new mmlNetworkSocketSignal(notify_sid, mSID);
	mmlAutoPtr < mmlIStreamControl > input_control;
	mOutput->GetControl(input_control.getterAddRef());
	if (input_control != mmlNULL)
	{
		input_control->SetSignal(mSignal);
	}
}


mmlNetworkSocket::~mmlNetworkSocket ()
{
	Close();
}

void mmlNetworkSocket::Close ()
{
	if ( mSID != 0 )
	{
#ifdef MML_WIN
		closesocket(mSID);
#else
		close(mSID);
#endif
		mSID = 0;
	}
	if ( mChunk != mmlNULL)
	{
		mmlFree(mChunk);
		mChunk = mmlNULL;
	}
	if ( mOutput != mmlNULL )
	{
		mOutput->Close();
		mOutput = mmlNULL;
	}
	if ( mInput != mmlNULL )
	{
		mInput->Close();
		mInput= mmlNULL;
	}
}


mmlBool mmlNetworkSocket::IsActive ()
{
	return mSID != 0 ? mmlTrue : mmlFalse;
}



mmlBool mmlNetworkSocket::Recv ()
{
	mmlMutexGuard g(mMutex);
	mmlInt32 rd = 0;
	if ( ( rd = recv(mSID, (char*)mChunk, mChunkSize, MSG_NOSIGNAL)) <= 0)
	{
		return mmlFalse;
	}
	if ( mInput->Write(rd, mChunk) != rd )
	{
		return mmlFalse;
	}
	return mmlTrue;
}

mmlBool mmlNetworkSocket::Send ()
{
	mmlInt32 actual_size = 0;
	if ( mOutput->IsEmpty() == mmlTrue )
	{
		return mmlFalse;
	}

	if ( (actual_size = mOutput->Read(mChunkSize, mChunk) ) < 0 )
	{
		return mmlFalse;
	}
	if ( actual_size > 0)
	{
		mSent += actual_size;
		mSignal->SetSending(mmlTrue);
		log_network("sent %d\n", actual_size);
		if ( send(mSID, (const char*)mChunk, actual_size, MSG_NOSIGNAL) <= 0 )
		{
			return mmlFalse;
		}
	}
	return mmlTrue;
}


mmlNetworkSocketPool::mmlNetworkSocketPool()
{
	mThread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
	mMutex = mmlNewObject(MML_OBJECT_UUID(mmlIMutex));
	mCondition = mmlNewObject(MML_OBJECT_UUID(mmlICondition));
	mThread->Create(new mmlThreadFunctionProxy<mmlNetworkSocketPool>(this) , PriorityNormal , MML_DEFAULT_STACK_SIZE);
#ifdef MML_WIN
	int server_sid = socket(AF_INET, SOCK_STREAM, 0);

	mNotifyPIPE[0] = 0;
	mNotifyPIPE[1] = 0;

	sockaddr_in sock_addr;
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = 0;
	sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (bind(server_sid, (sockaddr*)&sock_addr, sizeof(sock_addr)) != 0)
	{
		closesocket(server_sid); server_sid = 0;
	}
	if ( server_sid != 0 )
	{
		listen(server_sid, 1);
		struct sockaddr_in sin;
		socklen_t len = sizeof(sin);
		getsockname(server_sid, (struct sockaddr *)&sin, &len);

		mNotifyPIPE[1] = socket(AF_INET, SOCK_STREAM, 0);
		if ( connect(mNotifyPIPE[1], (struct sockaddr *)&sin, len) == 0 )
		{
			sockaddr_in remote_host;
			socklen_t remote_host_len = sizeof(remote_host);
			mNotifyPIPE[0] = accept(server_sid, (sockaddr*)&remote_host, &remote_host_len);

		}
		closesocket(server_sid);

	}
#else
	pipe(mNotifyPIPE);
#endif
}

mmlNetworkSocketPool::~mmlNetworkSocketPool()
{
	mThread->Join();
#ifdef MML_WIN
	closesocket(mNotifyPIPE[0]);
	closesocket(mNotifyPIPE[1]);
#else
	close(mNotifyPIPE[0]);
	close(mNotifyPIPE[1]);
#endif
}

mmlBool mmlNetworkSocketPool::Add ( int sid ,  mmlNetworkSocket * socket )
{
	mmlMutexGuard guard(mMutex);
	if ( mPendingSockets.size() + mSockets.size() < MAX_SOCKETS - 1)
	{
		mCondition->Signal();
		socket->SetSignal(mNotifyPIPE[1]);
		mPendingSockets.push_back(std::pair < int , mmlAutoPtr < mmlNetworkSocket > >(sid , socket));
		return mmlTrue;
	}
	return mmlFalse;
}

float mmlNetworkSocketPool::Score ()
{
	mmlMutexGuard guard(mMutex);
	return float(mPendingSockets.size()) + float(mSockets.size()) / MAX_SOCKETS;
}



void mmlNetworkSocketPool::Execute(mmlIThread *)
{
	mmlInt32 count = 0;
	mmlInt32 read_sids[MAX_SOCKETS];
	mmlInt32 read_count = 0;
	mmlInt32 write_sids[MAX_SOCKETS];
	mmlInt32 write_count = 0;
	mmlInt32 inv_sids[MAX_SOCKETS];
	mmlInt32 inv_count = 0;
	mmlNetworkSocketPoll poll;
	while (IsShutdown() == mmlFalse)
	{
		if ( mSockets.size() > 0 )
		{
			poll.Reset();
			poll.Add(mNotifyPIPE[0], mmlTrue, mmlFalse);
			for (std::map < int , mmlAutoPtr < mmlNetworkSocket > >::iterator sock = mSockets.begin(); IsShutdown() == mmlFalse && sock != mSockets.end();  )
			{
				if (sock->second->IsActive() == mmlTrue &&
					sock->second->mSID != 0)
				{
					poll.Add(sock->second->mSID, mmlTrue, sock->second->IsSending());
					++sock;
				}
				else
				{
					mSockets.erase(sock ++);
				}
			}
			if ( poll.IsEmpty() == mmlFalse )
			{
				if ( poll.Wait(10 , read_sids , &read_count, write_sids, &write_count , inv_sids, &inv_count) == mmlTrue )
				{
					for ( int index = 0 ; index < inv_count ; index ++ )
					{
						std::map < int , mmlAutoPtr < mmlNetworkSocket > >::iterator sock = mSockets.find(inv_sids[index]);
						if ( sock != mSockets.end())
						{
							mSockets.erase(sock);
						}
					}
					for ( int index = 0 ; IsShutdown() == mmlFalse && index < read_count; index ++ )
					{
						if ( read_sids[index] == mNotifyPIPE[0])
						{
							mmlInt32 sid = 0;
							if ( recv(mNotifyPIPE[0], (char*)&sid, sizeof(sid) , MSG_NOSIGNAL) == sizeof(sid))
							{
								std::map < int, mmlAutoPtr < mmlNetworkSocket > >::iterator sock = mSockets.find(sid);
								if (sock->second->IsSending() == mmlFalse)
								{
									if (sock->second->mSID > 0)
									{
										if (sock->second->Send() == mmlFalse)
										{
											mSockets.erase(sock++);
										}
									}
								}
							}
						}
						else
						{
							std::map < int, mmlAutoPtr < mmlNetworkSocket > >::iterator sock = mSockets.find(read_sids[index]);
							if (sock != mSockets.end())
							{
								if (sock->second->mSID > 0)
								{
									if (sock->second->Recv() == mmlFalse)
									{
										sock->second->Close();
										mSockets.erase(sock);
									}
								}
							}
						}

					}
					for ( int index = 0 ; index < write_count ; index ++ )
					{
						std::map < int , mmlAutoPtr < mmlNetworkSocket > >::iterator sock = mSockets.find(write_sids[index]);
						if ( sock != mSockets.end())
						{
							sock->second->SendComplete();
						}
					}
				}
				for ( std::map < int ,  mmlAutoPtr < mmlNetworkSocket > >::iterator sock = mSockets.begin(); sock != mSockets.end(); )
				{
					if ( sock->second->IsSending() == mmlFalse )
					{
						if ( sock->second->mSID > 0 )
						{
							if ( sock->second->Send() == mmlFalse )
							{
								mSockets.erase(sock++);
								continue;
							}
						}
					}
					sock ++;
				}

			}
		}
		else
		{
			mCondition->Wait(1000);
		}
		mmlMutexGuard guard(mMutex);
		for ( std::list < std::pair < int , mmlAutoPtr < mmlNetworkSocket> > >::iterator sock = mPendingSockets.begin(); IsShutdown() == mmlFalse && sock != mPendingSockets.end(); sock ++ )
		{
			mSockets[sock->first] = sock->second;
		}
		mPendingSockets.clear();
	}
}




MML_OBJECT_IMPL0(mmlNetworkSocketPool)


MML_OBJECT_IMPL2(mmlNetworkStreamServer, mmlINetworkStreamServer, mmlIStreamServer)






