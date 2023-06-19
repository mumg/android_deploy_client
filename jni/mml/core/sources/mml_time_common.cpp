#include "mml_memory.h"
#include "mml_strutils.h"
#include <time.h>
#include "mml_time.h"
#include <locale>

#define MML_INT64_MAX_LEN 20
#define TM_YEAR_BASE 1900

void __mml_localtime(struct tm * _Tm, const time_t * _Time);
void __mml_gmtime(struct tm * _Tm, const time_t * _Time);


mml_core void mml_clock_add(mml_timespec * ts, const mmlInt32 milliseconds)
{
	ts->tv_nsec += (milliseconds % 1000) * 1000000;
	if ( ts->tv_nsec >= 1000000000)
	{
		ts->tv_nsec -= 1000000000;
		ts->tv_sec ++;
	}
	ts->tv_sec += milliseconds / 1000;
}

mml_core void mml_clock_sub(mml_timespec * ts, const mmlInt32 milliseconds)
{
	ts->tv_nsec -= (milliseconds % 1000) * 1000000;
	if ( ts->tv_nsec < 0)
	{
		ts->tv_nsec += 1000000000;
		ts->tv_sec --;
	}
	ts->tv_sec -= milliseconds / 1000;
}

mml_core mmlInt32 mml_clock_cmp(mml_timespec * ts1 , mml_timespec * ts2)
{
	if ( ts1->tv_sec == ts2->tv_sec)
	{
		if ( ts1->tv_nsec == ts2->tv_nsec)
		{
			return 0;
		}
		else if ( ts1->tv_nsec > ts2->tv_nsec)
		{
			return 1;
		}
		return -1;	
	}
	else if ( ts1->tv_sec > ts2->tv_sec)
	{
		return 1;
	}
	return -1;
}

mml_core mmlInt32 mml_clock_tm_cmp(mml_tm * tm1 , mml_tm * tm2)
{
	mmlInt32 diff = 0;
	diff = tm1->tm_year - tm2->tm_year;
	if ( diff == 0 )
	{
		diff = tm1->tm_mon - tm2->tm_mon;
		if ( diff == 0 )
		{
			diff = tm1->tm_mday - tm2->tm_mday;
			if ( diff == 0 )
			{
				diff = tm1->tm_hour - tm2->tm_hour;
				if ( diff == 0 )
				{
					diff = tm1->tm_min - tm2->tm_min;
					if ( diff == 0 )
					{
						diff = tm1->tm_sec - tm2->tm_sec;
						if ( diff == 0 )
						{
							diff = tm1->tm_msec - tm2->tm_msec;
						}
					}
				}
			}
		}
	} 
	
	if ( diff == 0 ) return 0;
	if ( diff > 0 ) return 1;
	return -1;
}

mml_core mmlInt32 mml_clock_tm_cmp_date(mml_tm * tm1 , mml_tm * tm2)
{
	mmlInt32 diff = 0;
	diff = tm1->tm_year - tm2->tm_year;
	if ( diff == 0 )
	{
		diff = tm1->tm_mon - tm2->tm_mon;
		if ( diff == 0 )
		{
			diff = tm1->tm_mday - tm2->tm_mday;			
		}
	} 

	if ( diff == 0 ) return 0;
	if ( diff > 0 ) return 1;
	return -1;
}

mml_core mmlInt32 mml_clock_tm_cmp_time(mml_tm * tm1 , mml_tm * tm2)
{
	mmlInt32 diff = 0;
	
	diff = tm1->tm_hour - tm2->tm_hour;
	if ( diff == 0 )
	{
		diff = tm1->tm_min - tm2->tm_min;
		if ( diff == 0 )
		{
			diff = tm1->tm_sec - tm2->tm_sec;
			if ( diff == 0 )
			{
				diff = tm1->tm_msec - tm2->tm_msec;
			}
		}
	}
	
	if ( diff == 0 ) return 0;
	if ( diff > 0 ) return 1;
	return -1;
}

mml_core mmlInt64 mml_clock_diff (mml_timespec * ts1 , mml_timespec * ts2)
{
	return mmlInt64(ts1->tv_sec - ts2->tv_sec) * 1000 +
		   mmlInt64(ts1->tv_nsec - ts2->tv_nsec) / 1000000;
}

mml_core void mml_clock_localtime (mml_timespec * ts , mml_tm * tm , const mmlInt32 offset)
{
	time_t time = ts->tv_sec;
	struct tm sys_tm;
	if ( offset == MML_LOCALTIME_OFFSET_AUTO )
	{
		__mml_localtime(&sys_tm, &time);
	}
	else
	{
		time -= offset;
		__mml_gmtime(&sys_tm, &time);
	}
	tm->tm_msec = ts->tv_nsec / 1000000;
	tm->tm_sec = sys_tm.tm_sec;
	tm->tm_min = sys_tm.tm_min;
	tm->tm_hour = sys_tm.tm_hour;
	tm->tm_mday = sys_tm.tm_mday;
	tm->tm_wday = sys_tm.tm_wday;
	tm->tm_yday = sys_tm.tm_yday;
	tm->tm_mon = sys_tm.tm_mon + 1;
	tm->tm_year = sys_tm.tm_year + TM_YEAR_BASE;
	tm->tm_isdst = sys_tm.tm_isdst;
}

mml_core void mml_clock_gmtime (mml_timespec * ts , mml_tm * tm)
{
	time_t time = ts->tv_sec;
	struct tm sys_tm;
	__mml_gmtime(&sys_tm, &time);
	tm->tm_msec = ts->tv_nsec / 1000000;
	tm->tm_sec = sys_tm.tm_sec;
	tm->tm_min = sys_tm.tm_min;
	tm->tm_hour = sys_tm.tm_hour;
	tm->tm_mday = sys_tm.tm_mday;
	tm->tm_wday = sys_tm.tm_wday;
	tm->tm_yday = sys_tm.tm_yday;
	tm->tm_mon = sys_tm.tm_mon + 1;
	tm->tm_year = sys_tm.tm_year + TM_YEAR_BASE;
	tm->tm_isdst = sys_tm.tm_isdst;
}


static const mmlChar *day[7] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
	"Friday", "Saturday"
};
static const mmlChar *abday[7] = {
	"Sun","Mon","Tue","Wed","Thu","Fri","Sat"
};
static const mmlChar *mon[12] = {
	"January", "February", "March", "April", "May", "June", "July",
	"August", "September", "October", "November", "December"
};
static const mmlChar *abmon[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
static const mmlChar *am_pm[2] = {
	"AM", "PM"
};


#define MONTHS_NUMBER 12

static const mmlInt32 MONTHDAYS[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static mmlInt32 is_leap_year(const mmlInt32 year) 
{
	if (year % 400 == 0) { return 1; }
	else if (year % 100 == 0) { return 0; }
	else if (year % 4 == 0) { return 1; }
	else { return 0; }
}


static mmlInt32 __mml_add(mmlChar ** pt , const mmlChar *str, mmlInt32 * gsize)
{
	for (;; ++(*pt), --(*gsize)) {
		if (!(*gsize))
			return(0);
		if (!(*(*pt) = *str++))
			return(1);
	}
}

static mmlInt32 __mml_conv(mmlChar ** pt ,mmlInt32 n, mmlInt32 digits, mmlInt32 pad, mmlInt32 special, mmlInt32 * gsize) 
{
	mmlChar buf[10] = {0};
	mmlChar *p;
	for (p = buf + sizeof(buf) - 2; n > 0 && p > buf; n /= 10, --digits)
		*p-- = n % 10 + '0';
	while (special == 0 && p > buf && digits-- > 0)
		*p-- = pad;
	return(__mml_add(pt, ++p, gsize));
}


#define ALT_E			0x01
#define ALT_O			0x02
#define	LEGAL_ALT(x)		{ if (alt_format & ~(x)) return (0); }

static mmlInt32 __mml_conv_num(const mmlChar **buf, mmlInt32 *dest, mmlInt32 llim, mmlInt32 ulim)
{
	mmlInt32 result = 0;

	/* The limit also determines the number of valid digits. */
	mmlInt32 rulim = ulim;

	if (**buf < '0' || **buf > '9')
		return (0);

	do {
		result *= 10;
		result += *(*buf)++ - '0';
		rulim /= 10;
	} while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

	if (result < llim || result > ulim)
		return (0);

	*dest = result;
	return (1);
}

static mmlInt32 __mml_secs(mmlChar ** pt ,mml_tm *t, mmlInt32 * gsize)
{
	mmlChar buf[16] = {0};
	mml_timespec s;
	mmlChar *p;

	mml_clock_mktime(t, &s);

	for (p = buf + sizeof(buf) - 2; s.tv_sec > 0 && p > buf; s.tv_sec /= 10)
		*p-- = s.tv_sec % 10 + '0';
	return(__mml_add(pt , ++p, gsize));
}


static mmlChar * __mml_strptime(const mmlChar *buf, const mmlChar *fmt, mml_tm *tm)
{
	mmlChar c;
	const mmlChar *bp;
	mmlInt32 len = 0;
	mmlInt32 alt_format, i, split_year = 0;

	bp = buf;

	while ((c = *fmt) != '\0') {
		/* Clear `alternate' modifier prior to new conversion. */
		alt_format = 0;

		/* Eat up white-space. */
		if (isspace(c)) {
			while (isspace(*bp))
				bp++;

			fmt++;
			continue;
		}
				
		if ((c = *fmt++) != '%')
			goto literal;


again:	
		c = *fmt++;
		switch (c) {
		case '%':	/* "%%" is converted to "%". */
literal:
			if (c != *bp++)
				return (0);
			break;

		/*
		 * "Alternative" modifiers. Just set the appropriate flag
		 * and start over again.
		 */
		case 'E':	/* "%E?" alternative conversion modifier. */
			LEGAL_ALT(0);
			alt_format |= ALT_E;
			goto again;

		case 'O':	/* "%O?" alternative conversion modifier. */
			LEGAL_ALT(0);
			alt_format |= ALT_O;
			goto again;
			
		/*
		 * "Complex" conversion rules, implemented through recursion.
		 */
		case 'c':	/* Date and time, using the locale's format. */
			LEGAL_ALT(ALT_E);
			if (!(bp = __mml_strptime(bp, "%x %X", tm)))
				return (0);
			break;

		case 'D':	/* The date as "%m/%d/%y". */
			LEGAL_ALT(0);
			if (!(bp = __mml_strptime(bp, "%m/%d/%y", tm)))
				return (0);
			break;

		case 'R':	/* The time as "%H:%M". */
			LEGAL_ALT(0);
			if (!(bp = __mml_strptime(bp, "%H:%M", tm)))
				return (0);
			break;

		case 'r':	/* The time in 12-hour clock representation. */
			LEGAL_ALT(0);
			if (!(bp = __mml_strptime(bp, "%I:%M:%S %p", tm)))
				return (0);
			break;

		case 'T':	/* The time as "%H:%M:%S". */
			LEGAL_ALT(0);
			if (!(bp = __mml_strptime(bp, "%H:%M:%S", tm)))
				return (0);
			break;

		case 'X':	/* The time, using the locale's format. */
			LEGAL_ALT(ALT_E);
			if (!(bp = __mml_strptime(bp, "%H:%M:%S", tm)))
				return (0);
			break;

		case 'x':	/* The date, using the locale's format. */
			LEGAL_ALT(ALT_E);
			if (!(bp = __mml_strptime(bp, "%m/%d/%y", tm)))
				return (0);
			break;

		/*
		 * "Elementary" conversion rules.
		 */
		case 'A':	/* The day of week, using the locale's form. */
		case 'a':
			LEGAL_ALT(0);
			for (i = 0; i < 7; i++) {
				/* Full name. */
				len = mmlStrLength(day[i]);
				if (mmlStrNCompare(day[i], bp, len, MML_CASE_INSENSITIVE) == 0)
					break;

				/* Abbreviated name. */
				len = mmlStrLength(abday[i]);
				if (mmlStrNCompare(abday[i], bp, len, MML_CASE_INSENSITIVE) == 0)
					break;
			}

			/* Nothing matched. */
			if (i == 7)
				return (0);

			tm->tm_wday = i;
			bp += len;
			break;

		case 'B':	/* The month, using the locale's form. */
		case 'b':
		case 'h':
			LEGAL_ALT(0);
			for (i = 0; i < 12; i++) {
				/* Full name. */
				len = mmlStrLength(mon[i]);
				if (mmlStrNCompare(mon[i], bp, len, MML_CASE_INSENSITIVE) == 0)
					break;

				/* Abbreviated name. */
				len = mmlStrLength(abmon[i]);
				if (mmlStrNCompare(abmon[i], bp, len, MML_CASE_INSENSITIVE) == 0)
					break;
			}

			/* Nothing matched. */
			if (i == 12)
				return (0);

			tm->tm_mon = i;
			bp += len;
			break;

		case 'C':	/* The century number. */
			LEGAL_ALT(ALT_E);
			if (!(__mml_conv_num(&bp, &i, 0, 99)))
				return (0);

			if (split_year) {
				tm->tm_year = (tm->tm_year % 100) + (i * 100);
			} else {
				tm->tm_year = i * 100;
				split_year = 1;
			}
			break;

		case 'd':	/* The day of month. */
		case 'e':
			LEGAL_ALT(ALT_O);
			if (!(__mml_conv_num(&bp, &tm->tm_mday, 1, 31)))
				return (0);
			break;

		case 'k':	/* The hour (24-hour clock representation). */
			LEGAL_ALT(0);
			/* FALLTHROUGH */
		case 'H':
			LEGAL_ALT(ALT_O);
			if (!(__mml_conv_num(&bp, &tm->tm_hour, 0, 23)))
				return (0);
			break;

		case 'l':	/* The hour (12-hour clock representation). */
			LEGAL_ALT(0);
			/* FALLTHROUGH */
		case 'I':
			LEGAL_ALT(ALT_O);
			if (!(__mml_conv_num(&bp, &tm->tm_hour, 1, 12)))
				return (0);
			if (tm->tm_hour == 12)
				tm->tm_hour = 0;
			break;

		case 'j':	/* The day of year. */
			LEGAL_ALT(0);
			if (!(__mml_conv_num(&bp, &i, 1, 366)))
				return (0);
			tm->tm_yday = i;
			break;

		case 'M':	/* The minute. */
			LEGAL_ALT(ALT_O);
			if (!(__mml_conv_num(&bp, &tm->tm_min, 0, 59)))
				return (0);
			break;

		case 'm':	/* The month. */
			LEGAL_ALT(ALT_O);
			if (!(__mml_conv_num(&bp, &i, 1, 12)))
				return (0);
			tm->tm_mon = i;
			break;

		case 'p':	/* The locale's equivalent of AM/PM. */
			LEGAL_ALT(0);
			/* AM? */
			len = mmlStrLength(am_pm[0]);
			if (mmlStrNCompare(am_pm[0], bp, len, MML_CASE_INSENSITIVE) == 0) {
				if (tm->tm_hour > 11)
					return (0);

				bp += len;
				break;
			}

			/* PM? */
			len = mmlStrLength(am_pm[1]);
			if (mmlStrNCompare(am_pm[1], bp, len, MML_CASE_INSENSITIVE) == 0) {
				if (tm->tm_hour > 11)
					return (0);

				tm->tm_hour += 12;
				bp += len;
				break;
			}

			/* Nothing matched. */
			return (0);

		case 'S':	/* The seconds. */
			LEGAL_ALT(ALT_O);
			if (!(__mml_conv_num(&bp, &tm->tm_sec, 0, 61)))
				return (0);
			break;

		case 'U':	/* The week of year, beginning on sunday. */
		case 'W':	/* The week of year, beginning on monday. */
			LEGAL_ALT(ALT_O);
			/*
			 * XXX This is bogus, as we can not assume any valid
			 * information present in the tm structure at this
			 * point to calculate a real value, so just check the
			 * range for now.
			 */
			 if (!(__mml_conv_num(&bp, &i, 0, 53)))
				return (0);
			 break;

		case 'w':	/* The day of week, beginning on sunday. */
			LEGAL_ALT(ALT_O);
			if (!(__mml_conv_num(&bp, &tm->tm_wday, 0, 6)))
				return (0);
			break;

		case 'Y':	/* The year. */
			LEGAL_ALT(ALT_E);
			if (!(__mml_conv_num(&bp, &i, 0, 9999)))
				return (0);

			tm->tm_year = i;
			break;

		case 'y':	/* The year within 100 years of the epoch. */
			LEGAL_ALT(ALT_E | ALT_O);
			if (!(__mml_conv_num(&bp, &i, 0, 99)))
				return (0);

			if (split_year) {
				tm->tm_year = ((tm->tm_year / 100) * 100) + i;
				break;
			}
			split_year = 1;
			if (i <= 68)
				tm->tm_year = i + 2000;
			else
				tm->tm_year = i + 1900;
			break;

		/*
		 * Miscellaneous conversions.
		 */
		case 'n':	/* Any kind of white-space. */
		case 't':
			LEGAL_ALT(0);
			while (isspace(*bp))
				bp++;
			break;


		default:	/* Unknown/unsupported conversion. */
			return (0);
		}
	}
	return ((mmlChar *)bp);
}


mml_core mmlBool mml_clock_strptime (const mmlChar * str , const mmlChar * formatter , mml_tm * tm)
{
	return __mml_strptime(str, formatter, tm) == mmlNULL ? mmlFalse : mmlTrue;
}

#define TM_EPOCH_YEAR	1970


mml_core void mml_clock_mktime(mml_tm * tm, mml_timespec * ts, const mmlInt32 offset)
{
	if ( offset == MML_LOCALTIME_OFFSET_AUTO )
	{
		struct tm sys_tm = {0};
		sys_tm.tm_sec = tm->tm_sec;
		sys_tm.tm_min = tm->tm_min;
		sys_tm.tm_hour = tm->tm_hour;
		sys_tm.tm_mday = tm->tm_mday;
		sys_tm.tm_wday = tm->tm_wday;
		sys_tm.tm_yday = tm->tm_yday;
		sys_tm.tm_mon = tm->tm_mon - 1;
		sys_tm.tm_year = tm->tm_year - TM_YEAR_BASE;
		sys_tm.tm_isdst = tm->tm_isdst;
		ts->tv_sec = mktime(&sys_tm);
		ts->tv_nsec = 0;
		return;
	}

	time_t tres;
	mmlInt32 doy;
	mmlInt32 i;

	if (tm->tm_mon < 0 || tm->tm_mon > 11 || tm->tm_year < TM_EPOCH_YEAR)
	{
		return ;
	}

	tres = 0;

	for (i = TM_EPOCH_YEAR; i < tm->tm_year; i++)
	{
		tres += 365 + is_leap_year(i);
	}

	doy = 0;
	for (i = 0; i < tm->tm_mon - 1; i++)
	{
		doy += MONTHDAYS[i];
	}
	tres += doy;

	tm->tm_yday = doy + tm->tm_mday;

	if (tm->tm_mon > 2 && is_leap_year(tm->tm_year))
	{
		tres++;
	}

	tres = 24 * (tres + tm->tm_mday - 1) + tm->tm_hour;

	tres = 60 * tres + tm->tm_min;

	tres = 60 * tres + tm->tm_sec;


	ts->tv_sec = tres - offset;
	ts->tv_nsec = 0;
}

static mmlInt32 __mml_fmt(mmlChar ** pt ,const mmlChar *format, mml_tm *t , mmlInt32 * gsize)
{


	mmlInt32 special = 0;
	mmlChar s;
	for (; *format; ++format) {
		if (*format == '%' || special == 1)
			if ( *format == '%' ) special = 0;
		if (*format == '%')
		{
		switch( s = *++format) {
			case '\0':
				--format;
				break;
			case '#':
				special = 1;
				--format;
				continue;
			case 'A':
				if (t->tm_wday < 0 || t->tm_wday > 6)
					return(0);
				if (!__mml_add(pt, day[t->tm_wday], gsize))
					return(0);
				continue;
			case 'a':
				if (t->tm_wday < 0 || t->tm_wday > 6)
					return(0);
				if (!__mml_add(pt,abday[t->tm_wday], gsize))
					return(0);
				continue;
			case 'B':
				if (t->tm_mon < 0 || t->tm_mon > 11)
					return(0);
				if (!__mml_add(pt,mon[t->tm_mon], gsize))
					return(0);
				continue;
			case 'b':
			case 'h':
				if (t->tm_mon < 0 || t->tm_mon > 11)
					return(0);
				if (!__mml_add(pt,abmon[t->tm_mon], gsize))
					return(0);
				continue;
			case 'C':
				if (!__mml_fmt(pt,"%a %b %e %H:%M:%S %Y", t, gsize))
					return(0);
				continue;
			case 'c':
				if (!__mml_fmt(pt,"%m/%d/%y %H:%M:%S", t, gsize))
					return(0);
				continue;
			case 'D':
				if (!__mml_fmt(pt,"%m/%d/%y", t, gsize))
					return(0);
				continue;
			case 'd':
				if (!__mml_conv(pt,t->tm_mday, 2, '0', special, gsize))
					return(0);
				continue;
			case 'e':
				if (!__mml_conv(pt,t->tm_mday, 2, ' ', special, gsize))
					return(0);
				continue;
			case 'H':
				if (!__mml_conv(pt,t->tm_hour, 2, '0', special, gsize))
					return(0);
				continue;
			case 'I':
				if (!__mml_conv(pt,t->tm_hour % 12 ?
					t->tm_hour % 12 : 12, 2, '0', special, gsize))
					return(0);
				continue;
			case 'j':
				if (!__mml_conv(pt,t->tm_yday + 1, 3, '0', special, gsize))
					return(0);
				continue;
			case 'k':
				if (!__mml_conv(pt,t->tm_hour, 2, ' ', special, gsize))
					return(0);
				continue;
			case 'l':
				if (!__mml_conv(pt,t->tm_hour % 12 ?
					t->tm_hour % 12 : 12, 2, ' ', special, gsize))
					return(0);
				continue;
			case 'M':
				if (!__mml_conv(pt,t->tm_min, 2, '0', special, gsize))
					return(0);
				continue;
			case 'm':
				if (!__mml_conv(pt,t->tm_mon , 2, '0', special, gsize))
					return(0);
				continue;
			case 'n':
				if (!__mml_add(pt,"\n", gsize))
					return(0);
				continue;
			case 'p':
				if (!__mml_add(pt,(mmlChar*)(t->tm_hour >= 12 ? "PM" : "AM"), gsize))
					return(0);
				continue;
			case 'R':
				if (!__mml_fmt(pt,"%H:%M", t, gsize))
					return(0);
				continue;
			case 'r':
				if (!__mml_fmt(pt,"%I:%M:%S %p", t, gsize))
					return(0);
				continue;
			case 'S':
				if (!__mml_conv(pt,t->tm_sec, 2, '0', special, gsize))
					return(0);
				continue;
			case 's':
				if (!__mml_secs(pt,t, gsize))
					return(0);
				continue;
			case 'T':
			case 'X':
				if (!__mml_fmt(pt,"%H:%M:%S", t, gsize))
					return(0);
				continue;
			case 't':
				if (!__mml_add(pt,"\t", gsize))
					return(0);
				continue;
			case 'U':
				if (!__mml_conv(pt,(t->tm_yday + 7 - t->tm_wday) / 7,
					2, '0', special, gsize))
					return(0);
				continue;
			case 'W':
				if (!__mml_conv(pt,(t->tm_yday + 7 -
					(t->tm_wday ? (t->tm_wday - 1) : 6))
					/ 7, 2, '0', special, gsize))
					return(0);
				continue;
			case 'w':
				if (!__mml_conv(pt,t->tm_wday, 1, '0', special, gsize))
					return(0);
				continue;
			case 'x':
				if (!__mml_fmt(pt,"%#m/%#d/%Y", t, gsize))
					return(0);
				continue;
			case 'y':
				if (!__mml_conv(pt,(t->tm_year)
					% 100, 2, '0', special, gsize))
					return(0);
				continue;
			case 'Y':
				if (!__mml_conv(pt,t->tm_year, 4, '0', special, gsize))
					return(0);
				continue;
			case 'Z':
				continue;
			case '%':
			default:
				break;
		}
		--format;
		}
		if (!(*gsize)--)
			return(0);
		*(*pt)++ = *format;
	}
	return(*gsize);
}

mmlInt32 mml_clock_strftime(const mmlChar * formatter , mml_tm* t, mmlChar * buffer , const mmlInt32 size)
{
	mmlInt32 gsize;
	if ((gsize = size) < 1)
		return(0);
	if (__mml_fmt(&buffer , formatter, t, &gsize)) 
	{
		*buffer = '\0';
		return(size - gsize);
	}
	return(0);
}
