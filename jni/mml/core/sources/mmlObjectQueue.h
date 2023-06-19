#ifndef MML_OBJECT_QUEUE_HEADER_INCLUDED
#define MML_OBJECT_QUEUE_HEADER_INCLUDED

#include "mmlIObjectQueue.h"
#include "mmlPtr.h"
#include "mml_time.h"
#include "mmlICondition.h"
#include "mmlIMutex.h"
#include <list>

class mmlObjectQueue : public mmlIObjectQueue
{
	MML_OBJECT_DECL;
public:

	mmlObjectQueue ();

	~mmlObjectQueue();

	mmlBool Construct ();

	mmlBool Construct ( mmlICondition * confition );

	mmlBool Post ( mmlIObject * object , const mmlInt32 delay);

	mmlBool Get ( mmlInt32 timeout,
		          mmlIObject ** object );

	void Enumerate (mmlIObjectQueueEnumerator * en);
	
	void Stat ( const mmlChar * name );

	mmlInt32 Size ();

private:

	mmlSharedPtr < mmlICondition > mEvent;

	mmlSharedPtr < mmlIMutex > mReadMutex;

	mmlSharedPtr < mmlIMutex > mWriteMutex;

	std::list < mmlAutoPtr < mmlIObject > > mQueue;
	std::list < std::pair < mml_timespec, mmlAutoPtr < mmlIObject > > > mDelayedQueue;
	
	mmlInt32 mProcessed;
	
	mml_timespec mLastStat;

};


#endif //MML_OBJECT_QUEUE_HEADER_INCLUDED

