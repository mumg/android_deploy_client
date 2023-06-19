#include "mmlMD5OutputStream.h"
#include "mmlMD5InputStream.h"
#include "mmlICString.h"
#include "mml_components.h"
#include "mml_md5_uuids.h"

MML_CONSTRUCTOR_IMPL(mmlMD5OutputStream);
MML_CONSTRUCTOR_IMPL1(mmlMD5OutputStream, mmlIOutputStream);
MML_CONSTRUCTOR_IMPL1(mmlMD5InputStream, mmlIInputStream);

static mmlObjectFactory mml_md5_component [] =
{
	{
		MML_MD5_OUTPUT_STREAM_UUID(),
		mmlNULL,
		MML_CONSTRUCTOR(mmlMD5OutputStream)
	},
	{
		MML_MD5_OUTPUT_STREAM_UUID(),
		mmlNULL,
		MML_CONSTRUCTOR1(mmlMD5OutputStream, mmlIOutputStream)
	},
	{
		MML_MD5_INPUT_STREAM_UUID(),
		mmlNULL,
		MML_CONSTRUCTOR1(mmlMD5InputStream, mmlIInputStream)
	}
};

MML_COMPONENT_IMPL(md5,mml_md5_component) 
