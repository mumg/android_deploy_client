#ifndef CORE_SPLITTER_H
#define CORE_SPLITTER_H

#include "mmlICStringStream.h"



class mmlHTTPSplitter 
{
public:
	mmlHTTPSplitter ( const mmlChar pair_delimiter,
		              const mmlChar keyvalue_delimiter = 0);

	mmlBool Split (mmlICStringInputStream * stream);

	mmlBool Split (const mmlChar * str , const mmlInt32 len);

protected:

	virtual mmlBool Handle ( mmlICString * key , mmlICString * value ) = 0;

private:

	mmlChar mPairDelimiter;
	mmlChar mKeyValueDelimiter;

};


#endif
