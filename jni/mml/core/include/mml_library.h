#ifndef MML_LIBRARY_HEADER_INCLUDED
#define MML_LIBRARY_HEADER_INCLUDED

#include "mml_core.h"
#include "mml_components.h"

class mmlILogger;

typedef mmlBool (* mml_components_reg_ext) ( mmlILogger * logger, void * args);
typedef void (*mml_components_unreg_ext)();

mml_core mmlBool mmlInitialize(mmlChar ** argv,
							   const mmlInt32 argc,
							   mml_components_reg_ext reg,
							   mml_components_unreg_ext unreg,
							   mml_component_constructor main_ctor,
                               mml_component_destructor main_dctor,
                               void * args);

mml_core void mmlDestroy ();

#endif //MML_LIBRARY_HEADER_INCLUDED



