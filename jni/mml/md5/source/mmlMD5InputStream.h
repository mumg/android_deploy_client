#ifndef MML_MD5_INPUT_STREAM_IMPL_HEADER_INCLUDED
#define MML_MD5_INPUT_STREAM_IMPL_HEADER_INCLUDED

#include "mmlIBuffer.h"
#include "mmlIStreamHash.h"
#include "mmlPtr.h"
extern "C"{
#include "md5.h"
};



class mmlMD5InputStream : public mmlIInputStreamHash
{
	MML_OBJECT_DECL
public:

	mmlMD5InputStream();

	~mmlMD5InputStream();

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

	MD5_CTX mMD5context;

};


#endif //MML_MD5_INPUT_STREAM_IMPL_HEADER_INCLUDED
