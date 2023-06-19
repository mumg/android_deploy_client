#ifndef MML_C_STRING_HEADER_INCLUDED
#define MML_C_STRING_HEADER_INCLUDED

#include "mmlIObject.h"

#include "mmlPtr.h"
#include <stdlib.h>
#include "mml_strutils.h"

class mmlIBuffer;

#define MML_ICSTRING_UUID {  0x9bbb194e , 0x76ea , 0x11de , 0x82da , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

class mmlICString : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_ICSTRING_UUID);
public:
	virtual mmlBool Assign ( const mmlChar * str ) = 0;
	virtual mmlBool Adopt ( const mmlChar * str ) = 0;
	virtual const mmlChar * Get () const = 0;
	virtual const mmlUInt32 Length () const = 0;
	virtual mmlBool CompareStr ( const mmlCompareOperator & op,
	                             const mmlChar * str,
								 mmlBool & result,
								 mml_char_conversion_t rule = MML_CASE_SENSITIVE) = 0;
};

mml_core mmlICString * mmlNewStaticCString( const mmlChar * str );
mml_core mmlICString * mmlNewCString( const mmlChar * str , const mmlInt32 len = -1, const mmlBool adopt = mmlFalse);
mml_core mmlICString * mmlNewCStringFromBuffer ( mmlIBuffer * buffer);

class mmlCStaticString
{
public:
	inline mmlCStaticString ( const mmlChar  * str )
	{
		mString = mmlNewStaticCString(str);
	}

	inline operator mmlICString * () const
	{
		return mString;
	}

	inline const mmlChar * const Get () const
	{
		return mString->Get();
	}

private:
	mmlAutoPtr < mmlICString > mString;
	// keep operator new unresolved to prevent dynamic usage
	void * operator new (const size_t size);
};

class mml_icstring_map_predicate_cs
{
public:
	mmlInt32 operator () ( const mmlAutoPtr < mmlICString > & left,
		                   const mmlAutoPtr < mmlICString > & right) const
	{
		mmlBool less;
		left->CompareStr(MML_LESS, right->Get(), less);
		return less == mmlTrue ? 1:0;
	}
};

class mml_icstring_map_predicate_ci
{
public:
	mmlInt32 operator () ( const mmlAutoPtr < mmlICString > & left,
		                   const mmlAutoPtr < mmlICString > & right) const
	{
		mmlBool less;
		left->CompareStr(MML_LESS, right->Get(), less, MML_CASE_INSENSITIVE);
		return less == mmlTrue ? 1:0;
	}
};

class mml_char_map_predicate_ci
{
public:
	mmlInt32 operator () ( const mmlChar * left,
		                   const mmlChar *  right) const
	{
		return mmlStrCompare(left, right, MML_CASE_INSENSITIVE) < 0 ? 1 : 0;
	}
};

#endif //MML_C_STRING_HEADER_INCLUDED

