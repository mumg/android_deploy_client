#ifndef MML_BUFFERED_STREAM_HEADER_INCLUDED
#define MML_BUFFERED_STREAM_HEADER_INCLUDED

#include "mmlIStream.h"

class mmlBufferedInputStream : public mmlIBufferedInputStream
{
	MML_OBJECT_DECL
public:

	mmlBufferedInputStream();

	~mmlBufferedInputStream();

	mmlBool Construct ( mmlIInputStream * stream );

	mmlBool GetControl ( mmlIStreamControl ** control );

	mmlInt64 Read ( const mmlInt64 size , void * data );

	mmlBool IsEmpty ();

	mmlInt64 Size ();

	mmlBool Close ();

	mmlBool SetBufferSize(const mmlInt32 size);

private:
	mmlUInt8 * mBuffer;
	mmlInt32 mBufferSize;
	mmlInt32 mBufferDataOffset;
	mmlInt32 mBufferDataLen;
	mmlAutoPtr < mmlIInputStream > mStream;
};


class mmlBufferedOutputStream : public mmlIBufferedOutputStream
{
	MML_OBJECT_DECL
public:

	mmlBufferedOutputStream();

	~mmlBufferedOutputStream();

	mmlBool Construct ( mmlIOutputStream * stream );

	mmlInt64 Write ( const mmlInt64 size , const void * data );

	mmlBool IsFull ();

	mmlBool GetControl ( mmlIStreamControl ** control );

	mmlBool Close ();

	mmlInt64 Size ();

	mmlBool Flush ();

	mmlBool SetBufferSize(const mmlInt32 size);

private:

	mmlUInt8 * mBuffer;
	mmlInt32 mBufferSize;
	mmlInt32 mBufferDataLen;
	mmlAutoPtr < mmlIOutputStream > mStream;
};

#endif //MML_BUFFERED_STREAM_HEADER_INCLUDED
