#ifndef MML_INTERFACE_CONDITION_HEADER_INCLUDED
#define MML_INTERFACE_CONDITION_HEADER_INCLUDED

#include "mmlIObject.h"

#define MML_ICONDITION_UUID {  0x0d9892d6 , 0x8419 , 0x11de , 0x9f6c , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

#define MML_CONDITION_INFINITE 0

class mmlICondition : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_ICONDITION_UUID);
public:
	virtual mmlBool Wait ( const mmlInt32 timeout ) = 0;
	virtual mmlBool Signal () = 0;
};

#endif //MML_INTERFACE_CONDITION_HEADER_INCLUDED

