#ifndef MML_THREAD_HEADER_INCLUDED
#define MML_THREAD_HEADER_INCLUDED

#include "mmlIThread.h"
#include "mmlPtr.h"
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>


typedef struct  
{
	mmlIThreadFunction * function;
	mmlIThread * thread;
}MML_THREAD_CONTEXT;

class mmlThread : public mmlIThread
{
	MML_OBJECT_DECL
public:

	mmlThread();

	~mmlThread();
	
	mmlBool Create ( mmlIThreadFunction * function,
		             const mmlThreadPriority & priority,
		             const mmlInt32 stack );

	void Join ();

	void SetPriority ( const mmlThreadPriority & priority );

	void BeginExec ();

	void EndExec ();

	void Shutdown();

	void Dispatch ();

private:

	pthread_t mThread;
	
	mmlInt64 mExecutionTime;

	struct timespec mTotalExecutionTime;

	struct timespec mCounter;
	
	mmlAutoPtr < mmlIThreadFunction > mFunction;

	MML_THREAD_CONTEXT mCTX;
	
	mmlBool mCreated;

};


#endif //MML_THREAD_HEADER_INCLUDED

