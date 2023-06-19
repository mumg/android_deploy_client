#include "mmlThread.h"



static DWORD WINAPI mmlThreadProc(void *arg)
{
	MML_THREAD_CONTEXT * ctx = (MML_THREAD_CONTEXT *)arg;
	ctx->function->Execute(ctx->thread);
	return 0;
}

void log_core(const mmlChar * formatter , ... );

MML_OBJECT_IMPL1(mmlThread, mmlIThread)

mmlThread::mmlThread()
:mThread(INVALID_HANDLE_VALUE), mTotalExecutionTime(0) , mExecutionTime(0) , mCounter(0)
{
}

mmlThread::~mmlThread()
{
	Join();
}

mmlBool mmlThread::Create ( mmlIThreadFunction * function,
				            const mmlThreadPriority & priority,
				            const mmlInt32 stack )
{
	DWORD thread_id;
	QueryPerformanceCounter((LARGE_INTEGER*)&mTotalExecutionTime);
	mCTX.function = function;
	mCTX.thread = this;
	mThread = ::CreateThread(mmlNULL , stack , mmlThreadProc , &mCTX , 0 , &thread_id  );
	if ( mThread != INVALID_HANDLE_VALUE )
	{
		mFunction = function;
		SetPriority(priority);
		return mmlTrue;
	}
	return mmlFalse;
}

void mmlThread::Join ()
{
	if ( mFunction != mmlNULL )
	{
		mFunction->Shutdown();
	}
	mmlBool resut = mmlFalse;
	if ( mThread != INVALID_HANDLE_VALUE)
	{
		 ::WaitForSingleObject(mThread , INFINITE);
		 CloseHandle(mThread);
	}
	mThread = INVALID_HANDLE_VALUE;
	mFunction = mmlNULL;
	if ( mTotalExecutionTime != 0 )
	{
		LARGE_INTEGER pc0 = *((LARGE_INTEGER*)&mTotalExecutionTime), pc1;
		QueryPerformanceCounter(&pc1);
		pc1.QuadPart-=pc0.QuadPart;
		QueryPerformanceFrequency( &pc0 );
		mmlInt64 execution_time = 1000000.0 * ((double)pc1.QuadPart) / ((double)pc0.QuadPart);
		//log_core("Thread report: total %lld active %lld (%0.2f)\n" , execution_time , mExecutionTime , 100.0 * mExecutionTime / execution_time);
		mTotalExecutionTime = 0;
	}
}

void mmlThread::SetPriority ( const mmlThreadPriority & priority )
{
	if ( mThread != INVALID_HANDLE_VALUE )
	{
		WORD os_priority = THREAD_PRIORITY_NORMAL;
		if ( priority == PriorityLowest )
			os_priority = THREAD_PRIORITY_LOWEST;
		else if ( priority == PriorityLow )
			os_priority = THREAD_PRIORITY_BELOW_NORMAL;
		else if ( priority == PriorityHigh )
			os_priority = THREAD_PRIORITY_ABOVE_NORMAL;
		else if ( priority == PriorityHighest )
			os_priority = THREAD_PRIORITY_HIGHEST;
		::SetThreadPriority(mThread , os_priority);
	}
}

void mmlThread::BeginExec ()
{
	QueryPerformanceCounter((LARGE_INTEGER*)&mCounter);
}

void mmlThread::EndExec ()
{
	LARGE_INTEGER pc0 = *((LARGE_INTEGER*)&mCounter), pc1;
	QueryPerformanceCounter(&pc1);
	pc1.QuadPart-=pc0.QuadPart;
	if( QueryPerformanceFrequency( &pc0 ) ) 
	{
		mExecutionTime += 1000000.0 * ((double)pc1.QuadPart) / ((double)pc0.QuadPart);
	}
}

void mmlThread::Shutdown ()
{
	mFunction->Shutdown();
}

mml_core void mml_sleep ( const mmlInt32 timeout )
{
	Sleep(timeout);
}

mml_core mmlUInt64 mml_get_current_thread_id ()
{
	return (mmlUInt64)GetCurrentThreadId();
}

#undef Yield

void mmlThread::Dispatch ()
{
	SwitchToThread();
}