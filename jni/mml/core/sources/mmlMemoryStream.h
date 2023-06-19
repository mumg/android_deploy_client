#ifndef MML_IMPL_MEMORY_STREAM_HEADER_INCLUDED
#define MML_IMPL_MEMORY_STREAM_HEADER_INCLUDED


#include "mmlIMemoryStream.h"
#include "mmlPtr.h"
#include "mmlIBuffer.h"

class mmlMemoryInputStreamControl : public mmlIStreamControl
{
	MML_OBJECT_DECL
public:
	mmlMemoryInputStreamControl( mmlIBuffer * buffer);

	mmlBool SetSignal(mmlIStreamSignal * signal) { return mmlFalse; }

	mmlBool SetTimeout ( const mmlInt32 timeout ){ return mmlFalse; }

	mmlBool GetTimeout ( mmlInt32 * timeout ) { return mmlFalse; }

	mmlInt64 GetPosition ();

	mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset );

	mmlInt64 Read ( const mmlInt64 size , void * data );

	mmlBool IsEmpty() { return mDataOffset >= mDataSize ? mmlTrue : mmlFalse; }

	mmlInt64 Size ();

	mmlBool Close ();

private:
	mmlAutoPtr < mmlIBuffer > mBuffer;
	
	mmlUInt8 * mData;
	mmlInt64 mDataSize;
	mmlInt64 mDataOffset;
};

class mmlMemoryInputStream : public mmlIMemoryInputStream
{
	MML_OBJECT_DECL
public:

	mmlMemoryInputStream();

	mmlBool Construct ( mmlIBuffer * buffer );

	mmlBool Construct ( mmlICString * str );

	mmlBool Construct( mmlIMemoryOutputStream * stream );

	mmlInt64 Read ( const mmlInt64 size , void * data );

	mmlBool IsEmpty() { return mControl->IsEmpty(); }

	mmlBool GetControl( mmlIStreamControl ** control ) { *control = mControl; MML_ADDREF(*control); return mmlTrue; }

	mmlInt64 Size ();

	mmlBool Close ();

	mmlBool Wait ( const mmlInt32 timeout ) { return mmlTrue; }

private:

	mmlAutoPtr < mmlMemoryInputStreamControl > mControl;
};

class mmlMemoryOutputStreamControlConstant : public mmlIOutputStreamControl
{
	MML_OBJECT_DECL;
public:

	mmlMemoryOutputStreamControlConstant( mmlIBuffer * buffer );

	mmlBool SetSignal(mmlIStreamSignal * signal) { return mmlFalse;  }

	mmlBool SetTimeout ( const mmlInt32 timeout ) { return mmlFalse; }

	mmlBool GetTimeout ( mmlInt32 * timeout ) { return mmlFalse; }

	mmlBool SetMaxSize ( const mmlInt64 size );

	mmlInt64 GetPosition ();

	mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset );

	mmlBool Close ();

	mmlBool IsFull () { if ( mMaxSize == 0 ) return mmlFalse; return mDataOffset < mMaxSize ? mmlFalse: mmlTrue;}

	mmlBool GetData( mmlIBuffer ** data ) { *data = mBuffer; MML_ADDREF(*data); return mmlTrue; }

	mmlInt64 Write ( const mmlInt64 size , const void * data );

	mmlInt64 GetMaxSize () { return mMaxSize; }

	mmlInt64 Size () { return mDataOffset; }

private:

	mmlAutoPtr < mmlIBuffer > mBuffer;

	mmlUInt8 * mData;
	mmlInt64 mDataSize;
	mmlInt64 mDataOffset;

	mmlInt64 mMaxSize;
};


class mmlMemoryOutputStreamData : public mmlIMemoryOutputStream
{
	MML_OBJECT_DECL
public:

	mmlMemoryOutputStreamData();

	mmlBool Construct ( mmlIBuffer * data );

	mmlInt64 Write ( const mmlInt64 size , const void * data );

	mmlBool IsFull () { return mControl->IsFull(); }

	mmlBool GetData( mmlIBuffer ** data ) { return mControl->GetData(data); }

	mmlBool GetControl( mmlIStreamControl ** control ) { *control = mControl; MML_ADDREF(*control); return mmlTrue; }

	mmlInt64 Size ();

	mmlBool Close ();

	mmlBool Flush () { return mmlTrue; }

protected:

	mmlAutoPtr < mmlMemoryOutputStreamControlConstant > mControl;

};


class mmlMemoryOutputStreamControlMutable : public mmlIOutputStreamControl
{
	MML_OBJECT_DECL
public:
	mmlMemoryOutputStreamControlMutable();

	~mmlMemoryOutputStreamControlMutable();

	mmlBool SetSignal(mmlIStreamSignal * signal) { return mmlFalse;  }

	mmlBool SetTimeout ( const mmlInt32 timeout ) { return mmlFalse; }

	mmlBool GetTimeout ( mmlInt32 * timeout ) { return mmlFalse; }

	mmlBool IsFull () { if ( mDataMaxSize == 0 ) return mmlFalse; return mDataOffset < mDataMaxSize ? mmlFalse: mmlTrue;}

	mmlBool SetMaxSize ( const mmlInt64 size );

	mmlInt64 Size ();

	mmlInt64 GetPosition ();

	mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset );

	mmlBool Close ();

	mmlBool GetData( mmlIBuffer ** data );

	mmlInt64 Write ( const mmlInt64 size , const void * data );

	mmlInt64 GetMaxSize () { return mDataMaxSize; }

protected:
	mmlUInt8 * mData;
	mmlInt64 mDataSize;
	mmlInt64 mDataOffset;
	mmlInt64 mDataExponent;
	mmlInt64 mDataMaxSize;

	mmlAutoPtr < mmlIBuffer > mBuffer;
};


class mmlMemoryOutputStreamMutable : public mmlIMemoryOutputStream
{
	MML_OBJECT_DECL
public:

	mmlMemoryOutputStreamMutable();

	mmlInt64 Write ( const mmlInt64 size , const void * data );

	mmlBool IsFull () { return mControl->IsFull(); }

	mmlBool GetControl( mmlIStreamControl ** control ) { *control = mControl; MML_ADDREF(*control); return mmlTrue; }

	mmlBool GetData ( mmlIBuffer ** data );

	mmlInt64 Size ();

	mmlBool Close ();

	mmlBool Flush () { return mmlTrue; }

protected:

	mmlAutoPtr < mmlMemoryOutputStreamControlMutable > mControl;
};


class mmlMemoryOutputStreamControlMutableCString : public mmlMemoryOutputStreamControlMutable
{
	MML_OBJECT_DECL
public:
	mmlMemoryOutputStreamControlMutableCString(mmlICString * str)
		:mStr(str)
	{

	}

	mmlBool Close ();

private:
	mmlAutoPtr < mmlICString > mStr;
};

class mmlMemoryOutputStreamCString : public mmlIMemoryOutputStream
{
	MML_OBJECT_DECL
public:

	mmlMemoryOutputStreamCString();

	~mmlMemoryOutputStreamCString();

	mmlBool Construct ( mmlICString * data );

	mmlInt64 Write ( const mmlInt64 size , const void * data );

	mmlBool IsFull () { return mControl->IsFull(); }

	mmlBool GetControl( mmlIStreamControl ** control ) { *control = mControl; MML_ADDREF(*control); return mmlTrue; }

	mmlBool GetData( mmlIBuffer ** data ) { return mmlFalse; }

	mmlInt64 Size ();

	mmlBool Close ();

	mmlBool Flush () { return mmlTrue; }

protected:

	mmlAutoPtr < mmlMemoryOutputStreamControlMutableCString > mControl;

};


class mmlMemoryOutputCachedStream : public mmlIMemoryOutputCachedStream
{
	MML_OBJECT_DECL
public:

	mmlMemoryOutputCachedStream();

	~mmlMemoryOutputCachedStream();

	mmlBool Construct ( mmlIOutputStream * data );

	mmlInt64 Write ( const mmlInt64 size , const void * data );

	mmlBool IsFull ();

	mmlBool GetControl ( mmlIStreamControl ** control );

	mmlBool Close ();

	mmlInt64 Size ();

	mmlBool Flush ();

private:

	mmlUInt8 * mCacheBuffer;
	mmlInt32 mCacheBufferSize;
	mmlInt32 mCacheSize;

	mmlAutoPtr < mmlIOutputStream > mOutput;
;
};



#endif //MML_IMPL_MEMORY_STREAM_HEADER_INCLUDED
