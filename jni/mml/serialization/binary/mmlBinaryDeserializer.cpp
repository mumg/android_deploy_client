#include "mmlBinaryDeserializer.h"
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


mmlBool mml_binary_read_integer (mmlIInputStream * stream, mmlInt64 & val)
{
	char len;
	ENSURE(stream->Read(sizeof(len), &len) == sizeof(len));
	mmlMemorySet(&val , 0 , sizeof(val));
	ENSURE(len <= 8);
	ENSURE(stream->Read(len, &val) == len);
	return mmlTrue;
}

mmlBool mml_binary_read_string (mmlIInputStream * stream, mmlICString ** str)
{
	mmlInt64 len;
	ENSURE(mml_binary_read_integer(stream , len));
	if ( len > 0 )
	{
		mmlChar * cstr =(mmlChar *) mmlAlloc(len + 1);
		mmlMemorySet(cstr , 0 , len + 1);
		if ( stream->Read( len, cstr) != len )
		{
			mmlFree(str);
			return mmlFalse;
		}
		*str = (mmlICString*)mmlNewObject(MML_OBJECT_UUID(mmlICString));
		(*str)->Adopt(cstr);
		MML_ADDREF(*str);
	}
	return mmlTrue;
}

mmlBool mml_binary_read_datetime (mmlIInputStream * stream, mml_tm & tm)
{
	mmlInt64 dt;
	ENSURE(stream->Read(sizeof(dt), &dt) == sizeof(dt));

	mmlInt32 years, months, days, ydays, wday, hours, mins, secs, ms;
	mmlInt32 l, n, i, j;
	mmlInt32 dt_days;
	mmlUInt32 dt_time;
	dt_time = mmlUInt32(dt);
	ms = ((dt_time % 300) * 1000 + 150) / 300;
	dt_time = dt_time / 300;
	secs = dt_time % 60;
	dt_time = dt_time / 60;

	dt_days = mmlInt32(dt >> 32);

	/*
	 * -53690 is minimun  (1753-1-1) (Gregorian calendar start in 1732) 
	 * 2958463 is maximun (9999-12-31)
	 */
	l = dt_days + 146038;
	wday = (l + 4) % 7;
	n = (4 * l) / 146097;	/* n century */
	l = l - (146097 * n + 3) / 4;	/* days from xx00-02-28 (y-m-d) */
	i = (4000 * (l + 1)) / 1461001;	/* years from xx00-02-28 */
	l = l - (1461 * i) / 4;	/* year days from xx00-02-28 */
	ydays = l >= 306 ? l - 305 : l + 60;
	l += 31;
	j = (80 * l) / 2447;
	days = l - (2447 * j) / 80;
	l = j / 11;
	months = j + 1 - 12 * l;
	years = 100 * (n + 15) + i + l;
	if (l == 0 && (years & 3) == 0 && (years % 100 != 0 || years % 400 == 0))
		++ydays;

	hours = dt_time / 60;
	mins = dt_time % 60;
	mmlMemorySet(&tm , 0 , sizeof(tm));
	tm.tm_year = years;
	tm.tm_mon = months;
	tm.tm_mday = days;
	tm.tm_yday = ydays;
	tm.tm_wday = wday;
	tm.tm_hour = hours;
	tm.tm_min = mins;
	tm.tm_sec = secs;
	tm.tm_msec = ms; 
	return mmlTrue;
}

mmlBool  mml_binary_restore ( mmlIInputStream * stream , mmlIVariant ** primitive )
{
	mmlUInt8 prim;
	ENSURE(stream->Read(sizeof(prim), &prim ) == sizeof(prim));
	if ( prim == _NULL )
	{
		return mmlTrue;
	}
	if ( prim == _STRUCT )
	{
		mmlInt64 val;
		ENSURE(mml_binary_read_integer(stream , val));
		mmlAutoPtr < mmlIVariantStruct > str = mmlNewVariantStruct();
		for ( mmlInt64 count = 0 ; count < val ; count ++ )
		{
			mmlAutoPtr < mmlICString > name;
			ENSURE(mml_binary_read_string(stream , name.getterAddRef()) == mmlTrue);
			mmlAutoPtr < mmlIVariant > var;
			ENSURE(mml_binary_restore(stream, var.getterAddRef()) == mmlTrue);
			str->Set(name , var);
		}
		*primitive = str;
		MML_ADDREF(*primitive);
		return mmlTrue;
	}
	else if ( prim == _ARRAY)
	{
		mmlInt64 val;
		ENSURE(mml_binary_read_integer(stream , val));
		mmlAutoPtr < mmlIVariantArray > str = mmlNewVariantArray();
		for ( mmlInt64 count = 0 ; count < val ; count ++ )
		{
			mmlAutoPtr < mmlIVariant > var;
			ENSURE(mml_binary_restore(stream, var.getterAddRef()) == mmlTrue);
			str->Push(var);
		}
		*primitive = str;
		MML_ADDREF(*primitive);
		return mmlTrue;
	}
	else if ( prim == _INTEGER )
	{
		mmlInt64 val;
		ENSURE(mml_binary_read_integer(stream , val));
		*primitive = mmlNewVariantInteger(val);
	}
	else if ( prim == _STRING )
	{
		mmlAutoPtr < mmlICString > val;
		ENSURE(mml_binary_read_string(stream , val.getterAddRef()));
		*primitive = mmlNewVariantString(val);
	}
	else if ( prim == _TRUE )
	{
		*primitive = mmlNewVariantBoolean(mmlTrue);
	}
	else if ( prim == _FALSE )
	{
		*primitive = mmlNewVariantBoolean(mmlFalse);
	}
	else if ( prim == _DOUBLE )
	{
		mmlFloat64 val;
		ENSURE(stream->Read(sizeof(val), &val) == sizeof(val));
		*primitive = mmlNewVariantDouble(val);
	}
	else if ( prim == _RAW )
	{
		mmlInt64 len;
		ENSURE(mml_binary_read_integer(stream , len));
		mmlChar * raw_data =(mmlChar *) mmlAlloc(len + 1);
		if ( stream->Read( len, raw_data) != len )
		{
			mmlFree(raw_data);
			return mmlFalse;
		}
		mmlIVariantRaw * raw = mmlNewVariantRaw();

		mmlAutoPtr < mmlIBuffer > buffer = mmlNewAllocatedBuffer(raw_data, len);
		raw->Set(buffer);
		*primitive = raw;
	}
	else if ( prim == _DATETIME)
	{
		mml_tm tm;
		ENSURE(mml_binary_read_datetime(stream, tm) == true);
		*primitive = mmlNewVariantDateTime(&tm);
	}
	if ( *primitive != NULL )
	{
		MML_ADDREF(*primitive);
		return mmlTrue;
	}
	return mmlFalse;
}


mmlBool mmlBinaryDeserializer::Read ( mmlIInputStream * stream , mmlIVariant ** variant )
{
	return mml_binary_restore(stream, variant);
}

MML_OBJECT_IMPL1(mmlBinaryDeserializer, mmlIVariantDeserializer);