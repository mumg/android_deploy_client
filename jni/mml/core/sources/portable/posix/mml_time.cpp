#include "mml_time.h"
#include "mml_memory.h"
#include "mml_strutils.h"
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>


#define MML_INT64_MAX_LEN 20


mml_core void mml_clock_gettime_realtime(mml_timespec * ts)
{
	struct timespec _ts;
	clock_gettime(CLOCK_REALTIME, &_ts);
	ts->tv_sec = _ts.tv_sec;
	ts->tv_nsec = _ts.tv_nsec;
}

mml_core void mml_clock_gettime_monotonic(mml_timespec * ts)
{
	struct timespec _ts;
	clock_gettime(CLOCK_MONOTONIC, &_ts);
	ts->tv_sec = _ts.tv_sec;
	ts->tv_nsec = _ts.tv_nsec;
}

mml_core void mml_clock_gettime_uptime(mml_timespec * ts)
{
	struct timespec _ts;
	clock_gettime(CLOCK_BOOTTIME , &_ts);
	ts->tv_sec = _ts.tv_sec;
	ts->tv_nsec = _ts.tv_nsec;
}

void __mml_localtime(struct tm * _Tm, const time_t * _Time)
{
	localtime_r(_Time, _Tm);
}
void __mml_gmtime(struct tm * _Tm, const time_t * _Time)
{
	gmtime_r(_Time, _Tm);
}
