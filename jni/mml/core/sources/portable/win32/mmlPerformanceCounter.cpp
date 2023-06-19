#include "mmlPerformanceCounter.h"
#include <Windows.h>

MML_OBJECT_IMPL1(mmlPerformanceCounter , mmlIPerformanceCounter)

mmlPerformanceCounter::mmlPerformanceCounter()
:mStarted(mmlFalse), mCreationTime(0) , mActiveTime(0)
{
	QueryPerformanceCounter((LARGE_INTEGER*)&mCreationTime);
}

mmlBool mmlPerformanceCounter::Start()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&mTime);
	mStarted = mmlTrue;
	return mmlTrue;
}

mmlBool mmlPerformanceCounter::Stop(mmlInt32 * time)
{
	if ( mStarted == mmlFalse )
	{
		return mmlFalse;
	}
	mStarted = mmlFalse;
	LARGE_INTEGER pc0 = *((LARGE_INTEGER*)&mTime), pc1;
	QueryPerformanceCounter(&pc1);
	pc1.QuadPart-=pc0.QuadPart;
	if( QueryPerformanceFrequency( &pc0 ) ) 
	{
		mmlInt32 diff = mmlInt32(1000000.0 * ((double)pc1.QuadPart) / ((double)pc0.QuadPart));
		mActiveTime += diff;
		if (time) *time = diff;
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlPerformanceCounter::GetReport (mmlInt64 * active_time,
				                          mmlInt64 * total_time )
{
	LARGE_INTEGER pc0 = *((LARGE_INTEGER*)&mCreationTime), pc1;
	QueryPerformanceCounter(&pc1);
	pc1.QuadPart-=pc0.QuadPart;
	QueryPerformanceFrequency( &pc0 );
	if ( active_time ) *active_time = mActiveTime;
	if ( total_time ) *total_time = 1000000.0 * ((double)pc1.QuadPart) / ((double)pc0.QuadPart);
	return mmlTrue;
}