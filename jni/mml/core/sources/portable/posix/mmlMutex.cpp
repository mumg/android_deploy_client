
#include "mmlMutex.h"

MML_OBJECT_IMPL1(mmlMutex, mmlIMutex)

mmlMutex::mmlMutex ()
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr , PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mMutex, &attr); 
	pthread_mutexattr_destroy(&attr);
}

mmlMutex::~mmlMutex()
{
	pthread_mutex_destroy(&mMutex);
}

mmlBool mmlMutex::Lock ()
{
	pthread_mutex_lock(&mMutex);
	return mmlTrue;
}

mmlBool mmlMutex::UnLock ()
{
	pthread_mutex_unlock(&mMutex);
	return mmlTrue;
}
