#include "mmlCStringStream.h"
#include "mmlCString.h"
#include "mmlIBuffer.h"
#include "mml_strutils.h"
#include <memory.h>
#include <string.h>

#define SIZE_LEFT (mBufferLen-mBufferOffset)

#define BUFFER(offset) (mBuffer + mBufferOffset + offset)

MML_OBJECT_IMPL1(mmlCStringInputStream ,  mmlICStringInputStream)

mmlCStringInputStream::mmlCStringInputStream()
:mBuffer(0), mBufferLen(0) , mBufferOffset(0)
{

}

mmlCStringInputStream::~mmlCStringInputStream()
{
	if ( mBuffer != mmlNULL )
	{
		mmlFree(mBuffer);
	}
}

mmlBool mmlCStringInputStream::Construct ( mmlIInputStream * stream )
{
	if ( stream == mmlNULL )
	{
		return mmlFalse;
	}
	mmlInt32 len = stream->Size();
	mBuffer = (mmlChar*)mmlAlloc ( len );
	mBufferLen = len;
	mmlInt32 offset = 0;
	while ( len > 0 )
	{
		mmlInt32 rd = stream->Read(len, mBuffer + offset);
		if ( rd <= 0)
		{
			return mmlFalse;
		}
		len -= rd;
		offset += rd;
	}
	return mmlTrue;
}

mmlBool mmlCStringInputStream::ReadStr ( const mmlInt32 len,
				                               mmlICString ** value )
{
	if ( SIZE_LEFT > 0 )
	{
		mmlInt32 _len = len < SIZE_LEFT ? len : SIZE_LEFT;
		*value = new mmlCString(BUFFER(0) , _len);
		MML_ADDREF(*value);
		mBufferOffset += _len;
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlCStringInputStream::ReadChar ( mmlChar & ch , const mmlBool seek)
{
	if ( SIZE_LEFT > 0 )
	{
		ch = *(BUFFER(0));
		if ( seek == mmlTrue )
		{
			mBufferOffset ++;
		}
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlCStringInputStream::ReadToken ( const mmlChar * seps,
				                           mmlBool seek,
				                           mmlICString ** value )
{
	const mmlChar * buffer = BUFFER(0);
	mmlInt32 index;
	if ( seps == mmlNULL )
	{
		index = SIZE_LEFT;
	}
	else
	{
		for (index = 0 ;  index < SIZE_LEFT ; index ++ )
		{
			for ( mmlInt32 seps_index = 0 ; seps[seps_index] != 0 ; seps_index ++ )
			{
				if ( buffer[index] == seps[seps_index] )
				{
					*value = new mmlCString(buffer , index);
					MML_ADDREF(*value);
					if ( seek == mmlTrue )
					{
						mBufferOffset += index;
					}
					return mmlTrue;
				}
			}
		}
	}

	if ( index == SIZE_LEFT )
	{
		mBufferOffset += index;
		*value = new mmlCString(buffer , index);
		MML_ADDREF(*value);
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlCStringInputStream::SkipToken ( const mmlChar * seps )
{
	const mmlChar * buffer = BUFFER(0);
	for (mmlInt32 index = 0 ; index < SIZE_LEFT ; index ++ )
	{
		for ( mmlInt32 seps_index = 0 ; seps[seps_index] != 0 ; seps_index ++ )
		{
			if ( buffer[index] == seps[seps_index] )
			{
				mBufferOffset += index;
				return mmlTrue;
			}
		}
	}
	return mmlFalse;
}

mmlBool mmlCStringInputStream::ReadUntil ( const mmlChar * terminators_list,
				                           const mmlChar * remove_from_begin,
				                           const mmlChar * remove_from_end,
										   mmlInt32 * removed,
										   mmlICString ** value )
{
	if ( SIZE_LEFT == 0 )
	{
		return mmlFalse;
	}
	const mmlChar * buffer = BUFFER(0);
	mmlInt32 _begin = 0;
	mmlInt32 _end = 0;
	if ( removed ) *removed = 0;
	for (_begin = 0 ; remove_from_begin != mmlNULL && _begin < SIZE_LEFT ; _begin ++ )
	{
		mmlBool match = mmlFalse;
		for ( mmlInt32 rmb_index = 0 ; remove_from_begin[rmb_index] != 0 ; rmb_index ++ )
		{
			if ( buffer[_begin] == remove_from_begin[rmb_index] )
			{
				if ( removed ) (*removed) ++;
				match = mmlTrue;
				break;
			}
		}
		if ( match == mmlFalse )
		{
			break;
		}
	}
	
	mmlBool _terminated = mmlFalse;
	for (_end = _begin ; _end < SIZE_LEFT ; _end ++ )
	{
		for ( mmlInt32 trm_index = 0 ; terminators_list[trm_index] != 0 ; trm_index ++ )
		{
			if ( buffer[_end] == terminators_list[trm_index] )
			{
				_terminated = mmlTrue;
				break;
			}
		}
		if ( _terminated == mmlTrue )
		{
			break;
		}
	}
	mBufferOffset += _end;
	for ( ; remove_from_end != mmlNULL && _end > _begin; _end -- )
	{
		mmlBool match = mmlFalse;
		for ( mmlInt32 rme_index = 0 ; remove_from_end[rme_index] != 0 ; rme_index ++ )
		{
			if ( buffer[_end-1] == remove_from_end[rme_index] )
			{
				if ( removed ) (*removed) ++;
				match = mmlTrue;
				break;
			}
		}
		if ( match == mmlFalse )
		{
			break;
		}
	}
	if ( _end - _begin > 0 )
	{
		*value = new mmlCString(buffer + _begin , _end - _begin);
		MML_ADDREF(*value);
	}
	return mmlTrue;
}


mmlBool mmlCStringInputStream::ReadEscaped (const mmlChar * terminators_list,
					 const mmlChar escapeChar,
					 mmlICString ** value )
{
	if ( SIZE_LEFT == 0 )
	{
		return mmlFalse;
	}
	mmlChar terminators [256] = {0};
	mmlInt32 terminators_count = strlen(terminators_list);
	for ( mmlInt32 index = 0 ; index < terminators_count; index ++ )
	{
		terminators[(mmlUInt8)terminators_list[index]] = 1;
	}
	const mmlChar * buffer = BUFFER(0);
	mmlInt32 _begin = 0;
	mmlInt32 _end = 0;
	for (_end = _begin ; _end < SIZE_LEFT ; _end ++ )
	{
		if (buffer[_end]  == escapeChar )
		{
			_end ++;
			continue;
		}
		if ( terminators[(mmlUInt8)buffer[_end]] == 1)
		{
			break;
		}
	}
	mBufferOffset += _end;
	if ( _end - _begin > 0 )
	{
		*value = new mmlCString(buffer + _begin , _end - _begin);
		MML_ADDREF(*value);
	}
	return mmlTrue;
}

mmlBool mmlCStringInputStream::ReadInteger64 ( mmlInt64 * value )
{
	mmlBool result = mmlFalse;
	mmlChar integer64_buffer[40] = {0};
	const mmlChar * buffer = BUFFER(0);
	mmlUInt32 index = 0;
               	for ( ; index < SIZE_LEFT  && index < sizeof(integer64_buffer) - 1; index ++ )
	{
		if ( (buffer[index] >= '0' &&
			  buffer[index] <= '9') ||
			  buffer[index] == '-' )
		{
			integer64_buffer[index] = buffer[index];
		}
		else
		{
			break;
		}
	}
	if ( index > 0 )
	{
		*value = mmlStoD(integer64_buffer, &result);
	}
	mBufferOffset += index;
	return result;
}

mmlBool mmlCStringInputStream::ReadInteger32 ( mmlInt32 * value )
{
	mmlBool result = mmlFalse;
	mmlChar integer32_buffer[40] = {0};
	const mmlChar * buffer = BUFFER(0);
	mmlUInt32 index = 0;
	for ( ; index < SIZE_LEFT  && index < sizeof(integer32_buffer) - 1; index ++ )
	{
		if ( (buffer[index] >= '0' &&
			buffer[index] <= '9') ||
			buffer[index] == '-' )
		{
			integer32_buffer[index] = buffer[index];
		}
		else
		{
			break;
		}
	}
	if ( index > 0 )
	{
		*value = (mmlInt32)mmlStoD(integer32_buffer, &result);
	}
	mBufferOffset += index;
	return result;
}

mmlBool mmlCStringInputStream::ReadInteger16 ( mmlInt16 * value )
{
	mmlBool result = mmlFalse;
	mmlChar integer16_buffer[40] = {0};
	const mmlChar * buffer = BUFFER(0);
	mmlUInt32 index = 0;
	for ( ; index < SIZE_LEFT  && index < sizeof(integer16_buffer) - 1; index ++ )
	{
		if ( (buffer[index] >= '0' &&
			buffer[index] <= '9') ||
			buffer[index] == '-' )
		{
			integer16_buffer[index] = buffer[index];
		}
		else
		{
			break;
		}
	}
	if ( index > 0 )
	{
		*value = (mmlInt16)mmlStoD(integer16_buffer, &result);
	}
	mBufferOffset += index;
	return result;
}


mmlBool mmlCStringInputStream::ReadUInteger64 ( mmlUInt64 * value )
{
	mmlBool result = mmlFalse;
	mmlChar integer64_buffer[40] = {0};
	const mmlChar * buffer = BUFFER(0);
	mmlUInt32 index = 0;
	for ( ; index < SIZE_LEFT  && index < sizeof(integer64_buffer) - 1; index ++ )
	{
		if (buffer[index] >= '0' &&
			buffer[index] <= '9')
		{
			integer64_buffer[index] = buffer[index];
		}
		else
		{
			break;
		}
	}
	if ( index > 0 )
	{
		*value = mmlStoUD(integer64_buffer, &result);
	}
	mBufferOffset += index;
	return result;
}

mmlBool mmlCStringInputStream::ReadUInteger32 ( mmlUInt32 * value )
{
	mmlBool result = mmlFalse;
	mmlChar integer32_buffer[40] = {0};
	const mmlChar * buffer = BUFFER(0);
	mmlUInt32 index = 0;
	for ( ; index < SIZE_LEFT  && index < sizeof(integer32_buffer) - 1; index ++ )
	{
		if (buffer[index] >= '0' &&
			buffer[index] <= '9')
		{
			integer32_buffer[index] = buffer[index];
		}
		else
		{
			break;
		}
	}
	if ( index > 0 )
	{
		*value = (mmlUInt32)mmlStoUD(integer32_buffer, &result);
	}
	mBufferOffset += index;
	return result;
}

mmlBool mmlCStringInputStream::ReadUInteger16 ( mmlUInt16 * value )
{
	mmlBool result = mmlFalse;
	mmlChar integer16_buffer[40] = {0};
	const mmlChar * buffer = BUFFER(0);
	mmlUInt32 index = 0;
	for ( ; index < SIZE_LEFT  && index < sizeof(integer16_buffer) - 1; index ++ )
	{
		if (buffer[index] >= '0' &&
			buffer[index] <= '9')
		{
			integer16_buffer[index] = buffer[index];
		}
		else
		{
			break;
		}
	}
	if ( index > 0 )
	{
		*value = (mmlUInt16)mmlStoUD(integer16_buffer, &result);
	}
	mBufferOffset += index;
	return result;
}

mmlBool mmlCStringInputStream::ReadFloat ( mmlFloat64 * value )
{
	mmlBool result = mmlFalse;
	mmlChar float_buffer[40] = {0};
	const mmlChar * buffer = BUFFER(0);
	mmlUInt32 index = 0;
	for ( ; index < SIZE_LEFT && index < sizeof(float_buffer) - 1; index ++ )
	{
		if ( (buffer[index] >= '0' &&
			buffer[index] <= '9' )||
			buffer[index] == '-' ||
			buffer[index] == '.')
		{
			float_buffer[index] = buffer[index];
		}
		else
		{
			break;
		}
	}
	if ( index > 0 )
	{
		*value = mmlStoF(float_buffer, &result);
	}
	mBufferOffset += index;
	return result;
}

mmlBool mmlCStringInputStream::Seek ( const mmlInt32 value , const mmlBool abs )
{
	if ( abs == mmlTrue )
	{
		if ( value >= 0 && value < mBufferLen )
		{
			mBufferOffset = value;
			return mmlTrue;
		}
	}
	else
	{
		if ( mBufferOffset + value >= 0 && mBufferOffset + value < mBufferLen )
		{
			mBufferOffset += value;
			return mmlTrue;
		}
	}
	return mmlFalse;
}

mmlBool mmlCStringInputStream::Skip ( const mmlChar * characters,
			                          mmlInt32 * value )
{
	if ( SIZE_LEFT == 0 )
	{
		return mmlFalse;
	}
	const mmlChar * buffer = BUFFER(0);
	if ( value != mmlNULL ) *value = 0;
	for ( mmlInt32 index = 0 ; index < SIZE_LEFT ; index ++ )
	{
		mmlBool match = mmlFalse;
		for ( mmlInt32 _char_index = 0 ; characters[_char_index] != 0; _char_index ++ )
		{
			if ( buffer[index] == characters[_char_index] )
			{
				match = mmlTrue;
				break;
			}
		}
		if ( match == mmlTrue )
		{
			if ( value != mmlNULL ) (*value) ++;
		}
		else
		{
			mBufferOffset += index;
			break;
		}
	}
	return mmlTrue;
}

mmlBool mmlCStringInputStream::GetRestorePoint ( mmlInt32 * value )
{
	*value = mBufferOffset;
	return mmlTrue;
}

mmlBool mmlCStringInputStream::Restore ( const mmlInt32 value )
{
	if ( value < mBufferLen)
	{
		mBufferOffset = value;
		return mmlTrue;
	}
	return mmlFalse;
}

MML_OBJECT_IMPL1(mmlCStringOutputStream, mmlICStringOutputStream)

mmlCStringOutputStream::mmlCStringOutputStream()
{

}
mmlCStringOutputStream::~mmlCStringOutputStream()
{
}

mmlBool mmlCStringOutputStream::Construct (mmlIOutputStream * stream)
{
	mStream = stream;
	return mmlTrue;
}

mmlBool mmlCStringOutputStream::WriteChar ( const mmlChar value )
{
	return mStream->Write(1, &value) == 1 ? mmlTrue : mmlFalse;
}

mmlBool mmlCStringOutputStream::WriteStr   ( const mmlChar * value , const mmlInt32 size)
{
	mmlInt32 sz = size == 0 ? mmlStrLength(value) : size;
	return mStream->Write(sz, value) == sz ? mmlTrue : mmlFalse;
}

mmlBool mmlCStringOutputStream::WriteInteger16 ( const mmlInt16 value , const mmlBool hex, const mmlInt32 min )
{
	mmlChar buffer[32];
	mmlChar formatter[8];
	mmlInt32 fmt_offset = 0;
	fmt_offset = mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "%");
	if (min > 0 )
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "0.%d" , min );
	}
	if ( hex == mmlTrue )
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "X");
	}
	else
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "d");
	}
	mmlInt32 len = mmlSprintf(buffer , sizeof(value) , formatter , value);
	return mStream->Write(len , buffer)  == len ? mmlTrue : mmlFalse;
}

mmlBool mmlCStringOutputStream::WriteInteger32 ( const mmlInt32 value , const mmlBool hex, const mmlInt32 min )
{
	mmlChar buffer[32];
	mmlChar formatter[8];
	mmlInt32 fmt_offset = 0;
	fmt_offset = mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "%");
	if (min > 0 )
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "0.%d" , min );
	}
	if ( hex == mmlTrue )
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "X");
	}
	else
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "d");
	}
	mmlInt32 len = mmlSprintf(buffer , sizeof(value) , formatter , value);
	return mStream->Write(len , buffer)  == len ? mmlTrue : mmlFalse;
}

mmlBool mmlCStringOutputStream::WriteInteger64 ( const mmlInt64 value , const mmlBool hex, const mmlInt32 min )
{
	mmlChar buffer[32];
	mmlChar formatter[8];
	mmlInt32 fmt_offset = 0;
	fmt_offset = mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "%");
	if (min > 0 )
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "0.%d" , min );
	}
	if ( hex == mmlTrue )
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "lX");
	}
	else
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , MML_INT64);
	}
	mmlInt32 len = mmlSprintf(buffer , sizeof(value) , formatter , value);
	return mStream->Write(len , buffer)  == len ? mmlTrue : mmlFalse;
}

mmlBool mmlCStringOutputStream::WriteUInteger16 ( const mmlUInt16 value , const mmlBool hex, const mmlInt32 min )
{
	mmlChar buffer[32];
	mmlChar formatter[8];
	mmlInt32 fmt_offset = 0;
	fmt_offset = mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "%");
	if (min > 0 )
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "0.%d" , min );
	}
	if ( hex == mmlTrue )
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "X");
	}
	else
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "u");
	}
	mmlInt32 len = mmlSprintf(buffer , sizeof(value) , formatter , value);
	return mStream->Write(len , buffer)  == len ? mmlTrue : mmlFalse;
}
mmlBool mmlCStringOutputStream::WriteUInteger32 ( const mmlUInt32 value , const mmlBool hex, const mmlInt32 min )
{
	mmlChar buffer[32];
	mmlChar formatter[8];
	mmlInt32 fmt_offset = 0;
	fmt_offset = mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "%");
	if (min > 0 )
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "0.%d" , min );
	}
	if ( hex == mmlTrue )
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "X");
	}
	else
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "u");
	}
	mmlInt32 len = mmlSprintf(buffer , sizeof(value) , formatter , value);
	return mStream->Write(len , buffer)  == len ? mmlTrue : mmlFalse;
}
mmlBool mmlCStringOutputStream::WriteUInteger64 ( const mmlUInt64 value , const mmlBool hex, const mmlInt32 min )
{
	mmlChar buffer[32];
	mmlChar formatter[8];
	mmlInt32 fmt_offset = 0;
	fmt_offset = mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "%");
	if (min > 0 )
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "0.%d" , min );
	}
	if ( hex == mmlTrue )
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "X");
	}
	else
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , MML_UINT64);
	}
	mmlInt32 len = mmlSprintf(buffer , sizeof(value) , formatter , value);
	return mStream->Write(len , buffer)  == len ? mmlTrue : mmlFalse;
}

mmlBool mmlCStringOutputStream::WriteFloat ( const mmlFloat64 value , const mmlInt32 min, const mmlInt32 precision )
{
	mmlChar buffer[32];
	mmlChar formatter[8];
	mmlInt32 fmt_offset = 0;
	fmt_offset = mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "%");
	if (min > 0 )
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%d" , min);
	}
	if ( min > 0 || precision > 0 )
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , ".");
	}
	if (precision > 0 )
	{
		fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%d" , precision);
	}
	fmt_offset += mmlSprintf(formatter + fmt_offset , sizeof(formatter) - fmt_offset , "%s" , "f");
	mmlInt32 len = mmlSprintf(buffer , sizeof(value) , formatter , value);
	return mStream->Write(len , buffer)  == len ? mmlTrue : mmlFalse;
}

mmlBool mmlCStringOutputStream::WriteFormatted ( const mmlChar * formatter , ... )
{
	va_list args;
	va_start(args , formatter);
	mmlBool result = WriteFormattedV(formatter ,args);
	va_end(args);
	return result;
}

mmlBool mmlCStringOutputStream::WriteFormattedV ( const mmlChar * formatter , va_list args)
{
#ifdef MML_WIN
	mmlInt32 len = mmlVSprintf(mmlNULL , 0 , formatter, args);
#else
	va_list copy;
	va_copy(copy, args);
	mmlInt32 len = mmlVSprintf(mmlNULL , 0 , formatter, copy);
	va_end(copy);
#endif
	mmlChar * buffer = (mmlChar*)mmlAlloc(len + 1);
	mmlVSprintf(buffer , len + 1 , formatter, args);
	mmlBool result = mStream->Write(len , buffer) == len ? mmlTrue : mmlFalse;
	mmlFree(buffer);
	return result;
}

mmlBool  mmlCStringOutputStream::Close ()
{
	return mStream->Close();
}

