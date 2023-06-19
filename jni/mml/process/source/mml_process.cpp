#include "mml_components.h"
#include "mmlProcess.h"


MML_CONSTRUCTOR_IMPL(mmlProcess);
#ifndef MML_WIN
MML_CONSTRUCTOR_IMPL(mmlProcessPty);
#endif
static mmlObjectFactory process_component [] =
{
	{
		MML_PROCESS_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlProcess)
	},
	{
		MML_PROCESS_PTY_UUID,
		mmlNULL,
#ifdef MML_WIN
		MML_CONSTRUCTOR(mmlProcess)
#else
		MML_CONSTRUCTOR(mmlProcessPty)
#endif
	}
};

MML_COMPONENT_IMPL(process,process_component)