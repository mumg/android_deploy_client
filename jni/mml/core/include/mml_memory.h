#ifndef MML_MEMORY_HEADER_INCLUDED
#define MML_MEMORY_HEADER_INCLUDED

#include "mml_core.h"


mml_core void * mmlAlloc ( mmlInt32 size );

mml_core void * mmlRealloc ( void * mem , mmlInt32 size );

mml_core void mmlFree ( void * mem );

mml_core void mmlMemorySet ( void * mem , const mmlUInt8 val , const mmlInt32 size );

mml_core void mmlMemoryCopy ( void * dst , const void * src , const mmlInt32 size );

mml_core void mmlMemoryMove ( void * dst , const void * src , const mmlInt32 size );

mml_core mmlBool mmlMemoryCmp ( const void * src1, const void * src2, const mmlInt32 size );

#endif //MML_MEMORY_HEADER_INCLUDED

