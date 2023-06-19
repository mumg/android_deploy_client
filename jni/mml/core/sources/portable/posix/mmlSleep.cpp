#include "mmlSleep.h"
#include <unistd.h>


MML_OBJECT_IMPL1(mmlSleep , mmlISleep)

mmlBool mmlSleep::Do ( const mmlInt32 timeout )
{
	usleep ( useconds_t(timeout) * 1000 );
	return mmlTrue;
}

