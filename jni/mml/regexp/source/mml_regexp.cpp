#include "mmlRegexp.h"
#include "mml_components.h"

MML_CONSTRUCTOR_IMPL(mmlRegexp);

static mmlObjectFactory mml_regexp_component [] =
{
	{
		MML_REGEXP_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlRegexp)
	}
};

MML_COMPONENT_IMPL(regexp,mml_regexp_component) 