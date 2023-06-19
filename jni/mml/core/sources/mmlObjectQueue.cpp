#include "mmlObjectQueue.h"
#include "mmlIMutex.h"
#include "mmlICondition.h"
#include "mml_components.h"

void log_core(const mmlChar * formatter , ... );
MML_OBJECT_IMPL1(mmlObjectQueue, mmlIObjectQueue)

mmlObjectQueue::mmlObjectQueue ()
{
	mWriteMutex = mmlNewObject(MML_OBJECT_UUID(mmlIMutex));
	mReadMutex = mmlNewObject(MML_OBJECT_UUID(mmlIMutex));
	mProcessed = 0;
	mml_clock_gettime_realtime(&mLastStat);
}

mmlObjectQueue::~mmlObjectQueue()
{
}

mmlBool mmlObjectQueue::Construct ()
{
	mEvent = mmlNewObject(MML_OBJECT_UUID(mmlICondition));
	return mmlTrue;
}

mmlBool mmlObjectQueue::Construct( mmlICondition * condition )
{
	mEvent = condition;
	return mmlTrue;
}

mmlBool mmlObjectQueue::Post ( mmlIObject * object , const mmlInt32 delay)
{
	mmlMutexGuard g(mWriteMutex);
	if (delay == 0)
	{
		mQueue.push_back(object);
		mEvent->Signal();
	}
	else
	{
		mml_timespec ts;
		mml_clock_gettime_monotonic(&ts);
		mml_clock_add(&ts, delay);
		for (std::list < std::pair < mml_timespec, mmlAutoPtr < mmlIObject > > >::iterator obj = mDelayedQueue.begin(); obj != mDelayedQueue.end(); obj++)
		{
			if (mml_clock_diff(&ts, &((*obj).first)) < 0)
			{
				mDelayedQueue.insert(obj, std::pair < mml_timespec, mmlAutoPtr < mmlIObject > >(ts, object));
				mEvent->Signal();
				return mmlTrue;
			}

		}
		mDelayedQueue.push_back(std::pair < mml_timespec, mmlAutoPtr < mmlIObject > >(ts, object));
		mEvent->Signal();
	}

	return mmlTrue;
}

mmlBool mmlObjectQueue::Get ( mmlInt32 timeout,
			                  mmlIObject ** object )
{
	mmlBool result = mmlFalse;
	mmlMutexGuard r(mReadMutex);
	mmlMutexGuard w(mWriteMutex);
	mml_timespec mts;
	mml_clock_gettime_monotonic(&mts);
	mml_clock_add(&mts, timeout);
	for (;;)
	{
		mml_timespec cts;
		mml_clock_gettime_monotonic(&cts);
		if (mDelayedQueue.size() > 0)
		{
			if (mml_clock_diff(&cts, &(mDelayedQueue.front().first)) >= 0)
			{
				*object = mDelayedQueue.front().second;
				MML_ADDREF(*object);
				mDelayedQueue.pop_front();
				return mmlTrue;
			}
		}
		if (mQueue.size() > 0)
		{
			*object = mQueue.front();
			MML_ADDREF(*object);
			mQueue.pop_front();
			return mmlTrue;
		}
		mmlInt64 diff = mml_clock_diff(&mts, &cts);
		if (diff <= 0)
		{
			return mmlFalse;
		}
		if (mDelayedQueue.size() > 0)
		{
			mmlInt64 delayed_diff = mml_clock_diff(&(mDelayedQueue.front().first), &cts);
			if (delayed_diff < diff) 
			{
				diff = delayed_diff;
			}
		}
		w.UnLock();
		mEvent->Wait((mmlInt32)diff);
		w.Lock();
	}
	return mmlFalse;
}

void mmlObjectQueue::Enumerate (mmlIObjectQueueEnumerator * en)
{
	mmlMutexGuard g(mWriteMutex);
	for ( std::list < mmlAutoPtr < mmlIObject > > ::iterator obj = mQueue.begin(); obj != mQueue.end();  )
	{
		mmlBool do_delete = mmlFalse;
		en->OnObject(*obj , &do_delete);
		if ( do_delete == mmlTrue )
		{
			mQueue.erase(obj++);
		}
		else
		{
			++obj;
		}
	}
}

mmlInt32 mmlObjectQueue::Size ()
{
	mmlInt32 size = 0;
	mmlMutexGuard g(mWriteMutex);
	size = mQueue.size();
	return size;
}

void mmlObjectQueue::Stat ( const mmlChar * name )
{
	mmlMutexGuard g(mWriteMutex);
	
	mml_timespec current_time;
	mml_clock_gettime_realtime(&current_time);

	time_t sec = current_time.tv_sec - mLastStat.tv_sec;
	long nsec = current_time.tv_nsec - mLastStat.tv_nsec;

	if (nsec < 0) {
		nsec += 1000000000;
		sec -= 1;
	}
	time_t msec = sec * 1000 + nsec / 1000000;
	
	log_core("QUEUE: %s - processed %d rate %ld current %d\n" , name , mProcessed , time_t(1000.0 * mProcessed / msec ) , mQueue.size());
	
	mLastStat =  current_time;
	
	mProcessed = 0;

}
