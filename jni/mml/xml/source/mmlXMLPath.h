#ifndef MML_XML_PATH_HEADER_INCLUDED
#define MML_XML_PATH_HEADER_INCLUDED

#include "mmlICString.h"
#include "mmlPtr.h"
#include <list>

class mmlXMLPath : public mmlIObject
{
	MML_OBJECT_DECL;
public:

	mmlBool Init ( mmlICString * path );

	mmlBool Top ( mmlICString ** top );

	mmlBool Push ( mmlICString * node );

	mmlBool Pop ();

	mmlBool HasMore ();

private:

	std::list < mmlAutoPtr < mmlICString > > mPath;

};


#endif //MML_XML_PATH_HEADER_INCLUDED

