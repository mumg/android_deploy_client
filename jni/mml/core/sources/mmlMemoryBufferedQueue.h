#ifndef MML_MEMORY_BUFFERED_QUEUE_IMPL_HEADER_INCLUDED
#define MML_MEMORY_BUFFERED_QUEUE_IMPL_HEADER_INCLUDED

#include "mmlIMemoryBufferedQueue.h"
#include "mmlICondition.h"
#include "mmlIMutex.h"

class mmlInputMemoryBufferedQueue : public mmlIInputMemoryBufferedQueue
{
	MML_OBJECT_DECL;
private:
	mmlAutoPtr < mmlIInputMemoryBufferedQueueReader > mReader;

	mmlUInt8 * mData;
	mmlInt64 mDataSize;
	mmlInt64 mDataOffset;
	mmlInt64 mDataExponent;

	const mmlChar * mPrefix;
	_logger_func mLogFunc;
	mmlSharedPtr< mmlIBinaryLogger> mLogger;

public:
	mmlInputMemoryBufferedQueue();

	~mmlInputMemoryBufferedQueue();

	mmlBool Construct(mmlIInputMemoryBufferedQueueReader * reader);

	mmlInt64 Write(const mmlInt64 size, const void * data);

	mmlBool IsFull() { return mmlFalse; }

	mmlBool GetControl(mmlIStreamControl ** control) { return mmlFalse; }

	mmlInt64 Size() { return mDataSize;  }

	mmlBool Close();

	mmlBool Flush() { return mmlTrue; }

	void setDump(const mmlChar * prefix, _logger_func func);
};


class mmlOutputMemoryBufferedQueueControl : public mmlIStreamControl
{
	MML_OBJECT_DECL
public:

	mmlInt32 timeout;
	mmlAutoPtr< mmlIStreamSignal > signal;

	mmlOutputMemoryBufferedQueueControl()
	{
		timeout = MML_CONDITION_INFINITE;
	}

	mmlBool SetSignal(mmlIStreamSignal * _signal)
	{
		signal = _signal;
		return mmlTrue;
	}

	mmlBool SetTimeout(const mmlInt32 _timeout)
	{
		timeout = _timeout;
		return mmlTrue;
	}

	mmlBool GetTimeout(mmlInt32 * _timeout)
	{
		*_timeout = timeout;
		return mmlTrue;
	}

	mmlInt64 GetPosition() 
	{
		return 0;
	}

	mmlBool Seek(const MML_SEEK_T mode, const mmlInt64 offset)
	{
		return mmlFalse;
	}
};

class mmlOutputMemoryBufferedQueue : public mmlIOutputMemoryBufferedQueue
{
	MML_OBJECT_DECL
private:
	mmlUInt8 * mData;
	mmlInt64 mDataSize;
	mmlInt64 mDataOffset;
	mmlInt64 mDataExponent;
	mmlSharedPtr < mmlIMutex > mMutex;
	mmlBool mIsClosed;

	mmlSharedPtr < mmlICondition > mCondition;

	mmlAutoPtr < mmlOutputMemoryBufferedQueueControl > mControl;

	const mmlChar * mPrefix;
	_logger_func mLogFunc;
	mmlSharedPtr< mmlIBinaryLogger> mLogger;

public:

	mmlOutputMemoryBufferedQueue();

	~mmlOutputMemoryBufferedQueue();

	mmlUInt64 Write(const mmlUInt64 size, const void * data);

	mmlBool GetControl(mmlIStreamControl ** control) { *control = mControl; MML_ADDREF(*control); return mmlTrue; }

	mmlInt64 Read(const mmlInt64 size, void * data);

	mmlBool IsEmpty() { return  mmlFalse;  }

	mmlInt64 Size() { return mDataOffset;  }

	mmlBool Close();

	void setDump(const mmlChar * prefix, _logger_func func);

};


class mmlOutputMemoryBufferedQueueStream : public  mmlIOutputMemoryBufferedQueueStream
{
	MML_OBJECT_DECL
private:
	mmlAutoPtr < mmlIOutputMemoryBufferedQueue > mQueue;
public:

	mmlBool Construct(mmlIOutputMemoryBufferedQueue * queue);

	mmlInt64 Write(const mmlInt64 size, const void * data);

	mmlBool IsFull() { return mmlFalse;  }

	virtual mmlBool GetControl(mmlIStreamControl ** control) { return mmlFalse; }

	virtual mmlBool Close() { return mmlTrue;  }

	virtual mmlInt64 Size() { return 0;  }

	virtual mmlBool Flush() { return mmlTrue;  }
};

#endif //MML_MEMORY_BUFFERED_QUEUE_IMPL_HEADER_INCLUDED
