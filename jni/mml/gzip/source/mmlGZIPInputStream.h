#ifndef MML_GZIP_INPUT_STREAM_IMPL_HEADER_INCLUDED
#define MML_GZIP_INPUT_STREAM_IMPL_HEADER_INCLUDED

#include "mmlIGZIPInputStream.h"
#include "mmlPtr.h"

#include "zlib.h" 

#define BUFLEN      32

class mmlGZIPInputStream : public mmlIGZIPInputStream
{
	MML_OBJECT_DECL
public:

	mmlGZIPInputStream();

	~mmlGZIPInputStream();

	mmlBool Construct ( mmlIInputStream * stream );

    mmlInt64 Size ();

	mmlInt64 GetPosition ();

    mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset );

    mmlInt64 Read ( const mmlInt64 size , void * data );

    mmlBool Close ();

	mmlBool IsEmpty();

	mmlBool GetControl ( mmlIStreamControl ** control ) { return mmlFalse; }

private:

	mmlInt64 mSize;

	mmlInt32 mOffset;

	mmlUInt8 mIData[BUFLEN];
	mmlAutoPtr < mmlIInputStream > mInput;

	z_stream * mStrm;
};


#endif //MML_GZIP_INPUT_STREAM_IMPL_HEADER_INCLUDED
