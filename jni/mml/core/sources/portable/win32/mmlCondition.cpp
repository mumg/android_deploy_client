#include "mmlCondition.h"
#include "mml_atomic.h"

MML_OBJECT_IMPL1(mmlCondition, mmlICondition)

mmlCondition::mmlCondition()
:mSignalCounter(0), mHandle(CreateEvent (NULL , FALSE , FALSE , NULL))
{

}

mmlCondition::~mmlCondition()
{
	CloseHandle(mHandle);
}

mmlBool mmlCondition::Wait ( const mmlInt32 timeout )
{
	mmlInt32 signals_pending = mmlAtomicGet(&mSignalCounter);
	if ( signals_pending > 0 )
	{
		mmlAtomicAdd(&mSignalCounter , -1);
		return mmlTrue;
	}
	else
	{
		if ( WaitForSingleObject(mHandle , timeout == 0 ? INFINITE : timeout) == WAIT_OBJECT_0 )
		{
			mmlAtomicAdd(&mSignalCounter , -1);
			return mmlTrue;
		}
	}
	return mmlFalse;
}

mmlBool mmlCondition::Signal ()
{
	mmlInt32 signals_pending = mmlAtomicAdd(&mSignalCounter, 1);
	if ( signals_pending == 1 )
	{
		SetEvent(mHandle);
	}
	return mmlTrue;
}

