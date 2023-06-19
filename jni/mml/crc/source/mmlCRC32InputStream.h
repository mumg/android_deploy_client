#ifndef MML_CRC32_INPUT_STREAM_IMPL_HEADER_INCLUDED
#define MML_CRC32_INPUT_STREAM_IMPL_HEADER_INCLUDED

#include "mmlIStreamHash.h"
#include "mmlPtr.h"
#include "mml_crc32.h"


class mmlCRC32InputStream : public mmlIInputStreamHash
{
	MML_OBJECT_DECL
public:

	mmlCRC32InputStream();

	~mmlCRC32InputStream();

	mmlBool Construct ( mmlIInputStream * stream );

    mmlInt64 Size ();

    mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset );

    mmlInt64 Read ( const mmlInt64 size , void * data );

    mmlBool Close ();

	mmlBool IsEmpty();

	mmlBool GetHash( mmlIBuffer ** hash );

	mmlBool GetHash( mmlIVariant ** hash);

	mmlBool GetControl ( mmlIStreamControl ** control );

private:

	mmlAutoPtr < mmlIInputStream > mInput;

	mml_crc32 mCRC32;
};


#endif //MML_CRC32_INPUT_STREAM_IMPL_HEADER_INCLUDED
