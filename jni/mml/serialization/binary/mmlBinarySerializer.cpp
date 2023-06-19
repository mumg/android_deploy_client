#include "mmlBinarySerializer.h"
#include "mmlICString.h"
#include "mmlIVariant.h"
#include "mml_components.h"

void log_binary_serialization (const mmlChar * formatter, ...);

enum
{
	_NULL,
	_STRUCT,
	_ARRAY,
	_INTEGER,
	_STRING,
	_TRUE,
	_FALSE,
	_DOUBLE,
	_RAW,
	_DATETIME
};

#define ENSURE(c) \
	if ( !(c) ) { log_binary_serialization("Failed: %s:%d\n" , __FILE__, __LINE__ ); return mmlFalse; }


mmlBool mml_binary_serialize (mmlIOutputStream * data, mmlIVariant * message);

mmlBool mml_write_integer (mmlIOutputStream * data,  const mmlUInt64 integer )
{
	mmlUInt8 * idata = (mmlUInt8 *)&integer;
	mmlUInt8 len = sizeof(integer);
	for ( ; len > 1 ; len -- )
	{
		if (idata[len-1] != 0)
		{
			break;
		}
	}
	ENSURE(data->Write(sizeof(len), &len) == sizeof(len));
	ENSURE(data->Write(len, idata) == len);
	return mmlTrue;
}

mmlBool mml_write_string (mmlIOutputStream * data, const char * str )
{
	if ( str == mmlNULL )
	{
		return mml_write_integer(data , 0);
	}
	else
	{
		mmlInt64 len = mmlStrLength(str);
		ENSURE(mml_write_integer(data , len) == mmlTrue);
		if ( len > 0 )
		{
			ENSURE(data->Write(len, (void*)str) == len);
		}
	}
	return mmlTrue;
}

mmlBool mml_binary_serialize_object ( mmlIOutputStream * data, mmlIVariant * message )
{
	mmlSharedPtr < mmlIVariantStruct > obj = message;
	if ( obj != mmlNULL )
	{
		mmlUInt8 bs = _STRUCT;
		ENSURE(data->Write(sizeof(bs), &bs) == sizeof(bs));
		ENSURE(mml_write_integer(data , obj->Size()) == mmlTrue);
		class struct_enumerator : public mmlIVariantStructEnumerator
		{
			MML_OBJECT_STATIC_DECL
		public:
			struct_enumerator(mmlIOutputStream * data)
				:mData(data)
			{

			}
			mmlBool OnMember ( const mmlInt32 tag, mmlICString * name , mmlIVariant * val )
			{
				ENSURE(mml_write_string(mData , name->Get()) == mmlTrue);
				ENSURE(mml_binary_serialize(mData , val) == mmlTrue);
				return mmlTrue;
			}

			mmlIOutputStream * mData;
		}se(data);
		return obj->Enumerate(&se);
	}
	return mmlFalse;
}

mmlBool mml_binary_serialize_integer (mmlIOutputStream * data, mmlIVariant * message)
{
	mmlSharedPtr < mmlIVariantInteger > integer = message;
	if ( integer != mmlNULL )
	{
		mmlUInt8 tp = _INTEGER;
		ENSURE(data->Write(sizeof(tp), &tp) == sizeof(tp));
		return mml_write_integer(data , integer->Get());
	}
	return mmlFalse;
}

mmlBool mml_binary_serialize_double (mmlIOutputStream * data, mmlIVariant * message)
{
	mmlSharedPtr < mmlIVariantDouble > dbl = message;
	if ( dbl != mmlNULL )
	{
		mmlUInt8 tp = _DOUBLE;
		ENSURE(data->Write(sizeof(tp), &tp) == sizeof(tp));
		mmlFloat64 val = dbl->Get();
		ENSURE(data->Write(sizeof(val), &val) == sizeof(val));
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mml_binary_serialize_boolean ( mmlIOutputStream * data, mmlIVariant * message )
{
	mmlSharedPtr < mmlIVariantBoolean > boolean = message;
	if ( boolean != mmlNULL )
	{
		char bv = _FALSE;
		if (  boolean->Get()  == mmlTrue )
		{
			bv = _TRUE;
		}
		ENSURE(data->Write(sizeof(bv), &bv) == sizeof(bv));
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mml_binary_serialize_string (mmlIOutputStream * data, mmlIVariant * message )
{
	mmlSharedPtr < mmlIVariantString > string = message;
	if ( string != mmlNULL)
	{
		mmlUInt8 st = _STRING;
		ENSURE(data->Write(sizeof(st), &st) == sizeof(st));
		ENSURE(mml_write_integer(data , string->Get() != mmlNULL ? string->Get()->Length() : 0 ) == mmlTrue);
		if ( string->Get() != mmlNULL && string->Get()->Length() > 0 )
		{
			ENSURE(data->Write(string->Get()->Length(), (void*)string->Get()->Get()) == string->Get()->Length());
		}
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mml_binary_serialize_array (mmlIOutputStream * data, mmlIVariant * message )
{
	mmlSharedPtr < mmlIVariantArray > obj = message;
	if ( obj != mmlNULL )
	{
		mmlUInt8 ba = _ARRAY;
		ENSURE(data->Write(sizeof(ba), &ba) == sizeof(ba));
		mml_write_integer(data , obj->Size());
		class array_enumerator : public mmlIVariantArrayEnumerator
		{
			MML_OBJECT_STATIC_DECL;
		public:
			array_enumerator(mmlIOutputStream * data)
				:mData(data)
			{

			}
			mmlBool OnMember ( const mmlInt32 index, mmlIVariant * val )
			{
				return mml_binary_serialize(mData , val);
			}
		private:
			mmlIOutputStream * mData;
		}ae(data);
		return obj->Enumerate(&ae);
	}
	return mmlFalse;
}

mmlBool mml_binary_serialize_raw (mmlIOutputStream * data, mmlIVariant * message )
{
	mmlSharedPtr < mmlIVariantRaw > raw = message;
	if ( raw != mmlNULL )
	{
		mmlUInt8 ba = _RAW;
		ENSURE(data->Write(sizeof(ba), &ba) == sizeof(ba));
		mmlAutoPtr < mmlIBuffer > buffer;
		raw->Get(buffer.getterAddRef());
		if ( buffer != mmlNULL )
		{
			ENSURE(mml_write_integer(data , buffer->Size()) == mmlTrue);
			ENSURE(data->Write(buffer->Size(), buffer->Get()) == buffer->Size());
		}
		else
		{
			ENSURE(mml_write_integer(data , 0) == mmlTrue);
		}
		return mmlTrue;
	}
	return mmlFalse;
}

static const mmlInt32 MONTHDAYS[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

static mmlInt32 is_leap_year(const mmlInt32 year) 
{
	if (year % 400 == 0) { return 1; }
	else if (year % 100 == 0) { return 0; }
	else if (year % 4 == 0) { return 1; }
	else { return 0; }
}

static mmlUInt64 mml_get_datetime (const mml_tm * tm)
{
	mmlInt32 days = 0;
	mmlInt32 i;

	for (i = 1970; i < tm->tm_year; i++)
	{
		days += 365 + is_leap_year(i);
	}

	for (i = 0; i < tm->tm_mon - 1; i++)
	{
		days += MONTHDAYS[i];
	}
	days += tm->tm_mday - 1;

	if (tm->tm_mon > 2 && is_leap_year(tm->tm_year))
	{
		days++;
	}
	return  ((mmlInt64(days) + 25567) << 32) | mmlInt64(((tm->tm_hour * 60 + tm->tm_min) * 60 + tm->tm_sec) * 300 + (tm->tm_msec * 300 - 150) / 1000);
}

mmlBool mml_binary_serialize_datetime (mmlIOutputStream * data, mmlIVariant * message )
{
	mmlSharedPtr < mmlIVariantDateTime > dt = message;
	if ( dt != mmlNULL )
	{
		mmlUInt8 ba = _DATETIME;
		ENSURE(data->Write(sizeof(ba), &ba) == sizeof(ba));
		mmlInt64 datetime = mml_get_datetime(&dt->Get());
		ENSURE(data->Write(sizeof(datetime), &datetime) == sizeof(datetime));
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mml_binary_serialize (mmlIOutputStream * data, mmlIVariant * message)
{
	if ( message == mmlNULL )
	{
		mmlUInt8 ba = _NULL;
		ENSURE(data->Write(sizeof(ba), &ba) == sizeof(ba));
		return mmlTrue;
	}
	switch(message->GetType())
	{
	case MML_STRUCT:
		return mml_binary_serialize_object(data, message);
	case MML_INTEGER:
		return mml_binary_serialize_integer(data, message);
	case MML_DOUBLE:
		return mml_binary_serialize_double(data , message);
	case MML_BOOLEAN:
		return mml_binary_serialize_boolean(data, message);
	case MML_STRING:
		return mml_binary_serialize_string(data , message);
	case MML_ARRAY:
		return mml_binary_serialize_array(data , message);
	case MML_RAW:
		return mml_binary_serialize_raw(data , message);
	case MML_DATETIME:
		return mml_binary_serialize_datetime(data , message);
	}
	return mmlFalse;
}

mmlBool mmlBinarySerializer::Write ( mmlIOutputStream * stream, mmlIVariant * value )
{
	return mml_binary_serialize ( stream, value);
}

MML_OBJECT_IMPL1(mmlBinarySerializer, mmlIVariantSerializer)
