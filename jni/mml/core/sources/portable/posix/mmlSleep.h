#ifndef MML_SLEEP_HEADER_INCLUDED
#define MML_SLEEP_HEADER_INCLUDED

#include "mmlISleep.h"

class mmlSleep : public mmlISleep
{
	MML_OBJECT_DECL
public:
	mmlBool Do ( const mmlInt32 timeout );
};


#endif //MML_SLEEP_HEADER_INCLUDED

