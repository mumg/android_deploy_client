#include "mmlThread.h"
#include <signal.h>
#include <time.h>

#ifdef MML_ANDROID
#include <sys/syscall.h>
#endif

void log_core(const mmlChar * formatter , ... );

static void * mmlThreadProc(void *arg)
{
	MML_THREAD_CONTEXT * ctx = (MML_THREAD_CONTEXT *)arg;
	ctx->function->Execute(ctx->thread);
	pthread_exit(0);
	return mmlNULL;
}

MML_OBJECT_IMPL1(mmlThread, mmlIThread)

	mmlThread::mmlThread()
	:mExecutionTime(0), mCreated(mmlFalse) 
{
	mTotalExecutionTime.tv_sec = 0;
	mTotalExecutionTime.tv_nsec = 0;
}

mmlThread::~mmlThread()
{
	Join();
}

mmlBool mmlThread::Create ( mmlIThreadFunction * function,
	const mmlThreadPriority & priority,
	const mmlInt32 stack )
{
	clock_gettime(CLOCK_REALTIME, &mTotalExecutionTime);
	mCTX.function = function;
	mCTX.thread = this;
	if ( pthread_create(&mThread , NULL , mmlThreadProc , &mCTX ) == 0 )
	{
		mFunction = function;
		mCreated = mmlTrue;
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
	if ( mCreated == mmlTrue )
	{
		void *status = mmlNULL;
		pthread_join(mThread , &status);
		mCreated = mmlFalse;
	}
	mThread = mmlNULL;
	mFunction = mmlNULL;
	if ( mTotalExecutionTime.tv_sec != 0 && mTotalExecutionTime.tv_nsec != 0)
	{
		struct timespec t;
		clock_gettime(CLOCK_REALTIME, &t);
		mmlInt64 total_exec = (((t.tv_sec - mTotalExecutionTime.tv_sec) * 1000000000) + t.tv_nsec - mTotalExecutionTime.tv_nsec) / 1000;
		//log_core("Thread report: total %lld active %lld (%0.2f)\n" , total_exec , mExecutionTime , 100.0 * mExecutionTime / total_exec);
		mTotalExecutionTime.tv_sec = 0;
		mTotalExecutionTime.tv_nsec = 0;
	}
}

void mmlThread::SetPriority ( const mmlThreadPriority & priority )
{
}

void mmlThread::BeginExec ()
{
	clock_gettime(CLOCK_REALTIME, &mCounter);
}

void mmlThread::EndExec ()
{
	struct timespec t;
	clock_gettime(CLOCK_REALTIME, &t);
	mmlInt64 res = ((t.tv_sec - mCounter.tv_sec) * 1000000000) + t.tv_nsec - mCounter.tv_nsec;
	mExecutionTime += res/1000;
}

void mmlThread::Shutdown ()
{
	mFunction->Shutdown();
}

mml_core void mml_sleep ( const mmlInt32 timeout )
{
	timer_t timer1 = 0;
	do{

		struct itimerspec ts;
		struct sigaction action;
		struct sigevent sevent;
		sigset_t set;
		int signum = SIGALRM;
		int thread_id = syscall(__NR_gettid);
		//log_core("entering sleep for %d in thread %d\n", timeout, thread_id);

		sevent.sigev_notify = SIGEV_THREAD_ID;
		sevent.sigev_notify_thread_id = thread_id;
		sevent.sigev_signo = signum;

		sigemptyset(&set);
		sigaddset(&set, signum);
		sigprocmask(SIG_BLOCK, &set, NULL);

		if (timer_create (CLOCK_MONOTONIC, &sevent, &timer1) == -1)
		{
			log_core ("timer_create failed\n");
			break;
		}

		ts.it_value.tv_sec = timeout / 1000;
		ts.it_value.tv_nsec = mmlUInt64(timeout) % 1000000;
		ts.it_interval.tv_sec = 0;
		ts.it_interval.tv_nsec = 0;
        //log_core("timer_settime\n");
		//log_core(" ts.it_interval.tv_sec = %d\n", (int)ts.it_interval.tv_sec);
		//log_core(" ts.it_interval.tv_nsec = %d\n", (int)ts.it_interval.tv_nsec);
		//log_core(" ts.it_value.tv_sec = %d\n", (int)ts.it_value.tv_sec);
		//log_core(" ts.it_value.tv_nsec = %d\n", (int)ts.it_value.tv_nsec);
		if (timer_settime (timer1, 0, &ts, 0) == -1)
		{
			log_core ("timer_settime failed\n");
			break;
		}
		//log_core("sigwait\n");
		if (sigwait (&set, &signum) == -1)
		{
			log_core ("sigwait failed\n");
			break;
		}
		//log_core("leaving sleep for %d in thread %d\n", timeout, thread_id);
		timer_delete(timer1);
		return;
	}while(0);
	if ( timer1 != 0 )
	{
		timer_delete(timer1);
	}
	log_core("using usleep\n");
	usleep ( useconds_t(timeout) * 1000 );
}

mml_core mmlUInt64 mml_get_current_thread_id ()
{
	return pthread_self();
}

void mmlThread::Dispatch ()
{
#ifndef MML_ANDROID
	pthread_yield();
#endif
}