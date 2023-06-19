#ifndef MML_ATOMIC_HEADER_INCLUDED
#define MML_ATOMIC_HEADER_INCLUDED

#include "mml_core.h"

mml_core void * mmlAtomicSet ( void ** ptr , void * value );
mml_core void * mmlAtomicGetPtr ( void ** ptr );
mml_core mmlInt32 mmlAtomicAdd ( mmlInt32 * ptr , mmlInt32 value );
mml_core mmlInt32 mmlAtomicGet ( mmlInt32 * ptr );

#endif //MML_ATOMIC_HEADER_INCLUDED

