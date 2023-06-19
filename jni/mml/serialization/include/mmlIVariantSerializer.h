#ifndef MML_SERIALIZER_HEADER_INCLUDED
#define MML_SERIALIZER_HEADER_INCLUDED

#include "mmlIStream.h"
#include "mmlIVariant.h"

#define MML_SERIALIZER_UUID { 0x692CF304, 0x6C6D, 0x1014, 0x954F , { 0xFD, 0x0A, 0x05, 0xE8, 0x70, 0xB7 } }

class mmlIVariantSerializer : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_SERIALIZER_UUID)
public:
	virtual mmlBool Write ( mmlIOutputStream * stream, mmlIVariant * value ) = 0;
};


#endif //MML_SERIALIZER_HEADER_INCLUDED
