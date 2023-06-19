#include "mmlGZIPInputStream.h"

mmlGZIPInputStream::mmlGZIPInputStream()
: mStrm(mmlNULL), mSize(0), mOffset(0)
{
	
}

mmlGZIPInputStream::~mmlGZIPInputStream()
{
	Close();
}

static void *zlib_alloc(void *q, unsigned n, unsigned m)
{
	q = Z_NULL;
	return mmlAlloc(n * m);
}

static void zlib_free(void *q, void *p)
{
	q = Z_NULL;
	mmlFree(p);
}

mmlBool mmlGZIPInputStream::Construct ( mmlIInputStream * stream )
{
	mInput = stream;
	mStrm = (z_stream *)mmlAlloc(sizeof(z_stream));
	mmlMemorySet(mStrm, 0 , sizeof(z_stream));
	mStrm->zalloc = zlib_alloc;
	mStrm->zfree = zlib_free;
	mStrm->opaque = Z_NULL;
	return inflateInit2(mStrm, 15 + 16) == Z_OK ? mmlTrue : mmlFalse; 
}

mmlInt64 mmlGZIPInputStream::Size ()
{
	return -1;
}

mmlInt64 mmlGZIPInputStream::GetPosition ()
{
	return -1;
}

mmlBool mmlGZIPInputStream::Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
{
	return mmlFalse;
}



mmlInt64 mmlGZIPInputStream::Read ( const mmlInt64 size , void * data )
{
	mmlInt64 _size = size;
	mmlUInt8 * _data = (mmlUInt8*)data;
	do 
	{
		mStrm->next_out = _data;
		mStrm->avail_out = _size;

		if ( mStrm->avail_in == 0 && mInput != mmlNULL )
		{
			mmlInt64 sz = mInput->Read(BUFLEN, mIData);
			if ( sz == 0 )
			{
				mInput->Close();
				mInput = mmlNULL;
			}
			else
			{
				mStrm->next_in = (Bytef*)mIData;
				mStrm->avail_in = (uInt)sz;
			}
		}

		switch (inflate(mStrm, Z_NO_FLUSH))
		{
		case Z_STREAM_END:
			{
				mInput->Close();
				mInput = mmlNULL;
				break;
			}
		case Z_OK:
			{
				break;
			}
		default:
			{
				return -1;
			}
		}
		_data += (_size - mStrm->avail_out);
		_size = _size - (_size - mStrm->avail_out);
	} while (_size > 0);

	return size - _size;
}

mmlBool mmlGZIPInputStream::Close ()
{
	if ( mStrm != mmlNULL )
	{
		inflateEnd(mStrm);
		mmlFree(mStrm);
		mStrm = mmlNULL;
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlGZIPInputStream::IsEmpty()
{
	if ( mInput->IsEmpty())
	{
		if (  mStrm == mmlNULL || mStrm->avail_out == 0)
		{
			return mmlTrue;
		}
	}
	return mmlFalse;
}

MML_OBJECT_IMPL2(mmlGZIPInputStream, mmlIGZIPInputStream, mmlIInputStream)