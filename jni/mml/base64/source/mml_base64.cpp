#include "mmlBase64OutputStream.h"
#include "mmlBase64InputStream.h"
#include "mmlICString.h"
#include "mml_components.h"
#include "mml_base64_uuids.h"

MML_CONSTRUCTOR_IMPL1(mmlBase64OutputStream, mmlIOutputStream);
MML_CONSTRUCTOR_IMPL1(mmlBase64InputStream, mmlIInputStream);

static mmlObjectFactory mml_BASE64_component [] =
{
	{
		MML_BASE64_OUTPUT_STREAM_UUID(),
		mmlNULL,
		MML_CONSTRUCTOR1(mmlBase64OutputStream, mmlIOutputStream)
	},
	{
		MML_BASE64_INPUT_STREAM_UUID(),
		mmlNULL,
		MML_CONSTRUCTOR1(mmlBase64InputStream, mmlIInputStream)
	}
};

MML_COMPONENT_IMPL(base64,mml_BASE64_component) 
