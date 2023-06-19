#ifndef MML_LIST_HEADER_INCLUDED
#define MML_LIST_HEADER_INCLUDED

#include "mmlIList.h"
#include "mmlPtr.h"
#include <list>


class mmlList : public mmlIList
{
	MML_OBJECT_DECL;
public:
	
	~mmlList();

	mmlBool PushBack( mmlIObject * object );

	mmlBool PushFront ( mmlIObject * object );

	mmlBool PopBack ();

	mmlBool PopFront ();

	mmlBool Back ( mmlIObject ** object );

	mmlBool Front ( mmlIObject ** object );

	mmlInt32 Length();

	mmlBool GetEnumerator ( mmlIListEnumerator ** enumerator );

	std::list < mmlAutoPtr < mmlIObject > > & Get()
	{
		return mList;
	}

private:

	std::list <  mmlAutoPtr < mmlIObject >  > mList;

};

#endif //MML_LIST_HEADER_INCLUDED

