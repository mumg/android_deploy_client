#include "mmlScheduler.h"
#include "mml_components.h"
#include "mmlISleep.h"
#include "mml_services.h"
#include "mmlICString.h"
#include "mmlPtr.h"
#include "mmlIVariant.h"

void log_scheduler( const mmlChar * formatter , ... );
	

class mmlPeriodicSchedulerRule : public mmlISchedulerRule
{
	MML_OBJECT_DECL
public:

	mmlPeriodicSchedulerRule()
		:mSuccessTimeout(0), mFailTimeout(0)
	{

	}

	mmlBool Configure ( mmlIVariantStruct * config )
	{
		mmlAutoPtr < mmlIVariantInteger > success_timeout;
		mmlAutoPtr < mmlIVariantInteger > fail_timeout;
		if ( config->Get("success", mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant >(success_timeout)) == mmlFalse ||
			 config->Get("fail", mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant >(fail_timeout)) == mmlFalse )
		{
			return mmlFalse;
		}
		mSuccessTimeout = success_timeout->Get();
		mFailTimeout = fail_timeout->Get();
		return mmlTrue;
	}

	void GetNextFireTime (const TASK_RESULT_T result, const mml_timespec & current_time , mml_timespec * next_time)
	{
		*next_time = current_time;
		if ( result == TASK_SUCCESS || result == TASK_FIRST_START)
		{
			mml_clock_add(next_time, mSuccessTimeout * 1000);
		}
		else
		{
			mml_clock_add(next_time, mFailTimeout * 1000);
		}
	}

private:

	mmlInt32 mSuccessTimeout;
	mmlInt32 mFailTimeout;


};

MML_OBJECT_IMPL1(mmlPeriodicSchedulerRule, mmlISchedulerRule)

mmlSchedulerTaskHolder::mmlSchedulerTaskHolder(mmlIVariantStruct * config, mmlISchedulerTask * task, mmlISchedulerRule * rule)
:mTask(task), mBusy(mmlFalse), mFinished(mmlFalse), mRule(rule), mConfig(config)
{
	mml_timespec ts;
	mml_clock_gettime_monotonic(&ts);
	mRule->GetNextFireTime(mmlISchedulerRule::TASK_FIRST_START, ts, &mFireTime);
}
mmlSchedulerTaskHolder::TASK_STATUS_T mmlSchedulerTaskHolder::GetStatus ( const mml_timespec & ts )
{
	if ( mBusy == mmlTrue )
	{
		return TS_TASK_BUSY;
	}
	if ( mSuspended == mmlTrue )
	{
		return TS_TASK_SUSPENDED;
	}
	if ( mml_clock_cmp((mml_timespec*)&mFireTime, (mml_timespec*)&ts)<= 0)
	{
		return TS_TASK_NEED_RUN;
	}
	return TS_TASK_IDLE;
}

mmlBool mmlSchedulerTaskHolder::Run ()
{
	mmlBool result = mmlFalse;
	mBusy = mmlTrue;

	mml_timespec tm;
	mml_clock_gettime_monotonic(&tm);

	mmlISchedulerRule::TASK_RESULT_T task_res = mTask->Run(mConfig);
	if ( task_res == mmlISchedulerRule::TASK_FINISHED )
	{
			mFinished = mmlTrue;
			result = mmlTrue;
	}
	else if ( task_res == mmlISchedulerRule::TASK_SUCCESS || task_res == mmlISchedulerRule::TASK_FAILED )
	{
		mRule->GetNextFireTime(task_res, tm, &mFireTime);
		mml_tm ct;
		mml_tm ft;
		mml_clock_localtime(&tm, &ct);
		mml_clock_localtime(&mFireTime, &ft);
		//log_scheduler("Current time   is %d.%d.%d %d:%d:%d\n",ct.tm_year, ct.tm_mon, ct.tm_mday, ct.tm_hour, ct.tm_min, ct.tm_sec);
		//log_scheduler("Next fire time is %d.%d.%d %d:%d:%d\n",ft.tm_year, ft.tm_mon, ft.tm_mday, ft.tm_hour, ft.tm_min, ft.tm_sec);
		result = mmlTrue;
	}
	mBusy = mmlFalse;
	return result;
}

mmlBool mmlSchedulerTaskHolder::Suspend ()
{
	mSuspended = mmlTrue;
	return mmlTrue;
}

mmlBool mmlSchedulerTaskHolder::Resume ()
{
	mSuspended = mmlFalse;
	return mmlTrue;
}

MML_OBJECT_IMPL0(mmlSchedulerTaskHolder)

mmlSchedulerThread::mmlSchedulerThread()
:mShutdown(mmlFalse)
{
	mConfition = mmlNewObject(MML_OBJECT_UUID(mmlICondition));
	mMutex = mmlNewObject(MML_OBJECT_UUID(mmlIMutex));
	mThread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
	mThread->Create(new mmlThreadFunctionProxy < mmlSchedulerThread >(this) , PriorityNormal, MML_DEFAULT_STACK_SIZE);
}

mmlSchedulerThread::~mmlSchedulerThread()
{
}

void mmlSchedulerThread::Join ()
{
	mThread->Join();
}

void mmlSchedulerThread::Execute (mmlIThread * thread)
{
	while ( mShutdown != mmlTrue )
	{
		mConfition->Wait(1000);
		mmlMutexGuard g(mMutex);
		if ( mTask != mmlNULL )
		{
			g.UnLock();
			mTask->Run();
			g.Lock();
			mTask = mmlNULL;
		}
	}
}

void mmlSchedulerThread::Shutdown ()
{
	mShutdown = mmlTrue;
	mConfition->Signal();
}

mmlBool mmlSchedulerThread::Post ( mmlSchedulerTaskHolder * task )
{
	mmlMutexGuard g(mMutex);
	if ( mTask == mmlNULL )
	{
		mTask = task;
		mConfition->Signal();
		return mmlTrue;
	}
	return mmlFalse;
}

MML_OBJECT_IMPL0(mmlSchedulerThread)

mmlScheduler::mmlScheduler()
:mNextHandle(0),mShutdown(mmlFalse), mMaxPoolSize(0)
{
	mMutex = mmlNewObject(MML_OBJECT_UUID(mmlIMutex));
	mThread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
}

mmlBool mmlScheduler::Start()
{
	return mThread->Create(new mmlThreadFunctionProxy<mmlScheduler>(this) , PriorityNormal, MML_DEFAULT_STACK_SIZE);
}

mmlScheduler::~mmlScheduler()
{
	mThread->Join();
}

mmlBool mmlScheduler::Add (mmlIVariantStruct * config ,mmlISchedulerTask * task , mmlISchedulerRule * rule, mmlInt32 * handle)
{
	mmlMutexGuard g(mMutex);
	mmlInt32 new_handle = ++mNextHandle;
	mTasks[new_handle] = new mmlSchedulerTaskHolder(config , task, rule);
	if ( handle != mmlNULL) *handle = new_handle;
	return mmlTrue;
}

mmlBool mmlScheduler::Suspend ( const mmlInt32 task_hdl )
{
	mmlMutexGuard g(mMutex);
	std::map < mmlInt32 , mmlAutoPtr < mmlSchedulerTaskHolder > >::iterator task = mTasks.find(task_hdl);
	if ( task != mTasks.end())
	{
		return task->second->Suspend();
	}
	return mmlFalse;
}

mmlBool mmlScheduler::Resume ( const mmlInt32 task_hdl )
{
	mmlMutexGuard g(mMutex);
	std::map < mmlInt32 , mmlAutoPtr < mmlSchedulerTaskHolder > >::iterator task = mTasks.find(task_hdl);
	if ( task != mTasks.end())
	{
		return task->second->Resume();
	}
	return mmlFalse;
}

mmlBool mmlScheduler::Remove ( const mmlInt32 task_hdl )
{
	mmlMutexGuard g(mMutex);
	std::map < mmlInt32 , mmlAutoPtr < mmlSchedulerTaskHolder > >::iterator task = mTasks.find(task_hdl);
	if ( task != mTasks.end())
	{
		mTasks.erase(task);
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlScheduler::Run ( const mmlInt32 task_hdl )
{
	mmlMutexGuard g(mMutex);
	mmlAutoPtr < mmlSchedulerTaskHolder > holder;
	std::map < mmlInt32 , mmlAutoPtr < mmlSchedulerTaskHolder > >::iterator task = mTasks.find(task_hdl);
	if ( task != mTasks.end())
	{
		holder = task->second;
	}
	if ( holder == mmlNULL )
	{
		return mmlFalse;
	}
	return mmlTrue;
}

void mmlScheduler::Execute ( mmlIThread * thread )
{
	mmlSharedPtr < mmlISleep > sleep = mmlNewObject(MML_OBJECT_UUID(mmlISleep));
	std::list < mmlInt32 > queue;
	while (mShutdown != mmlTrue)
	{
		{
			mmlMutexGuard g(mMutex);
			mml_timespec tm;
			mml_clock_gettime_monotonic(&tm);
			for ( std::map < mmlInt32 , mmlAutoPtr < mmlSchedulerTaskHolder > >::iterator task = mTasks.begin();task != mTasks.end(); task ++)
			{
				mmlSchedulerTaskHolder::TASK_STATUS_T status = task->second->GetStatus(tm);
				if ( status == mmlSchedulerTaskHolder::TS_TASK_FINISHED)
				{
					mTasks.erase(task++);
				}
				else if ( status == mmlSchedulerTaskHolder::TS_TASK_NEED_RUN )
				{
					mmlBool found = mmlFalse;
					for ( std::list < mmlInt32 >::iterator q = queue.begin(); q != queue.end(); q ++ )
					{
						if ( *q == task->first )
						{
							found = mmlTrue;
						}
					}
					if ( found == mmlFalse )
					{
						queue.push_back(task->first);
					}
				}
			}
			while ( queue.size() > 0 )
			{
				 std::map < mmlInt32 , mmlAutoPtr < mmlSchedulerTaskHolder > >::iterator task = mTasks.find(queue.front());
				 if ( task != mTasks.end() )
				 {
					mmlBool posted = mmlFalse;
					for ( std::list < mmlAutoPtr < mmlSchedulerThread > >::iterator thr = mThreadPool.begin(); thr != mThreadPool.end(); thr ++ )
					{
						if ( (*thr)->Post(task->second) == mmlTrue )
						{
							posted = mmlTrue;
							break;
						}
					}
					if ( posted == mmlFalse )
					{
						if ( mMaxPoolSize == 0 || mThreadPool.size() < mMaxPoolSize )
						{
							mmlAutoPtr < mmlSchedulerThread > new_thr = new mmlSchedulerThread();
							new_thr->Post(task->second);
							mThreadPool.push_back(new_thr);
							posted = mmlTrue;
						}
					}
					if ( posted == mmlTrue )
					{
						queue.pop_front();
					}
					else
					{
						break;
					}
				}
			}
		}
		mml_sleep(1000);
	}
}

void mmlScheduler::SvcStop ()
{
	for ( std::list < mmlAutoPtr < mmlSchedulerThread > >::iterator thr = mThreadPool.begin(); thr != mThreadPool.end(); thr ++ )
	{
		(*thr)->Shutdown();
	}
	mShutdown = mmlTrue;
}

void mmlScheduler::SvcWaitForStop ()
{
	for ( std::list < mmlAutoPtr < mmlSchedulerThread > >::iterator thr = mThreadPool.begin(); thr != mThreadPool.end(); thr ++ )
	{
		(*thr)->Join();
	}
	mThread->Join();
}

void mmlScheduler::Shutdown ()
{
	mShutdown = mmlTrue;
}

MML_OBJECT_IMPL3(mmlScheduler, mmlScheduler, mmlIScheduler, mmlIService)


#include "mml_components.h"

MML_CONSTRUCTOR_IMPL(mmlScheduler);
MML_CONSTRUCTOR_IMPL(mmlPeriodicSchedulerRule);

static mmlObjectFactory mml_scheduler_component [] =
{
	{
		MML_SCHEDULER_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlScheduler)
	},
	{
		{ 0x6777AFC7, 0x6C82, 0x1014, 0x86C8 , { 0xCE, 0x86, 0x39, 0xC8, 0xD5, 0x39 } },
		"mml/scheduler/rules/periodic",
		MML_CONSTRUCTOR(mmlPeriodicSchedulerRule)
	}
};

#include "mmlICStringStream.h"

void log_scheduler(const mmlChar * formatter , ...);

mmlBool mml_scheduler_start ()
{
	mmlSharedPtr < mmlIVariantStruct > config = mmlGetService(MML_GLOBAL_CONFIG_UUID());
	if ( config == mmlNULL )
	{
		return mmlTrue;
	}
	mmlAutoPtr < mmlIVariantStruct > scheduler;
	if ( config->Get("scheduler" , mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant>(scheduler)) == mmlFalse )
	{
		return mmlTrue;
	}
	if ( scheduler == mmlNULL )
	{
		return mmlTrue;
	}
	mmlSharedPtr < mmlScheduler > scheduler_service = mmlGetService(MML_OBJECT_UUID(mmlIScheduler));

	mmlAutoPtr < mmlIVariantInteger > max;
	scheduler->Get("max", mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant >(max));
	if ( max != mmlNULL )
	{
		scheduler_service->SetMaxSize(max->Get());
	}
	mmlAutoPtr < mmlIVariantArray > tasks;
	scheduler->Get("tasks", mmlRelativePtrAddRef < mmlIVariantArray, mmlIVariant >(tasks));
	if (tasks == mmlNULL )
	{
		return mmlFalse;
	}


	for ( mmlInt32 index = 0 ; index < tasks->Size(); index ++ )
	{
		mmlAutoPtr < mmlIVariantStruct > task;
		tasks->Get(index, mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant >(task));
		if ( task != mmlNULL )
		{
			mmlAutoPtr < mmlIVariantString > name;
			mmlAutoPtr < mmlIVariantString > rule;

			if ( task->Get("name", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(name)) == mmlFalse ||
				 task->Get("rule", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(rule)) == mmlFalse )
			{
				continue;
			}

			mmlChar task_full_name[256] = {0};
			mmlSprintf(task_full_name, sizeof(task_full_name) - 1, "mml/scheduler/tasks/%s", name->Get()->Get());

			mmlUUID task_uuid;
			if ( mmlGetObjectUUID(task_full_name, task_uuid) == mmlFalse )
			{
				log_scheduler("Task '%s' is not registered yet\n", name->Get()->Get());
				continue;
			}

			mmlChar rule_full_name[256] = {0};
			mmlSprintf(rule_full_name, sizeof(rule_full_name) - 1, "mml/scheduler/rules/%s", rule->Get()->Get());
			mmlUUID rule_uuid;
			if ( mmlGetObjectUUID(rule_full_name, rule_uuid) == mmlFalse )
			{
				log_scheduler("Rule '%s' is not registered yet\n", name->Get()->Get());
				continue;
			}

			mmlSharedPtr < mmlISchedulerTask > task_instance = mmlNewObject(task_uuid);
			if ( task_instance == mmlNULL )
			{
				log_scheduler("Task '%s' could not be constructed\n", name->Get());
				continue;
			}
			mmlSharedPtr < mmlISchedulerRule > rule_instance = mmlNewObject(rule_uuid);
			if ( rule_instance == mmlNULL )
			{
				log_scheduler("Rule '%s' could not be constructed\n", name->Get());
				continue;
			}
			if ( rule_instance->Configure(task) == mmlFalse )
			{
				log_scheduler("Rule '%s' could not be configured\n", name->Get());
				continue;
			}
			scheduler_service->Add(task, task_instance, rule_instance, mmlNULL);
		}
	}
	return scheduler_service->Start();
}


MML_COMPONENT_IMPL_C(scheduler, mml_scheduler_component, mml_scheduler_start, mmlNULL , mmlNULL); 


