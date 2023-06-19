#include "mmlRandom.h"
#include <time.h>
#include "mml_time.h"


mmlUInt16 mmlRandom::rand()
{
	bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
	return lfsr =  (lfsr >> 1) | (bit << 15);
}

MML_OBJECT_IMPL1(mmlRandom, mmlIRandom)


mmlRandom::mmlRandom()
{
	mml_timespec ts;
	mml_clock_gettime_realtime(&ts);
	lfsr = ts.tv_sec ^ ts.tv_nsec;
}

mmlBool mmlRandom::NewRandom ( mmlInt64 * value )
{
	mmlInt64 r1 = rand();
	mmlInt64 r2 = rand();
	*value = r1 << 48 | r2 << 32;
	r1 = rand();
	r2 = rand ();
	*value = *value | r1 << 16 | r2;
	return mmlTrue;
}

mmlBool mmlRandom::NewRandomFill ( void * buffer , const mmlInt32 len )
{
	mmlInt32 words = len / 2;
	mmlChar * ptr = (mmlChar*)buffer;
	for ( mmlInt32 word = 0 ; word < words ; word ++)
	{
		mmlUInt16 rnd = rand();
		*ptr = (rnd >> 8 & 0xFF);
		ptr ++;
		*ptr = (rnd & 0xFF);
		ptr ++;
	}
	return mmlTrue;
}
 
