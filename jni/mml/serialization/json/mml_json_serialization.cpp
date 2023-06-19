#include "mml_components.h"
#include "mmlJSONSerializer.h"
#include "mmlJSONDeserializer.h"

MML_CONSTRUCTOR_IMPL(mmlJSONSerializer);
MML_CONSTRUCTOR_IMPL(mmlJSONDeserializer);

#define MML_JSON_VARIANT_SERIALIZER_UUID { 0x407065FF, 0x6C70, 0x1014, 0xBDA0 , { 0xF9, 0xC2, 0xF4, 0x90, 0x1F, 0xFA } }

#define MML_JSON_VARIANT_DESERIALIZER_UUID { 0x4383190F, 0x6C70, 0x1014, 0x9A99 , { 0x9C, 0xA2, 0x29, 0x60, 0xE2, 0xE9 } }

static mmlObjectFactory json_serialization_component [] =
{
	{
		MML_JSON_VARIANT_SERIALIZER_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlJSONSerializer)
	},
	{
		MML_JSON_VARIANT_DESERIALIZER_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlJSONDeserializer)
	}
};

MML_COMPONENT_IMPL(json_serialization,json_serialization_component)