#ifndef MML_REGEXP_HEADER_INCLUDED
#define MML_REGEXP_HEADER_INCLUDED

#include "mmlIObject.h"
#include "mmlIList.h"

#define MML_REGEXP_UUID { 0x38321964, 0x6C5F, 0x1014, 0x9021 , { 0x97, 0x2E, 0xFA, 0xD1, 0x91, 0xAF } }

class mmlIRegexp : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_REGEXP_UUID)
public:
	virtual mmlBool Match ( const mmlChar * pattern , mmlICString * string , const mmlBool full = mmlTrue) = 0;
	virtual mmlBool Find(const mmlChar * pattern, mmlICString * string, mmlICString ** result) = 0;
	virtual mmlBool Find(const mmlChar * patter, mmlICString * string, mmlIList ** result) = 0;
};


#endif //MML_REGEXP_HEADER_INCLUDED
