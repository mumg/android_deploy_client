#include "mmlJSONDeserializer.h"
#include "mmlICStringStream.h"
#include "mmlICStringUtils.h"
#include "mml_components.h"
#include "mmlIVariant.h"
#include <string.h>

#define ENSURE(c) if (!(c)) return mmlFalse;


mmlBool  mml_json_restore ( mmlICStringInputStream * stream , mmlIVariant ** primitive );

mmlBool mml_json_read_object (  mmlICStringInputStream * stream , mmlIVariant ** primitive)
{
	char c;
	mmlSharedPtr < mmlIVariantStruct > object;
	while ( stream->ReadChar(c, mmlFalse) == mmlTrue )
	{
		if ( c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			stream->Seek(1 , mmlFalse);
			continue;
		}
		if ( c == '{')
		{
			stream->Seek(1 , mmlFalse);
			object = mmlNewVariantStruct();
			continue;
		}

		if ( c == '}')
		{
			stream->Seek(1 , mmlFalse);
			*primitive = object;
			MML_ADDREF(*primitive);
			return mmlTrue;
		}

		if ( c == '[')
		{
			return mmlFalse;
		}
		if ( object == NULL )
		{
			return mmlFalse;
		}
		mmlAutoPtr < mmlICString> name;
		int count;
		if ( stream->ReadUntil(" \r\n\t{}[]:,", "\"" , "\"" , &count,name.getterAddRef()) == mmlFalse ||
			count != 2)
		{
			return mmlFalse;
		}
		while ( stream->ReadChar(c, mmlTrue) == mmlTrue )
		{
			if ( c == ' ' || c == '\t' || c == '\r' || c == '\n')
			{
				continue;
			}
			else if ( c == ':')
			{
				break;
			}
			else
			{
				return mmlFalse;
			}
		}
		mmlAutoPtr < mmlIVariant > value;
		if ( mml_json_restore(stream , value.getterAddRef()) == mmlFalse )
		{
			return mmlFalse;
		}
		object->Set(name, value);
		while ( stream->ReadChar(c, mmlTrue) == mmlTrue )
		{
			if ( c == ' ' || c == '\t' || c == '\r' || c == '\n')
			{
				continue;
			}
			else if ( c == ',')
			{
				break;
			}
			else if( c == '}')
			{
				//stream->Seek(1 , mmlFalse);
				*primitive = object;
				MML_ADDREF(*primitive);
				return mmlTrue;
			}
			else
			{
				return mmlFalse;
			}
		}
	}
	return mmlFalse;
}

mmlBool mml_json_read_array ( mmlICStringInputStream * stream , mmlIVariant ** primitive )
{
	char c;
	mmlSharedPtr < mmlIVariantArray > array;
	while ( stream->ReadChar(c, mmlFalse) == mmlTrue )
	{
		if ( c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			stream->Seek(1 , mmlFalse);
			continue;
		}
		if ( c == '[')
		{
			stream->Seek(1 , mmlFalse);
			array = mmlNewVariantArray();
			stream->ReadChar(c, mmlFalse);
			if( c == ']')
			{
				stream->Seek(1 , mmlFalse);
				*primitive = array;
				MML_ADDREF(*primitive);
				return mmlTrue;
			}
			continue;
		}
		mmlAutoPtr < mmlIVariant > value;
		if ( mml_json_restore(stream , value.getterAddRef()) == mmlFalse )
		{
			return mmlFalse;
		}
		array->Push(value);
		while ( stream->ReadChar(c, mmlTrue) == mmlTrue )
		{
			if ( c == ' ' || c == '\t' || c == '\r' || c == '\n')
			{
				continue;
			}
			else if ( c == ',')
			{
				break;
			}
			else if( c == ']')
			{
				*primitive = array;
				MML_ADDREF(*primitive);
				return mmlTrue;
			}
			else
			{
				return mmlFalse;
			}
		}
	}
	return mmlFalse;
}

const mmlUInt32 UNICODE_REPLACEMENT_CHAR = 0x0000FFFD;

static const mmlUInt32 UNICODE_MAX_LEGAL_UTF32 = 0x0010FFFF;

static const mmlUInt8 s_firstByteMarks[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

static const mmlUInt32 s_offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL, 0x03C82080UL, 0xFA082080UL, 0x82082080UL };

mmlInt32 utf8_encode( mmlUInt32 ch, mmlChar * target )
{
	mmlInt32 bytes = 0;
	if ( ch < 0x80 )
	{
		bytes = 1;
	}
	else if ( ch < 0x800 ) 
	{
		bytes = 2;
	}
	else if ( ch < 0x10000 )
	{
		bytes = 3;
	}
	else if ( ch <= UNICODE_MAX_LEGAL_UTF32 )
	{
		bytes = 4;
	}
	else
	{
		bytes = 3, ch = UNICODE_REPLACEMENT_CHAR;
	}

	const unsigned bytemask = 0xBF;
	const unsigned bytemark = 0x80;
	target += bytes;
	switch (bytes)
	{
	case 4: *--target = (char)((ch | bytemark) & bytemask); ch >>= 6;
	case 3: *--target = (char)((ch | bytemark) & bytemask); ch >>= 6;
	case 2: *--target = (char)((ch | bytemark) & bytemask); ch >>= 6;
	case 1: *--target = (char) (ch | s_firstByteMarks[bytes]);
	}
	return bytes;
}

static mmlBool is_hex(mmlChar ch )
{
	if ( ch >= '0' && ch <= '9' )
	{
		return mmlTrue;
	}
	if ( ch >= 'a' && ch <= 'f' )
	{
		return mmlTrue;
	}
	if ( ch >= 'A' && ch <= 'F')
	{
		return mmlTrue;
	}
	return mmlFalse;
}

static mmlUInt16 from_hex ( mmlChar ch)
{
	if ( ch >= '0' && ch <= '9' )
	{
		return (mmlUInt16)(ch - '0');
	}
	if ( ch >= 'a' && ch <= 'f' )
	{
		return (mmlUInt16)(ch - 'a' + 0x0A);
	}
	if ( ch >= 'A' && ch <= 'F')
	{
		return (mmlUInt16)(ch - 'A' + 0x0A);
	}
	return 0;
}


mmlBool  mml_json_restore ( mmlICStringInputStream * stream , mmlIVariant ** primitive )
{
	char c;
	while ( stream->ReadChar(c, mmlFalse) == mmlTrue )
	{
		if ( c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			stream->Seek(1 , mmlFalse);
			continue;
		}
		else if ( c == ',' || c == ':')
		{
			return mmlFalse;
		}
		else if ( c == '[')
		{
			return mml_json_read_array(stream, primitive);
		}
		else if ( c == '{')
		{
			return mml_json_read_object(stream, primitive);
		}
		else if ( c == '"')
		{
			mmlAutoPtr < mmlICString > value;
			stream->Seek(1 , mmlFalse);
			if ( stream->ReadEscaped("\"\r\n\t", '\\' ,  value.getterAddRef()) == mmlTrue )
			{
				int count = 0;
				if ( stream->Skip("\"" , &count) == mmlTrue && count == 1)
				{
					if ( value != mmlNULL )
					{
						const mmlChar * ptr = value->Get();
						mmlInt32 escapes_count = 0;
						mmlInt32 unicodes_count = 0;
						for ( ; (*ptr) != 0; ptr ++ )
						{
							if ( *ptr == '\\' )
							{
								ptr ++;
								if ( *ptr == 0 )
								{
									return mmlFalse;
								}
								else
								{
									if ( *ptr == 'u')
									{
										unicodes_count ++;
										ptr ++;
										if ( *ptr == 0 )
										{
											return mmlFalse;
										}
										if ( is_hex(*ptr) == mmlFalse )
										{
											return mmlFalse;
										}
										ptr ++;
										if ( *ptr == 0 )
										{
											return mmlFalse;
										}
										if ( is_hex(*ptr) == mmlFalse )
										{
											return mmlFalse;
										}
										ptr ++;
										if ( *ptr == 0 )
										{
											return mmlFalse;
										}
										if ( is_hex(*ptr) == mmlFalse )
										{
											return mmlFalse;
										}
										ptr ++;
										if ( *ptr == 0 )
										{
											return mmlFalse;
										}
										if ( is_hex(*ptr) == mmlFalse )
										{
											return mmlFalse;
										}
									}
									escapes_count ++;
								}
							}
						}
						if ( escapes_count > 0 )
						{
							mmlInt32 len = 1 + value->Length() - (escapes_count - unicodes_count);
							mmlChar * str = (mmlChar*) malloc(len);
							mmlChar * dst = str;
							mmlMemorySet(str , 0 , len);
							ptr = value->Get();
							for ( ; *ptr != 0; ptr ++ )
							{
								if ( *ptr == '\\' )
								{
									ptr ++;
									if ( *ptr == 'u')
									{
										mmlUInt16 wc = 0;
										ptr ++;
										wc |= from_hex(*ptr) << 12;
										ptr ++;
										wc |= from_hex(*ptr) << 8;
										ptr ++;
										wc |= from_hex(*ptr) << 4;
										ptr ++;
										wc |= from_hex(*ptr);
										dst += utf8_encode(wc , dst);
									}
									else
									{
										*dst = *ptr;
										dst ++;
									}
								}
								else
								{
									*dst = *ptr;
									dst ++;
								}
							}
							mmlSharedPtr < mmlICString > _str = mmlNewObject(MML_OBJECT_UUID(mmlICString));
							_str->Adopt(str);
							value = (mmlICString*)_str;
						}
					}

					*primitive = mmlNewVariantString(value);
					MML_ADDREF(*primitive);
					return mmlTrue;
				}
			}
			return mmlFalse;
		}
		else if ( c == 't' || c == 'f' || c == 'n')
		{
			mmlAutoPtr < mmlICString > value;
			if ( stream->ReadUntil("\",\r\n\t ,:[]{}" , NULL , NULL , NULL, value.getterAddRef()) == mmlTrue )
			{
				mmlBool equal;
				if ( value->CompareStr(MML_EQUAL , "true" , equal) == mmlTrue && equal == mmlTrue )
				{
					*primitive = mmlNewVariantBoolean(mmlTrue);
					MML_ADDREF(*primitive);
					return mmlTrue;
				}
				else if ( value->CompareStr(MML_EQUAL , "false" , equal) == mmlTrue && equal == mmlTrue )
				{
					*primitive = mmlNewVariantBoolean(mmlFalse);
					MML_ADDREF(*primitive);
					return mmlTrue;
				}
				else if ( value->CompareStr(MML_EQUAL , "null" , equal) == mmlTrue && equal == mmlTrue )
				{
					*primitive  = mmlNULL;
					return mmlTrue;
				}
			}
			return mmlFalse;
		}
		else
		{
			mmlAutoPtr < mmlICString > value;
			if ( stream->ReadUntil("\",\r\n\t ,:[]{}" , NULL , NULL , NULL, value.getterAddRef()) == mmlTrue )
			{
				if ( strstr(value->Get() , ".") != NULL )
				{
					*primitive = mmlNewVariantDouble(mmlStoF(value->Get() , mmlNULL));
					MML_ADDREF(*primitive);
					return mmlTrue;
				}
				else
				{
					*primitive = mmlNewVariantInteger(mmlStoD(value->Get(), mmlNULL));
					MML_ADDREF(*primitive);
					return mmlTrue;
				}
			}
			else
			{
				return mmlFalse;
			}

		}
	}
	return mmlFalse;
}


mmlBool mmlJSONDeserializer::Read ( mmlIInputStream * stream , mmlIVariant ** variant )
{
	mmlSharedPtr < mmlICStringInputStream > str_stream = mmlNewObject(MML_OBJECT_UUID(mmlICStringInputStream), stream);
	if ( str_stream == mmlNULL )
	{
		return mmlFalse;
	}
	return mml_json_restore(str_stream, variant);
}

MML_OBJECT_IMPL1(mmlJSONDeserializer, mmlIVariantDeserializer);