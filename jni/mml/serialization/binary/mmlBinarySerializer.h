#ifndef MML_BINARY_SERIALIZER_HEADER_INCLUDED
#define MML_BINARY_SERIALIZER_HEADER_INCLUDED

#include "mmlIVariantSerializer.h"


class mmlBinarySerializer : public mmlIVariantSerializer
{
	MML_OBJECT_DECL
public:
	mmlBool Write ( mmlIOutputStream * stream, mmlIVariant * value );
};


#endif //MML_BINARY_SERIALIZER_HEADER_INCLUDED
