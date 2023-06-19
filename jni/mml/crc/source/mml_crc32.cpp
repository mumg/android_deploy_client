#include "mmlCRC32OutputStream.h"
#include "mmlCRC32InputStream.h"
#include "mmlICString.h"
#include "mml_components.h"
#include "mml_crc32_uuids.h"

MML_CONSTRUCTOR_IMPL(mmlCRC32OutputStream);
MML_CONSTRUCTOR_IMPL1(mmlCRC32OutputStream, mmlIOutputStream);
MML_CONSTRUCTOR_IMPL1(mmlCRC32InputStream, mmlIInputStream);

static mmlObjectFactory mml_crc32_component [] =
{
	{
		MML_CRC32_OUTPUT_STREAM_UUID(),
		mmlNULL,
		MML_CONSTRUCTOR(mmlCRC32OutputStream)
	},
	{
		MML_CRC32_OUTPUT_STREAM_UUID(),
		mmlNULL,
		MML_CONSTRUCTOR1(mmlCRC32OutputStream, mmlIOutputStream)
	},
	{
		MML_CRC32_INPUT_STREAM_UUID(),
		mmlNULL,
		MML_CONSTRUCTOR1(mmlCRC32InputStream, mmlIInputStream)
	}
};

MML_COMPONENT_IMPL(crc,mml_crc32_component) 
