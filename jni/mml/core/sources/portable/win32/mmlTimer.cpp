#include "mmlTimer.h"

#pragma comment(lib , "winmm.lib")

MML_OBJECT_IMPL1( mmlTimer , mmlITimer )

mmlTimer::mmlTimer ()
:mRes(mmlNULL)
{
	mEvent = CreateEvent (NULL , FALSE , FALSE , NULL);
}

mmlTimer::~mmlTimer()
{
	if ( mRes != mmlNULL )
	{
		timeKillEvent(mRes);
		mRes = mmlNULL;
	}
	CloseHandle(mEvent);

}

mmlBool mmlTimer::Setup ( const mmlInt32 timeout,
			              const mmlBool periodical )
{
	mRes = timeSetEvent (timeout , 0 , (LPTIMECALLBACK)mEvent , 0 , TIME_CALLBACK_EVENT_PULSE | (periodical == mmlTrue ? TIME_PERIODIC : TIME_ONESHOT) );
	if ( mRes != mmlNULL )
	{
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlTimer::Wait ()
{
	if ( WaitForSingleObject(mEvent , INFINITE) == WAIT_OBJECT_0 )
	{
		return mmlTrue;
	}
	return mmlFalse;
}

