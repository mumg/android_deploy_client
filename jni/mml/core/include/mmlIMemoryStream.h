#ifndef MML_MEMORY_STREAM_HEADER_INCLUDED
#define MML_MEMORY_STREAM_HEADER_INCLUDED

#include "mmlIStream.h"



/*
  Available constructors:
  1. Attach data buffer
	arg0 - mmlIBuffer
  2. Attach string
	arg0 - mmlICString
*/

#define MML_MEMORY_INPUT_STREAM_UUID { 0xD127756C, 0x6CA1, 0x1014, 0x9180 , { 0xB6, 0x06, 0xF7, 0x06, 0x94, 0x83 } }

class mmlIMemoryInputStream : public mmlIInputStream
{
    MML_OBJECT_UUID_DECL(MML_MEMORY_INPUT_STREAM_UUID)
};

/*
  Available constructors:
  1. Dynamic stream

  2. Attach data buffer
    arg0 - mmlIBuffer
  3. Attach string
    arg0 - mmlICString

*/

#define MML_MEMORY_OUTPUT_STREAM_UUID { 0xB97B32E6, 0x6C11, 0x1014, 0x96C5 , { 0xD5, 0x42, 0x07, 0x2C, 0xDB, 0x7E } }

class mmlIBuffer;

class mmlIMemoryOutputStream : public mmlIOutputStream
{
	MML_OBJECT_UUID_DECL(MML_MEMORY_OUTPUT_STREAM_UUID)
public:
	virtual mmlBool GetData ( mmlIBuffer ** data ) = 0;
};


/*
  Available constructors:
  1. Dynamic stream

*/

#define MML_MEMORY_OUTPUT_CACHED_STREAM_UUID { 0x2B343605, 0x6FEF, 0x1014, 0x822E , { 0x89, 0x90, 0x8F, 0x51, 0x4D, 0x51 } }

class mmlIMemoryOutputCachedStream : public mmlIOutputStream
{
	MML_OBJECT_UUID_DECL(MML_MEMORY_OUTPUT_CACHED_STREAM_UUID)
public:

};

#endif //MML_MEMORY_STREAM_HEADER_INCLUDED




