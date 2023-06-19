#include "mmlBase64OutputStream.h"

static const mmlChar cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

mmlBase64OutputStream::mmlBase64OutputStream()
	:mLength(0)
{
	
}

mmlBase64OutputStream::~mmlBase64OutputStream()
{
	Close();
}


mmlBool mmlBase64OutputStream::Construct ( mmlIOutputStream * stream )
{
	mOutput = stream;
	return mmlTrue;
}


mmlBool mmlBase64OutputStream::Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
{
	return mmlFalse;
}

mmlBool mmlBase64OutputStream::Flush()
{
	if ( mLength > 0 )
	{
		if ( mLength < 2)
		{
			mTriple[1] = 0;
		}
		if ( mLength < 3 )
		{
			mTriple[2] = 0;
		}
		mmlChar quad[4];
		quad[0] = (unsigned char) cb64[ (int)(mTriple[0] >> 2) ];
		quad[1] = (unsigned char) cb64[ (int)(((mTriple[0] & 0x03) << 4) | ((mTriple[1] & 0xf0) >> 4)) ];
		quad[2] = (unsigned char) (mLength > 1 ? cb64[ (int)(((mTriple[1] & 0x0f) << 2) | ((mTriple[2] & 0xc0) >> 6)) ] : '=');
		quad[3] = (unsigned char) (mLength > 2 ? cb64[ (int)(mTriple[2] & 0x3f) ] : '=');
		mLength = 0;
		if ( mOutput->Write(4, quad) != 4 )
		{
			return mmlFalse;
		}
	}
	return mOutput->Flush();
}

mmlInt64 mmlBase64OutputStream::Write ( const mmlInt64 size , const void * data )
{
	mmlUInt8 * src = (mmlUInt8*)data;
	mmlInt32 sz = size;
	for (;sz > 0;)
	{
		mmlInt32 s = sz > 3 - mLength ? 3 - mLength : sz;
		sz -=s;
		mmlMemoryCopy(mTriple + mLength, src , s);
		mLength += s;
		if ( mLength < 3)
		{
			return size;
		}
		src += s;

		mmlChar quad[4];
		quad[0] = (unsigned char) cb64[ (int)(mTriple[0] >> 2) ];
		quad[1] = (unsigned char) cb64[ (int)(((mTriple[0] & 0x03) << 4) | ((mTriple[1] & 0xf0) >> 4)) ];
		quad[2] = (unsigned char) cb64[ (int)(((mTriple[1] & 0x0f) << 2) | ((mTriple[2] & 0xc0) >> 6)) ];
		quad[3] = (unsigned char) cb64[ (int)(mTriple[2] & 0x3f) ];
		mLength = 0;
		if ( mOutput->Write(4, quad) != 4 )
		{
			return -1;
		}
	}
	return size;
}

mmlBool mmlBase64OutputStream::Close ()
{
	if ( Flush() == mmlFalse )
	{
		return mmlFalse;
	}
	if ( mOutput == mmlNULL ) return mmlTrue;
	return mOutput->Close();
}

mmlBool mmlBase64OutputStream::IsFull ()
{
	if ( mOutput == mmlNULL ) return mmlFalse;
	return mOutput->IsFull();
}


mmlBool mmlBase64OutputStream::GetControl ( mmlIStreamControl ** control )
{
	if ( mOutput == mmlNULL ) return mmlFalse;
	return mOutput->GetControl(control);
}

mmlInt64 mmlBase64OutputStream::Size ()
{
	if ( mOutput == mmlNULL ) return 0;
	return (mLength > 0 ? 4 : 0) + mOutput->Size();
}

MML_OBJECT_IMPL1(mmlBase64OutputStream, mmlIOutputStream)