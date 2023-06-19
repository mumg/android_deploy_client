#include "mmlJSONSerializer.h"
#include "mmlICStringStream.h"
#include "mmlICStringUtils.h"
#include "mml_components.h"
#include "mmlIVariant.h"
#include <string.h>

#define ENSURE(c) if (!(c)) return mmlFalse;

mmlBool mml_serialize (mmlICStringUtils * utils , mmlIOutputStream * data, mmlIVariant * message);

mmlBool mml_serialize_object ( mmlICStringUtils * utils, mmlIOutputStream * data, mmlIVariant * message )
{
	mmlSharedPtr < mmlIVariantStruct > obj = message;
	if ( obj != mmlNULL )
	{
		ENSURE(data->Write(1, (void*)"{") == 1);
		class struct_enumerator : public mmlIVariantStructEnumerator
		{
			MML_OBJECT_STATIC_DECL
		public:
			mmlBool begin;
			struct_enumerator(mmlICStringUtils * utils, mmlIOutputStream * data)
				:begin(mmlTrue), mUtils(utils), mData(data)
			{

			}
			mmlBool OnMember ( const mmlInt32 tag, mmlICString * name , mmlIVariant * val )
			{
				if ( begin == mmlTrue )
				{
					begin = mmlFalse;
				}
				else
				{
					ENSURE(mData->Write(1, (void*)"," ) == 1);
				}
				ENSURE(mData->Write(1, (void*)"\"" ) == 1);
				ENSURE(mData->Write(name->Length(), (void*)name->Get()) == name->Length());
				ENSURE(mData->Write(2, (void*)"\":") == 2);
				return mml_serialize(mUtils , mData , val);
			}

			mmlICStringUtils * mUtils;
			mmlIOutputStream * mData;
		}se(utils, data);
		ENSURE(obj->Enumerate(&se) == mmlTrue);
		ENSURE(data->Write(1, (void*)"}") == 1);
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mml_serialize_integer (mmlIOutputStream * data, mmlIVariant * message)
{
	mmlSharedPtr < mmlIVariantInteger > integer = message;
	if ( integer != mmlNULL )
	{
		mmlChar str[64];
		mmlInt32 len = mmlSprintf(str , sizeof(str), "%" MML_INT64 , integer->Get());
		ENSURE(data->Write(len, str) == len);
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mml_serialize_double (mmlIOutputStream * data, mmlIVariant * message)
{
	mmlSharedPtr < mmlIVariantDouble > dbl = message;
	if ( dbl != mmlNULL )
	{
		mmlChar str[64];
		mmlInt32 len = mmlSprintf(str ,sizeof(str) , "%f" , dbl->Get());
		ENSURE(data->Write(len, str) == len);
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mml_serialize_boolean ( mmlIOutputStream * data, mmlIVariant * message )
{
	mmlSharedPtr < mmlIVariantBoolean > boolean = message;
	if ( boolean != mmlNULL )
	{
		if ( boolean->Get() == mmlTrue )
		{
			ENSURE(data->Write(4, (void*)"true") == 4);
		}
		else
		{
			ENSURE(data->Write(5, (void*)"false") == 5);
		}
		return mmlTrue;
	}
	return mmlFalse;
}

static const mmlChar * replacers [] =
{
	"\\\"",
	"\\n",
	"\\r",
	"\\t",
	"\\\\",
};


mmlBool mml_serialize_string (mmlICStringUtils * utils, mmlIOutputStream * data, mmlIVariant * message )
{
	mmlSharedPtr < mmlIVariantString > string = message;
	ENSURE(data->Write(1, (void*)"\"") == 1);
	if ( string != mmlNULL && string->Get() != mmlNULL )
	{
		if ( string->Get() != mmlNULL )
		{
			mmlAutoPtr < mmlICString > escaped;
			utils->Escape(string->Get(), "\"\n\r\t\\" , replacers, escaped.getterAddRef());
			ENSURE(data->Write(escaped->Length(), (void*)escaped->Get()) == escaped->Length());
		}
	}
	ENSURE(data->Write(1, (void*)"\"") == 1);
	return mmlTrue;
}

mmlBool mml_serialize_array (mmlICStringUtils * utils , mmlIOutputStream * data, mmlIVariant * message )
{
	mmlSharedPtr < mmlIVariantArray > obj = message;
	if ( obj != mmlNULL )
	{
		ENSURE(data->Write(1, (void*)"[" ) == 1);
		class array_enumerator : public mmlIVariantArrayEnumerator
		{
			MML_OBJECT_STATIC_DECL;
		public:
			array_enumerator(mmlICStringUtils * utils , mmlIOutputStream * data)
				:mUtils(utils), mBegin(mmlTrue), mData(data)
			{

			}
			mmlBool OnMember ( const mmlInt32 index , mmlIVariant * val )
			{
				if ( mBegin == mmlTrue )
				{
					mBegin = mmlFalse;
				}
				else
				{
					ENSURE(mData->Write(1, (void*)",") == 1);
				}
				return mml_serialize(mUtils , mData , val);
			}
		private:
			mmlICStringUtils * mUtils;
			mmlBool mBegin;
			mmlIOutputStream * mData;
		}ae(utils , data);
		ENSURE(obj->Enumerate(&ae) == mmlTrue);
		ENSURE(data->Write(1, (void*)"]") == 1);
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mml_serialize (mmlICStringUtils * utils ,mmlIOutputStream * data, mmlIVariant * message)
{
	if ( message == mmlNULL )
	{
		return data->Write(4, "null") == 4 ? mmlTrue : mmlFalse;
	}
	switch(message->GetType())
	{
	case MML_STRUCT:
		return mml_serialize_object(utils, data, message);
	case MML_INTEGER:
		return mml_serialize_integer(data, message);
	case MML_DOUBLE:
		return mml_serialize_double(data , message);
	case MML_BOOLEAN:
		return mml_serialize_boolean(data, message);
	case MML_STRING:
		return mml_serialize_string(utils, data , message);
	case MML_ARRAY:
		return mml_serialize_array(utils, data , message);
	}
	return mmlFalse;
}



mmlBool mmlJSONSerializer::Write ( mmlIOutputStream * stream, mmlIVariant * value )
{
	mmlSharedPtr < mmlICStringUtils > utils = mmlNewObject(MML_OBJECT_UUID(mmlICStringUtils));

	return mml_serialize (utils, stream, value);
}

MML_OBJECT_IMPL1(mmlJSONSerializer, mmlIVariantSerializer)
