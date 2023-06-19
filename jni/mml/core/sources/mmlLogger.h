#ifndef MML_LOGGER_HEADER_INCLUDED
#define MML_LOGGER_HEADER_INCLUDED

#include "mmlILogger.h"
#include "mmlPtr.h"
#include <map>
#include <string>

#ifdef MML_WIN
#include "portable/win32/mmlMutex.h"
#endif

#ifdef MML_POSIX
#include "portable/posix/mmlMutex.h"
#endif

#include "mml_time.h"

class mmlLogger : public mmlILogger
{
	MML_OBJECT_DECL;
public:

	mmlLogger();

	mmlBool Create ( const mmlChar * component , mmlILoggerModule ** module );

	mmlBool Attach ( mmlILoggerOutput * output , mmlInt32 * output_id );

	mmlBool Detach ( const mmlInt32 output_id );

	mmlBool Configure ( const mmlChar * cfg );

	mmlBool DoLog ( const mmlChar * str );

	void ShowTime ( const mmlBool enable );

	mmlBool DoShowTime () { return mShowTime; }

private:

	mmlMutex mMutex;

	std::string mCfg;

	std::map < mmlInt32 , mmlAutoPtr < mmlILoggerOutput > > mOutputList;

	mmlInt32 mNextOutputId;

	mmlBool mShowTime;

};


class mmlBinaryLogger : public mmlIBinaryLogger
{
	MML_OBJECT_DECL;
public:
	
	void DoLog (const mmlChar * prefix , void * data , const mmlInt32 data_len , _logger_func func);

};

class mmlMultilineLogger : public mmlIMultilineLogger
{
	MML_OBJECT_DECL;
public:

	void DoLog (const mmlChar * prefix , void * data , const mmlInt32 data_len , _logger_func func);

};

#endif //MML_LOGGER_HEADER_INCLUDED

