#ifndef MML_INTERFACE_SLEEP_HEADER_INCLUDED
#define MML_INTERFACE_SLEEP_HEADER_INCLUDED

#include "mmlIObject.h"

#define MML_ISLEEP_UUID {  0x75c5606c , 0x8c31 , 0x11de , 0x96a4 , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

class mmlISleep : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_ISLEEP_UUID)
public:
	virtual mmlBool Do ( const mmlInt32 timeout ) = 0;
};

#endif //MML_INTERFACE_SLEEP_HEADER_INCLUDED

