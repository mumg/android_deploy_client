#ifndef MML_I_VARIANT_HEADER_INCLUDED
#define MML_I_VARIANT_HEADER_INCLUDED

#include "mmlIObject.h"
#include "mmlICString.h"
#include "mmlIList.h"
#include "mmlIBuffer.h"
#include "mml_time.h"

typedef enum
{
	MML_STRUCT,
	MML_INTEGER,
	MML_DOUBLE,
	MML_BOOLEAN,
	MML_STRING,
	MML_ARRAY,
	MML_RAW,
	MML_DATETIME,
	MML_CLASS,
	MML_CUSTOM,
	MML_INVALID
}mml_variant_type;


#define MML_VARIANT_PRIMITIVE_UUID { 0x6A2F6083, 0x6CF2, 0x1014, 0x848B , { 0xA6, 0x9E, 0x61, 0xE1, 0x64, 0x99 } }

typedef void (*mml_variant_log_func) ( const mmlInt32 level, void * arg, const mmlChar * formatter , ... );

class mmlIVariant : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_VARIANT_PRIMITIVE_UUID)
public:
	virtual mml_variant_type GetType () = 0;
	virtual void Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func ) = 0;

	virtual mmlBool Compare ( mmlIVariant * object,
		                      const mmlCompareOperator mode,
		                      mmlBool * _retval ) = 0;

	virtual mmlBool IsChanged () = 0;
	virtual void ResetChanged () = 0;
};


class mmlIVariantStructEnumerator : public mmlIObject
{
public:
	virtual mmlBool OnMember ( const mmlInt32 tag, mmlICString * name , mmlIVariant * val ) = 0;
};

#define MML_VARIANT_STRUCT_UUID { 0x768BE224, 0x6CF2, 0x1014, 0xA28E , { 0xE4, 0x40, 0xCF, 0x1F, 0xBB, 0x4A } }

class mmlIVariantStruct : public mmlIVariant
{
	MML_OBJECT_UUID_DECL(MML_VARIANT_STRUCT_UUID)
public:
	virtual void Set ( const mmlChar * name , mmlIVariant * value ) = 0;
	virtual void Set ( mmlICString * name , mmlIVariant * value ) = 0;
	virtual mmlBool Has ( const mmlChar * name ) = 0;
	virtual mmlBool Get ( const mmlChar * name , mmlIVariant ** value , const mmlBool create = mmlFalse ) = 0;
	virtual mmlInt32 Size () = 0;
	virtual mmlBool Enumerate ( mmlIVariantStructEnumerator * en ) = 0;
	virtual void Inherit ( mmlIVariantStruct * str ) =0;
};

#define MML_VARIANT_INTEGER_UUID { 0x792265C6, 0x6CF2, 0x1014, 0xBF68 , { 0xFB, 0x77, 0x67, 0x44, 0x5D, 0x09 } }

class mmlIVariantInteger : public mmlIVariant
{
	MML_OBJECT_UUID_DECL(MML_VARIANT_INTEGER_UUID)
public:
	virtual void Set ( const mmlInt64 & value ) = 0;
	virtual mmlInt64 Get () = 0;
};

#define MML_VARIANT_DOUBLE_UUID { 0x7CFA13DE, 0x6CF2, 0x1014, 0x88FC , { 0xF1, 0x4A, 0xDB, 0x3F, 0x3B, 0xAC } }

class mmlIVariantDouble : public mmlIVariant
{
	MML_OBJECT_UUID_DECL(MML_VARIANT_DOUBLE_UUID)
public:
	virtual void Set ( const mmlFloat64 & value ) = 0;
	virtual mmlFloat64 Get () = 0;
};

#define MML_VARIABLE_BOOLEAN_UUID { 0x80DC5188, 0x6CF2, 0x1014, 0xADBF , { 0xAF, 0x42, 0xA2, 0x98, 0x73, 0xE0 } }

class mmlIVariantBoolean : public mmlIVariant
{
	MML_OBJECT_UUID_DECL(MML_VARIABLE_BOOLEAN_UUID)
public:
	virtual void Set ( const mmlBool & value ) = 0;
	virtual mmlBool Get () = 0;
};

#define MML_VARIANT_STRING_UUID { 0x84D835F5, 0x6CF2, 0x1014, 0xB516 , { 0xA0, 0xEF, 0x33, 0xDE, 0xFE, 0x0D } }

class mmlIVariantString : public mmlIVariant
{
	MML_OBJECT_UUID_DECL(MML_VARIANT_STRING_UUID)
public:
	virtual void Set ( const mmlChar * value ) = 0;
	virtual void Set ( mmlICString * value ) = 0;
	virtual mmlICString * Get () = 0;
};

#define MML_VARIANT_ARRAY_UUID { 0x89B5E7DE, 0x6CF2, 0x1014, 0xA71F , { 0x96, 0xB3, 0xBB, 0xA9, 0x0C, 0x87 } }

class mmlIVariantArrayEnumerator : public mmlIObject
{
public:
	virtual mmlBool OnMember ( const mmlInt32 index , mmlIVariant * val ) = 0;
};

typedef mmlInt32 (*mml_variable_array_predicate_t)( mmlIVariant * left, mmlIVariant * right);

class mmlIVariantArray : public mmlIVariant
{
	MML_OBJECT_UUID_DECL(MML_VARIANT_ARRAY_UUID)
public:
	virtual void Push (mmlIVariant * primitive) = 0;
	virtual mmlInt32 Size () = 0;
	virtual mmlBool Get ( const mmlInt32 index , mmlIVariant ** val ) = 0;
	virtual mmlBool Set ( const mmlInt32 index , mmlIVariant * val) = 0;
	virtual mmlBool Erase ( const mmlInt32 index ) = 0;
	virtual mmlBool Enumerate ( mmlIVariantArrayEnumerator * en ) = 0;
	virtual mmlBool Pop ( mmlIVariant ** primitive) = 0;
	virtual mmlBool Sort (mml_variable_array_predicate_t predicate) = 0;
};

#define MML_VARIANT_RAW_UUID { 0x8F14D290, 0x6CF2, 0x1014, 0x89D4 , { 0xE4, 0x61, 0xFB, 0xAA, 0xBA, 0xA5 } }

class mmlIVariantRaw : public mmlIVariant
{
	MML_OBJECT_UUID_DECL(MML_VARIANT_RAW_UUID)
public:
	virtual void Set ( mmlIBuffer * data ) = 0;
	virtual void Get ( mmlIBuffer ** data ) = 0;
};

#define MML_VARIANT_DATETIME_UUID { 0x503e1a41, 0xef0c, 0x402b, 0xa10a , { 0x14, 0xcd, 0x7d, 0x46, 0xd2, 0xfa } }

class mmlIVariantDateTime : public mmlIVariant
{
	MML_OBJECT_UUID_DECL(MML_VARIANT_DATETIME_UUID)
public:
	virtual void Set ( const mml_tm & value ) = 0;
	virtual const mml_tm & Get () = 0;
};

mml_core mmlIVariantStruct   * mmlNewVariantStruct  (mmlIVariantStruct * inherited = mmlNULL);
mml_core mmlIVariantArray    * mmlNewVariantArray   ();
mml_core mmlIVariantArray    * mmlNewVariantArrayFill ( mmlIVariant * a1, mmlIVariant * a2 = mmlNULL, mmlIVariant * a3 = mmlNULL, mmlIVariant * a4 = mmlNULL, mmlIVariant * a5 = mmlNULL, mmlIVariant * a6 = mmlNULL, mmlIVariant * a7 = mmlNULL,mmlIVariant * a8 = mmlNULL,
	                                                    mmlIVariant * a9 = mmlNULL, mmlIVariant * a10 = mmlNULL, mmlIVariant * a11 = mmlNULL, mmlIVariant * a12 = mmlNULL, mmlIVariant * a13 = mmlNULL, mmlIVariant * a14 = mmlNULL, mmlIVariant * a15 = mmlNULL, mmlIVariant * a16 = mmlNULL);
mml_core mmlIVariantInteger  * mmlNewVariantInteger ( const mmlInt64 val = 0);
mml_core mmlIVariantBoolean  * mmlNewVariantBoolean ( const mmlBool val = mmlFalse);
mml_core mmlIVariantDouble   * mmlNewVariantDouble  ( const mmlFloat64 val = 0.0);
mml_core mmlIVariantString   * mmlNewVariantString  ( mmlICString * val = mmlNULL);
mml_core mmlIVariantRaw      * mmlNewVariantRaw     ( mmlIBuffer * data = mmlNULL );
mml_core mmlIVariantDateTime * mmlNewVariantDateTime( const mml_tm * val = mmlNULL);


#define MML_VARIANT_IMPL \
	private: \
	    class variant_changed { \
		private: mmlBool mChanged; \
		public: \
			variant_changed() { mChanged = mmlTrue; } \
			operator mmlBool & () { return mChanged; } \
			void operator = ( const mmlBool state ) { mChanged = state; } \
		}; \
		variant_changed mChanged; \
	public: \
		mmlBool IsChanged () { return mChanged; } \
		void ResetChanged () { mChanged = mmlFalse; }

#endif
