#ifndef MML_TIMER_HEADER_INCLUDED
#define MML_TIMER_HEADER_INCLUDED

#include "mmlITimer.h"
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

class mmlTimer : public mmlITimer
{
	MML_OBJECT_DECL;
public:
	mmlTimer ();

	~mmlTimer();

	mmlBool Setup ( const mmlInt32 timeout,
		            const mmlBool periodical );

	mmlBool Wait ();

private:

	mmlBool mPeriodical;
	struct timespec mFireTime;
	mmlInt32 mPeriod;

};

#endif //MML_TIME_HEADER_INCLUDED

