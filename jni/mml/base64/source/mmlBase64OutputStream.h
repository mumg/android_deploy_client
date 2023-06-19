#ifndef MML_BASE64_OUTPUT_STREAM_IMPL_HEADER_INCLUDED
#define MML_BASE64_OUTPUT_STREAM_IMPL_HEADER_INCLUDED

#include "mmlIStream.h"
#include "mmlPtr.h"


class mmlBase64OutputStream : public mmlIOutputStream
{
	MML_OBJECT_DECL
public:

	mmlBase64OutputStream();

	~mmlBase64OutputStream();

	mmlBool Construct ( mmlIOutputStream * stream );

	mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset );

	mmlInt64 Write ( const mmlInt64 size , const void * data );

	mmlBool Close ();

	mmlBool IsFull ();

	mmlBool GetControl ( mmlIStreamControl ** control );

	mmlInt64 Size ();

	mmlBool Flush ();

private:

	mmlAutoPtr < mmlIOutputStream > mOutput;

	mmlUInt8 mTriple[3];
	mmlInt32 mLength;

};


#endif //MML_BASE64_OUTPUT_STREAM_IMPL_HEADER_INCLUDED
