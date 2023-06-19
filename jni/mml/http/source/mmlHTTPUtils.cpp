#include "mmlHTTPUtils.h"
#include "mmlICStringStream.h"
#include "mml_components.h"
#include <string>

mmlHTTPSplitter::mmlHTTPSplitter ( const mmlChar pair_delimiter,
		                       const mmlChar keyvalue_delimiter)
							   :mPairDelimiter(pair_delimiter),
							    mKeyValueDelimiter(keyvalue_delimiter)
{

}

mmlBool mmlHTTPSplitter::Split (const mmlChar * str , const mmlInt32 len)
{
	mmlSharedPtr < mmlICStringMemoryInputStream > stream = mmlNewObject(MML_OBJECT_UUID(mmlICStringMemoryInputStream));
	stream->AttachBuffer(str , len);
	return Split(stream);
}

mmlBool mmlHTTPSplitter::Split (mmlICStringInputStream * stream)
{
	mmlInt32 count = 0;
	for(;;)
	{
		mmlAutoPtr < mmlICString > key;
		mmlChar delim[3] = {0};
		delim[0] = mPairDelimiter;
		if ( mKeyValueDelimiter != 0)
		{
			delim[1] = mKeyValueDelimiter;
		}
		if ( stream->ReadUntil(delim , NULL , NULL , NULL , key.getterAddRef()) == mmlFalse ) break;
		if ( key->Length() > 0 )
		{
			if ( mKeyValueDelimiter != 0 )
			{
				mmlChar delim[2] = {0};
				mmlInt32 skipped = 0;
				delim[0] = mKeyValueDelimiter;
				if ( stream->Skip(delim , &skipped) == mmlFalse ) return mmlFalse;
				if ( skipped == 1)
				{
					mmlAutoPtr < mmlICString > value;
					delim[0] = mPairDelimiter;
					if ( stream->ReadUntil(delim , NULL ,  NULL , NULL , value.getterAddRef()) == mmlFalse )
					{
						if ( Handle(key , NULL ) == mmlFalse )
						{
							return mmlFalse;
						}
					}
					else
					{
						stream->Skip(delim , NULL);
						if ( Handle(key , value ) == mmlFalse )
						{
							return mmlFalse;
						}
					}
					count ++;
				}
				else if ( skipped == 0)
				{
					mmlChar delim[2] = {0};
					delim[0] = mPairDelimiter;
					stream->Skip(delim , NULL);
					Handle(key , NULL );
					count ++;
				}
				else
				{
					break;
				}
			}
			else
			{
				mmlChar delim[2] = {0};
				delim[0] = mPairDelimiter;
				stream->Skip(delim , NULL);
				Handle(key , NULL );
				count ++;
			}
		}
		else
		{
			mmlChar delim[2] = {0};
			delim[0] = mPairDelimiter;
			stream->Skip(delim , NULL);
			Handle(key , NULL );
			count ++;
		}
	}
	return count == 0 ? mmlFalse : mmlTrue;
}
