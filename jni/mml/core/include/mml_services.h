#ifndef MML_SERVICES_HEADER_INCLUDED
#define MML_SERVICES_HEADER_INCLUDED

#include "mml_core.h"
#include "mmlIObject.h"

mml_core mmlIObject * mmlGetService ( const mmlUUID & uuid );

mml_core void mmlSetService ( const mmlUUID & uuid , mmlIObject * service );

#endif //MML_SERVICES_HEADER_INCLUDED

