#ifndef MML_MUTEX_HEADER_INCLUDED
#define MML_MUTEX_HEADER_INCLUDED

#include "mmlIObject.h"
#include "mmlIMutex.h"
#include <pthread.h>


class mmlMutex : public mmlIMutex
{
	MML_OBJECT_DECL
public:

	mmlMutex ();

	~mmlMutex();

	mmlBool Lock ();

	mmlBool UnLock ();

private:

	pthread_mutex_t  mMutex;
};

#endif //MML_MUTEX_HEADER_INCLUDED

