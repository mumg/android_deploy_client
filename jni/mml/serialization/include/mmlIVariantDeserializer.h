#ifndef MML_DESERIALIZER_HEADER_INCLUDED
#define MML_DESERIALIZER_HEADER_INCLUDED

#include "mmlIStream.h"
#include "mmlIVariant.h"

#define MML_VARIANT_DESERIALIZER_UUID { 0x7716E348, 0x6C6D, 0x1014, 0x9721 , { 0xC8, 0xF5, 0x92, 0xF7, 0x56, 0xE2 } }

class mmlIVariantDeserializer : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_VARIANT_DESERIALIZER_UUID)
public:
	virtual mmlBool Read ( mmlIInputStream * stream , mmlIVariant ** variant ) = 0;
};

#endif //MML_DESERIALIZER_HEADER_INCLUDED
