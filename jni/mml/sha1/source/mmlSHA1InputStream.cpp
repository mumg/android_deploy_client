#include "mmlSHA1InputStream.h"
#include "mmlICString.h"

mmlSHA1InputStream::mmlSHA1InputStream()
{
	SHA1Init(&mSHA1context);
}

mmlSHA1InputStream::~mmlSHA1InputStream()
{
	Close();
}

mmlBool mmlSHA1InputStream::Construct ( mmlIInputStream * stream )
{
	mInput = stream;
	return mmlTrue;
}

mmlInt64 mmlSHA1InputStream::Size ()
{
	return mInput->Size();
}


mmlBool mmlSHA1InputStream::Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
{
	return mmlFalse;
}

mmlInt64 mmlSHA1InputStream::Read ( const mmlInt64 size , void * data )
{
	mmlInt64 sz = mInput->Read(size, data);
	if ( sz > 0 )
	{
		SHA1Update( &mSHA1context, (unsigned char*)data, size);
	}
	return sz;
}

mmlBool mmlSHA1InputStream::Close ()
{
	return mInput->Close();
}

mmlBool mmlSHA1InputStream::IsEmpty()
{
	return mInput->IsEmpty();
}

mmlBool mmlSHA1InputStream::GetHash(mmlIVariant ** val)
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


mmlBool mmlSHA1InputStream::GetHash(mmlIBuffer ** val)
{
	mmlAutoPtr < mmlIBuffer > buffer = mmlAllocateBuffer(20);
	SHA1Final ( (mmlUInt8*)buffer->Get(), &mSHA1context);
	*val = buffer;
	MML_ADDREF(*val);
	return mmlTrue;
}


mmlBool mmlSHA1InputStream::GetControl ( mmlIStreamControl ** control )
{
	return mInput->GetControl(control);
}

MML_OBJECT_IMPL2(mmlSHA1InputStream, mmlIInputStreamHash, mmlIInputStream)