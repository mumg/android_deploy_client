#ifndef MML_VARIABLE_H
#define MML_VARIABLE_H

#include "mmlIVariant.h"
#include "mmlPtr.h"
#include <list>
#include <string>
#include <deque>
#include <vector>
#include <map>
#include <string.h>



class mmlVariantStruct : public mmlIVariantStruct
{
	MML_OBJECT_DECL
public:
	mmlVariantStruct(mmlIVariantStruct * inherited = mmlNULL)
		:mInherited(inherited)
	{

	}

	mml_variant_type GetType ();

	void Set ( const mmlChar * name , mmlIVariant * value );

	void Set ( mmlICString * name , mmlIVariant * value );

	mmlBool Get ( const mmlChar * name , mmlIVariant ** value , const mmlBool create);

	mmlBool Enumerate ( mmlIVariantStructEnumerator * en );
	
	mmlInt32 Size ();

	void Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func);

	mmlBool Compare ( mmlIVariant * object,
		              const mmlCompareOperator mode,
		              mmlBool * _retval );

	mmlBool Has ( const mmlChar * name );

	mmlBool IsChanged ();
	
	void ResetChanged ();

	void Inherit ( mmlIVariantStruct * str )
	{
		mInherited = str;
	}

private:
	std::deque < std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlIVariant > > > mMembers;
	mmlAutoPtr < mmlIVariantStruct > mInherited;
};

class mmlVariantInteger : public mmlIVariantInteger
{
	MML_OBJECT_DECL
public:
	mml_variant_type GetType ();

	mmlVariantInteger(const mmlInt64 val = 0);

	void Set ( const mmlInt64 & value );

	mmlInt64 Get ();

	void Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func);


	mmlBool Compare ( mmlIVariant * object,
		              const mmlCompareOperator mode,
		              mmlBool * _retval );

private:
	mmlInt64 mValue;
	MML_VARIANT_IMPL
};

class mmlVariantDouble : public mmlIVariantDouble
{
	MML_OBJECT_DECL
public:
	mml_variant_type GetType ();

	mmlVariantDouble( const mmlFloat64 val = 0.0);

	void Set ( const mmlFloat64 & value );

	mmlFloat64 Get ();

	void Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func);

	mmlBool Compare ( mmlIVariant * object,
		              const mmlCompareOperator mode,
		              mmlBool * _retval );


private:
	mmlFloat64 mValue;
	MML_VARIANT_IMPL
};

class mmlVariantBoolean : public mmlIVariantBoolean
{
	MML_OBJECT_DECL
public:
	mml_variant_type GetType ();

	mmlVariantBoolean ( const mmlBool val = mmlFalse);

	void Set ( const mmlBool & value );

	mmlBool Get ();

	void Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func);

	mmlBool Compare ( mmlIVariant * object,
		              const mmlCompareOperator mode,
		              mmlBool * _retval );

private:
	mmlBool mValue;
	MML_VARIANT_IMPL
};

class mmlVariantString : public mmlIVariantString
{
	MML_OBJECT_DECL
public:

	mmlVariantString( mmlICString * val  = NULL );

	mml_variant_type GetType ();

	void Set ( const mmlChar * value );

	void Set ( mmlICString * value );

	mmlICString * Get ();

	void Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func);

	mmlBool Compare ( mmlIVariant * object,
		              const mmlCompareOperator mode,
		              mmlBool * _retval );


private:
	mmlAutoPtr < mmlICString > mValue;
	MML_VARIANT_IMPL
};

class mmlVariantArray : public mmlIVariantArray
{
	MML_OBJECT_DECL
public:

	mml_variant_type GetType ();

	void Push (mmlIVariant * primitive);

	mmlInt32 Size ();

	mmlBool Get ( const mmlInt32 index , mmlIVariant ** primitive );

	mmlBool Set ( const mmlInt32 index , mmlIVariant * primitive);

	mmlBool Erase ( const mmlInt32 index );

	mmlBool Enumerate ( mmlIVariantArrayEnumerator * en );

	mmlBool Pop ( mmlIVariant ** primitive);
	
	void Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func);

	mmlBool Compare ( mmlIVariant * object,
		              const mmlCompareOperator mode,
		              mmlBool * _retval );


	mmlBool Sort (mml_variable_array_predicate_t predicate);

	mmlBool IsChanged ();

	void ResetChanged ();

private:

	std::deque < mmlAutoPtr < mmlIVariant > > mMembers;
};


class mmlVariantRaw : public mmlIVariantRaw
{
	MML_OBJECT_DECL
public:

	mmlVariantRaw(mmlIBuffer * data = mmlNULL);

	~mmlVariantRaw();

	mml_variant_type GetType ();

	void Set ( mmlIBuffer * data );

	void Get ( mmlIBuffer ** data );

	void Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func);

	mmlBool Compare ( mmlIVariant * object,
		              const mmlCompareOperator mode,
		              mmlBool * _retval );

private:

	mmlAutoPtr < mmlIBuffer > mData;
	MML_VARIANT_IMPL
};

class mmlVariantDateTime : public mmlIVariantDateTime
{
	MML_OBJECT_DECL
public:
	mml_variant_type GetType ();

	mmlVariantDateTime( const mml_tm * val = mmlNULL );

	void Set ( const mml_tm & value );

	const mml_tm & Get ();

	void Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func);


	mmlBool Compare ( mmlIVariant * object,
					  const mmlCompareOperator mode,
					  mmlBool * _retval );

private:
	 mml_tm mValue;
	 MML_VARIANT_IMPL
};

#endif
