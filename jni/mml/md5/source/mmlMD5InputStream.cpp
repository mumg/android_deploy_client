#include "mmlMD5InputStream.h"
#include "mmlICString.h"

mmlMD5InputStream::mmlMD5InputStream()
{
	MD5Init(&mMD5context);
}

mmlMD5InputStream::~mmlMD5InputStream()
{
	Close();
}

mmlBool mmlMD5InputStream::Construct ( mmlIInputStream * stream )
{
	mInput = stream;
	return mmlTrue;
}

mmlInt64 mmlMD5InputStream::Size ()
{
	return mInput->Size();
}


mmlBool mmlMD5InputStream::Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
{
	return mmlFalse;
}

mmlInt64 mmlMD5InputStream::Read ( const mmlInt64 size , void * data )
{
	mmlInt64 sz = mInput->Read(size, data);
	if ( sz > 0 )
	{
		MD5Update( &mMD5context, (unsigned char*)data, size);
	}
	return sz;
}

mmlBool mmlMD5InputStream::Close ()
{
	return mInput->Close();
}

mmlBool mmlMD5InputStream::IsEmpty()
{
	return mInput->IsEmpty();
}

mmlBool mmlMD5InputStream::GetHash(mmlIVariant ** val)
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


mmlBool mmlMD5InputStream::GetHash(mmlIBuffer ** val)
{
	mmlAutoPtr < mmlIBuffer > buffer = mmlAllocateBuffer(16);
	MD5Final ( (mmlUInt8*)buffer->Get(), &mMD5context);
	*val = buffer;
	MML_ADDREF(*val);
	return mmlTrue;
}


mmlBool mmlMD5InputStream::GetControl ( mmlIStreamControl ** control )
{
	return mInput->GetControl(control);
}

MML_OBJECT_IMPL2(mmlMD5InputStream, mmlIInputStreamHash, mmlIInputStream)