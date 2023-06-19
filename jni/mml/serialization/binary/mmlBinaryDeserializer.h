#ifndef MML_BINARY_DESERIALIZER_HEADER_INCLUDED
#define MML_BINARY_DESERIALIZER_HEADER_INCLUDED

#include "mmlIVariantDeserializer.h"


class mmlBinaryDeserializer : public mmlIVariantDeserializer
{
	MML_OBJECT_DECL
public:
	mmlBool Read ( mmlIInputStream * stream , mmlIVariant ** variant );
};


#endif //MML_BINARY_DESERIALIZER_HEADER_INCLUDED
