#include "mmlTimer.h"
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>

/**/
MML_OBJECT_IMPL1( mmlTimer , mmlITimer )

static timespec mml_time_add ( struct timespec & t1 , const mmlInt64 timeout )
{
	struct timespec res = t1;
	res.tv_nsec += timeout;
        if ( res.tv_nsec > 1000000000 )
	{
		res.tv_nsec -= 1000000000;
		res.tv_sec ++;
	}
	return res;
}


static mmlInt64 mml_time_sub ( struct timespec & t1 , struct timespec & t2 )
{
	mmlInt64 res = 
	     ((t1.tv_sec - t2.tv_sec) * 1000000000) + t1.tv_nsec - t2.tv_nsec;
       return res; 
}

mmlTimer::mmlTimer ()
: mPeriodical(mmlFalse) , mPeriod(0)
{
}

mmlTimer::~mmlTimer()
{
}

mmlBool mmlTimer::Setup ( const mmlInt32 timeout,
			              const mmlBool periodical )
{
	struct timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	mFireTime = mml_time_add(t , timeout * 1000000); 
	mPeriod = timeout;
	mPeriodical = periodical;
	return mmlTrue;
}

mmlBool mmlTimer::Wait ()
{
	for (;;)
	{
		struct timespec t;
		clock_gettime(CLOCK_REALTIME, &t);
                mmlInt64 delta = mml_time_sub ( mFireTime , t );
		if ( delta <= 0 )
		{
			mFireTime = mml_time_add(mFireTime , mPeriod * 1000000);
			break;
		}
		usleep(100);
	}
	return mmlTrue;
}

