#ifndef MML_CONDITION_HEADER_INCLUDED
#define MML_CONDITION_HEADER_INCLUDED

#include "mmlICondition.h"
#include <pthread.h>

class mmlCondition : public mmlICondition
{
	MML_OBJECT_DECL;
public:
	mmlCondition();

	~mmlCondition();

	mmlBool Wait ( const mmlInt32 timeout );

	mmlBool Signal ();

private:
	pthread_cond_t mHandle;
	pthread_mutex_t mMutex;
	mmlInt32 mSignalCounter;
};

#endif //MML_CONDITION_HEADER_INCLUDED

