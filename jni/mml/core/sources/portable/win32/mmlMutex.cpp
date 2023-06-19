#include "mmlMutex.h"

MML_OBJECT_IMPL1(mmlMutex, mmlIMutex)

mmlMutex::mmlMutex ()
{
	InitializeCriticalSection(&mMutex);
}

mmlMutex::~mmlMutex()
{
	DeleteCriticalSection(&mMutex);
}

mmlBool mmlMutex::Lock ()
{
	EnterCriticalSection(&mMutex);
	return mmlTrue;
}

mmlBool mmlMutex::UnLock ()
{
	LeaveCriticalSection(&mMutex);
	return mmlTrue;
}
