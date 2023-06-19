#include "mmlIBuffer.h"

class mmlBuffer : public mmlIBuffer
{
	MML_OBJECT_DECL
public:

	mmlBuffer ( const mmlInt32 size )
		:mData(size > 0 ? mmlAlloc(size) : mmlNULL ), mDataSize(size), mRelease(mmlTrue)
	{

	}

	mmlBuffer ( void * data, const mmlInt32 size , const mmlBool adopt )
		:mData(data), mDataSize(size), mRelease(adopt)
	{

	}

	~mmlBuffer()
	{
		if ( mRelease == mmlTrue && mData != mmlNULL )
		{
			mmlFree(mData);
		}
	}



	void * Get ()
	{
		return mData;
	}

	mmlInt32 Size ()
	{
		return mDataSize;
	}	

	mmlBool Copy ( const mmlInt32 offset, void * data, const mmlInt32 size )
	{
		if ( offset + size >= mDataSize )
		{
			return mmlFalse;
		}
		mmlMemoryCopy((mmlUInt8*)data + offset, data, size);
		return mmlTrue;
	}

private:
	void * mData;
	mmlInt32 mDataSize;
	mmlBool mRelease;
};

MML_OBJECT_IMPL1(mmlBuffer, mmlIBuffer)


mml_core mmlIBuffer * mmlAllocateBuffer ( const mmlInt32 size )
{
	return new mmlBuffer(size);
}

mml_core mmlIBuffer * mmlNewAllocatedBuffer ( void * data, const mmlInt32 size )
{
	return new mmlBuffer(data, size, mmlTrue);
}

mml_core mmlIBuffer * mmlNewStaticBuffer ( void * data, const mmlInt32 size )
{
	return new mmlBuffer(data, size, mmlFalse);
}
