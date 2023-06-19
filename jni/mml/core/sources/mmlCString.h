#ifndef MML_CSTRING_HEADER_INCLUDED
#define MML_CSTRING_HEADER_INCLUDED

#include "mmlICString.h"


class mmlCString : public mmlICString
{
	MML_OBJECT_DECL;
public:

	mmlCString();

	mmlCString ( const mmlChar * str , const mmlInt32 len, const mmlBool adopt = mmlFalse );

	mmlCString( mmlIBuffer * buffer);

	~mmlCString();

	const mmlChar * Get () const;

	const mmlUInt32 Length () const;

	mmlBool CompareStr ( const mmlCompareOperator & op,
		                 const mmlChar * str,
		                 mmlBool & result,
						 mml_char_conversion_t rule);

	mmlBool Assign ( const mmlChar * str );
	mmlBool Adopt ( const mmlChar * str );

protected:

	mmlAutoPtr < mmlIBuffer > mBuffer;

	mmlChar * mStr;

	mmlInt32 mLength;

	static const mmlChar * mEmptyStr;

};




#endif //MML_CSTRING_HEADER_INCLUDED

