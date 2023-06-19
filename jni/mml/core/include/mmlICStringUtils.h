#ifndef MML_INTERFACE_STRING_UTILS_HEADER_INCLUDED
#define MML_INTERFACE_STRING_UTILS_HEADER_INCLUDED

#include "mmlICString.h"
#include "mmlIList.h"
#include "mmlIStream.h"

#define MML_ICSTRING_UTILS_UUID {  0xc600b4f2 , 0x76ea , 0x11de , 0x869b , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

class mmlICStringUtils : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_ICSTRING_UTILS_UUID);
public:
	virtual mmlBool Find ( mmlICString * str , const mmlChar * sstr , mmlInt32 * pos, mml_char_conversion_t rule = MML_CASE_SENSITIVE) = 0;
	virtual mmlBool FindFirstOf ( mmlICString * str , const mmlChar * sstr , mmlInt32 * pos, mml_char_conversion_t rule = MML_CASE_SENSITIVE) = 0;
	virtual mmlBool FindFirstNotOf ( mmlICString * str , const mmlChar * sstr , mmlInt32 * pos, mml_char_conversion_t rule = MML_CASE_SENSITIVE) = 0;
	virtual mmlBool FindLastOf ( mmlICString * str ,const  mmlChar * sstr , mmlInt32 * pos, mml_char_conversion_t rule = MML_CASE_SENSITIVE) = 0;
	virtual mmlBool FindLastNotOf ( mmlICString * str , const mmlChar * sstr , mmlInt32 * pos, mml_char_conversion_t rule = MML_CASE_SENSITIVE) = 0;
	virtual mmlBool Get ( mmlICString * str , const  mmlInt32 pos , const mmlInt32 len , mmlICString ** sub_str ) = 0;
	virtual mmlBool ToInteger16 ( mmlICString * str , mmlInt16 * value ) = 0;
	virtual mmlBool ToInteger32 ( mmlICString * str , mmlInt32 * value ) = 0;
	virtual mmlBool ToInteger64 ( mmlICString * str , mmlInt64 * value ) = 0;
	virtual mmlBool ToUInteger16 ( mmlICString * str, mmlUInt16 * value ) = 0;
	virtual mmlBool ToUInteger32 ( mmlICString * str, mmlUInt32 * value ) = 0;
	virtual mmlBool ToUInteger64 ( mmlICString * str, mmlUInt64 * value ) = 0;
	virtual mmlBool ToFloat ( mmlICString * str , const  mmlInt32 pos , const mmlInt32 len , mmlFloat64 * value ) = 0;
	virtual mmlBool Explode ( mmlICString * str , const mmlChar * separators , mmlIList ** lst ) = 0;
	virtual mmlBool Implode ( mmlIList * lst , mmlICString ** str ) = 0;
	virtual mmlBool Split ( mmlICString * src , const mmlInt32 pos, mmlICString ** left , mmlICString ** right ) = 0;
	virtual mmlBool Concat ( mmlICString * left , mmlICString * right , mmlICString ** result ) = 0;
	virtual mmlBool Integer16( const mmlInt16 integer , mmlICString ** value ) = 0;
	virtual mmlBool Integer32( const mmlInt32 integer , mmlICString ** value ) = 0;
	virtual mmlBool Integer64( const mmlInt64 integer , mmlICString ** value ) = 0;
	virtual mmlBool UInteger16( const mmlUInt16 integer , mmlICString ** value ) = 0;
	virtual mmlBool UInteger32( const mmlUInt32 integer , mmlICString ** value ) = 0;
	virtual mmlBool UInteger64( const mmlUInt64 integer , mmlICString ** value ) = 0;
	virtual mmlBool Float( const mmlFloat64 integer , mmlICString ** value ) = 0;
	virtual mmlBool Escape ( mmlICString * src , const mmlChar * search, const mmlChar ** replace , mmlICString ** dst ) = 0;
    virtual mmlBool LowerCase ( mmlICString * src , mmlICString ** dst ) = 0;
    virtual mmlBool UpperCase ( mmlICString * src , mmlICString ** dst ) = 0;
	virtual mmlBool Trim ( mmlICString * src , mmlICString ** dst ) = 0;
	virtual mmlBool Replace ( mmlICString * src , const mmlChar * sstr, const mmlChar * replacer, mmlICString ** dst, mml_char_conversion_t rule = MML_CASE_SENSITIVE ) = 0;
	virtual mmlBool MultiReplace ( mmlICString * src , const mmlChar * sstr, const mmlChar * replacer, mmlICString ** dst, mml_char_conversion_t rule = MML_CASE_SENSITIVE  ) = 0;
	virtual mmlBool Read ( mmlIInputStream * stream, mmlICString ** dst , const mmlInt32 len = 0 ) = 0;


};

#endif //MML_INTERFACE_STRING_UTILS_HEADER_INCLUDED

