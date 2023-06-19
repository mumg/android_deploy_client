#ifndef MML_CRC32_OUTPUT_STREAM_IMPL_HEADER_INCLUDED
#define MML_CRC32_OUTPUT_STREAM_IMPL_HEADER_INCLUDED

#include "mmlIStreamHash.h"
#include "mmlPtr.h"
#include "mml_crc32.h"


class mmlCRC32OutputStream : public mmlIOutputStreamHash
{
	MML_OBJECT_DECL
public:

	mmlCRC32OutputStream();

	~mmlCRC32OutputStream();

	mmlBool Construct ( mmlIOutputStream * stream );

	mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset );

	mmlInt64 Write ( const mmlInt64 size , const void * data );

	mmlBool Close ();

	mmlBool IsFull ();

	mmlBool GetHash( mmlIBuffer ** hash );

	mmlBool GetHash( mmlIVariant ** hash);

	mmlBool GetControl ( mmlIStreamControl ** control );

	mmlInt64 Size ();

	mmlBool Flush () { if ( mOutput == mmlNULL ) return mmlTrue; return mOutput->Flush(); }

private:

	mmlAutoPtr < mmlIOutputStream > mOutput;

	mml_crc32 mCRC32;

};


#endif //MML_CRC32_OUTPUT_STREAM_IMPL_HEADER_INCLUDED
