#include "mmlMemoryStream.h"
#include "mmlICString.h"
#include "mmlIBuffer.h"

mmlMemoryInputStreamControl::mmlMemoryInputStreamControl( mmlIBuffer * buffer)
	:mBuffer(buffer), mData((mmlUInt8*)mBuffer->Get()), mDataSize(mBuffer->Size()), mDataOffset(0)
{

}

mmlInt64 mmlMemoryInputStreamControl::Size ()
{
	return mDataSize;
}

mmlInt64 mmlMemoryInputStreamControl::GetPosition ()
{
	return mDataOffset;
}

mmlBool mmlMemoryInputStreamControl::Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
{
	if ( mode == MML_SEEK_SET )
	{
		if ( offset < 0 || offset >= mDataSize )
		{
			return mmlFalse;
		}
		mDataOffset = offset;
		return mmlTrue;
	}
	else if ( mode == MML_SEEK_END )
	{
		if ( offset <= 0 || offset >= mDataSize )
		{
			return mmlFalse;
		}
		mDataOffset = mDataSize - offset;
		return mmlTrue;
	}
	else
	{
		if ( mDataOffset + offset < 0 ||
			mDataOffset + offset >= mDataSize )
		{
			return mmlFalse;
		}
		mDataOffset += offset;
		return mmlTrue;
	}
}

mmlInt64 mmlMemoryInputStreamControl::Read ( const mmlInt64 size , void * data )
{
	mmlInt32 sz = (mmlInt32)(mDataSize - mDataOffset);
	if ( size < sz )
	{
		sz = (mmlInt32)size;
	}
	if ( sz > 0 )
	{
		mmlMemoryCopy(data , mData + mDataOffset , sz );
		mDataOffset += sz;
	}
	return sz;
}

mmlBool mmlMemoryInputStreamControl::Close ()
{
	return mmlTrue;
}

MML_OBJECT_IMPL1(mmlMemoryInputStreamControl, mmlIStreamControl)

mmlMemoryInputStream::mmlMemoryInputStream()
{

}

mmlBool mmlMemoryInputStream::Construct ( mmlIBuffer * buffer )
{
	mControl = new mmlMemoryInputStreamControl(buffer);
	return mmlTrue;
}


mmlBool mmlMemoryInputStream::Construct( mmlIMemoryOutputStream * stream )
{
	mmlAutoPtr < mmlIBuffer > data;
	stream->GetData(data.getterAddRef());
	return Construct(data);
}

class mmlCStringBuffer : public mmlIBuffer
{
	MML_OBJECT_DECL
public:
	mmlCStringBuffer ( mmlICString * str )
		:mStr(str)
	{

	}

	void * Get () { return (void*)mStr->Get(); }

	mmlInt32 Size () { return mStr->Length(); }

	mmlBool Copy ( const mmlInt32 offset, void * data, const mmlInt32 size )
	{
		if ( offset + size >= mStr->Length() )
		{
			return mmlFalse;
		}
		mmlMemoryCopy((mmlUInt8*)mStr->Get() + offset, data, size);
		return mmlTrue;
	}

private:

	mmlAutoPtr < mmlICString > mStr;
};

MML_OBJECT_IMPL1(mmlCStringBuffer, mmlIBuffer)

mmlBool mmlMemoryInputStream::Construct ( mmlICString * str )
{
	mControl = new mmlMemoryInputStreamControl(new mmlCStringBuffer(str));
	return mmlTrue;
}

mmlInt64 mmlMemoryInputStream::Read ( const mmlInt64 size , void * data )
{
	return mControl->Read(size, data);
}

mmlInt64 mmlMemoryInputStream::Size ()
{
	return mControl->Size();
}

mmlBool mmlMemoryInputStream::Close ()
{
	return mControl->Close();
}

MML_OBJECT_IMPL2(mmlMemoryInputStream, mmlIMemoryInputStream, mmlIInputStream)

mmlMemoryOutputStreamControlConstant::mmlMemoryOutputStreamControlConstant( mmlIBuffer * buffer)
	:mBuffer(buffer), mData((mmlUInt8*)buffer->Get()), mDataSize(buffer->Size()), mDataOffset(0), mMaxSize(0)
{

}

mmlBool mmlMemoryOutputStreamControlConstant::SetMaxSize ( const mmlInt64 size )
{
	mMaxSize = size;
	return mmlTrue;
}

mmlInt64 mmlMemoryOutputStreamControlConstant::GetPosition ()
{
	return mDataOffset;
}

mmlBool mmlMemoryOutputStreamControlConstant::Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
{
	if ( mode == MML_SEEK_SET )
	{
		if ( offset < 0 || offset >= mDataSize )
		{
			return mmlFalse;
		}
		mDataOffset = offset;
		return mmlTrue;
	}
	else if ( mode == MML_SEEK_END )
	{
		if ( offset <= 0 || offset >= mDataSize )
		{
			return mmlFalse;
		}
		mDataOffset = mDataSize - offset;
		return mmlTrue;
	}
	else
	{
		if ( mDataOffset + offset < 0 ||
			mDataOffset + offset >= mDataSize )
		{
			return mmlFalse;
		}
		mDataOffset += offset;
		return mmlTrue;
	}
}

mmlInt64 mmlMemoryOutputStreamControlConstant::Write ( const mmlInt64 size , const void * data )
{
	mmlUInt32 sz = (mmlUInt32)( (mMaxSize == 0 ? mDataSize : mMaxSize) - mDataOffset);
	if ( size < sz )
	{
		sz = (mmlUInt32)size;
	}
	if ( sz > 0 )
	{
		mmlMemoryCopy(mData + mDataOffset , data , sz );
		mDataOffset += sz;
	}
	return sz;
}

mmlBool mmlMemoryOutputStreamControlConstant::Close ()
{
	return mmlTrue;
}

MML_OBJECT_IMPL2(mmlMemoryOutputStreamControlConstant, mmlIOutputStreamControl, mmlIStreamControl)

mmlMemoryOutputStreamData::mmlMemoryOutputStreamData()
{

}

mmlBool mmlMemoryOutputStreamData::Construct ( mmlIBuffer * data )
{
	mControl = new mmlMemoryOutputStreamControlConstant(data);
	return mmlTrue;
}

mmlInt64 mmlMemoryOutputStreamData::Write ( const mmlInt64 size , const void * data )
{
	return mControl->Write(size, data);
}

mmlInt64 mmlMemoryOutputStreamData::Size ()
{
	return mControl->Size();
}


mmlBool mmlMemoryOutputStreamData::Close ()
{
	return mControl->Close();
}

MML_OBJECT_IMPL2(mmlMemoryOutputStreamData, mmlIMemoryOutputStream, mmlIOutputStream)


mmlMemoryOutputStreamControlMutable::mmlMemoryOutputStreamControlMutable()
{
	mData = ( mmlUInt8 *)mmlAlloc(256);
	mDataSize = 256;
	mDataExponent = 8;
	mDataOffset = 0;
	mDataMaxSize = 0;
}

mmlMemoryOutputStreamControlMutable::~mmlMemoryOutputStreamControlMutable()
{
	if ( mData != mmlNULL )
	{
		mmlFree(mData);
	}
}

mmlBool mmlMemoryOutputStreamControlMutable::SetMaxSize ( const mmlInt64 size )
{
	mDataMaxSize = size;
	return mmlTrue;
}

mmlInt64 mmlMemoryOutputStreamControlMutable::Size ()
{
	return mDataSize;
}

mmlInt64 mmlMemoryOutputStreamControlMutable::GetPosition ()
{
	return mDataOffset;
}

mmlBool mmlMemoryOutputStreamControlMutable::Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
{
	if ( mode == MML_SEEK_SET )
	{
		if ( offset < 0 || offset >= mDataSize )
		{
			return mmlFalse;
		}
		mDataOffset = offset;
		return mmlTrue;
	}
	else if ( mode == MML_SEEK_END )
	{
		if ( offset <= 0 || offset >= mDataSize )
		{
			return mmlFalse;
		}
		mDataOffset = mDataSize - offset;
		return mmlTrue;
	}
	else
	{
		if ( mDataOffset + offset < 0 ||
			mDataOffset + offset >= mDataSize )
		{
			return mmlFalse;
		}
		mDataOffset += offset;
		return mmlTrue;
	}
}

mmlBool mmlMemoryOutputStreamControlMutable::Close ()
{
	mmlUInt8 zero = 0;
	if ( Write(1, &zero) != 1 )
	{
		return mmlFalse;
	}
	if ( mData == mmlNULL )
	{
		return mmlFalse;
	}
	if ( mBuffer != mmlNULL )
	{
		return mmlFalse;
	}
	mBuffer = mmlNewAllocatedBuffer(mData, mDataOffset - 1);
	mData = mmlNULL;
	mDataOffset = 0;
	mDataSize = 0;
	return mmlTrue;
}

mmlBool mmlMemoryOutputStreamControlMutable::GetData( mmlIBuffer ** data )
{
	if ( mBuffer == mmlNULL )
	{
		return mmlFalse;
	}
	*data = mBuffer;
	MML_ADDREF(*data);
	return mmlTrue;
}

mmlInt64 mmlMemoryOutputStreamControlMutable::Write ( const mmlInt64 size , const void * data )
{
	mmlUInt32 sz = size;
	if ( mDataMaxSize > 0 && mDataOffset + sz > mDataMaxSize )
	{
		sz = mDataMaxSize - mDataOffset;
	}
	if ( mDataOffset + sz > mDataSize )
	{
		while (mDataSize < mDataOffset + sz)
		{
			mDataExponent ++;
			mDataSize = 2 ^ mDataExponent;
		}
		mData = (mmlUInt8*)mmlRealloc(mData , mDataSize);
	}
	mmlMemoryCopy(mData + mDataOffset , data , sz );
	mDataOffset += sz;
	return size;
}

MML_OBJECT_IMPL2(mmlMemoryOutputStreamControlMutable, mmlIOutputStreamControl, mmlIStreamControl)


mmlMemoryOutputStreamMutable::mmlMemoryOutputStreamMutable()
{
	mControl = new mmlMemoryOutputStreamControlMutable();
}

mmlInt64 mmlMemoryOutputStreamMutable::Write ( const mmlInt64 size , const void * data )
{
	return mControl->Write(size, data);
}

mmlBool mmlMemoryOutputStreamMutable::GetData ( mmlIBuffer ** data )
{
	return mControl->GetData(data);
}

mmlInt64 mmlMemoryOutputStreamMutable::Size ()
{
	return mControl->Size();
}

mmlBool mmlMemoryOutputStreamMutable::Close ()
{
	return mControl->Close();
}

MML_OBJECT_IMPL2(mmlMemoryOutputStreamMutable, mmlIMemoryOutputStream, mmlIOutputStream)

mmlBool mmlMemoryOutputStreamControlMutableCString::Close ()
{
	if ( mData == mmlNULL )
	{
		return mmlFalse;
	}
	mmlChar zero = 0;
	Write(sizeof(zero), &zero);
	mStr->Adopt((mmlChar*)mData);
	mData = mmlNULL;
	mDataOffset = 0;
	mDataSize = 0;
	return mmlTrue;
}

MML_OBJECT_IMPL2(mmlMemoryOutputStreamControlMutableCString, mmlIOutputStreamControl, mmlIStreamControl)

mmlMemoryOutputStreamCString::mmlMemoryOutputStreamCString()
{

}

mmlBool mmlMemoryOutputStreamCString::Construct ( mmlICString * str )
{
	mControl = new mmlMemoryOutputStreamControlMutableCString(str);
	return mmlTrue;
}

mmlMemoryOutputStreamCString::~mmlMemoryOutputStreamCString()
{
}


mmlInt64 mmlMemoryOutputStreamCString::Write ( const mmlInt64 size , const void * data )
{
	return mControl->Write(size, data);
}

mmlInt64 mmlMemoryOutputStreamCString::Size ()
{
	return mControl->Size();
}

mmlBool mmlMemoryOutputStreamCString::Close ()
{
	return mControl->Close();
}

MML_OBJECT_IMPL2(mmlMemoryOutputStreamCString, mmlIMemoryOutputStream, mmlIOutputStream)

mmlMemoryOutputCachedStream::mmlMemoryOutputCachedStream()
:mCacheBuffer(mmlNULL), mCacheBufferSize(0), mCacheSize(0)
{

}

mmlMemoryOutputCachedStream::~mmlMemoryOutputCachedStream()
{
	if ( mCacheBuffer != mmlNULL )
	{
		mmlFree(mCacheBuffer);
	}
}

mmlBool mmlMemoryOutputCachedStream::Construct ( mmlIOutputStream * data )
{
	mOutput = data;
	return mmlTrue;
}

mmlInt64 mmlMemoryOutputCachedStream::Write ( const mmlInt64 size , const void * data )
{
	if (mCacheBufferSize == 0 || mCacheSize + size > (1 << mCacheBufferSize) )
	{
		while ( mCacheSize + size > (1 << mCacheBufferSize) )
		{
			mCacheBufferSize ++;
		}
		mCacheBuffer = (mmlUInt8*)mmlRealloc(mCacheBuffer, 1 << mCacheBufferSize);
	}
	mmlMemoryCopy (mCacheBuffer + mCacheSize, data, size);
	mCacheSize += size;
	return size;
}

mmlBool mmlMemoryOutputCachedStream::IsFull ()
{
	if ( Flush() == mmlFalse )
	{
		return mmlTrue;
	}
	return mOutput->IsFull();
}

mmlBool mmlMemoryOutputCachedStream::GetControl ( mmlIStreamControl ** control )
{
	if ( Flush() == mmlFalse )
	{
		return mmlFalse;
	}
	return mOutput->GetControl(control);
}

mmlBool mmlMemoryOutputCachedStream::Close ()
{
	if ( Flush() == mmlFalse )
	{
		return mmlFalse;
	}
	return mOutput->Close();
}

mmlInt64 mmlMemoryOutputCachedStream::Size ()
{
	mmlInt64 size = mOutput->Size();
	size += mCacheSize;
	return size;
}

mmlBool mmlMemoryOutputCachedStream::Flush ()
{
	if ( mCacheSize == 0 )
	{
		return mmlTrue;
	}
	if ( mOutput->Write(mCacheSize, mCacheBuffer) != mCacheSize )
	{
		return mmlFalse;
	}
	mCacheSize = 0;
	return mmlTrue;
}

MML_OBJECT_IMPL2(mmlMemoryOutputCachedStream, mmlIMemoryOutputCachedStream, mmlIOutputStream)