#ifndef MML_IMUTEX_HEADER_INCLUDED
#define MML_IMUTEX_HEADER_INCLUDED

#include "mmlIObject.h"


#define MML_IMUTEX_UUID { 0xee1fa01c , 0x7a12 , 0x11de , 0xb660 , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

class mmlIMutex : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_IMUTEX_UUID);
public:

	virtual mmlBool Lock () = 0;

	virtual mmlBool UnLock () = 0;

};


class mmlMutexGuard
{
private:
	mmlIMutex * mMutex;
	mmlBool mLocked;
public:
	mmlMutexGuard(mmlIMutex * mutex)
		:mMutex(mutex), mLocked(mmlTrue)
	{
		MML_ADDREF(mutex);
		mMutex->Lock();
	}

	~mmlMutexGuard()
	{
		if ( mLocked == mmlTrue )
		{
			mMutex->UnLock();
		}
		MML_RELEASE(mMutex);
	}

	void Lock ()
	{
		if ( mLocked == mmlFalse )
		{
			mMutex->Lock();
			mLocked = mmlTrue;
		}
	}

	void UnLock ()
	{
		if ( mLocked == mmlTrue )
		{
			mMutex->UnLock();
			mLocked = mmlFalse;
		}
	}

};

#endif //MML_IMUTEX_HEADER_INCLUDED

