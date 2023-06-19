#include "mmlStreamCopy.h"


mmlStreamCopy::mmlStreamCopy()
	:mChunkSize(4096)
{

}

mmlBool mmlStreamCopy::SetChunkSize ( const mmlInt32 size )
{
	mChunkSize = size;
	return mmlTrue;
}

class buffer
{
private:
	void * mBuffer;
public:

	buffer ( const mmlInt32 sz)
	{
		mBuffer = mmlAlloc(sz);
	}

	~buffer()
	{
		mmlFree(mBuffer);
	}

	operator void *()
	{
		return mBuffer;
	}
};

mmlBool mmlStreamCopy::Copy ( mmlIInputStream * in, mmlIOutputStream * out, const mmlInt64 size)
{
	mmlInt64 copied = 0;
	buffer bf(mChunkSize);
	if ( size == 0 )
	{
		while (in->IsEmpty() == mmlFalse && out->IsFull() == mmlFalse )
		{
			mmlInt64 rd = in->Read(mChunkSize, bf);
			if ( rd == 0 )
			{
				return mmlTrue;
			}
			if ( rd < 0 )
			{
				return mmlFalse;
			}
			mmlInt64 wr = out->Write(rd, bf);
			if ( wr < 0 )
			{
				return mmlFalse;
			}
			if ( wr == 0 )
			{
				return mmlTrue;
			}
			if ( wr < rd )
			{
				return mmlTrue;
			}
			copied+=wr;
			if ( mListener != mmlNULL ) mListener->OnProgress(copied);
		}
	}
	else
	{
		for ( mmlInt64 sz = size ; sz > 0 && in->IsEmpty() == mmlFalse && out->IsFull() == mmlFalse ; )
		{
			mmlInt64 s = mChunkSize > sz ? sz : mChunkSize;
			mmlInt64 rd = in->Read(s, bf);
			if ( rd == 0 )
			{
				return mmlFalse;
			}
			if ( rd < 0 )
			{
				return mmlFalse;
			}
			mmlInt64 wr = out->Write(rd, bf);
			if ( wr < 0 )
			{
				return mmlFalse;
			}
			if ( wr == 0 )
			{
				return mmlFalse;
			}
			if ( wr < rd )
			{
				return mmlFalse;
			}
			sz -= rd;
			copied+=wr;
			if ( mListener != mmlNULL ) mListener->OnProgress(copied);
		}
	}
	return mmlTrue;
}

MML_OBJECT_IMPL1(mmlStreamCopy, mmlIStreamCopy)