#include "mmlCString.h"
#include "mml_memory.h"
#include "mml_strutils.h"
#include "mmlIBuffer.h"

const mmlChar * mmlCString::mEmptyStr = "";

MML_OBJECT_IMPL2(mmlCString, mmlCString , mmlICString)

mmlCString::mmlCString()
:mStr(mmlNULL), mLength(0)
{
}

mmlCString::mmlCString ( const mmlChar * str , const mmlInt32 len , const mmlBool adopt )
{
	if ( adopt == mmlFalse )
	{
		mStr = (mmlChar*)mmlAlloc(len + 1);
		mmlStrCopy(mStr , str , len);
		mLength = len;
	}
	else
	{
		mmlInt32 _len = len == -1 ? mmlStrLength(str) : len;
		if ( len > 0 )
		{
			mStr = const_cast < mmlChar * >(str);
			mLength = _len;
		}
	}
}

mmlCString::mmlCString( mmlIBuffer * buffer)
	:mStr((mmlChar*)buffer->Get()), mLength(buffer->Size()), mBuffer(buffer)
{

}

mmlCString::~mmlCString()
{
	if (mBuffer== mmlNULL && mStr != mmlNULL) mmlFree(mStr);
}

const mmlChar * mmlCString::Get () const
{
	if ( !mStr ) return mEmptyStr;
	return mStr;
}

const mmlUInt32 mmlCString::Length () const
{
	if ( !mStr ) return 0;
	return mLength;
}


mmlBool mmlCString::CompareStr ( const mmlCompareOperator & op,
				              const mmlChar * str,
				              mmlBool & result, 
							  mml_char_conversion_t rule )
{
	result = mmlFalse;
	if ( mStr != mmlNULL )
	{
		mmlInt32 res = mmlStrCompare( mStr , str , rule);
		if ( op == MML_EQUAL )
		{
			result = (res == 0 ) ? mmlTrue : mmlFalse;
		}
		else if ( op == MML_MORE )
		{
			result = (res > 0 ) ? mmlTrue : mmlFalse;
		}
		else if ( op == MML_MORE_OR_EQUAL )
		{
			result = ( res > 0 || res == 0 ) ? mmlTrue : mmlFalse;
		}
		else if ( op == MML_LESS )
		{
			result = ( res < 0 ) ? mmlTrue : mmlFalse;
		}
		else if ( op == MML_LESS_OR_EQUAL )
		{
			result = ( res < 0 || res == 0 ) ? mmlTrue : mmlFalse;
		}
		else
		{
			result = (res != 0 ) ? mmlTrue : mmlFalse;
		}
	}
	return mmlTrue;
}


mmlBool mmlCString::Assign ( const mmlChar * str )
{
	if ( mBuffer == mmlNULL && mStr != mmlNULL )
	{
		mmlFree(mStr);
		mStr = mmlNULL;
		mLength = 0;
	}
	mBuffer = mmlNULL;
	mmlInt32 len = mmlStrLength(str);
	if ( len > 0 )
	{
		mStr = (mmlChar*)mmlAlloc(len + 1);
		mmlStrCopy(mStr , str , len + 1);
		mLength = len;
	}
	return mmlTrue;
}

mmlBool mmlCString::Adopt ( const mmlChar * str )
{
	if ( mBuffer == mmlNULL && mStr != mmlNULL )
	{
		mmlFree(mStr);
		mStr = mmlNULL;
		mLength = 0;
	}
	mBuffer = mmlNULL;
	mmlInt32 len = mmlStrLength(str);
	if ( len > 0 )
	{
		mStr = const_cast < mmlChar * >(str);
		mLength = len;
	}
	return mmlTrue;
}

class mmlCStaticStringImpl : public mmlICString
{
	MML_OBJECT_DECL;
public:

	mmlCStaticStringImpl( const mmlChar * str )
		:mStr(str)
	{
		mLength = mmlStrLength(mStr);
	}

	~mmlCStaticStringImpl()
	{

	}

	const mmlChar * Get () const
	{
		return mStr;
	}

	const mmlUInt32 Length () const
	{
		return mLength;
	}

	mmlBool CompareStr ( const mmlCompareOperator & op,
		              const mmlChar * str,
		              mmlBool & result,
					  mml_char_conversion_t rule)
	{
		result = mmlFalse;
		if ( mStr != mmlNULL )
		{
			mmlInt32 res = mmlStrCompare( mStr , str , rule);
			if ( op == MML_EQUAL )
			{
				result = (res == 0 ) ? mmlTrue : mmlFalse;
			}
			else if ( op == MML_MORE )
			{
				result = (res > 0 ) ? mmlTrue : mmlFalse;
			}
			else if ( op == MML_MORE_OR_EQUAL )
			{
				result = ( res > 0 || res == 0 ) ? mmlTrue : mmlFalse;
			}
			else if ( op == MML_LESS )
			{
				result = ( res < 0 ) ? mmlTrue : mmlFalse;
			}
			else if ( op == MML_LESS_OR_EQUAL )
			{
				result = ( res < 0 || res == 0 ) ? mmlTrue : mmlFalse;
			}
			else
			{
				result = (res != 0 ) ? mmlTrue : mmlFalse;
			}
		}
		return mmlTrue;
	}

	mmlBool Assign ( const mmlChar * str )
	{
		return mmlFalse;
	}

	mmlBool Adopt ( const mmlChar * str )
	{
		return mmlFalse;
	}

private:

	const mmlChar * mStr;

	mmlInt32 mLength;

};


MML_OBJECT_IMPL1(mmlCStaticStringImpl, mmlICString)

mml_core mmlICString * mmlNewStaticCString( const mmlChar * str  )
{
	return new mmlCStaticStringImpl(str);
}

mml_core mmlICString * mmlNewCString( const mmlChar * str , const mmlInt32 len, const mmlBool adopt)
{
	return new mmlCString(str , len == -1 ? mmlStrLength(str) : len , adopt);
}

mml_core mmlICString * mmlNewCStringFromBuffer ( mmlIBuffer * buffer)
{
	return new mmlCString(buffer);
}