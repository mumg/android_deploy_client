#ifndef MML_PERFORMACE_COUNTER_HEADER_INCLUDED
#define MML_PERFORMACE_COUNTER_HEADER_INCLUDED

#include "mmlIPerformanceCounter.h"


class mmlPerformanceCounter : public mmlIPerformanceCounter
{
	MML_OBJECT_DECL;
public:
	mmlPerformanceCounter();
	mmlBool Start ();
	mmlBool Stop (mmlInt32 * time);
	mmlBool GetReport (mmlInt64 * active_time,
		               mmlInt64 * total_time );
private:
	mmlInt64 mTime;
	mmlInt64 mCreationTime;
	mmlInt64 mActiveTime;
	mmlBool mStarted;
};



#endif //MML_PERFORMACE_COUNTER_HEADER_INCLUDED
