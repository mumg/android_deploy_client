#include "mml_atomic.h"
#include <pthread.h>

pthread_mutex_t mml_atomic_mutex;
mmlBool mml_atomic_initialized = mmlFalse;

void mmlAtomicInit ()
{
	if ( mml_atomic_initialized == mmlFalse )
	{
		pthread_mutex_init(&mml_atomic_mutex, NULL); 
		mml_atomic_initialized = mmlTrue;
	}
}

void mmlAtomicDestroy ()
{
	if ( mml_atomic_initialized == mmlTrue )
	{
		pthread_mutex_destroy(&mml_atomic_mutex);
		mml_atomic_initialized = mmlFalse;
	}
}

mml_core void * mmlAtomicSet ( void ** ptr , void * value )
{
	void * result = mmlNULL;
	pthread_mutex_lock(&mml_atomic_mutex);
	result = *ptr;
	*ptr = value;
	pthread_mutex_unlock(&mml_atomic_mutex);
	return result;
}

mml_core void * mmlAtomicGetPtr ( void ** ptr )
{
	void * result = mmlNULL;
	pthread_mutex_lock(&mml_atomic_mutex);
	result = *ptr;
	pthread_mutex_unlock(&mml_atomic_mutex);
	return result;
}

mml_core mmlInt32 mmlAtomicAdd ( mmlInt32 * ptr , mmlInt32 value )
{
	mmlInt32 result = 0;
	pthread_mutex_lock(&mml_atomic_mutex);
	*ptr += value;
	result = *ptr;
	pthread_mutex_unlock(&mml_atomic_mutex);
	return result;
}

mml_core mmlInt32 mmlAtomicGet ( mmlInt32 * ptr )
{
	mmlInt32 result = 0;
	pthread_mutex_lock(&mml_atomic_mutex);
	result = *ptr;
	pthread_mutex_unlock(&mml_atomic_mutex);
	return result;
}

