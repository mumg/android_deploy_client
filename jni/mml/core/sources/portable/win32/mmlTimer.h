#ifndef MML_TIMER_HEADER_INCLUDED
#define MML_TIMER_HEADER_INCLUDED

#include "mmlITimer.h"
#include <Windows.h>
#include <MMSystem.h>

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

	MMRESULT mRes;
	HANDLE mEvent;

};

#endif //MML_TIME_HEADER_INCLUDED

