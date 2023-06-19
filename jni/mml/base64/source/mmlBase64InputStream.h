#ifndef MML_BASE64_INPUT_STREAM_IMPL_HEADER_INCLUDED
#define MML_BASE64_INPUT_STREAM_IMPL_HEADER_INCLUDED

#include "mmlIStream.h"
#include "mmlPtr.h"


class mmlBase64InputStream : public mmlIInputStream
{
	MML_OBJECT_DECL
public:

	mmlBase64InputStream();

	~mmlBase64InputStream();

	mmlBool Construct ( mmlIInputStream * stream );

    mmlInt64 Size ();

    mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset );

    mmlInt64 Read ( const mmlInt64 size , void * data );

    mmlBool Close ();

	mmlBool IsEmpty();

	mmlBool GetControl ( mmlIStreamControl ** control );

private:

	mmlAutoPtr < mmlIInputStream > mInput;

};


#endif //MML_BASE64_INPUT_STREAM_IMPL_HEADER_INCLUDED
