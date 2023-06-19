#include "mmlMD5OutputStream.h"
#include "mmlICString.h"

mmlMD5OutputStream::mmlMD5OutputStream()
{
	MD5Init(&mMD5context);
}

mmlMD5OutputStream::~mmlMD5OutputStream()
{
	Close();
}


mmlBool mmlMD5OutputStream::Construct ( mmlIOutputStream * stream )
{
	mOutput = stream;
	return mmlTrue;
}


mmlBool mmlMD5OutputStream::Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
{
	return mmlFalse;
}

#include <stdio.h>


mmlInt64 mmlMD5OutputStream::Write ( const mmlInt64 size , const void * data )
{
	MD5Update( &mMD5context, (unsigned char*)data, size);
	if ( mOutput == mmlNULL ) return size;
	return mOutput->Write(size, data);
}

mmlBool mmlMD5OutputStream::Close ()
{
	if ( mOutput == mmlNULL ) return mmlTrue;
	return mOutput->Close();
}

mmlBool mmlMD5OutputStream::IsFull ()
{
	if ( mOutput == mmlNULL ) return mmlFalse;
	return mOutput->IsFull();
}



mmlBool mmlMD5OutputStream::GetControl ( mmlIStreamControl ** control )
{
	if ( mOutput == mmlNULL ) return mmlFalse;
	return mOutput->GetControl(control);
}

mmlInt64 mmlMD5OutputStream::Size ()
{
	if ( mOutput == mmlNULL ) return 0;
	return mOutput->Size();
}

mmlBool mmlMD5OutputStream::GetHash(mmlIVariant ** val)
{
	mmlUInt8 hash[16];
	static const mmlChar * hex  = "0123456789abcdef";
	mmlChar result [33] = {0};
	MD5Final ( hash, &mMD5context);
	for ( int count = 0 ; count < 16; count ++ )
	{
		result[count * 2] = hex[(hash[count] & 0xF0) >> 4];
		result[count * 2 + 1] = hex[(hash[count]  & 0x0F)];
	}
	*val = mmlNewVariantString(mmlNewCString(result , 32));
	MML_ADDREF(*val);
	return mmlTrue;
}

mmlBool mmlMD5OutputStream::GetHash(mmlIBuffer ** val)
{
	mmlAutoPtr < mmlIBuffer > buffer = mmlAllocateBuffer(16);
	MD5Final ( (mmlUInt8*)buffer->Get(), &mMD5context);
	*val = buffer;
	MML_ADDREF(*val);
	return mmlTrue;
}

MML_OBJECT_IMPL2(mmlMD5OutputStream, mmlIOutputStreamHash, mmlIOutputStream)