#include "mml_components.h"
#include "mmlDatabase.h"

MML_CONSTRUCTOR_IMPL1(mmlDatabase,mmlIVariantStruct);
MML_CONSTRUCTOR_IMPL2(mmlDatabase,mmlIVariantStruct,mmlIDatabaseScheme);

static mmlObjectFactory database_component [] =
{
	{
		MML_DATABASE_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlDatabase,mmlIVariantStruct)
	},
	{
		MML_DATABASE_UUID,
		mmlNULL,
		MML_CONSTRUCTOR2(mmlDatabase,mmlIVariantStruct,mmlIDatabaseScheme)
	}
};

MML_COMPONENT_IMPL(database,database_component)