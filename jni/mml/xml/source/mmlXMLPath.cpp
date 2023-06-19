#include "mmlXMLPath.h"
#include "mmlICStringUtils.h"
#include "mml_components.h"

MML_OBJECT_IMPL0(mmlXMLPath);

mmlBool mmlXMLPath::Init ( mmlICString * path )
{
	const mmlChar * begin = path->Get();
	mmlInt32 length = 0;
	const mmlChar * str = begin;
	mmlBool is_root = mmlTrue;
	while(*str != 0 )
	{
		if (*str == '/' ||
			*str == '.')
		{
			if ( is_root == mmlTrue )
			{
				is_root = mmlFalse;
				continue;
			}
			else
			{
				mPath.push_back(mmlNewCString(begin , length));
				length = 0;
			}
			str ++;
		}
		str ++;
		length ++;
	}
	mPath.push_back(mmlNewCString(begin));
	return mmlTrue;
}

mmlBool mmlXMLPath::Top ( mmlICString ** top )
{
	if ( mPath.size() > 0 )
	{
		*top = mPath.front();
		MML_ADDREF(*top);
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlXMLPath::Push ( mmlICString * node )
{
	mPath.push_back(node);
	return mmlTrue;
}

mmlBool mmlXMLPath::Pop ()
{
	if ( mPath.size() > 0 )
	{
		mPath.pop_front();
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlXMLPath::HasMore ()
{
	return mPath.size() > 0 ? mmlTrue : mmlFalse;
}


