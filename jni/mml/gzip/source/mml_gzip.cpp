#include "mmlGZIPOutputStream.h"
#include "mmlGZIPInputStream.h"
#include "mmlICString.h"
#include "mml_components.h"

MML_CONSTRUCTOR_IMPL1(mmlGZIPOutputStream, mmlIOutputStream);
MML_CONSTRUCTOR_IMPL1(mmlGZIPInputStream, mmlIInputStream);

static mmlObjectFactory mml_gzip_component [] =
{
	{
		MML_GZIP_OUTPUT_STREAM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlGZIPOutputStream, mmlIOutputStream)
	},
	{
		MML_GZIP_INPUT_STREAM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlGZIPInputStream, mmlIInputStream)
	}
};

MML_COMPONENT_IMPL(gzip,mml_gzip_component) 
