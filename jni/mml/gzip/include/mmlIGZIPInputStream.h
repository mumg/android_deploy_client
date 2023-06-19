#ifndef MML_GZIP_INPUT_STREAM_HEADER_INCLUDED
#define MML_GZIP_INPUT_STREAM_HEADER_INCLUDED

#include "mmlIStream.h"

#define MML_GZIP_INPUT_STREAM_UUID { 0x7BB7F129, 0x6BF5, 0x1014, 0xB898 , { 0xFB, 0x9D, 0x9D, 0x29, 0x72, 0x08 } }

class mmlIGZIPInputStream : public mmlIInputStream
{
	MML_OBJECT_UUID_DECL(MML_GZIP_INPUT_STREAM_UUID)
};

#endif //MML_GZIP_INPUT_STREAM_HEADER_INCLUDED