#include "mml_memory.h"
#include <stdlib.h>
#include <string.h>
#include <memory.h>

mml_core void * mmlAlloc ( mmlInt32 size )
{
	void * ptr = malloc(size);
	memset(ptr , 0 , size);
	return ptr;
}

mml_core void * mmlRealloc ( void * mem , mmlInt32 size )
{
	return realloc(mem , size);
}

mml_core void mmlFree ( void * mem )
{
	free(mem);
}

mml_core void mmlMemorySet ( void * mem , const mmlUInt8 val , const mmlInt32 size )
{
	memset(mem , val , size );
}

mml_core void mmlMemoryCopy ( void * dst , const void * src , const mmlInt32 size )
{
	memcpy(dst , src , size);
}

mml_core void mmlMemoryMove ( void * dst , const void * src , const mmlInt32 size )
{
	memmove(dst , src , size);
}

mml_core mmlBool mmlMemoryCmp ( const void * src1, const void * src2, const mmlInt32 size )
{
	return memcmp(src1, src2, size) == 0 ? mmlTrue : mmlFalse;
}