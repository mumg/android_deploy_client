#ifndef MML_SHA1_OUTPUT_STREAM_IMPL_HEADER_INCLUDED
#define MML_SHA1_OUTPUT_STREAM_IMPL_HEADER_INCLUDED

#include "mmlPtr.h"
#include "mmlIStreamHash.h"
extern "C"{
#include "sha1.h"
};


class mmlSHA1OutputStream : public mmlIOutputStreamHash
{
	MML_OBJECT_DECL
public:

	mmlSHA1OutputStream();

	~mmlSHA1OutputStream();

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

	SHA1_CTX mSHA1context;

};


#endif //MML_SHA1_OUTPUT_STREAM_IMPL_HEADER_INCLUDED
