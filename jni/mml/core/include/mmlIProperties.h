#ifndef MML_INTERFACE_PROPERTIES_HEADER_INCLUDED
#define MML_INTERFACE_PROPERTIES_HEADER_INCLUDED

#include "mmlICString.h"

#define MML_IPROPERTIES_UUID {  0xa8e2b588 , 0x8c24 , 0x11de , 0xa119 , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } } 

class mmlIProperties : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_IPROPERTIES_UUID);
public:

	virtual mmlBool GetPropertyAsString ( mmlICString * key , mmlICString ** value ) = 0;
	virtual mmlBool GetPropertyAsInteger ( mmlICString * key , mmlInt32 * value ) = 0;
	virtual mmlBool GetPropertyAsFloat ( mmlICString * key , mmlFloat64 * value ) = 0;
	virtual mmlBool GetPropertyAsBoolean ( mmlICString * key , mmlBool * value ) = 0;
};


#endif //MML_INTERFACE_PROPERTIES_HEADER_INCLUDED

