#ifndef MML_INTERFACE_EVENT_QUEUE_HEADER_INCLUDED
#define MML_INTERFACE_EVENT_QUEUE_HEADER_INCLUDED

#include "mmlIObject.h"


class mmlIObjectQueueEnumerator : public mmlIObject
{
public:
	virtual void OnObject ( mmlIObject * object , mmlBool * do_delete ) = 0;
};

#define MML_IOBJECT_QUEUE_UUID { 0xf6c72734 , 0x80bc , 0x11de , 0xb0bb , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }
#define MML_QUEUE_INFINITE_TIMEOUT 0



class mmlIObjectQueue : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_IOBJECT_QUEUE_UUID);
public:

	virtual mmlBool Post ( mmlIObject * object, const mmlInt32 delay = 0 ) = 0;

	virtual mmlBool Get ( mmlInt32 timeout,
		                  mmlIObject ** object ) = 0;

	virtual void Enumerate (mmlIObjectQueueEnumerator * en) = 0;
	
	virtual void Stat ( const mmlChar * name ) = 0;

	virtual mmlInt32 Size () = 0;

};

#endif //MML_INTERFACE_EVENT_QUEUE_HEADER_INCLUDED

