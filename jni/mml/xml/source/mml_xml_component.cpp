#include "mmlXMLDocument.h"
#include "mmlXMLNode.h"
#include "mml_components.h"

MML_CONSTRUCTOR_IMPL(mmlXMLDocument);
MML_CONSTRUCTOR_IMPL1(mmlXMLNode, mmlICStringInputStream);

static mmlObjectFactory mml_xml_component [] =
{
	{
		MML_IXMLDOCUMENT_UUID,
		"@mml/xml",
		MML_CONSTRUCTOR(mmlXMLDocument)
	},
	{
		MML_IXMLNODE_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlXMLNode, mmlICStringInputStream)
	}
};

MML_COMPONENT_IMPL(xml,mml_xml_component)
