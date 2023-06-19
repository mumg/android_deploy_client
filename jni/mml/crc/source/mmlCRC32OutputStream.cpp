#include "mmlCRC32OutputStream.h"

mmlCRC32OutputStream::mmlCRC32OutputStream()
{
	
}

mmlCRC32OutputStream::~mmlCRC32OutputStream()
{
	Close();
}


mmlBool mmlCRC32OutputStream::Construct ( mmlIOutputStream * stream )
{
	mOutput = stream;
	return mmlTrue;
}


mmlBool mmlCRC32OutputStream::Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
{
	return mmlFalse;
}

#include <stdio.h>


mmlInt64 mmlCRC32OutputStream::Write ( const mmlInt64 size , const void * data )
{
	//fprintf(stdout, "CRC32: len %d\n", size); fflush(stdout);
	mCRC32.Update((mmlUInt8*)data, (mmlInt32)size);
	if ( mOutput == mmlNULL ) return size;
	return mOutput->Write(size, data);
}

mmlBool mmlCRC32OutputStream::Close ()
{
	if ( mOutput == mmlNULL ) return mmlTrue;
	return mOutput->Close();
}

mmlBool mmlCRC32OutputStream::IsFull ()
{
	if ( mOutput == mmlNULL ) return mmlFalse;
	return mOutput->IsFull();
}


mmlBool mmlCRC32OutputStream::GetHash( mmlIBuffer ** hash )
{
	mmlUInt32 crc32 = mCRC32.Get();
	*hash = mmlAllocateBuffer(4);
	mmlMemoryCopy((*hash)->Get(), &crc32, 4);
	MML_ADDREF(*hash);
	return mmlTrue;
}

mmlBool mmlCRC32OutputStream::GetHash( mmlIVariant ** hash)
{
	*hash = mmlNewVariantInteger(mCRC32.Get());
	MML_ADDREF(*hash);
	return mmlTrue;
}

mmlBool mmlCRC32OutputStream::GetControl ( mmlIStreamControl ** control )
{
	if ( mOutput == mmlNULL ) return mmlFalse;
	return mOutput->GetControl(control);
}

mmlInt64 mmlCRC32OutputStream::Size ()
{
	if ( mOutput == mmlNULL ) return 0;
	return mOutput->Size();
}

MML_OBJECT_IMPL2(mmlCRC32OutputStream, mmlIOutputStreamHash, mmlIOutputStream)