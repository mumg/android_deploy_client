#include "mmlSHA1OutputStream.h"
#include "mmlICString.h"

mmlSHA1OutputStream::mmlSHA1OutputStream()
{
	SHA1Init(&mSHA1context);
}

mmlSHA1OutputStream::~mmlSHA1OutputStream()
{
	Close();
}


mmlBool mmlSHA1OutputStream::Construct ( mmlIOutputStream * stream )
{
	mOutput = stream;
	return mmlTrue;
}


mmlBool mmlSHA1OutputStream::Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
{
	return mmlFalse;
}

#include <stdio.h>


mmlInt64 mmlSHA1OutputStream::Write ( const mmlInt64 size , const void * data )
{
	SHA1Update( &mSHA1context, (unsigned char*)data, size);
	if ( mOutput == mmlNULL ) return size;
	return mOutput->Write(size, data);
}

mmlBool mmlSHA1OutputStream::Close ()
{
	if ( mOutput == mmlNULL ) return mmlTrue;
	return mOutput->Close();
}

mmlBool mmlSHA1OutputStream::IsFull ()
{
	if ( mOutput == mmlNULL ) return mmlFalse;
	return mOutput->IsFull();
}



mmlBool mmlSHA1OutputStream::GetControl ( mmlIStreamControl ** control )
{
	if ( mOutput == mmlNULL ) return mmlFalse;
	return mOutput->GetControl(control);
}

mmlInt64 mmlSHA1OutputStream::Size ()
{
	if ( mOutput == mmlNULL ) return 0;
	return mOutput->Size();
}

mmlBool mmlSHA1OutputStream::GetHash(mmlIVariant ** val)
{
	mmlUInt8 hash[20];
	static const mmlChar * hex  = "0123456789abcdef";
	mmlChar result [41] = {0};
	SHA1Final ( hash, &mSHA1context);
	for ( int count = 0 ; count < 20; count ++ )
	{
		result[count * 2] = hex[(hash[count] & 0xF0) >> 4];
		result[count * 2 + 1] = hex[(hash[count]  & 0x0F)];
	}
	*val = mmlNewVariantString(mmlNewCString(result , 40));
	MML_ADDREF(*val);
	return mmlTrue;
}

mmlBool mmlSHA1OutputStream::GetHash(mmlIBuffer ** val)
{
	mmlAutoPtr < mmlIBuffer > buffer = mmlAllocateBuffer(20);
	SHA1Final ( (mmlUInt8*)buffer->Get(), &mSHA1context);
	*val = buffer;
	MML_ADDREF(*val);
	return mmlTrue;
}

MML_OBJECT_IMPL2(mmlSHA1OutputStream, mmlIOutputStreamHash, mmlIOutputStream)