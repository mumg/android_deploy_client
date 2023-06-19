#include "mmlBase64InputStream.h"

mmlBase64InputStream::mmlBase64InputStream()
{
	
}

mmlBase64InputStream::~mmlBase64InputStream()
{
	Close();
}

mmlBool mmlBase64InputStream::Construct ( mmlIInputStream * stream )
{
	mInput = stream;
	return mmlTrue;
}

mmlInt64 mmlBase64InputStream::Size ()
{
	return mInput->Size();
}


mmlBool mmlBase64InputStream::Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
{
	return mmlFalse;
}

mmlInt64 mmlBase64InputStream::Read ( const mmlInt64 size , void * data )
{
	return 0;
}

mmlBool mmlBase64InputStream::Close ()
{
	return mInput->Close();
}

mmlBool mmlBase64InputStream::IsEmpty()
{
	return mInput->IsEmpty();
}


mmlBool mmlBase64InputStream::GetControl ( mmlIStreamControl ** control )
{
	return mInput->GetControl(control);
}

MML_OBJECT_IMPL1(mmlBase64InputStream, mmlIInputStream)