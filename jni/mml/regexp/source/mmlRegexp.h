#ifndef MML_REGEXP_IMPL_HEADER_INCLUDED
#define MML_REGEXP_IMPL_HEADER_INCLUDED

#include "mmlIRegexp.h"


class mmlRegexp : public mmlIRegexp
{
	MML_OBJECT_DECL
public:

	mmlBool Match ( const char * pattern , mmlICString * string, const mmlBool full);
	mmlBool Find(const mmlChar * pattern, mmlICString * string, mmlICString ** result);
	mmlBool Find(const mmlChar * patter, mmlICString * string, mmlIList ** result);
};


#endif //MML_REGEXP_IMPL_HEADER_INCLUDED
