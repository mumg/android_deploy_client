#ifndef MML_THREAD_HEADER_INCLUDED
#define MML_THREAD_HEADER_INCLUDED

#include "mmlIThread.h"
#include "mmlPtr.h"
#include <Windows.h>

#undef Yield

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

	HANDLE mThread;

	mmlInt64 mExecutionTime;

	mmlInt64 mTotalExecutionTime;

	mmlInt64 mCounter;
	
	mmlAutoPtr < mmlIThreadFunction > mFunction;

	MML_THREAD_CONTEXT mCTX;

};


#endif //MML_THREAD_HEADER_INCLUDED

