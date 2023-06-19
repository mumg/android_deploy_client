#include "mml_components.h"
#include "mmlIProcess.h"
#include "mmlIFileSystem.h"
#include "mmlICStringStream.h"
#include "mml_services.h"
#include "android_properties.h"
#include <stdio.h>
#include "mmlIRegexp.h"
#include "android_utils.h"

#define SKIP_SPACES() \
{\
	out_stream->Skip(" \r\n", mmlNULL); \
}

#define SKIP_CHAR(ch) \
{ \
	mmlInt32 sz; \
	if ( out_stream->Skip(ch, &sz) == mmlFalse) return mmlFalse; \
	if ( sz == 0 ) return mmlFalse; \
}

#define READ_AND_MATCH_TOKEN(seps,token) \
{ \
	mmlAutoPtr < mmlICString > rd_token; \
	if ( out_stream ->ReadToken(seps, mmlTrue, rd_token.getterAddRef()) == mmlFalse ) return mmlFalse; \
	if ( rd_token == mmlNULL) return mmlFalse; \
	mmlBool eq; \
	if ( rd_token->CompareStr(MML_EQUAL, token, eq, MML_CASE_SENSITIVE) == mmlFalse ) return mmlFalse; \
	if ( eq == mmlFalse ) return mmlFalse; \
} \


#define SKIP_TOKEN() \
{ \
	out_stream->SkipToken(" \n\r"); \
}


#ifdef MML_ANDROID
#define ANDROID_GETPROP "/system/bin/getprop"
#define ANDROID_SERVICE "/system/bin/service"
#define ANDROID_SETTINGS "/system/bin/sh /system/bin/settings"
#else
#define ANDROID_GETPROP "getprop"
#define ANDROID_SERVICE "service"
#define ANDROID_SETTINGS "settings"
#endif

mmlBool android_property_line_reader(mmlIInputStream * input, mmlICString ** value)
{
	mmlSharedPtr < mmlICStringInputStream > out_stream = mmlNewObject(MML_OBJECT_UUID(mmlICStringInputStream), input);
	if ( out_stream == mmlNULL )
	{
		return mmlFalse;
	}
	mmlAutoPtr < mmlICString > val;
	if (out_stream->ReadUntil("\r\n", mmlNULL, mmlNULL, mmlNULL, val.getterAddRef()) == mmlFalse )
	{
		return mmlFalse;
	}
	if ( val == mmlNULL )
	{
		return mmlFalse;
	}
	if (val->Length() == 0 )
	{
		return mmlFalse;
	}
	*value = val;
	MML_ADDREF(*value);
	return mmlTrue;
}

mmlBool android_property_imei_reader(mmlIInputStream * input, mmlICString ** value)
{
	mmlSharedPtr < mmlICStringInputStream > out_stream = mmlNewObject(MML_OBJECT_UUID(mmlICStringInputStream), input);
	if ( out_stream == mmlNULL )
	{
		return mmlFalse;
	}
	//reading Result token
	SKIP_SPACES();
	READ_AND_MATCH_TOKEN(":", "Result");
	SKIP_CHAR(":");
	SKIP_SPACES();
	READ_AND_MATCH_TOKEN("(", "Parcel");
	SKIP_CHAR("(");
	SKIP_SPACES();

	mmlSharedPtr < mmlIMemoryOutputStream > binary_output = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));

	mmlBool end = mmlFalse;
	for (;end == mmlFalse;)
	{
		SKIP_SPACES();
		mmlAutoPtr < mmlICString > offset;
		if ( out_stream->ReadToken(":", mmlTrue, offset.getterAddRef()) == mmlFalse )
		{
			break;
		}
		SKIP_CHAR(":");
		SKIP_SPACES();
		for (mmlInt32 index = 0 ; index < 4; index ++ )
		{
			mmlAutoPtr < mmlICString > hex;
			if ( out_stream->ReadToken(" ", mmlTrue, hex.getterAddRef()) == mmlFalse )
			{
				break;
			}

			if ( hex == mmlNULL )
			{
				break;
			}
			if ( hex->Get()[0] == '\'')
			{
				end = mmlTrue;
				break;
			}
			SKIP_SPACES();
			const mmlChar * hex_str = hex->Get();
			mmlUInt8 buffer[4];
			mmlInt32 idx = 3;
			while ( *hex_str != 0 )
			{
				mmlUInt8 bt = 0;
				mmlChar ch1 = *hex_str;
				hex_str ++;
				mmlChar ch2 = *hex_str;
				hex_str ++;
				if ( ch1 >= '0' && ch1 <= '9' ) bt = (ch1 - '0') << 4;
				if ( ch1 >= 'a' && ch1 <= 'f' ) bt = ((ch1 - 'a') + 10) << 4;
				if ( ch1 >= 'A' && ch1 <= 'F' ) bt = ((ch1 - 'A') + 10) << 4;

				if ( ch2 >= '0' && ch2 <= '9' ) bt |= (ch2 - '0');
				if ( ch2 >= 'a' && ch2 <= 'f' ) bt |= ((ch2 - 'a') + 10);
				if ( ch2 >= 'A' && ch2 <= 'F' ) bt |= ((ch2 - 'A') + 10);
				buffer[idx] = bt;
				idx --;
			}
			binary_output->Write(4 - (idx + 1), buffer + (idx + 1));
		}
		SKIP_SPACES();
		SKIP_TOKEN();
	}
	binary_output->Close();
	mmlSharedPtr < mmlIInputStream > binary_input = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), binary_output);
	mmlInt32 str_sz = 0;
	if ( binary_input->Read(sizeof(str_sz), &str_sz) != sizeof(str_sz) ) return mmlFalse;
	if ( binary_input->Read(sizeof(str_sz), &str_sz) != sizeof(str_sz) ) return mmlFalse;

	mmlUInt16 * unicode_str = (mmlUInt16*)mmlAlloc(sizeof(mmlUInt16) * (str_sz+1));
	mmlChar * str = (mmlChar*) mmlAlloc(sizeof(mmlChar) * (str_sz + 1));
	if (binary_input->Read(sizeof(mmlUInt16) * str_sz, unicode_str) != sizeof(mmlUInt16) * str_sz) return mmlFalse;
	for ( mmlInt32 idx = 0; idx < str_sz ; idx ++ )
	{
		str[idx] = (mmlChar)unicode_str[idx];
	}
	mmlFree(unicode_str);

	*value = mmlNewCString(str, str_sz, mmlTrue);
	MML_ADDREF(*value);

	return mmlTrue;
}

typedef mmlBool (*android_property_reader_t)(mmlIInputStream * input, mmlICString ** value);

typedef struct
{
	ANDROID_PROPERTY_T prop;
	const mmlChar * cmd_line;
	android_property_reader_t reader;
	const mmlChar * validator;
}ANDROID_PROPERTY_DESC_T;

static const ANDROID_PROPERTY_DESC_T properties[] =
{
	{
		ANDROID_PROPERTY_SERIALNO,
		ANDROID_GETPROP" ro.serialno",
		android_property_line_reader,
		"[0-9A-Za-z]*"
	},
	{
		ANDROID_PROPERTY_MODEL,
		ANDROID_GETPROP" ro.product.model",
		android_property_line_reader,
		"[0-9A-Za-z_\\-\\ \\.]*"
	},
	{
		ANDROID_PROPERTY_CPU,
		ANDROID_GETPROP" ro.product.cpu.abi",
		android_property_line_reader,
		"[0-9A-Za-z\\-_]*"
	},
	{
		ANDROID_PROPERTY_DEVICE,
		ANDROID_GETPROP" ro.product.device",
		android_property_line_reader,
		"[0-9A-Za-z_\\-\\ \\.]*"
	},
	{
		ANDROID_PROPERTY_MANUFACTURER,
		ANDROID_GETPROP" ro.product.manufacturer",
		android_property_line_reader,
		"[0-9A-Za-z_\\-\\ \\.]*"
	},
	{
		ANDROID_PROPERTY_RELEASE,
		ANDROID_GETPROP" ro.build.version.release",
		android_property_line_reader,
		"[0-9\\.]*"
	},
	{
		ANDROID_PROPERTY_SDK,
		ANDROID_GETPROP" ro.build.version.sdk",
		android_property_line_reader,
		"[0-9]*"
	},
	{
		ANDROID_PROPERTY_HARDWARE,
		ANDROID_GETPROP" ro.hardware",
		android_property_line_reader,
		"[0-9A-Za-z]*"
	},
	{
		ANDROID_PROPERTY_IMEI,
		ANDROID_SERVICE" call iphonesubinfo 1",
		android_property_imei_reader,
		"[0-9A-Za-z]*"
	},
	{
		ANDROID_PROPERTY_ID,
		ANDROID_SETTINGS" get secure android_id",
		android_property_line_reader,
		"[0-9a-z]*"
	},
	{
		ANDROID_PROPERTY_CARRIER,
		ANDROID_GETPROP" gsm.operator.alpha",
		android_property_line_reader,
		mmlNULL
	},
	{
		ANDROID_PROPERTY_MCCMNC,
		ANDROID_GETPROP" gsm.operator.numeric",
		android_property_line_reader,
		mmlNULL
	}

};

mmlBool android_property_get(const ANDROID_PROPERTY_T prop, mmlICString ** value )
{
	const ANDROID_PROPERTY_DESC_T * desc = mmlNULL;
	for ( mmlInt32 index = 0 ; index < sizeof(properties) / sizeof(ANDROID_PROPERTY_DESC_T); index ++)
	{
		if ( properties[index].prop == prop )
		{
			desc = &properties[index];
			break;
		}
	}
	if ( desc == mmlNULL )
	{
		return mmlFalse;
	}
	
	mmlSharedPtr < mmlIInputStream > output;
	if ( android_run_process(desc->cmd_line, output.getterAddRef()) == mmlFalse )
	{
		return mmlFalse;
	}

	
	mmlAutoPtr < mmlICString > val;
	if ( desc->reader(output, val.getterAddRef()) == mmlFalse )
	{
		return mmlFalse;
	}
	if ( val == mmlNULL ||
		 val->Length() == 0  )
	{
		return mmlFalse;
	}
	if (desc->validator != mmlNULL)
	{
		mmlSharedPtr < mmlIRegexp > regexp = mmlNewObject(MML_OBJECT_UUID(mmlIRegexp));
		if (regexp == mmlNULL)
		{
			return mmlFalse;
		}
		if (regexp->Match(desc->validator, val) == mmlFalse)
		{
			return mmlFalse;
		}
	}
	*value = val;
	MML_ADDREF(*value);
	return mmlTrue;
}