#ifndef MML_TIME_INTERFACE_HEADER_INCLUDED
#define MML_TIME_INTERFACE_HEADER_INCLUDED

#include <time.h>
#include "mmlIObject.h"

typedef struct
{
	time_t tv_sec;
	long tv_nsec;
} mml_timespec; 

typedef struct  
{
	mmlInt32 tm_msec;
	mmlInt32 tm_sec;     /* seconds after the minute - [0,59] */
	mmlInt32 tm_min;     /* minutes after the hour - [0,59] */
	mmlInt32 tm_hour;    /* hours since midnight - [0,23] */
	mmlInt32 tm_mday;    /* day of the month - [1,31] */
	mmlInt32 tm_mon;     /* months since January - [1,12] */
	mmlInt32 tm_year;    /* years since 0 */
	mmlInt32 tm_wday;    /* days since Sunday - [0,6] */
	mmlInt32 tm_yday;    /* days since January 1 - [0,365] */
	mmlInt32 tm_isdst;   /* daylight savings time flag */
}mml_tm;

#define MML_LOCALTIME_OFFSET_AUTO (mmlInt32)0xFF000000

mml_core void mml_clock_gettime_realtime(mml_timespec * ts);

mml_core void mml_clock_gettime_monotonic (mml_timespec * ts);

mml_core void mml_clock_gettime_uptime (mml_timespec * ts);

mml_core void mml_clock_add(mml_timespec * ts, const int milliseconds);

mml_core void mml_clock_sub(mml_timespec * ts, const int milliseconds);

mml_core mmlInt32 mml_clock_cmp(mml_timespec * ts1 , mml_timespec * ts2);

mml_core mmlInt32 mml_clock_tm_cmp(mml_tm * tm1 , mml_tm * tm2);

mml_core mmlInt32 mml_clock_tm_cmp_date(mml_tm * tm1 , mml_tm * tm2);

mml_core mmlInt32 mml_clock_tm_cmp_time(mml_tm * tm1 , mml_tm * tm2);

mml_core mmlInt64 mml_clock_diff (mml_timespec * ts1 , mml_timespec * ts2);

mml_core void mml_clock_localtime (mml_timespec * ts , mml_tm * tm, const mmlInt32 offset = MML_LOCALTIME_OFFSET_AUTO);

mml_core void mml_clock_gmtime (mml_timespec * ts , mml_tm * tm);

mml_core mmlBool mml_clock_strptime (const mmlChar * str , const mmlChar * formatter , mml_tm * tm);

mml_core void mml_clock_mktime(mml_tm * tm, mml_timespec * ts, const mmlInt32 offset = MML_LOCALTIME_OFFSET_AUTO);

mml_core mmlInt32 mml_clock_strftime(const mmlChar * formatter , mml_tm * ts, mmlChar * buffer , const mmlInt32 size );

#endif //MML_TIME_INTERFACE_HEADER_INCLUDED


