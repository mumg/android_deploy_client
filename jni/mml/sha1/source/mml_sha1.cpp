#include "mmlSHA1OutputStream.h"
#include "mmlSHA1InputStream.h"
#include "mmlICString.h"
#include "mml_components.h"
#include "mml_sha1_uuids.h"

MML_CONSTRUCTOR_IMPL(mmlSHA1OutputStream);
MML_CONSTRUCTOR_IMPL1(mmlSHA1OutputStream, mmlIOutputStream);
MML_CONSTRUCTOR_IMPL1(mmlSHA1InputStream, mmlIInputStream);

static mmlObjectFactory mml_SHA1_component [] =
{
	{
		MML_SHA1_OUTPUT_STREAM_UUID(),
		mmlNULL,
		MML_CONSTRUCTOR(mmlSHA1OutputStream)
	},
	{
		MML_SHA1_OUTPUT_STREAM_UUID(),
		mmlNULL,
		MML_CONSTRUCTOR1(mmlSHA1OutputStream, mmlIOutputStream)
	},
	{
		MML_SHA1_INPUT_STREAM_UUID(),
		mmlNULL,
		MML_CONSTRUCTOR1(mmlSHA1InputStream, mmlIInputStream)
	}
};

MML_COMPONENT_IMPL(sha1,mml_SHA1_component) 
