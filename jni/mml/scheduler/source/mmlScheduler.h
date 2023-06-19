#ifndef MML_SCHEDULER_IMPL_HEADER_INCLUDED
#define MML_SCHEDULER_IMPL_HEADER_INCLUDED

#include "mmlIScheduler.h"
#include "mmlIThread.h"
#include "mmlIMutex.h"
#include "mmlICondition.h"
#include "mmlIVariant.h"
#include "mmlPtr.h"
#include <map>
#include <list>

class mmlSchedulerTaskHolder : public mmlIObject
{
	MML_OBJECT_DECL
public:

	mmlSchedulerTaskHolder(mmlIVariantStruct * config , mmlISchedulerTask * task, mmlISchedulerRule * rule);

	typedef enum 
	{
		TS_TASK_IDLE,
		TS_TASK_BUSY,
		TS_TASK_SUSPENDED,
		TS_TASK_NEED_RUN,
		TS_TASK_FINISHED
	}TASK_STATUS_T;

	mmlBool Run ();

	mmlBool Suspend ();

	mmlBool Resume ();

	TASK_STATUS_T GetStatus ( const mml_timespec & ts );

private:

	mmlBool mBusy;

	mmlBool mFinished;

	mmlBool mSuspended;

	mml_timespec mFireTime;

	mmlAutoPtr < mmlISchedulerTask > mTask;

	mmlAutoPtr < mmlISchedulerRule > mRule;

	mmlAutoPtr < mmlIVariantStruct > mConfig;

};


class mmlSchedulerThread : public mmlIThreadFunction
{
	MML_OBJECT_DECL
public:

	mmlSchedulerThread();

	~mmlSchedulerThread();

	void Execute (mmlIThread * thread);

	void Shutdown ();

	mmlBool Post ( mmlSchedulerTaskHolder * task );

	void Join ();

private:

	mmlBool mShutdown;

	mmlSharedPtr < mmlIMutex > mMutex;

	mmlSharedPtr < mmlICondition > mConfition;

	mmlAutoPtr < mmlSchedulerTaskHolder > mTask;

	mmlSharedPtr < mmlIThread > mThread;

};

#define MML_SCHEDULER_IMPL_UUID { 0x7D8E7BCB, 0x6C74, 0x1014, 0xB392 , { 0xF8, 0x40, 0x41, 0x64, 0xD1, 0xC1 } }

class mmlScheduler : public mmlIScheduler
{
	MML_OBJECT_DECL
	MML_OBJECT_UUID_DECL(MML_SCHEDULER_IMPL_UUID)
public:

	mmlScheduler();

	~mmlScheduler();

	mmlBool Add (mmlIVariantStruct * config , mmlISchedulerTask * task , mmlISchedulerRule * rule, mmlInt32 * handle);

	mmlBool Suspend ( const mmlInt32 task_hdl );

	mmlBool Resume ( const mmlInt32 task_hdl );

	mmlBool Remove ( const mmlInt32 task_hdl );

	mmlBool Run ( const mmlInt32 task_hdl );

	void Execute ( mmlIThread * thread );

	void Shutdown ();

	void SetMaxSize (const mmlInt32 max)
	{
		mMaxPoolSize = max;
	}

	mmlBool Start ();



private:

	void SvcStop ();

	void SvcWaitForStop ();

	mmlSharedPtr < mmlIMutex > mMutex;

	std::map < mmlInt32 , mmlAutoPtr < mmlSchedulerTaskHolder > > mTasks;

	std::list < mmlAutoPtr < mmlSchedulerThread > > mThreadPool;

	mmlInt32 mNextHandle;

	mmlSharedPtr < mmlIThread > mThread;

	mmlBool mShutdown;

	mmlInt32 mMaxPoolSize;

};


#endif //MML_SCHEDULER_IMPL_HEADER_INCLUDED
