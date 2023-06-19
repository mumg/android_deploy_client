#include "mml_memory.h"
#include "mml_strutils.h"
#include <Windows.h>
#include <time.h>
#include "mml_time.h"


mml_core void mml_clock_gettime_realtime(mml_timespec * ts)
{
	FILETIME file_time;
	GetSystemTimeAsFileTime (&file_time);
	ULARGE_INTEGER _100ns = {file_time.dwLowDateTime,
		file_time.dwHighDateTime};
	_100ns.QuadPart -= 0x19db1ded53e8000;
	ts->tv_sec = _100ns.QuadPart / 10000000;
	ts->tv_nsec = (_100ns.QuadPart % 10000000) * 100;
}

mml_core void mml_clock_gettime_monotonic(mml_timespec * ts)
{
	mml_clock_gettime_realtime(ts);
}

mml_core void mml_clock_gettime_uptime (mml_timespec * ts)
{
	mmlUInt64 milis = GetTickCount64();
	ts->tv_sec = milis / 1000;
	ts->tv_nsec = (milis % 1000) * 1000000;
}

void __mml_localtime(struct tm * _Tm, const time_t * _Time)
{
	localtime_s(_Tm, _Time);
}
void __mml_gmtime(struct tm * _Tm, const time_t * _Time)
{
	gmtime_s(_Tm, _Time);
}
