#ifndef MML_INTERFACE_LIST_HEADER_INCLUDED
#define MML_INTERFACE_LIST_HEADER_INCLUDED

#include "mmlIObject.h"

#define MML_ILIST_ENUMERATOR_UUID {  0x588d372c , 0x80bf , 0x11de , 0xa9e5 , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

class mmlIListEnumerator : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_ILIST_ENUMERATOR_UUID)
public:
	virtual mmlBool HasMore ( mmlBool * has_more ) = 0;
	virtual mmlBool Next ( mmlIObject ** object ) = 0;
	virtual mmlBool Reverse ( const mmlBool reverse ) = 0;
};

#define MML_ILIST_UUID {  0x4c8e961e , 0x80bf , 0x11de , 0xa722 , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

class mmlIList : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_ILIST_UUID)
public:
	virtual mmlBool PushBack( mmlIObject * object ) = 0;
	virtual mmlBool PushFront ( mmlIObject * object ) = 0;
	virtual mmlBool PopBack () = 0;
	virtual mmlBool PopFront () = 0;
	virtual mmlBool Back ( mmlIObject ** object ) = 0;
	virtual mmlBool Front ( mmlIObject ** object ) = 0;
	virtual	mmlInt32 Length() = 0;
	virtual mmlBool GetEnumerator ( mmlIListEnumerator ** enumerator ) = 0;
};

#endif //MML_INTERFACE_LIST_HEADER_INCLUDED

