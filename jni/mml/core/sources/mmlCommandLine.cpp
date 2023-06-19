#include "mmlCommandLine.h"

MML_OBJECT_IMPL1(mmlCommandLine, mmlICommandLine)


mmlCommandLine::mmlCommandLine(std::map < const mmlChar * , mmlAutoPtr < mmlICString > , mml_char_map_predicate_ci > & cmd)
{
	mCommandLine = cmd;
}

mmlBool mmlCommandLine::Load ( const mmlInt32 argc, const mmlChar ** argv )
{
	const mmlChar * key = mmlNULL;
	for ( mmlInt32 arg = 1 ; arg < argc ; arg ++ )
	{
		mmlInt32 len = mmlStrLength(argv[arg]);
		if ( len >= 1)
		{
			if ( argv[arg][0] == '-')
			{
				if ( key != mmlNULL )
				{
					mCommandLine[key] = mmlNULL;
					key = mmlNULL;
				}
				key = argv[arg]+1;
			}
			else
			{
				if ( key == mmlNULL ) return mmlFalse;
				mCommandLine[key] = mmlNewCString(argv[arg]);
			}
		}
		else
		{
			return mmlFalse;
		}
	}
	if ( key != mmlNULL )
	{
		mCommandLine[key] = mmlNULL;
	}
	return mmlTrue;
}

mmlBool mmlCommandLine::Exists ( const mmlChar * name )
{
	std::map < const mmlChar * , mmlAutoPtr < mmlICString > , mml_char_map_predicate_ci >::iterator arg = mCommandLine.find(name);
	if ( arg == mCommandLine.end())
	{
		return mmlFalse;
	}
	return mmlTrue;
}

mmlBool mmlCommandLine::Get ( const mmlChar * name , mmlInt32 & val )
{
	std::map < const mmlChar * , mmlAutoPtr < mmlICString > , mml_char_map_predicate_ci >::iterator arg = mCommandLine.find(name);
	if ( arg == mCommandLine.end())
	{
		return mmlFalse;
	}
	if ( arg->second == mmlNULL )
	{
		return mmlFalse;
	}
	val = (mmlInt32)mmlStoD(arg->second->Get(), mmlNULL);
	return mmlTrue;
}

mmlBool mmlCommandLine::Get ( const mmlChar * name , mmlInt64 & val )
{
	std::map < const mmlChar * , mmlAutoPtr < mmlICString > , mml_char_map_predicate_ci >::iterator arg = mCommandLine.find(name);
	if ( arg == mCommandLine.end())
	{
		return mmlFalse;
	}
	if ( arg->second == mmlNULL )
	{
		return mmlFalse;
	}
	val = mmlStoD(arg->second->Get(), mmlNULL);
	return mmlTrue;
}

mmlBool mmlCommandLine::Get ( const mmlChar * name , mmlBool & val )
{
	std::map < const mmlChar * , mmlAutoPtr < mmlICString > , mml_char_map_predicate_ci >::iterator arg = mCommandLine.find(name);
	if ( arg == mCommandLine.end())
	{
		return mmlFalse;
	}
	if ( arg->second == mmlNULL )
	{
		return mmlFalse;
	}
	mmlBool eq;
	if ( arg->second->CompareStr(MML_EQUAL, "true", eq, MML_CASE_INSENSITIVE) == mmlTrue && eq == mmlTrue )
	{
		val = mmlTrue;
	}
	else if ( arg->second->CompareStr(MML_EQUAL, "false", eq, MML_CASE_INSENSITIVE) == mmlTrue && eq == mmlTrue )
	{
		val = mmlFalse;
	}
	else
	{
		return mmlFalse;
	}
	return mmlTrue;
}

mmlBool mmlCommandLine::Get ( const mmlChar * name, mmlFloat64 & val )
{
	std::map < const mmlChar * , mmlAutoPtr < mmlICString > , mml_char_map_predicate_ci >::iterator arg = mCommandLine.find(name);
	if ( arg == mCommandLine.end())
	{
		return mmlFalse;
	}
	if ( arg->second == mmlNULL )
	{
		return mmlFalse;
	}
	val = mmlStoF(arg->second->Get(), mmlNULL);
	return mmlTrue;
}

mmlBool mmlCommandLine::Get ( const mmlChar * name , mmlICString ** val )
{
	std::map < const mmlChar * , mmlAutoPtr < mmlICString > , mml_char_map_predicate_ci >::iterator arg = mCommandLine.find(name);
	if ( arg == mCommandLine.end())
	{
		return mmlFalse;
	}
	if ( arg->second == mmlNULL )
	{
		return mmlFalse;
	}
	*val = arg->second;
	MML_ADDREF(*val);
	return mmlTrue;
}

mmlBool mmlCommandLine::Clone ( mmlICommandLine ** cmd_line)
{
	*cmd_line = new mmlCommandLine(mCommandLine);
	MML_ADDREF(*cmd_line);
	return mmlTrue;
}

mmlBool mmlCommandLine::Set ( const mmlChar * name , const mmlInt32 & val )
{
	mmlChar value[32];
	mmlSprintf(value, sizeof(value), "%d", val);
	mCommandLine[name] = mmlNewCString(value);
	return mmlTrue;
}

mmlBool mmlCommandLine::Set ( const mmlChar * name , const mmlInt64 & val )
{
	mmlChar value[32];
	mmlSprintf(value, sizeof(value), MML_INT64, val);
	mCommandLine[name] = mmlNewCString(value);
	return mmlTrue;
}

mmlBool mmlCommandLine::Set ( const mmlChar * name , const mmlBool & val )
{
	if ( val == mmlTrue )
	{
		mCommandLine[name] = mmlNewCString("true");
	}
	else
	{
		mCommandLine[name] = mmlNewCString("false");
	}
	return mmlTrue;
}

mmlBool mmlCommandLine::Set ( const mmlChar * name, const mmlFloat64 & val )
{
	mmlChar value[32];
	mmlSprintf(value, sizeof(value), "%f", val);
	mCommandLine[name] = mmlNewCString(value);
	return mmlTrue;
}

mmlBool mmlCommandLine::Set ( const mmlChar * name , mmlICString * val )
{
	mCommandLine[name] = val;
	return mmlTrue;
}

mmlBool mmlCommandLine::Enumerate (mmlICommandLineEnumerator * en )
{
	for ( std::map < const mmlChar * , mmlAutoPtr < mmlICString > , mml_char_map_predicate_ci >::iterator arg = mCommandLine.begin(); arg != mCommandLine.end(); arg ++ )
	{
		if ( en->OnArg(arg->first, arg->second) == mmlFalse )
		{
			return mmlFalse;
		}
	}
	return mmlTrue;
}
