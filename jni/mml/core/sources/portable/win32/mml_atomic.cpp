#include "mml_atomic.h"
#include <Windows.h>

CRITICAL_SECTION mml_atomic_mutex;
mmlBool mml_atomic_initialized = mmlFalse;

void mmlAtomicInit ()
{
	if ( mml_atomic_initialized == mmlFalse )
	{
		InitializeCriticalSection(&mml_atomic_mutex);
		mml_atomic_initialized = mmlTrue;
	}
}

void mmlAtomicDestroy ()
{
	if ( mml_atomic_initialized == mmlTrue )
	{
		DeleteCriticalSection(&mml_atomic_mutex);
		mml_atomic_initialized = mmlFalse;
	}
}

mml_core void * mmlAtomicSet ( void ** ptr , void * value )
{
	void * result = mmlNULL;
	EnterCriticalSection(&mml_atomic_mutex);
	result = *ptr;
	*ptr = value;
	LeaveCriticalSection(&mml_atomic_mutex);
	return result;
}

mml_core void * mmlAtomicGetPtr ( void ** ptr )
{
	void * result = mmlNULL;
	EnterCriticalSection(&mml_atomic_mutex);
	result = *ptr;
	LeaveCriticalSection(&mml_atomic_mutex);
	return result;
}

mml_core mmlInt32 mmlAtomicAdd ( mmlInt32 * ptr , mmlInt32 value )
{
	mmlInt32 result = 0;
	EnterCriticalSection(&mml_atomic_mutex);
	*ptr += value;
	result = *ptr;
	LeaveCriticalSection(&mml_atomic_mutex);
	return result;
}

mml_core mmlInt32 mmlAtomicGet ( mmlInt32 * ptr )
{
	mmlInt32 result = 0;
	EnterCriticalSection(&mml_atomic_mutex);
	result = *ptr;
	LeaveCriticalSection(&mml_atomic_mutex);
	return result;
}
