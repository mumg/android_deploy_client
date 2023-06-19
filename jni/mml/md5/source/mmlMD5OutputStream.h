#ifndef MML_MD5_OUTPUT_STREAM_IMPL_HEADER_INCLUDED
#define MML_MD5_OUTPUT_STREAM_IMPL_HEADER_INCLUDED

#include "mmlPtr.h"
#include "mmlIStreamHash.h"
extern "C"{
#include "md5.h"
};


class mmlMD5OutputStream : public mmlIOutputStreamHash
{
	MML_OBJECT_DECL
public:

	mmlMD5OutputStream();

	~mmlMD5OutputStream();

	mmlBool Construct ( mmlIOutputStream * stream );

	mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset );

	mmlInt64 Write ( const mmlInt64 size , const void * data );

	mmlBool Close ();

	mmlBool IsFull ();

	mmlBool GetControl ( mmlIStreamControl ** control );

	mmlInt64 Size ();

	mmlBool Flush () { if ( mOutput == mmlNULL ) return mmlTrue; return mOutput->Flush(); }

	mmlBool GetHash(mmlIVariant ** hash);

	mmlBool GetHash(mmlIBuffer ** val);

private:

	mmlAutoPtr < mmlIOutputStream > mOutput;

	MD5_CTX mMD5context;

};


#endif //MML_MD5_OUTPUT_STREAM_IMPL_HEADER_INCLUDED
