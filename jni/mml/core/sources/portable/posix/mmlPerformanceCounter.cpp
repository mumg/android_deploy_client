#include "mmlPerformanceCounter.h"

MML_OBJECT_IMPL1(mmlPerformanceCounter , mmlIPerformanceCounter)

mmlPerformanceCounter::mmlPerformanceCounter()
:mStarted(mmlFalse)
{
	clock_gettime(CLOCK_REALTIME, &mCreationTime);
}

mmlBool mmlPerformanceCounter::Start()
{
	clock_gettime(CLOCK_REALTIME, &mTime);
	mStarted = mmlTrue;
	return mmlTrue;
}

mmlBool mmlPerformanceCounter::Stop(mmlInt32 * time)
{
	if ( mStarted == mmlTrue )
	{
		mStarted = mmlFalse;
		struct timespec t;
		clock_gettime(CLOCK_REALTIME, &t);
		mmlInt64 diff = (((t.tv_sec - mTime.tv_sec) * 1000000000) + t.tv_nsec - mTime.tv_nsec)/1000;
        mActiveTime += diff;
		if (time) *time = diff;
		return mmlTrue;
	
	}
	return mmlFalse;
}

mmlBool mmlPerformanceCounter::GetReport (mmlInt64 * active_time,
				                          mmlInt64 * total_time )
{
    struct timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	mmlInt64 live_time = (((t.tv_sec - mCreationTime.tv_sec) * 1000000000) + t.tv_nsec - mCreationTime.tv_nsec)/1000;

	if ( active_time ) *active_time = mActiveTime;
	if ( total_time ) *total_time = live_time;
	return mmlTrue;
}