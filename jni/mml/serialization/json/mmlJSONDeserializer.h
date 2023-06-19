#ifndef MML_JSON_DESERIALIZER_HEADER_INCLUDED
#define MML_JSON_DESERIALIZER_HEADER_INCLUDED

#include "mmlIVariantDeserializer.h"


class mmlJSONDeserializer : public mmlIVariantDeserializer
{
	MML_OBJECT_DECL
public:
	mmlBool Read ( mmlIInputStream * stream , mmlIVariant ** variant );
};


#endif //MML_JSON_DESERIALIZER_HEADER_INCLUDED
