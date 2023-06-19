#ifndef MML_BUFFER_HEADER_INCLUDED
#define MML_BUFFER_HEADER_INCLUDED

#include "mmlIObject.h"
#include "mmlPtr.h"

#define MML_BUFFER_UUID { 0xFBE2475D, 0x6C1C, 0x1014, 0x9E60 , { 0xEE, 0x24, 0xAC, 0x5D, 0x3B, 0x0C } }

class mmlIBuffer : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_BUFFER_UUID)
public:
	virtual void * Get () = 0;

	virtual mmlInt32 Size () = 0;

	virtual mmlBool Copy ( const mmlInt32 offset, void * data, const mmlInt32 size ) = 0;
};

mml_core mmlIBuffer * mmlAllocateBuffer ( const mmlInt32 size );

mml_core mmlIBuffer * mmlNewAllocatedBuffer ( void * data, const mmlInt32 size );

mml_core mmlIBuffer * mmlNewStaticBuffer ( void * data, const mmlInt32 size );

class mmlStaticBuffer
{
public:
	mmlStaticBuffer ( void * data, const mmlInt32 size )
		:mBuffer(mmlNewStaticBuffer(data, size))
	{

	}

	operator mmlIBuffer * ()
	{
		return mBuffer;
	}

private:
	mmlAutoPtr < mmlIBuffer > mBuffer;
};



#endif //MML_BUFFER_HEADER_INCLUDED
