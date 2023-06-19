#ifndef MML_JSON_SERIALIZER_HEADER_INCLUDED
#define MML_JSON_SERIALIZER_HEADER_INCLUDED

#include "mmlIVariantSerializer.h"


class mmlJSONSerializer : public mmlIVariantSerializer
{
	MML_OBJECT_DECL
public:
	mmlBool Write ( mmlIOutputStream * stream, mmlIVariant * value );
};


#endif //MML_JSON_SERIALIZER_HEADER_INCLUDED
