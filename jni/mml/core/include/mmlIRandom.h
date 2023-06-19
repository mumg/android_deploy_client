#ifndef MML_INTERFACE_RANDOM_HEADER_INCLUDED
#define MML_INTERFACE_RANDOM_HEADER_INCLUDED

#include "mmlIObject.h"

#define MML_IRANDOM_UUID {  0x3f90bf9c , 0x8802 , 0x11de , 0xbde5 , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

class mmlIRandom : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_IRANDOM_UUID)
public:
	virtual mmlBool NewRandom ( mmlInt64 * value ) = 0;
	virtual mmlBool NewRandomFill ( void * buffer , const mmlInt32 len ) = 0;
};

#endif //MML_INTERFACE_RANDOM_HEADER_INCLUDED

