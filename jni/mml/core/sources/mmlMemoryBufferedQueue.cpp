#include "mmlMemoryBufferedQueue.h"
#include "mml_components.h"

mmlInputMemoryBufferedQueue::mmlInputMemoryBufferedQueue()
{
	mData = (mmlUInt8 *)mmlAlloc(256);
	mDataSize = 256;
	mDataExponent = 8;
	mDataOffset = 0;
	mLogFunc = mmlNULL;
	mPrefix = mmlNULL;
}

mmlInputMemoryBufferedQueue::~mmlInputMemoryBufferedQueue()
{
	mmlFree(mData);
}

mmlBool mmlInputMemoryBufferedQueue::Construct(mmlIInputMemoryBufferedQueueReader * reader)
{
	mReader = reader;
	return mmlTrue;
}

mmlInt64 mmlInputMemoryBufferedQueue::Write(const mmlInt64 size, const void * data)
{
	if (mLogger != mmlNULL)
	{
		mLogger->DoLog(mPrefix, (void*)data, size, mLogFunc);
	}

	mmlUInt32 sz = size;
	if (mDataOffset + sz > mDataSize)
	{
		while (mDataSize < mDataOffset + sz)
		{
			mDataExponent++;
			mDataSize = 2 ^ mDataExponent;
		}
		mData = (mmlUInt8*)mmlRealloc(mData, mDataSize);
	}
	mmlMemoryCopy(mData + mDataOffset, data, sz);
	mDataOffset += sz;
	mmlUInt8 * ptr = (mmlUInt8*)mData;
	mmlInt32 total = 0;
	for(;;)
	{
		mmlInt32 processed = 0;
		if (mDataOffset - total == 0)
		{
			break;
		}
		if (mReader->Read(ptr, mDataOffset - total, &processed) == mmlFalse)
		{
			return -1;
		}
		if (processed == 0)
		{
			break;
		}

		ptr += processed;
		total += processed;
	} 
	if (total > 0)
	{
		mmlMemoryMove(mData, (mmlUInt8*)mData + total, mDataOffset - total);
		mDataOffset -= total;
	}
	return size;
}

mmlBool mmlInputMemoryBufferedQueue::Close()
{
	return mReader->Close(mData, mDataOffset);
}

void mmlInputMemoryBufferedQueue::setDump(const mmlChar * prefix, _logger_func func)
{
	if ( prefix == mmlNULL || func == mmlNULL )
	{
		mLogger = mmlNULL;
		mPrefix = mmlNULL;
		mLogFunc = mmlNULL;
	}
	else
	{
		mPrefix = prefix;
		mLogFunc = func;
		mLogger = mmlNewObject(MML_OBJECT_UUID(mmlIBinaryLogger));

	}
}

MML_OBJECT_IMPL2(mmlInputMemoryBufferedQueue, mmlIInputMemoryBufferedQueue, mmlIOutputStream)

MML_OBJECT_IMPL1(mmlOutputMemoryBufferedQueueControl, mmlIStreamControl);

mmlOutputMemoryBufferedQueue::mmlOutputMemoryBufferedQueue()
{
	mData = (mmlUInt8 *)mmlAlloc(256);
	mDataSize = 256;
	mDataExponent = 8;
	mDataOffset = 0;
	mIsClosed = mmlFalse;
	mMutex = mmlNewObject(MML_OBJECT_UUID(mmlIMutex));
	mCondition = mmlNewObject(MML_OBJECT_UUID(mmlICondition));
	mLogFunc = mmlNULL;
	mPrefix = mmlNULL;
	mControl = new mmlOutputMemoryBufferedQueueControl();
}

mmlOutputMemoryBufferedQueue::~mmlOutputMemoryBufferedQueue()
{
	mmlFree(mData);
}

mmlUInt64 mmlOutputMemoryBufferedQueue::Write(const mmlUInt64 size, const void * data)
{
	if (mIsClosed == mmlTrue)
	{
		return -1;
	}

	mmlMutexGuard g(mMutex);
	mmlUInt32 sz = size;
	if (mDataOffset + sz > mDataSize)
	{
		while (mDataSize < mDataOffset + sz)
		{
			mDataExponent++;
			mDataSize = 2 ^ mDataExponent;
		}
		mData = (mmlUInt8*)mmlRealloc(mData, mDataSize);
	}
	mmlMemoryCopy(mData + mDataOffset, data, sz);
	mDataOffset += sz;



	if ( mControl != mmlNULL && mControl->signal != mmlNULL )
	{
		mControl->signal->Signal();
	}
	if (mControl->timeout > 0)
	{
		mCondition->Signal();
	}
	return size;
}

mmlInt64 mmlOutputMemoryBufferedQueue::Read(const mmlInt64 size, void * data)
{
	if (mIsClosed == mmlTrue)
	{
		return -1;
	}
	if (mControl->timeout > 0)
	{
		mCondition->Wait(mControl->timeout);
	}
	mmlMutexGuard g(mMutex);
	mmlInt64 sz = size > mDataOffset ? mDataOffset : size;
	if (sz > 0)
	{
		mmlMemoryCopy(data, mData, sz);
		if (mLogger != mmlNULL)
		{
			mLogger->DoLog(mPrefix, (void*)data, sz, mLogFunc);
		}
		if (mDataOffset - sz > 0)
		{
			mmlMemoryMove(mData, (mmlUInt8*)mData + sz, mDataOffset - sz);
		}
		mDataOffset -= sz;
	}
	return sz;
}

mmlBool mmlOutputMemoryBufferedQueue::Close()
{
	mIsClosed = mmlTrue;
	return mmlTrue;
}

void mmlOutputMemoryBufferedQueue::setDump(const mmlChar * prefix, _logger_func func)
{
	if (prefix == mmlNULL || func == mmlNULL)
	{
		mLogger = mmlNULL;
		mPrefix = mmlNULL;
		mLogFunc = mmlNULL;
	}
	else
	{
		mPrefix = prefix;
		mLogFunc = func;
		mLogger = mmlNewObject(MML_OBJECT_UUID(mmlIBinaryLogger));

	}
}

MML_OBJECT_IMPL2(mmlOutputMemoryBufferedQueue, mmlIOutputMemoryBufferedQueue, mmlIInputStream)

mmlBool mmlOutputMemoryBufferedQueueStream::Construct(mmlIOutputMemoryBufferedQueue * queue)
{
	mQueue = queue;
	return mmlTrue;
}

mmlInt64 mmlOutputMemoryBufferedQueueStream::Write(const mmlInt64 size, const void * data)
{
	return mQueue->Write(size, data);
}

MML_OBJECT_IMPL2(mmlOutputMemoryBufferedQueueStream, mmlIOutputMemoryBufferedQueueStream, mmlIOutputStream)
