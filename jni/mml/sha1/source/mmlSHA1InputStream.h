#ifndef MML_SHA1_INPUT_STREAM_IMPL_HEADER_INCLUDED
#define MML_SHA1_INPUT_STREAM_IMPL_HEADER_INCLUDED

#include "mmlIBuffer.h"
#include "mmlIStreamHash.h"
#include "mmlPtr.h"
extern "C"{
#include "sha1.h"
};



class mmlSHA1InputStream : public mmlIInputStreamHash
{
	MML_OBJECT_DECL
public:

	mmlSHA1InputStream();

	~mmlSHA1InputStream();

	mmlBool Construct ( mmlIInputStream * stream );

    mmlInt64 Size ();

    mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset );

    mmlInt64 Read ( const mmlInt64 size , void * data );

    mmlBool Close ();

	mmlBool IsEmpty();

	mmlBool GetControl ( mmlIStreamControl ** control );

	mmlBool GetHash(mmlIVariant ** hash);

	mmlBool GetHash(mmlIBuffer ** val);

private:

	mmlAutoPtr < mmlIInputStream > mInput;

	SHA1_CTX mSHA1context;

};


#endif //MML_SHA1_INPUT_STREAM_IMPL_HEADER_INCLUDED
