#define _CRT_SECURE_NO_WARNINGS

#include "mmlVariant.h"
#include "mmlICStringUtils.h"
#include "mml_components.h"
#include <string.h>
#include <math.h>



mml_core mmlIVariantStruct  * mmlNewVariantStruct   (mmlIVariantStruct * inherited)
{
	return new mmlVariantStruct(inherited);
}
mml_core mmlIVariantArray    * mmlNewVariantArray    ()
{
	return new mmlVariantArray;
}

#define ARR_PUSH(a)  if ( a != mmlNULL ) arr->Push(a);
mml_core mmlIVariantArray    * mmlNewVariantArrayFill ( mmlIVariant * a1, 
	                                                    mmlIVariant * a2, 
														mmlIVariant * a3, 
														mmlIVariant * a4, 
														mmlIVariant * a5, 
														mmlIVariant * a6, 
														mmlIVariant * a7,
														mmlIVariant * a8,
	                                                    mmlIVariant * a9,
														mmlIVariant * a10,
														mmlIVariant * a11, 
														mmlIVariant * a12, 
														mmlIVariant * a13, 
														mmlIVariant * a14, 
														mmlIVariant * a15, 
														mmlIVariant * a16)
{
	mmlVariantArray * arr = new mmlVariantArray();
	arr->Push(a1);
	ARR_PUSH(a2);
	ARR_PUSH(a3);
	ARR_PUSH(a4);
	ARR_PUSH(a5);
	ARR_PUSH(a6);
	ARR_PUSH(a7);
	ARR_PUSH(a8);
	ARR_PUSH(a9);
	ARR_PUSH(a10);
	ARR_PUSH(a11);
	ARR_PUSH(a12);
	ARR_PUSH(a13);
	ARR_PUSH(a14);
	ARR_PUSH(a15);
	ARR_PUSH(a16);
	return arr;
}

mml_core mmlIVariantInteger  * mmlNewVariantInteger  ( const mmlInt64 val )
{
	return new mmlVariantInteger(val);
}
mml_core mmlIVariantBoolean  * mmlNewVariantBoolean  ( const mmlBool val )
{
	return new mmlVariantBoolean(val);
}
mml_core mmlIVariantDouble   * mmlNewVariantDouble   ( const mmlFloat64 val )
{
	return new mmlVariantDouble(val);
}
mml_core mmlIVariantString   * mmlNewVariantString   ( mmlICString * val )
{
	return new mmlVariantString(val);
}
mml_core mmlIVariantRaw      * mmlNewVariantRaw      (mmlIBuffer * data)
{
	return new mmlVariantRaw(data);
}
mml_core mmlIVariantDateTime * mmlNewVariantDateTime ( const mml_tm * val )
{
	return new mmlVariantDateTime(val);
}


MML_OBJECT_IMPL2(mmlVariantStruct,mmlIVariantStruct,mmlIVariant)

mml_variant_type mmlVariantStruct::GetType ()
{
	return MML_STRUCT;
}

void mmlVariantStruct::Set ( const mmlChar * name , mmlIVariant * value )
{
	for ( std::deque < std::pair < mmlAutoPtr < mmlICString>  , mmlAutoPtr < mmlIVariant > > >::iterator member = mMembers.begin(); member != mMembers.end(); member ++ )
	{
		mmlBool equal;
		if ( member->first->CompareStr(MML_EQUAL , name , equal, MML_CASE_INSENSITIVE) == mmlTrue && equal == mmlTrue  )
		{
			member->second = value;
			return;
		}
	}
	mMembers.push_back(std::pair < mmlAutoPtr < mmlICString>  , mmlAutoPtr < mmlIVariant > > (mmlNewCString(name) , value));
}

void mmlVariantStruct::Set ( mmlICString * name , mmlIVariant * value )
{
	for ( std::deque < std::pair < mmlAutoPtr < mmlICString>  , mmlAutoPtr < mmlIVariant > > >::iterator member = mMembers.begin(); member != mMembers.end(); member ++ )
	{
		mmlBool equal;
		if ( member->first->CompareStr(MML_EQUAL , name->Get() , equal, MML_CASE_INSENSITIVE) == mmlTrue && equal == mmlTrue  )
		{
			member->second = value;
			return;
		}
	}
	mMembers.push_back(std::pair < mmlAutoPtr < mmlICString>  , mmlAutoPtr < mmlIVariant > > (name , value));
}

mmlBool mmlVariantStruct::Has ( const mmlChar * name )
{
	for ( std::deque < std::pair < mmlAutoPtr < mmlICString>  , mmlAutoPtr < mmlIVariant > > >::iterator member = mMembers.begin(); member != mMembers.end(); member ++ )
	{
		mmlBool equal;
		if ( member->first->CompareStr(MML_EQUAL , name , equal, MML_CASE_INSENSITIVE) == mmlTrue && equal == mmlTrue  )
		{
			return mmlTrue;
		}
	}
	return mmlFalse;
}

mmlBool mmlVariantStruct::Get ( const mmlChar * name , mmlIVariant ** value , const mmlBool create)
{
	for ( std::deque < std::pair < mmlAutoPtr < mmlICString> , mmlAutoPtr < mmlIVariant > > >::iterator member = mMembers.begin(); member != mMembers.end(); member ++ )
	{
		mmlBool equal;
		if ( member->first->CompareStr(MML_EQUAL , name , equal, MML_CASE_INSENSITIVE) == mmlTrue && equal == mmlTrue  )
		{
			*value = member->second;
			MML_ADDREF(*value);
			return mmlTrue;
		}
	}
	if ( mInherited != mmlNULL )
	{
		return mInherited->Get(name, value, create);
	}
	return mmlFalse;
}

mmlBool mmlVariantStruct::Compare ( mmlIVariant * object,
                                     const mmlCompareOperator mode,
	                                 mmlBool * _retval )
{
	return mmlFalse;
}


mmlBool mmlVariantStruct::Enumerate ( mmlIVariantStructEnumerator * en )
{
	for ( std::deque< std::pair< mmlAutoPtr< mmlICString > , mmlAutoPtr< mmlIVariant > > >::iterator pr = mMembers.begin(); pr != mMembers.end(); pr ++ )
	{
		if ( en->OnMember(0, pr->first, pr->second) == mmlFalse )
		{
			return mmlFalse;
		}
	}
	return mInherited == mmlNULL ? mmlTrue : mInherited->Enumerate(en);
}

void mmlVariantStruct::Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func )
{
	func(depth, arg, "{\n");
	for ( std::deque< std::pair< mmlAutoPtr< mmlICString > , mmlAutoPtr< mmlIVariant > > >::iterator pr = mMembers.begin(); pr != mMembers.end(); pr ++ )
	{
		func(depth + 1 , arg, "%s=\n" ,  pr->first->Get());
		if ( pr->second == mmlNULL )
		{
			func(depth + 1 , arg, "null\n");
		}
		else
		{
			pr->second->Dump(depth + 2, arg, func);
		}
	}
	func(depth, arg, "}\n");
}


mmlInt32 mmlVariantStruct::Size ()
{
	return mMembers.size();
}

mmlBool mmlVariantStruct::IsChanged ()
{
	mmlBool is_changed = mmlFalse;
	for ( mmlInt32 index = 0 ; index < mMembers.size(); index ++ )
	{
		is_changed = mMembers[index].second->IsChanged();
		if ( is_changed == mmlTrue )
		{
			break;
		}
	}
	return is_changed;
}

void mmlVariantStruct::ResetChanged ()
{
	for ( mmlInt32 index = 0 ; index < mMembers.size(); index ++ )
	{
		mMembers[index].second->ResetChanged();
	}
}


MML_OBJECT_IMPL_ADDREF(mmlVariantInteger)
MML_OBJECT_IMPL_RELEASE(mmlVariantInteger)
MML_OBJECT_IMPL_GETREF(mmlVariantInteger)
MML_OBJECT_INTERFACE_MAP_BEGIN(mmlVariantInteger)
	MML_OBJECT_INTERFACE_MAP_ENTRY(mmlIVariantInteger)
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIVariantBoolean)) == 0)
	{
		mmlVariantBoolean * boolean = new mmlVariantBoolean();
		boolean->Set(this->Get() == 0 ? mmlFalse : mmlTrue );
		foundInterface = boolean;
	}
	else
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIVariantDouble)) == 0)
	{
		mmlIVariantDouble * dbl = new mmlVariantDouble();
		dbl->Set((mmlFloat64)this->Get());
		foundInterface = dbl;
	}
	else
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIVariantString)) == 0)
	{
		mmlIVariantString * str = new mmlVariantString();
		mmlChar int_str[64];
		mmlSprintf(int_str , sizeof(int_str) , "%" MML_INT64 , this->Get());
		str->Set(int_str);
		foundInterface = str;
	}
	else
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIVariantDateTime)) == 0)
	{
		mmlIVariantDateTime * dt = new mmlVariantDateTime();
		mml_timespec ts = {0};
		ts.tv_sec = Get();
		mml_tm tm = {0};
		mml_clock_localtime(&ts, &tm);
		dt->Set(tm);
		foundInterface = dt;
	}
	else
	MML_OBJECT_INTERFACE_MAP_ENTRY(mmlIVariant)
MML_OBJECT_INTERFACE_MAP_END(mmlVariantInteger)
MML_OBJECT_RELATIVE_MAP_BEGIN(mmlVariantInteger)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantInteger)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantBoolean)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantDouble)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantString)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariant)
MML_OBJECT_RELATIVE_MAP_END(mmlVariantInteger)

mmlVariantInteger::mmlVariantInteger(const mmlInt64 val)
{
		mValue = val;
}

mml_variant_type mmlVariantInteger::GetType ()
{
	return MML_INTEGER;
}

void mmlVariantInteger::Set ( const mmlInt64 & value )
{
	mValue = value;
}

mmlInt64 mmlVariantInteger::Get ()
{
	return mValue;
}

void mmlVariantInteger::Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func )
{
	func (depth, arg , "%" MML_INT64 "\n", mValue);
}

mmlBool mmlVariantInteger::Compare ( mmlIVariant * object,
                                      const mmlCompareOperator mode,
	                                  mmlBool * _retval )
{
	mmlSharedPtr < mmlIVariantInteger > int_var = object;
	if ( int_var == mmlNULL )
	{
		return mmlFalse;
	}
	if ( mode == MML_EQUAL )
	{
		*_retval = mValue == int_var->Get() ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_NOT_EQUAL )
	{
		*_retval = mValue != int_var->Get() ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_LESS )
	{
		*_retval = mValue < int_var->Get() ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_MORE )
	{
		*_retval = mValue > int_var->Get() ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_LESS_OR_EQUAL )
	{
		*_retval = mValue <= int_var->Get() ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_MORE_OR_EQUAL )
	{
		*_retval = mValue >= int_var->Get() ? mmlTrue : mmlFalse;
	}
	return mmlTrue;
}


MML_OBJECT_IMPL_ADDREF(mmlVariantDouble)
MML_OBJECT_IMPL_RELEASE(mmlVariantDouble)
MML_OBJECT_IMPL_GETREF(mmlVariantDouble)
MML_OBJECT_INTERFACE_MAP_BEGIN(mmlVariantDouble)
	MML_OBJECT_INTERFACE_MAP_ENTRY(mmlIVariantDouble)
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIVariantBoolean)) == 0)
	{
		mmlVariantBoolean * boolean = new mmlVariantBoolean();
		boolean->Set(this->Get() == 0.0 ? mmlFalse : mmlTrue );
		foundInterface = boolean;
	}
	else
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIVariantInteger)) == 0)
	{
		mmlIVariantInteger * integer = new mmlVariantInteger();
		integer->Set((mmlInt64)this->Get());
		foundInterface = integer;
	}
	else
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIVariantString)) == 0)
	{
		mmlIVariantString * str = new mmlVariantString();
		mmlFloat64 dbl = floor(this->Get());
		if ( this->Get() - dbl > 0.0 )
		{
			mmlChar dbl_str[64];
			mmlSprintf(dbl_str , sizeof(dbl_str) , "%f" , this->Get());
			str->Set(dbl_str);
		}
		else
		{
			mmlChar int_str[64];
			mmlSprintf(int_str , sizeof(int_str) , "%lld" , (mmlInt64)this->Get());
			str->Set(int_str);
		}
		foundInterface = str;
	}
	else
	MML_OBJECT_INTERFACE_MAP_ENTRY(mmlIVariant)
MML_OBJECT_INTERFACE_MAP_END(mmlVariantDouble)
MML_OBJECT_RELATIVE_MAP_BEGIN(mmlVariantDouble)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantInteger)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantBoolean)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantDouble)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantString)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariant)
MML_OBJECT_RELATIVE_MAP_END(mmlVariantDouble)

mmlVariantDouble::mmlVariantDouble( const mmlFloat64 val )
:mValue(val)
{
}

mml_variant_type mmlVariantDouble::GetType ()
{
	return MML_DOUBLE;
}

void mmlVariantDouble::Set ( const mmlFloat64 & value )
{
	mChanged = mmlTrue;
	mValue = value;
}

mmlFloat64 mmlVariantDouble::Get ()
{
	return mValue;
}

void mmlVariantDouble::Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func )
{
	func (depth , arg, "%f\n", mValue);
}

mmlBool mmlVariantDouble::Compare ( mmlIVariant * object,
                                      const mmlCompareOperator mode,
	                                  mmlBool * _retval )
{
	mmlSharedPtr < mmlIVariantDouble > dbl_var = object;
	if ( dbl_var == mmlNULL )
	{
		return mmlFalse;
	}
	if ( mode == MML_EQUAL )
	{
		*_retval = mValue == dbl_var->Get() ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_NOT_EQUAL )
	{
		*_retval = mValue != dbl_var->Get() ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_LESS )
	{
		*_retval = mValue < dbl_var->Get() ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_MORE )
	{
		*_retval = mValue > dbl_var->Get() ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_LESS_OR_EQUAL )
	{
		*_retval = mValue <= dbl_var->Get() ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_MORE_OR_EQUAL )
	{
		*_retval = mValue >= dbl_var->Get() ? mmlTrue : mmlFalse;
	}
	return mmlTrue;
}

MML_OBJECT_IMPL_ADDREF(mmlVariantBoolean)
MML_OBJECT_IMPL_RELEASE(mmlVariantBoolean)
MML_OBJECT_IMPL_GETREF(mmlVariantBoolean)
MML_OBJECT_INTERFACE_MAP_BEGIN(mmlVariantBoolean)
	MML_OBJECT_INTERFACE_MAP_ENTRY(mmlIVariantBoolean)
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIVariantDouble)) == 0)
	{
		mmlIVariantDouble * dbl = new mmlVariantDouble();
		dbl->Set(this->Get() == mmlFalse ? 0.0 : 1.0 );
		foundInterface = dbl;
	}
	else
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIVariantInteger)) == 0)
	{
		mmlIVariantInteger * integer = new mmlVariantInteger();
		integer->Set(this->Get() == 0 ? mmlFalse : mmlTrue );
		foundInterface = integer;
	}
	else
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIVariantString)) == 0)
	{
		mmlIVariantString * str = new mmlVariantString();
		str->Set(this->Get() == mmlTrue ? "true" : "false");
		foundInterface = str;
	}
	else
	MML_OBJECT_INTERFACE_MAP_ENTRY(mmlIVariant)
MML_OBJECT_INTERFACE_MAP_END(mmlVariantBoolean)
MML_OBJECT_RELATIVE_MAP_BEGIN(mmlVariantBoolean)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantInteger)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantBoolean)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantDouble)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantString)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariant)
MML_OBJECT_RELATIVE_MAP_END(mmlVariantBoolean)

mmlVariantBoolean::mmlVariantBoolean(const mmlBool val)
:mValue(val)
{

}

mml_variant_type mmlVariantBoolean::GetType ()
{
	return MML_BOOLEAN;
}

void mmlVariantBoolean::Set ( const mmlBool & value )
{
	mChanged = mmlTrue;
	mValue = value;
}

mmlBool mmlVariantBoolean::Get ()
{
	return mValue;
}

void mmlVariantBoolean::Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func )
{
	func (depth , arg, "%s\n" , mValue == mmlTrue ? "true" : "false");
}

mmlBool mmlVariantBoolean::Compare ( mmlIVariant * object,
                                      const mmlCompareOperator mode,
	                                  mmlBool * _retval )
{
	mmlSharedPtr < mmlVariantBoolean > dbl_var = object;
	if ( dbl_var == mmlNULL )
	{
		return mmlFalse;
	}
	if ( mode == MML_EQUAL )
	{
		*_retval = mValue == dbl_var->Get() ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_NOT_EQUAL )
	{
		*_retval = mValue != dbl_var->Get() ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_LESS )
	{
		*_retval = mValue < dbl_var->Get() ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_MORE )
	{
		*_retval = mValue > dbl_var->Get() ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_LESS_OR_EQUAL )
	{
		*_retval = mValue <= dbl_var->Get() ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_MORE_OR_EQUAL )
	{
		*_retval = mValue >= dbl_var->Get() ? mmlTrue : mmlFalse;
	}
	return mmlTrue;
}


MML_OBJECT_IMPL_ADDREF(mmlVariantString)
MML_OBJECT_IMPL_RELEASE(mmlVariantString)
MML_OBJECT_IMPL_GETREF(mmlVariantString)
MML_OBJECT_INTERFACE_MAP_BEGIN(mmlVariantString)
	MML_OBJECT_INTERFACE_MAP_ENTRY(mmlIVariantString)
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIVariantDouble)) == 0)
	{
		mmlIVariantDouble * dbl = new mmlVariantDouble();
		dbl->Set(this->Get() == mmlNULL ? 0.0 : mmlStoF( this->Get()->Get() , mmlNULL));
		foundInterface = dbl;
	}
	else
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIVariantInteger)) == 0)
	{
		mmlIVariantInteger * integer = new mmlVariantInteger();
		mmlInt64 val = -1;
		if ( this->Get() != mmlNULL &&
			 this->Get()->Get()[0] >= '0' &&
			 this->Get()->Get()[0] <= '9' )
		{
			val = mmlStoD(this->Get()->Get() , mmlNULL);
		}
		integer->Set(val);
		foundInterface = integer;
	}
	else
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIVariantBoolean)) == 0)
	{
		mmlIVariantBoolean * boolean = new mmlVariantBoolean();
		mmlBool equal = mmlFalse;
		if ( this->Get() != mmlNULL)
		{
			this->Get()->CompareStr(MML_EQUAL , "true" , equal);
		}
		boolean->Set(equal);
		foundInterface = boolean;
	}
	else
	MML_OBJECT_INTERFACE_MAP_ENTRY(mmlIVariant)
MML_OBJECT_INTERFACE_MAP_END(mmlVariantString)
MML_OBJECT_RELATIVE_MAP_BEGIN(mmlVariantString)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantInteger)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantBoolean)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantDouble)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantString)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariant)
MML_OBJECT_RELATIVE_MAP_END(mmlVariantString)

mmlVariantString::mmlVariantString( mmlICString * val ) :
	mValue(val)
{

}

mml_variant_type mmlVariantString::GetType ()
{
	return MML_STRING;
}

void mmlVariantString::Set ( const mmlChar * value )
{
	mChanged = mmlTrue;
	mValue = mmlNewCString(value);
}

void mmlVariantString::Set ( mmlICString * value )
{
	mChanged = mmlTrue;
	mValue = value;
}

mmlICString * mmlVariantString::Get ()
{
	return mValue;
}

#include "mmlCString.h"

void mmlVariantString::Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func)
{
	func (depth, arg, "%s\n" , mValue == mmlNULL ? "(null)" : mValue->Get());
}

mmlBool mmlVariantString::Compare ( mmlIVariant * object,
                                     const mmlCompareOperator mode,
	                                 mmlBool * _retval )
{
	mmlSharedPtr < mmlVariantString > str_var = object;
	if ( str_var == mmlNULL )
	{
		return mmlFalse;
	}
	if ( mode == MML_EQUAL )
	{
		if ( mValue == mmlNULL && str_var->Get() == mmlNULL )
		{
			*_retval = mmlTrue;
		}
		else if ( mValue == mmlNULL && str_var->Get() != mmlNULL )
		{
			*_retval = mmlFalse;
		}
		else if ( mValue != mmlNULL && str_var->Get() == mmlNULL )
		{
			*_retval = mmlFalse;
		}
		else
		{
			mValue->CompareStr(MML_EQUAL , str_var->Get()->Get() , *_retval);
		}
	}
	else if ( mode == MML_NOT_EQUAL )
	{
		if ( mValue == mmlNULL && str_var->Get() == mmlNULL )
		{
			*_retval = mmlFalse;
		}
		else if ( mValue == mmlNULL && str_var->Get() != mmlNULL )
		{
			*_retval = mmlTrue;
		}
		else if ( mValue != mmlNULL && str_var->Get() == mmlNULL )
		{
			*_retval = mmlTrue;
		}
		else
		{
			mValue->CompareStr(MML_NOT_EQUAL , str_var->Get()->Get() , *_retval);
		}
	}
	else if ( mode == MML_LESS )
	{
		if ( mValue == mmlNULL && str_var->Get() == mmlNULL )
		{
			*_retval = mmlFalse;
		}
		else if ( mValue == mmlNULL && str_var->Get() != mmlNULL )
		{
			*_retval = mmlTrue;
		}
		else if ( mValue != mmlNULL && str_var->Get() == mmlNULL )
		{
			*_retval = mmlFalse;
		}
		else
		{
			mValue->CompareStr(MML_LESS , str_var->Get()->Get() , *_retval);
		}
	}
	else if ( mode == MML_MORE )
	{
		if ( mValue == mmlNULL && str_var->Get() == mmlNULL )
		{
			*_retval = mmlFalse;
		}
		else if ( mValue == mmlNULL && str_var->Get() != mmlNULL )
		{
			*_retval = mmlFalse;
		}
		else if ( mValue != mmlNULL && str_var->Get() == mmlNULL )
		{
			*_retval = mmlTrue;
		}
		else
		{
			mValue->CompareStr(MML_MORE , str_var->Get()->Get() , *_retval);
		}
	}
	else if ( mode == MML_LESS_OR_EQUAL )
	{
		if ( mValue == mmlNULL && str_var->Get() == mmlNULL )
		{
			*_retval = mmlTrue;
		}
		else if ( mValue == mmlNULL && str_var->Get() != mmlNULL )
		{
			*_retval = mmlTrue;
		}
		else if ( mValue != mmlNULL && str_var->Get() == mmlNULL )
		{
			*_retval = mmlFalse;
		}
		else
		{
			mValue->CompareStr(MML_LESS_OR_EQUAL , str_var->Get()->Get() , *_retval);
		}
	}
	else if ( mode == MML_MORE_OR_EQUAL )
	{
		if ( mValue == mmlNULL && str_var->Get() == mmlNULL )
		{
			*_retval = mmlTrue;
		}
		else if ( mValue == mmlNULL && str_var->Get() != mmlNULL )
		{
			*_retval = mmlFalse;
		}
		else if ( mValue != mmlNULL && str_var->Get() == mmlNULL )
		{
			*_retval = mmlTrue;
		}
		else
		{
			mValue->CompareStr(MML_MORE_OR_EQUAL , str_var->Get()->Get() , *_retval);
		}
	}
	return mmlTrue;
}

MML_OBJECT_IMPL2(mmlVariantArray,mmlIVariantArray,mmlIVariant)

mml_variant_type mmlVariantArray::GetType ()
{
	return MML_ARRAY;
}

void mmlVariantArray::Push (mmlIVariant * primitive)
{
	mMembers.push_back(primitive);
}

mmlInt32 mmlVariantArray::Size ()
{
	return (mmlInt32)mMembers.size();
}

mmlBool mmlVariantArray::Get ( const mmlInt32 index , mmlIVariant ** primitive )
{
	if ( index < 0 || index >= mMembers.size() )
	{
		return mmlFalse;
	}
	*primitive = mMembers[index];
	MML_ADDREF(*primitive);
	return mmlTrue;
}

mmlBool mmlVariantArray::Set ( const mmlInt32 index , mmlIVariant * primitive)
{
	if ( index < 0 || index >= mMembers.size() )
	{
		return mmlFalse;
	}
	mMembers[index] = primitive;
	return mmlTrue;
}


mmlBool mmlVariantArray::Pop ( mmlIVariant ** primitive)
{
	if ( mMembers.size() > 0 )
	{
		*primitive = mMembers.front();
		MML_ADDREF(*primitive);
		mMembers.pop_front();
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool  mmlVariantArray::Erase ( const mmlInt32 index )
{
	if ( index < 0 || index >= mMembers.size() )
	{
		return mmlFalse;
	}
	mmlInt32 count = 0;
	for ( std::deque < mmlAutoPtr < mmlIVariant > >::iterator member = mMembers.begin(); member != mMembers.end(); member ++ , count ++)
	{
		if ( count == index )
		{
			mMembers.erase(member);
			return mmlTrue;
		}
	}
	return mmlFalse;
}

mmlBool mmlVariantArray::Enumerate ( mmlIVariantArrayEnumerator * en )
{
	mmlInt32 index = 0;
	for ( std::deque < mmlAutoPtr < mmlIVariant > >::iterator member = mMembers.begin(); member != mMembers.end(); member ++ , index ++)
	{
		if ( en->OnMember(index, *member) == mmlFalse )
		{
			return mmlFalse;
		}
	}
	return mmlTrue;
}

void mmlVariantArray::Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func )
{
	func(depth, arg, "[\n");
	for ( std::deque < mmlAutoPtr < mmlIVariant > >::iterator member = mMembers.begin(); member != mMembers.end(); member ++ )
	{
		(*member)->Dump(depth + 1, arg, func);
	}
	func(depth, arg,"]\n");
}
#define SWAP(A, B) { mmlAutoPtr < mmlIVariant > t = A; A = B; B = t; }

mmlBool mmlVariantArray::Sort (mml_variable_array_predicate_t predicate)
{
	mmlInt32 i, j;
	for (i = mMembers.size() - 1; i > 0; i--)
	{
		for (j = 0; j < i; j++)
		{
			if (predicate(mMembers[j + 1] , mMembers[j]))
			{
				SWAP( mMembers[j], mMembers[j + 1] );
			}
		}
	}
	return mmlTrue;
}

mmlBool mmlVariantArray::IsChanged ()
{
	mmlBool is_changed = mmlFalse;
	for ( mmlInt32 index = 0 ; index < mMembers.size(); index ++ )
	{
		is_changed = mMembers[index]->IsChanged();
		if ( is_changed == mmlTrue )
		{
			break;
		}
	}
	return is_changed;
}

void mmlVariantArray::ResetChanged ()
{
	for ( mmlInt32 index = 0 ; index < mMembers.size(); index ++ )
	{
		mMembers[index]->ResetChanged();
	}
}

mmlBool mmlVariantArray::Compare ( mmlIVariant * object,
                                    const mmlCompareOperator mode,
	                                mmlBool * _retval )
{
	return mmlFalse;
}

MML_OBJECT_IMPL2(mmlVariantRaw,mmlIVariantRaw,mmlIVariant)

mmlVariantRaw::mmlVariantRaw(mmlIBuffer * data)
:mData(data)
{
	
}


mmlVariantRaw::~mmlVariantRaw()
{

}

mml_variant_type mmlVariantRaw::GetType ()
{
	return MML_RAW;
}

void mmlVariantRaw::Set (mmlIBuffer * data)
{
	mChanged = mmlTrue;
	mData = data;
}


void mmlVariantRaw::Get( mmlIBuffer ** data )
{
	*data = mData;
	MML_ADDREF(*data);
}


void mmlVariantRaw::Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func)
{
	func(depth, arg, "RAW size %d\n", mData == mmlNULL ? 0 : mData->Size());
}

mmlBool mmlVariantRaw::Compare ( mmlIVariant * object,
                                  const mmlCompareOperator mode,
	                              mmlBool * _retval )
{
	return mmlFalse;
}

MML_OBJECT_IMPL_ADDREF(mmlVariantDateTime)
MML_OBJECT_IMPL_RELEASE(mmlVariantDateTime)
MML_OBJECT_IMPL_GETREF(mmlVariantDateTime)
MML_OBJECT_INTERFACE_MAP_BEGIN(mmlVariantDateTime)
	MML_OBJECT_INTERFACE_MAP_ENTRY(mmlVariantDateTime)
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIVariantString)) == 0)
	{
		mmlIVariantString * str = new mmlVariantString();
		mmlChar buffer[32];
		mml_tm tm = Get();
		mml_clock_strftime("%Y-%m-%dT%H:%M:%S" , &tm, buffer, sizeof(buffer));
		str->Set(buffer);
		foundInterface = str;
	}
	else
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIVariantInteger)) == 0)
	{
		mmlVariantInteger * integer = new mmlVariantInteger();
		mml_tm tm = Get();
		mml_timespec ts;
		mml_clock_mktime(&tm, &ts);
		integer->Set(ts.tv_sec);
		foundInterface = integer;
	}
	else
	MML_OBJECT_INTERFACE_MAP_ENTRY(mmlIVariant)
MML_OBJECT_INTERFACE_MAP_END(mmlVariantDateTime)
MML_OBJECT_RELATIVE_MAP_BEGIN(mmlVariantDateTime)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariantString)
	MML_OBJECT_RELATIVE_MAP_ENTRY(mmlIVariant)
MML_OBJECT_RELATIVE_MAP_END(mmlVariantDateTime)

mmlVariantDateTime::mmlVariantDateTime( const mml_tm * val )
{
	if ( val != mmlNULL )
	{	
		mValue.tm_msec  = val->tm_msec;
		mValue.tm_sec   = val->tm_sec;
		mValue.tm_min   = val->tm_min;
		mValue.tm_hour  = val->tm_hour;
		mValue.tm_mday  = val->tm_mday;
		mValue.tm_mon   = val->tm_mon;
		mValue.tm_year  = val->tm_year;
		mValue.tm_wday  = val->tm_wday;
		mValue.tm_yday  = val->tm_yday;
		mValue.tm_isdst = val->tm_isdst;
	}
	else
	{
		mValue.tm_msec  = 0;
		mValue.tm_sec   = 0;
		mValue.tm_min   = 0;
		mValue.tm_hour  = 0;
		mValue.tm_mday  = 0;
		mValue.tm_mon   = 0;
		mValue.tm_year  = 0;
		mValue.tm_wday  = 0;
		mValue.tm_yday  = 0;
		mValue.tm_isdst = 0;
	}
}

mml_variant_type mmlVariantDateTime::GetType ()
{
	return MML_DATETIME;
}

void mmlVariantDateTime::Set ( const mml_tm & value )
{
	mChanged = mmlTrue;
	mValue.tm_msec  = value.tm_msec;
	mValue.tm_sec   = value.tm_sec;
	mValue.tm_min   = value.tm_min;
	mValue.tm_hour  = value.tm_hour;
	mValue.tm_mday  = value.tm_mday;
	mValue.tm_mon   = value.tm_mon;
	mValue.tm_year  = value.tm_year;
	mValue.tm_wday  = value.tm_wday;
	mValue.tm_yday  = value.tm_yday;
	mValue.tm_isdst = value.tm_isdst;
}

const mml_tm & mmlVariantDateTime::Get ()
{
	return mValue;
}

void mmlVariantDateTime::Dump (const mmlInt32 depth , void * arg, mml_variant_log_func func )
{
	char date[64];
	mml_clock_strftime("%Y-%m-%d %H:%M:%S\n", &mValue, date , sizeof(date));
	func( depth, arg, date);
}

mmlBool mmlVariantDateTime::Compare ( mmlIVariant * object,
									   const mmlCompareOperator mode,
									   mmlBool * _retval )
{
	mmlSharedPtr < mmlIVariantDateTime > datetime_var = object;
	if ( datetime_var == mmlNULL  )
	{
		return mmlFalse;
	}

	mmlInt32 diff = 0;

	diff = mValue.tm_year - datetime_var->Get().tm_year;
	if ( diff == 0 )
	{
		diff = mValue.tm_mon - datetime_var->Get().tm_mon;
		if ( diff == 0 )
		{
			diff = mValue.tm_mday - datetime_var->Get().tm_mday;
			if ( diff == 0 )
			{
				diff = mValue.tm_hour - datetime_var->Get().tm_hour;
				if ( diff == 0 )
				{
					diff = mValue.tm_min - datetime_var->Get().tm_min;
					if ( diff == 0 )
					{
						diff = mValue.tm_sec - datetime_var->Get().tm_sec;
						if ( diff == 0 )
						{
							diff = mValue.tm_msec - datetime_var->Get().tm_msec;
						}
					}
				}
			}
		}
	}

	if ( mode == MML_EQUAL )
	{
		*_retval = diff == 0 ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_NOT_EQUAL )
	{
		*_retval = diff != 0 ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_LESS )
	{
		*_retval = diff < 0 ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_MORE )
	{
		*_retval = diff > 0 ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_LESS_OR_EQUAL )
	{
		*_retval = diff <= 0 ? mmlTrue : mmlFalse;
	}
	else if ( mode == MML_MORE_OR_EQUAL )
	{
		*_retval = diff >= 0 ? mmlTrue : mmlFalse;
	}
	return mmlTrue;
}

