#ifndef MML_STREAM_HASH_HEADER_INCLUDED
#define MML_STREAM_HASH_HEADER_INCLUDED

#include "mmlIStream.h"
#include "mmlIVariant.h"

#define  MML_INPUT_STREAM_HASH_UUID { 0xA7E34809, 0x7001, 0x1014, 0x9FBD , { 0xB3, 0x40, 0x13, 0x6D, 0xF6, 0x4F } }

class mmlIInputStreamHash : public mmlIInputStream
{
public:
	virtual mmlBool GetHash( mmlIVariant ** hash ) = 0;
	virtual mmlBool GetHash( mmlIBuffer ** hash ) = 0;
};

#define MML_OUTPUT_STREAM_HASH_UUID { 0xAB41CB80, 0x7001, 0x1014, 0x9FE1 , { 0xEF, 0x0B, 0x63, 0x65, 0x19, 0xC6 } }

class mmlIOutputStreamHash : public mmlIOutputStream
{
public:
	virtual mmlBool GetHash( mmlIVariant ** hash ) = 0;
	virtual mmlBool GetHash( mmlIBuffer ** hash ) = 0;
};

#endif //MML_STREAM_HASH_HEADER_INCLUDED
