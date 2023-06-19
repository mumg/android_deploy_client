#include "mml_components.h"
#include "mmlBinarySerializer.h"
#include "mmlBinaryDeserializer.h"

MML_CONSTRUCTOR_IMPL(mmlBinarySerializer);
MML_CONSTRUCTOR_IMPL(mmlBinaryDeserializer);

#define MML_BINARY_VARIANT_SERIALIZER_UUID { 0x6392D37F, 0x6C6E, 0x1014, 0xAE5B , { 0xDC, 0x81, 0x31, 0xDC, 0xEA, 0xA1 } }

#define MML_BINARY_VARIANT_DESERIALIZER_UUID { 0x6E1257B1, 0x6C6E, 0x1014, 0x8067 , { 0x8F, 0x47, 0x3C, 0xAD, 0xC9, 0x8C } }

static mmlObjectFactory binary_serialization_component [] =
{
	{
		MML_BINARY_VARIANT_SERIALIZER_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlBinarySerializer)
	},
	{
		MML_BINARY_VARIANT_DESERIALIZER_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlBinaryDeserializer)
	}
};

MML_COMPONENT_IMPL(binary_serialization,binary_serialization_component)