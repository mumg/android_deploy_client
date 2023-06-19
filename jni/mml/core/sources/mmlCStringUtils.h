#ifndef MML_CSTRING_UTILS_HEADER_INCLUDED
#define MML_CSTRING_UTILS_HEADER_INCLUDED

#include "mmlICStringUtils.h"

class mmlCStringUtils : public mmlICStringUtils
{
	MML_OBJECT_DECL;
public:
	mmlBool Find ( mmlICString * str , const mmlChar * sstr , mmlInt32 * pos , mml_char_conversion_t rule);
	mmlBool FindFirstOf ( mmlICString * str , const mmlChar * sstr , mmlInt32 * pos, mml_char_conversion_t rule);
	mmlBool FindFirstNotOf ( mmlICString * str , const mmlChar * sstr , mmlInt32 * pos, mml_char_conversion_t rule);
	mmlBool FindLastOf ( mmlICString * str , const mmlChar * sstr , mmlInt32 * pos, mml_char_conversion_t rule);
	mmlBool FindLastNotOf ( mmlICString * str , const mmlChar * sstr , mmlInt32 * pos, mml_char_conversion_t rule);
	mmlBool Get ( mmlICString * str , const  mmlInt32 pos , const mmlInt32 len , mmlICString ** sub_str );
	mmlBool ToInteger16 ( mmlICString * str , mmlInt16 * value );
	mmlBool ToInteger32 ( mmlICString * str , mmlInt32 * value );
	mmlBool ToInteger64 ( mmlICString * str , mmlInt64 * value );
	mmlBool ToUInteger16 ( mmlICString * str, mmlUInt16 * value );
	mmlBool ToUInteger32 ( mmlICString * str, mmlUInt32 * value );
	mmlBool ToUInteger64 ( mmlICString * str, mmlUInt64 * value );
	mmlBool ToFloat ( mmlICString * str , const  mmlInt32 pos , const mmlInt32 len , mmlFloat64 * value );
	mmlBool Explode ( mmlICString * str , const mmlChar * separators , mmlIList ** lst );
	mmlBool Implode ( mmlIList * lst , mmlICString ** str );
	mmlBool Split ( mmlICString * src , const mmlInt32 pos, mmlICString ** left , mmlICString ** right );
	mmlBool Concat ( mmlICString * left , mmlICString * right , mmlICString ** result );
	mmlBool NewCString ( const mmlChar * sec , mmlInt32 len , mmlICString ** result );
	mmlBool Integer16( const mmlInt16 integer , mmlICString ** value );
	mmlBool Integer32( const mmlInt32 integer , mmlICString ** value );
	mmlBool Integer64( const mmlInt64 integer , mmlICString ** value );
	mmlBool UInteger16( const mmlUInt16 integer , mmlICString ** value );
	mmlBool UInteger32( const mmlUInt32 integer , mmlICString ** value );
	mmlBool UInteger64( const mmlUInt64 integer , mmlICString ** value );
	mmlBool Float( const mmlFloat64 integer , mmlICString ** value );
	mmlBool Escape ( mmlICString * src , const mmlChar * search, const mmlChar ** replace , mmlICString ** dst );
    mmlBool LowerCase ( mmlICString * src , mmlICString ** dst );
    mmlBool UpperCase ( mmlICString * src , mmlICString ** dst );
	mmlBool Trim ( mmlICString * src , mmlICString ** dst );
	mmlBool Replace ( mmlICString * src , const mmlChar * sstr, const mmlChar * replacer, mmlICString ** dst, mml_char_conversion_t rule );
	mmlBool MultiReplace ( mmlICString * src , const mmlChar * sstr, const mmlChar * replacer, mmlICString ** dst, mml_char_conversion_t rule );
	mmlBool Read ( mmlIInputStream * stream, mmlICString ** dst , const mmlInt32 len );
};


#endif //MML_CSTRING_UTILS_HEADER_INCLUDED

