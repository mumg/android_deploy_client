#include "mmlList.h"

MML_OBJECT_IMPL1(mmlList, mmlIList)

mmlList::~mmlList()
{

}

mmlBool mmlList::PushBack( mmlIObject * object )
{
	mList.push_back(object);
	return mmlTrue;
}

mmlBool mmlList::PushFront ( mmlIObject * object )
{
	mList.push_front(object);
	return mmlTrue;
}

mmlBool mmlList::PopBack ()
{
	if ( mList.size() > 0 )
	{
		mList.pop_back();
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlList::PopFront ()
{
	if ( mList.size() > 0 )
	{
		mList.pop_front();
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlList::Back ( mmlIObject ** object )
{
	if ( object != mmlNULL &&
		 mList.size() > 0)
	{
		*object = mList.back();
		MML_ADDREF(*object);
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlList::Front ( mmlIObject ** object )
{
	if ( object != mmlNULL &&
		mList.size() > 0)
	{
		*object = mList.front();
		MML_ADDREF(*object);
		return mmlTrue;
	}
	return mmlFalse;
}

mmlInt32 mmlList::Length()
{
	return (mmlUInt32)mList.size();
}


class mmlListEnumerator : public mmlIListEnumerator
{
	MML_OBJECT_DECL;
public:

	mmlListEnumerator ( mmlList * lst )
		:mList(lst), mReverse(mmlFalse)
	{
		mIterator = mList->Get().begin();
	}

	mmlBool HasMore ( mmlBool * has_more )
	{
		if ( mReverse == mmlTrue )
		{
			*has_more = mReverseIterator != mList->Get().rend() ? mmlTrue : mmlFalse;
		}
		else
		{
			*has_more = mIterator != mList->Get().end() ? mmlTrue : mmlFalse;
		}
		return mmlTrue;
	}

	mmlBool Next ( mmlIObject ** object )
	{
		if ( mReverse == mmlTrue )
		{
			if ( mReverseIterator != mList->Get().rend())
			{
				*object = *mReverseIterator;
				MML_ADDREF(*object);
				mReverseIterator ++;
				return mmlTrue;
			}
		}
		else
		{
			if ( mIterator != mList->Get().end())
			{
				*object = *mIterator;
				MML_ADDREF(*object);
				mIterator ++;
				return mmlTrue;
			}
		}
		return mmlFalse;
	}

	mmlBool Reverse ( const mmlBool reverse )
	{
		mReverse = reverse;
		if ( mReverse == mmlTrue )
		{
			mReverseIterator = mList->Get().rbegin();
		}
		else
		{
			mIterator = mList->Get().begin();
		}
		return mmlTrue;
	}

private:

	std::list <  mmlAutoPtr < mmlIObject >  >::iterator mIterator;
	std::list <  mmlAutoPtr < mmlIObject >  >::reverse_iterator mReverseIterator;
	mmlAutoPtr <mmlList> mList;
	mmlBool mReverse;
};

MML_OBJECT_IMPL1(mmlListEnumerator, mmlIListEnumerator)


mmlBool mmlList::GetEnumerator ( mmlIListEnumerator ** enumerator )
{
	*enumerator = new mmlListEnumerator(this);
	MML_ADDREF(*enumerator);
	return mmlTrue;
}
