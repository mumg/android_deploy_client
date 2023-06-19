#include "mmlBufferedStream.h"


mmlBufferedInputStream::mmlBufferedInputStream()
	:mBuffer(mmlNULL), mBufferSize(4096), mBufferDataOffset(0), mBufferDataLen(0)
{

}

mmlBufferedInputStream::~mmlBufferedInputStream()
{
	if ( mBuffer != mmlNULL ) mmlFree(mBuffer);
}

mmlBool mmlBufferedInputStream::Construct ( mmlIInputStream * stream )
{
	mStream = stream;
	return mmlTrue;
}

mmlBool mmlBufferedInputStream::GetControl ( mmlIStreamControl ** control )
{
	return mStream->GetControl(control);
}

mmlInt64 mmlBufferedInputStream::Read ( const mmlInt64 size , void * data )
{
	if ( mBuffer == mmlNULL )
	{
		mBuffer = (mmlUInt8 *)mmlAlloc(mBufferSize);
	}
	if ( mBufferDataLen == 0 )
	{
		mBufferDataOffset = 0;
		mBufferDataLen = mStream->Read(mBufferSize, mBuffer);
		if ( mBufferDataLen < 0 )
		{
			return -1;
		}
	}
	if ( mBufferDataLen > 0 )
	{
		mmlInt64 sz = mBufferDataLen > size ? size : mBufferDataLen;
		mmlMemoryCopy(data, mBuffer + mBufferDataOffset, sz );
		mBufferDataOffset += sz;
		mBufferDataLen -= sz;
		return sz;
	}
	return 0;
}

mmlBool mmlBufferedInputStream::IsEmpty ()
{
	if ( mBufferDataLen == 0 && mStream->IsEmpty() == mmlTrue )
	{
		return mmlTrue;
	}
	return mmlFalse;
}

mmlInt64 mmlBufferedInputStream::Size ()
{
	return mBufferDataLen + mStream->Size();
}

mmlBool mmlBufferedInputStream::Close ()
{
	mBufferDataLen = 0;
	mBufferDataOffset = 0;
	return mStream->Close();
}

mmlBool mmlBufferedInputStream::SetBufferSize(const mmlInt32 size)
{
	mBufferSize = size;
	if ( mBufferSize == mmlNULL )
	{
		mBuffer = (mmlUInt8*)mmlAlloc(mBufferSize);
	}
	else
	{
		mBuffer = (mmlUInt8 *)mmlRealloc(mBuffer, mBufferSize);
	}
	return mmlTrue;
}


MML_OBJECT_IMPL2(mmlBufferedInputStream, mmlIBufferedInputStream, mmlIInputStream)

mmlBufferedOutputStream::mmlBufferedOutputStream()
:mBuffer(mmlNULL), mBufferSize(4096),  mBufferDataLen(0)
{
	mBuffer = (mmlUInt8*) mmlAlloc(mBufferSize);
}


mmlBufferedOutputStream::~mmlBufferedOutputStream()
{
	if ( mBuffer != mmlNULL ) mmlFree(mBuffer);
}

mmlBool mmlBufferedOutputStream::Construct ( mmlIOutputStream * stream )
{
	mStream = stream;
	return mmlTrue;
}

mmlInt64 mmlBufferedOutputStream::Write ( const mmlInt64 size , const void * data )
{
	mmlInt64 sz = size;
	mmlUInt8 * data_ptr = (mmlUInt8*)data;
	while (sz > 0)
	{
		mmlInt64 available = mBufferSize - mBufferDataLen;
		if ( available > sz )
		{
			available = sz;
		}
		if ( available > 0 )
		{
			mmlMemoryCopy(mBuffer + mBufferDataLen , data_ptr, available);
			data_ptr += available;
		}
		sz -= available;
		mBufferDataLen += available;
		if ( mBufferSize - mBufferDataLen == 0)
		{
			if ( mStream->Write(mBufferDataLen, mBuffer) != mBufferDataLen )
			{
				return -1;
			}
			mBufferDataLen = 0;
		}
	}
	return size;
}

mmlBool mmlBufferedOutputStream::IsFull ()
{
	return mmlFalse;
}

mmlBool mmlBufferedOutputStream::GetControl ( mmlIStreamControl ** control )
{
	return mStream->GetControl(control);
}

mmlBool mmlBufferedOutputStream::Close ()
{
	if ( mBufferDataLen > 0 )
	{
		mmlInt64 wr= mStream->Write(mBufferDataLen, mBuffer);
		if ( mBufferDataLen > 0 && wr <= 0 )
		{
			mBufferDataLen = 0;
			return mmlFalse;
		}
		mBufferDataLen = 0;
	}
	return mStream->Close();
}

mmlInt64 mmlBufferedOutputStream::Size ()
{
	return mBufferDataLen + mStream->Size();
}

mmlBool mmlBufferedOutputStream::Flush ()
{
	if ( mBufferDataLen > 0 )
	{
		mmlInt64 wr= mStream->Write(mBufferDataLen, mBuffer);
		if ( mBufferDataLen > 0 && wr <= 0 )
		{
			mBufferDataLen = 0;
			return mmlTrue;
		}
		mBufferDataLen = 0;
	}
	return mStream->Flush();
}

mmlBool mmlBufferedOutputStream::SetBufferSize(const mmlInt32 size)
{
	mBufferSize = size;
	if ( mBufferSize == mmlNULL )
	{
		mBuffer = (mmlUInt8*)mmlAlloc(mBufferSize);
	}
	else
	{
		mBuffer = (mmlUInt8 *)mmlRealloc(mBuffer, mBufferSize);
	}
	return mmlTrue;
}

MML_OBJECT_IMPL2(mmlBufferedOutputStream, mmlIBufferedOutputStream, mmlIOutputStream)