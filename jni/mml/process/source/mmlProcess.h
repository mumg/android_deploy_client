#ifndef MML_PROCESS_IMPL_HEADER_INCLUDED
#define MML_PROCESS_IMPL_HEADER_INCLUDED

#include "mmlIProcess.h"
#include "mmlIThread.h"


#ifdef MML_WIN
#include <Windows.h>
#endif

class mmlProcess : public mmlIProcess
{
	MML_OBJECT_DECL
public:

	mmlProcess ();

	~mmlProcess();
	
	mmlBool Exec (  mmlICString * executable,
				    mmlICString * dir,
		            const mmlInt32 argc,
					const mmlChar ** argv,
					mmlIInputStream * std_in,
		            mmlIOutputStream * std_out,
				    mmlIOutputStream * std_err);

	mmlBool Wait ( const mmlInt32 timeout,
		           mmlBool * finished,
				   mmlInt32 * result);

	mmlBool Shutdown ( const mmlBool kill );


protected:

	enum
	{
		IDLE,
		RUN,
		WAIT_FOR,
		FINISHED
	}mProcessState;

#ifdef MML_WIN
	HANDLE mProcessHandle;
	DWORD mProcessGroupId;

#else
	pid_t	mProcessHandle;

#endif

	mmlSharedPtr < mmlIThread > mSTDIN_Thread;
	mmlSharedPtr < mmlIThread > mSTDOUT_Thread;
	mmlSharedPtr < mmlIThread > mSTDERR_Thread;


};

#ifndef MML_WIN

class mmlProcessPty : public mmlProcess
{
	MML_OBJECT_DECL
public:
	mmlBool Exec (  mmlICString * executable,
				    mmlICString * dir,
		            const mmlInt32 argc,
					const mmlChar ** argv,
					mmlIInputStream * std_in,
		            mmlIOutputStream * std_out,
				    mmlIOutputStream * std_err);
};

#endif

#endif //MML_PROCESS_IMPL_HEADER_INCLUDED
