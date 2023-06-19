#ifndef MML_ITHREAD_HEADER_INCLUDED
#define MML_ITHREAD_HEADER_INCLUDED

#include "mmlIObject.h"

#define MML_DEFAULT_STACK_SIZE (1024 * 1024)

typedef enum
{
	PriorityLowest,
	PriorityLow,
	PriorityNormal,
	PriorityHigh,
	PriorityHighest
} mmlThreadPriority;

class mmlIThread;

#define MML_ITHREAD_FUNCTION_UUID {  0xf09a6914 , 0x8c35 , 0x11de , 0x9402 , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

class mmlIThreadFunction : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_ITHREAD_FUNCTION_UUID)
public:
	virtual void Execute (mmlIThread * thread) = 0;
	virtual void Shutdown () = 0;
};

#define MML_ITHREAD_UUID { 0x0baa8420 , 0x7a14 , 0x11de , 0x819d , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }


template < class T >
class mmlThreadFunctionProxy : public mmlIThreadFunction
{
public:
	mmlThreadFunctionProxy(T* func)
		:mFunc(func), mRef(0)
	{

	}

	void Execute (mmlIThread * thread)
	{
		mFunc->Execute(thread);
	}

	void Shutdown ()
	{
		mFunc->Shutdown ();
	}

	void AddRef()                 
	{                             
		mmlAtomicAdd( &mRef , 1 );
	}                            

	void Release()
	{
		mmlInt32 count = mmlAtomicAdd(&mRef , -1);
		if ( count == 0 )
		{ 
			delete this;
		}
	}

	mmlInt32 GetRef()
	{
		return  mmlAtomicGet(&mRef); 
	}

	mmlBool QueryInterface(const mmlUUID & uuid, void ** ptr) 
	{
		mmlIObject * foundInterface = mmlNULL;
		mmlUUID mml_object_uuid = mmlIObjectUUID;
		if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIThreadFunction)) == 0)
		{
			foundInterface = static_cast<mmlIThreadFunction*>(this);
		}
		else if( mmlCompareUUID(uuid , mml_object_uuid ) == 0)
		{
			foundInterface = static_cast<mmlIObject*>(this);
		}
		
		*ptr = foundInterface;
		MML_ADDREF(foundInterface);
		if ( ptr != mmlNULL ) return mmlTrue;
		return mmlFalse;
	}

	mmlBool IsRelative(const mmlUUID & uuid)
	{
		if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(mmlIThreadFunction)) == 0)
			return mmlTrue;
		mmlUUID mml_object_uuid = mmlIObjectUUID;
		if ( mmlCompareUUID(uuid , mml_object_uuid ) == 0) return mmlTrue;
		return mmlFalse;
	}  

private:
	T * mFunc;
	mmlInt32 mRef;
};


class mmlThreadShutdownFlag
{
public:
	mmlBool state;
public:
	mmlThreadShutdownFlag()
		:state(mmlFalse)
	{

	}
};

#define MML_THREAD_DECLARE_SHUTDOWN_FLAG \
private: \
	mmlThreadShutdownFlag mShutdown; \
public: \
	void Shutdown () \
	{ \
		mShutdown.state = mmlTrue; \
    } \
	mmlBool IsShutdown () \
	{ \
		return mShutdown.state; \
	}


#undef Yield

class mmlIThread : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_ITHREAD_UUID);
public:

	virtual mmlBool Create ( mmlIThreadFunction * function,
		                     const mmlThreadPriority & priority,
						     const mmlInt32 stack ) = 0;

	virtual void Join () = 0;

	virtual void SetPriority ( const mmlThreadPriority & priority ) = 0;

	virtual void BeginExec() = 0;

	virtual void EndExec () = 0;

	virtual void Shutdown () = 0;

	virtual void Dispatch () = 0;

};

mml_core void mml_sleep ( const mmlInt32 timeout );
mml_core mmlUInt64 mml_get_current_thread_id ();

#endif //MML_ITHREAD_HEADER_INCLUDED

