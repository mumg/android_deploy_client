#include "mmlCRC32InputStream.h"

mmlCRC32InputStream::mmlCRC32InputStream()
{
	
}

mmlCRC32InputStream::~mmlCRC32InputStream()
{
	Close();
}

mmlBool mmlCRC32InputStream::Construct ( mmlIInputStream * stream )
{
	mInput = stream;
	return mmlTrue;
}

mmlInt64 mmlCRC32InputStream::Size ()
{
	return mInput->Size();
}


mmlBool mmlCRC32InputStream::Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
{
	return mmlFalse;
}

mmlInt64 mmlCRC32InputStream::Read ( const mmlInt64 size , void * data )
{
	mmlInt64 sz = mInput->Read(size, data);
	if ( sz > 0 )
	{
		mCRC32.Update((mmlUInt8*)data, (mmlInt32)sz);
	}
	return sz;
}

mmlBool mmlCRC32InputStream::Close ()
{
	return mInput->Close();
}

mmlBool mmlCRC32InputStream::IsEmpty()
{
	return mInput->IsEmpty();
}

mmlBool mmlCRC32InputStream::GetHash( mmlIBuffer ** hash )
{
	mmlUInt32 crc32 = mCRC32.Get();
	*hash = mmlAllocateBuffer(4);
	mmlMemoryCopy((*hash)->Get(), &crc32, 4);
	MML_ADDREF(*hash);
	return mmlTrue;
}

mmlBool mmlCRC32InputStream::GetHash( mmlIVariant ** hash)
{
	*hash = mmlNewVariantInteger(mCRC32.Get());
	MML_ADDREF(*hash);
	return mmlTrue;
}

mmlBool mmlCRC32InputStream::GetControl ( mmlIStreamControl ** control )
{
	return mInput->GetControl(control);
}

MML_OBJECT_IMPL2(mmlCRC32InputStream, mmlIInputStreamHash, mmlIInputStream)