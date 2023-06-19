#ifndef MML_XML_DOCUMENT_HEADER_INCLUDED
#define MML_XML_DOCUMENT_HEADER_INCLUDED

#include "mmlIXMLDocument.h"
#include "mmlPtr.h"
#include "mmlICString.h"
#include "mmlXMLNode.h"

class mmlXMLDocument : public mmlIXMLDocument
{
public:
	MML_IXMLDOCUMENT_DECL;

	mmlXMLDocument();

private:

	mmlSharedPtr < mmlXMLNode > mRoot;

	mmlInt32 mVerMajor;

	mmlInt32 mVerMinor;

	mmlAutoPtr < mmlICString > mEncoding;
};

#endif //MML_XML_DOCUMENT_HEADER_INCLUDED

