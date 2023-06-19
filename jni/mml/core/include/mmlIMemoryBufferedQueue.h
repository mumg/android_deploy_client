#ifndef MML_MEMORY_BUFFERED_QUEUE_HEADER_INCLUDED
#define MML_MEMORY_BUFFERED_QUEUE_HEADER_INCLUDED

#include "mmlIStream.h"
#include "mmlILogger.h"

#define MML_INPUT_MEMORY_BUFFERED_QUEUE_READER_UUID  { 0xF127756C, 0x6CB2, 0x1014, 0x9180 , { 0xB6, 0x06, 0xF7, 0x06, 0x94, 0x83 } }

class mmlIInputMemoryBufferedQueueReader : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_INPUT_MEMORY_BUFFERED_QUEUE_READER_UUID)
public:
	virtual mmlBool Read(void * data, const mmlInt32 data_size, mmlInt32 * processed) = 0;
	virtual mmlBool Close(void * data, const mmlInt32 data_size) = 0;
};

#define MML_INPUT_MEMORY_BUFFERED_QUEUE_UUID  { 0xF188756C, 0x6CB2, 0x1014, 0x9180 , { 0xB6, 0x06, 0xF7, 0x06, 0x94, 0x83 } }

/*
Available constructors:
1. arg0 - mmlIInputMemoryBufferedQueueReader
*/
class mmlIInputMemoryBufferedQueue : public mmlIOutputStream
{
	MML_OBJECT_UUID_DECL(MML_INPUT_MEMORY_BUFFERED_QUEUE_UUID)
public:
	virtual void setDump(const mmlChar * prefix, _logger_func func) = 0;
};

#define MML_OUTPUT_MEMORY_BUFFERED_QUEUE_UUID  { 0x8127757C, 0x6CB2, 0x1014, 0x9180 , { 0xB6, 0x06, 0xF7, 0x06, 0x94, 0x83 } }

class mmlIOutputMemoryBufferedQueue : public mmlIInputStream
{
	MML_OBJECT_UUID_DECL(MML_OUTPUT_MEMORY_BUFFERED_QUEUE_UUID)
public:
	virtual mmlUInt64 Write(const mmlUInt64 size, const void * data) = 0;
	virtual void setDump(const mmlChar * prefix, _logger_func func) = 0;
};

#define MML_OUTPUT_MEMORY_BUFFERED_QUEUE_STREAM_UUID { 0x0709DF75, 0x6C98, 0x1014, 0x9D5D , { 0xD0, 0x52, 0x2C, 0xB5, 0xEF, 0xEB } }

class mmlIOutputMemoryBufferedQueueStream : public mmlIOutputStream
{
	MML_OBJECT_UUID_DECL(MML_OUTPUT_MEMORY_BUFFERED_QUEUE_STREAM_UUID)
};

#endif //MML_MEMORY_BUFFERED_QUEUE_HEADER_INCLUDED