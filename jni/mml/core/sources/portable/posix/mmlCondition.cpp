#include "mmlCondition.h"
#include "mml_atomic.h"

MML_OBJECT_IMPL1(mmlCondition, mmlICondition)

mmlCondition::mmlCondition()
:mSignalCounter(0)
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr , PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mMutex, &attr); 
	pthread_mutexattr_destroy(&attr);
	pthread_cond_init(&mHandle, NULL);
}

mmlCondition::~mmlCondition()
{
	pthread_cond_destroy(&mHandle);
	pthread_mutex_destroy(&mMutex);
}

mmlBool mmlCondition::Wait ( const mmlInt32 timeout )
{
	mmlBool result = mmlFalse;
	mmlInt32 signals_pending = mmlAtomicGet(&mSignalCounter);
	if ( signals_pending > 0 )
	{
		mmlAtomicAdd(&mSignalCounter , -1);
		result = mmlTrue;
	}
	else
	{
		pthread_mutex_lock(&mMutex);
		timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += timeout / 1000;
		ts.tv_nsec += 1000000 * (timeout % 1000);
		if (ts.tv_nsec > 1000000000L) 
		{
            ts.tv_sec++ ;  ts.tv_nsec = ts.tv_nsec - 1000000000L ;
        }
		int res = pthread_cond_timedwait(&mHandle , &mMutex , &ts);
		if ( res == 0 )
		{
			mmlAtomicAdd(&mSignalCounter , -1);
			result = mmlTrue;
		}
		pthread_mutex_unlock(&mMutex);
	}
	return result;
}

mmlBool mmlCondition::Signal ()
{
	mmlInt32 signals_pending = mmlAtomicAdd(&mSignalCounter, 1);
	if ( signals_pending == 1 )
	{
		pthread_mutex_lock(&mMutex);
		pthread_cond_signal(&mHandle);
		pthread_mutex_unlock(&mMutex);
	}
	return mmlTrue;
}

