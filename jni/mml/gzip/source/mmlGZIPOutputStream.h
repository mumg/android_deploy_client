#ifndef MML_GZIP_OUTPUT_STREAM_IMPL_HEADER_INCLUDED
#define MML_GZIP_OUTPUT_STREAM_IMPL_HEADER_INCLUDED

#include "mmlIGZIPOutputStream.h"
#include "mmlPtr.h"

#include "zlib.h" 

class mmlGZIPOutputStream : public mmlIGZIPOutputStream
{
	MML_OBJECT_DECL
public:

	mmlGZIPOutputStream();

	~mmlGZIPOutputStream();

	mmlBool Construct ( mmlIOutputStream * stream );

	mmlInt64 Size ();

	mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset );

	mmlInt64 Write ( const mmlInt64 size , const void * data );

	mmlBool Close ();

	mmlBool IsFull ();

	mmlBool SetMaxSize( const mmlInt64 size );

	mmlBool Flush();

	mmlBool GetControl ( mmlIStreamControl ** control ) { return mmlFalse; }

private:

	mmlInt64 mWritten;

	mmlInt64 mMaxSize;

	mmlInt64 mSize;

	mmlAutoPtr < mmlIOutputStream > mOutput;

	z_stream * mStrm;

};


#endif //MML_GZIP_OUTPUT_STREAM_IMPL_HEADER_INCLUDED
