#include "mmlGZIPOutputStream.h"

mmlGZIPOutputStream::mmlGZIPOutputStream()
:mSize(0), mStrm(mmlNULL), mMaxSize(0) , mWritten(0)
{
	
}

mmlGZIPOutputStream::~mmlGZIPOutputStream()
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

mmlBool mmlGZIPOutputStream::Construct ( mmlIOutputStream * stream )
{
	mOutput = stream;
	mStrm = (z_stream *)mmlAlloc(sizeof(z_stream));
	mmlMemorySet(mStrm, 0 , sizeof(mStrm));
	mStrm->zalloc = zlib_alloc;
	mStrm->zfree = zlib_free;
	mStrm->opaque = Z_NULL;
	return deflateInit2(mStrm , -1, 8, 15 + 16, 8, 0 ) == Z_OK ? mmlTrue : mmlFalse; 
}

mmlInt64 mmlGZIPOutputStream::Size ()
{
	return mSize;
}

mmlBool mmlGZIPOutputStream::Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
{
	return mmlFalse;
}

#define BUFLEN      16384

mmlInt64 mmlGZIPOutputStream::Write ( const mmlInt64 size , const void * data )
{
	unsigned char out[BUFLEN];
	mStrm->next_in = (Bytef *)data;
	mStrm->avail_in = (uInt)size;
	do {
		mStrm->next_out = out;
		mStrm->avail_out = BUFLEN;
		(void)deflate(mStrm, Z_NO_FLUSH);
		if ( mOutput->Write(BUFLEN - mStrm->avail_out, out) != (BUFLEN - mStrm->avail_out))
		{
			return -1;
		}
		mSize += (BUFLEN - mStrm->avail_out);
	} while (mStrm->avail_out == 0);
	return size;
}

mmlBool mmlGZIPOutputStream::Close ()
{
	if ( mStrm != mmlNULL )
	{
		unsigned char out[BUFLEN];
		mStrm->next_in = Z_NULL;
		mStrm->avail_in = 0;
		do {
			mStrm->next_out = out;
			mStrm->avail_out = BUFLEN;
			(void)deflate(mStrm, Z_FINISH);
			if ( mOutput->Write(BUFLEN - mStrm->avail_out, out) != (BUFLEN - mStrm->avail_out))
			{
				deflateEnd(mStrm);
				mmlFree(mStrm);
				return mmlFalse;
			}
			mSize += (BUFLEN - mStrm->avail_out);
		} while (mStrm->avail_out == 0);
		deflateEnd(mStrm);
		mmlFree(mStrm);
		mStrm = mmlNULL;
		return mOutput->Close();
	}
	return mmlFalse;
}

mmlBool mmlGZIPOutputStream::Flush()
{
	if ( mStrm != mmlNULL )
	{
		unsigned char out[BUFLEN];
		mStrm->next_in = Z_NULL;
		mStrm->avail_in = 0;
		do {
			mStrm->next_out = out;
			mStrm->avail_out = BUFLEN;
			(void)deflate(mStrm, Z_FINISH);
			if ( mOutput->Write(BUFLEN - mStrm->avail_out, out) != (BUFLEN - mStrm->avail_out))
			{
				deflateEnd(mStrm);
				mmlFree(mStrm);
				return mmlFalse;
			}
			mSize += (BUFLEN - mStrm->avail_out);
		} while (mStrm->avail_out == 0);
		deflateEnd(mStrm);
		mmlFree(mStrm);
		mStrm = mmlNULL;
		return mOutput->Flush();
	}
	return mmlFalse;
}

mmlBool mmlGZIPOutputStream::IsFull ()
{
	if ( mMaxSize != 0 )
	{
		return mWritten < mMaxSize ? mmlFalse : mmlTrue;
	}
	unsigned char out[BUFLEN];
	mStrm->next_in = Z_NULL;
	mStrm->avail_in = 0;
	do {
		mStrm->next_out = out;
		mStrm->avail_out = BUFLEN;
		(void)deflate(mStrm, Z_FULL_FLUSH);
		if ( mOutput->Write(BUFLEN - mStrm->avail_out, out) != (BUFLEN - mStrm->avail_out))
		{
			deflateEnd(mStrm);
			mmlFree(mStrm);
			return mmlFalse;
		}
		mSize += (BUFLEN - mStrm->avail_out);
	} while (mStrm->avail_out == 0);


	return mmlTrue;
}

mmlBool mmlGZIPOutputStream::SetMaxSize( const mmlInt64 size )
{
	mMaxSize = size;
	return mmlTrue;
}


MML_OBJECT_IMPL2(mmlGZIPOutputStream, mmlIGZIPOutputStream, mmlIOutputStream)