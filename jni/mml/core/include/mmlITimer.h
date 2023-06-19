#ifndef MML_INTERFACE_TIMER_HEADER_INCLUDED
#define MML_INTERFACE_TIMER_HEADER_INCLUDED

#include "mmlIObject.h"

#define MML_ITIMER_UUID {  0x4ac76584 , 0xc315 , 0x11de , 0x9665 , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

class mmlITimer : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_ITIMER_UUID);
public:

	virtual mmlBool Setup ( const mmlInt32 timeout,
		                    const mmlBool periodical ) = 0;

	virtual mmlBool Wait () = 0;
};

#endif //MML_INTERFACE_TIMER_HEADER_INCLUDED

