#include "mmlSleep.h"
#include <Windows.h>

MML_OBJECT_IMPL1(mmlSleep , mmlISleep)

mmlBool mmlSleep::Do ( const mmlInt32 timeout )
{
	Sleep(timeout);
	return mmlTrue;
}
