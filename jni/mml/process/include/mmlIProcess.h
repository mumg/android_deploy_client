#ifndef MML_PROCESS_HEADER_INCLUDED
#define MML_PROCESS_HEADER_INCLUDED

#include "mmlICString.h"
#include "mmlICommandLine.h"
#include "mmlIStream.h"

#define PROCESS_WAIT_INFINITE -1

#define MML_PROCESS_UUID { 0xEE3AD4A1, 0x6D3A, 0x1014, 0x8818 , { 0x91, 0x69, 0xCC, 0x39, 0x16, 0xA2 } }

class mmlIProcess : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_PROCESS_UUID)
public:
	virtual
	mmlBool Exec (  mmlICString * executable,
                    mmlICString * dir,
					const mmlInt32 argc,
					const mmlChar ** argv,
					mmlIInputStream * std_in,
		            mmlIOutputStream * std_out,
				    mmlIOutputStream * std_err) = 0;
	virtual
	mmlBool Wait ( const mmlInt32 timeout,
		           mmlBool * finished,
				   mmlInt32 * result = mmlNULL ) = 0;
	virtual
	mmlBool Shutdown ( const mmlBool kill = mmlFalse ) = 0;
};

#define MML_PROCESS_PTY_UUID { 0x7A57E70C, 0x6C21, 0x1014, 0xB82C , { 0xBE, 0xFB, 0xED, 0x7B, 0x2F, 0xFF } }

class mmlIProcessPty : public mmlIProcess
{
	MML_OBJECT_UUID_DECL(MML_PROCESS_PTY_UUID)
};

#endif //MML_PROCESS_HEADER_INCLUDED
