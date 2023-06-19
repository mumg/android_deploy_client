#include "mmlRegexp.h"
#include "mmlICString.h"
#include "pcrecpp.h"
#include "mml_components.h"

using namespace pcrecpp;

void log_mml_regexp ( const mmlChar * formatter , ... );

MML_OBJECT_IMPL1(mmlRegexp , mmlIRegexp)

mmlBool mmlRegexp::Match ( const char * pattern , mmlICString * string, const mmlBool full)
{
	if ( string == mmlNULL )
	{
		return mmlFalse;
	}
	RE re (pattern);
	if (full == mmlTrue)
	{
		if (re.FullMatch(string->Get()) == false)
		{
			return mmlFalse;
		}
	}
	else
	{
		if (re.PartialMatch(string->Get()) == false)
		{
			return mmlFalse;
		}
	}

	return mmlTrue;
}

mmlBool mmlRegexp::Find(const mmlChar * pattern, mmlICString * string, mmlICString ** result)
{
	mmlAutoPtr< mmlIList > res;
	if ( Find(pattern, string, res.getterAddRef()) == mmlFalse )
	{
		return mmlFalse;
	}
	if ( res == mmlNULL || res->Length() == 0 )
	{
		return mmlFalse;
	}
	mmlAutoPtr< mmlICString > res_str;
	res->Front(res_str.getterAddRef());
	if (res_str == mmlNULL )
	{
		return mmlFalse;
	}
	*result = res_str;
	MML_ADDREF(*result);
	return mmlTrue;
}
mmlBool mmlRegexp::Find(const mmlChar * pattern, mmlICString * string, mmlIList ** result)
{
	pcrecpp::RE regex(pattern);
	pcrecpp::StringPiece input(string->Get());
	std::string match;
	mmlBool res = mmlFalse;

	while (regex.FindAndConsume(&input, &match)) {
		res = mmlTrue;
		if (*result == mmlNULL)
		{
			mmlSharedPtr< mmlIList > lst  = mmlNewObject(MML_OBJECT_UUID(mmlIList));
			*result = lst;
			MML_ADDREF(*result);
		}
		(*result)->PushBack(mmlNewCString(match.c_str()));
	}
	return res;
}