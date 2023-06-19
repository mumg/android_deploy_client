#ifndef MML_WIN
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>


#define __USE_GNU 1
#define __USE_XOPEN 1
#include <stdlib.h>
#undef __USE_GNU
#undef __USE_XOPEN

#include <errno.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#endif
#include "mmlProcess.h"
#include "mmlICStringStream.h"
#include "mml_components.h"
#include "mmlISleep.h"
#include "mml_time.h"
#include <stdio.h>

void log_process(const mmlChar * formatter, ...);

mmlProcess::mmlProcess ()
	:mProcessState(IDLE)
#ifdef MML_WIN
	, mProcessHandle(INVALID_HANDLE_VALUE), mProcessGroupId(0)
#else
	, mProcessHandle (-1)
#endif
{

}

mmlProcess::~mmlProcess()
{
	Shutdown(mmlTrue);
	Wait(-1, mmlNULL, mmlNULL);
#ifdef MML_WIN
	if ( mProcessHandle != INVALID_HANDLE_VALUE )
	{
		CloseHandle(mProcessHandle);
	}
#endif
	if ( mSTDERR_Thread != mmlNULL )
	{
		mSTDERR_Thread->Join();
		mSTDERR_Thread = mmlNULL;
	}
	if ( mSTDOUT_Thread != mmlNULL )
	{
		mSTDOUT_Thread->Join();
		mSTDOUT_Thread = mmlNULL;
	}
	if ( mSTDIN_Thread != mmlNULL )
	{
		mSTDIN_Thread->Join();
		mSTDIN_Thread = mmlNULL;
	}

}

#ifdef MML_WIN

class mmlHandlePtr
{
public:

	mmlHandlePtr(HANDLE handle = INVALID_HANDLE_VALUE)
		:mHandle(handle)
	{

	}

	~mmlHandlePtr()
	{
		if ( mHandle != INVALID_HANDLE_VALUE )
		{
			CloseHandle(mHandle);
		}
	}

	void Close ()
	{
		if ( mHandle != INVALID_HANDLE_VALUE )
		{
			CloseHandle(mHandle);
			mHandle = INVALID_HANDLE_VALUE;
		}
	}

	operator HANDLE & ()
	{
		return mHandle;
	}

	operator HANDLE * ()
	{
		return &mHandle;
	}

	HANDLE Detach ()
	{
		HANDLE hdl = mHandle;
		mHandle = INVALID_HANDLE_VALUE;
		return hdl;
	}

private:
	HANDLE mHandle;
	HANDLE * mPHandle;
};

class mmlProcessInputRedirect : public mmlIThreadFunction
{
	MML_OBJECT_DECL
	MML_THREAD_DECLARE_SHUTDOWN_FLAG
public:
	mmlProcessInputRedirect(HANDLE handle, mmlIInputStream * input)
		:mHandle(handle), mInput(input)
	{

	}

	void Execute ( mmlIThread * thread )
	{
		mmlUInt8 buffer[4096];
		mmlUInt8 filtered_buffer[4096];
		while(IsShutdown() == mmlFalse )
		{
			mmlInt64 rd = mInput->Read(sizeof(buffer), buffer);
			if ( rd < 0)
			{
				break;
			}
			if ( rd > 0 )
			{
				DWORD written = 0;
				if ( WriteFile (mHandle, (LPCVOID)buffer, (DWORD)rd, &written, NULL) == FALSE )
				{
					break;
				}
			}
		}
	}

private:
	mmlHandlePtr mHandle;
	mmlAutoPtr < mmlIInputStream > mInput;
};

MML_OBJECT_IMPL0(mmlProcessInputRedirect)

class mmlProcessOutputRedirect : public mmlIThreadFunction
{
	MML_OBJECT_DECL
	MML_THREAD_DECLARE_SHUTDOWN_FLAG
public:
	mmlProcessOutputRedirect(HANDLE handle, mmlIOutputStream * output)
		:mHandle(handle), mOutput(output)
	{

	}

	void Execute ( mmlIThread * thread )
	{
		mmlUInt8 buffer[4096];
		while(IsShutdown() == mmlFalse )
		{
			DWORD bytes_read;
			if (!ReadFile(mHandle,buffer ,sizeof(buffer),
				          &bytes_read,NULL) || !bytes_read)
			{
				mOutput->Write(0 , mmlNULL);
				break;
			}
			if ( mOutput->Write(bytes_read, buffer) != bytes_read )
			{
				break;
			}
			if ( mOutput->Flush() == mmlFalse )
			{
				break;
			}
		}
	}

private:
	mmlHandlePtr mHandle;
	mmlAutoPtr < mmlIOutputStream > mOutput;
};

MML_OBJECT_IMPL0(mmlProcessOutputRedirect)

#else

class mmlProcessStreamHandler : public mmlIObject
{
	MML_OBJECT_DECL
private:
	int mFD;
public:
	mmlProcessStreamHandler ( int fd )
	{
		mFD = fd;
	}

	~mmlProcessStreamHandler()
	{
		close(mFD);
	}

	int Get ()
	{
		return mFD;
	}
};

MML_OBJECT_IMPL0(mmlProcessStreamHandler)

class mmlProcessInputRedirect : public mmlIThreadFunction
{
	MML_OBJECT_DECL
		MML_THREAD_DECLARE_SHUTDOWN_FLAG
public:
	mmlProcessInputRedirect(mmlProcessStreamHandler * handle, mmlIInputStream * input)
		:mHandle(handle), mInput(input)
	{

	}


	~mmlProcessInputRedirect()
	{
	}
	void Execute ( mmlIThread * thread )
	{
		mmlUInt8 buffer[4096];
		while(IsShutdown() == mmlFalse)
		{
			mmlInt64 rd = mInput->Read(sizeof(buffer), buffer);
			if ( rd < 0)
			{
				break;
			}
			if ( rd > 0 )
			{
				//log_process("Write to stdin %d %d\n", mHandle->Get(), rd);
				if ( write(mHandle->Get(), buffer, rd) != rd )
				{
					break;
				}
			}
		}
	}

private:
	mmlAutoPtr < mmlProcessStreamHandler >  mHandle;
	mmlAutoPtr < mmlIInputStream > mInput;
};

MML_OBJECT_IMPL0(mmlProcessInputRedirect)

class mmlProcessOutputRedirect : public mmlIThreadFunction
{
	MML_OBJECT_DECL
		MML_THREAD_DECLARE_SHUTDOWN_FLAG
public:
	mmlProcessOutputRedirect(mmlProcessStreamHandler * handle, mmlIOutputStream * output)
		:mHandle(handle), mOutput(output)
	{

	}

	~mmlProcessOutputRedirect()
	{
	}

	void Execute ( mmlIThread * thread )
	{
		mmlUInt8 buffer[4096];
		while(IsShutdown() == mmlFalse)
		{
			int bytes_read = read(mHandle->Get(), buffer, sizeof(buffer));
			//log_process("Read stdout %d %d\n", mHandle->Get(), bytes_read);
			if ( bytes_read <= 0 )
			{
				break;
			}
			if ( mOutput->Write(bytes_read, buffer) != bytes_read )
			{
				break;
			}
			if ( mOutput->Flush() == mmlFalse )
			{
				break;
			}
		}
	}

private:
	mmlAutoPtr < mmlProcessStreamHandler > mHandle;
	mmlAutoPtr < mmlIOutputStream > mOutput;
};

MML_OBJECT_IMPL0(mmlProcessOutputRedirect)

#endif

mmlBool mmlProcess::Exec (  mmlICString * executable,
	                        mmlICString * dir,
							const mmlInt32 argc,
							const mmlChar ** argv,
	                        mmlIInputStream * std_in,
	                        mmlIOutputStream * std_out,
	                        mmlIOutputStream * std_err)
{
#ifdef MML_WIN
	mmlHandlePtr hProcessOUT,hProcessIN,hProcessERR;
	mmlHandlePtr hStreamOUT, hStreamERR, hStreamIN;
	SECURITY_ATTRIBUTES sa;
	sa.nLength= sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(hStreamOUT,hProcessOUT,&sa,0))
	{
		return mmlFalse;
	}

	SetHandleInformation(hStreamOUT, HANDLE_FLAG_INHERIT, 0);

	if (!CreatePipe(hStreamERR,hProcessERR,&sa,0))
	{
		return mmlFalse;
	}

	SetHandleInformation(hStreamERR, HANDLE_FLAG_INHERIT, 0);

	if (!CreatePipe(hProcessIN, hStreamIN, &sa, 0)) 
	{
		return mmlFalse;
	}
	SetHandleInformation(hStreamIN, HANDLE_FLAG_INHERIT, 0);

	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo;
	BOOL bSuccess = FALSE; 
	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO); 
	siStartInfo.hStdError = hProcessERR;
	siStartInfo.hStdOutput = hProcessOUT;
	siStartInfo.hStdInput = hProcessIN;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	MML_CREATE_CSTRING_MEMORY_OUTPUT_STREAM(cmd);
	cmd->WriteFormatted("\"%s\"", executable->Get());
	if ( argc > 0)
	{
		for ( mmlInt32 index = 0 ; index < argc ; index ++ )
		{
			cmd->WriteFormatted(" %s", argv[index]);
		}
	}

	MML_CLOSE_CSTRING_MEMORY_OUTPUT_STREAM(cmd);

	if ( CreateProcess (NULL , (LPSTR)cmd_str->Get() , NULL , NULL , TRUE , CREATE_NO_WINDOW , NULL , dir != mmlNULL ? dir->Get() : mmlNULL , &siStartInfo , &piProcInfo ) == FALSE )
	{
		return mmlFalse;
	}
	else
	{
		if (std_in != mmlNULL)
		{
			mSTDIN_Thread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
			mSTDIN_Thread->Create(new mmlProcessInputRedirect(hStreamIN.Detach(), std_in), PriorityNormal, MML_DEFAULT_STACK_SIZE);
		}

		if ( std_out != mmlNULL )
		{
			mSTDOUT_Thread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
			mSTDOUT_Thread->Create(new mmlProcessOutputRedirect(hStreamOUT.Detach(), std_out), PriorityNormal, MML_DEFAULT_STACK_SIZE);
		}

		if ( std_err != mmlNULL )
		{
			mSTDERR_Thread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
			mSTDERR_Thread->Create(new mmlProcessOutputRedirect(hStreamERR.Detach(), std_err), PriorityNormal, MML_DEFAULT_STACK_SIZE);
		}
		hProcessERR.Close();
		hProcessOUT.Close();
		hProcessIN.Close();
		CloseHandle(piProcInfo.hThread);
		mProcessHandle = piProcInfo.hProcess;
		mProcessState = RUN;
		return mmlTrue;
	}
#else
	int stdin_pipe[2] = {-1,-1};
	int stdout_pipe [2] = {-1,-1};
	int stderr_pipe [2] = { -1, -1};
    
	if ( pipe(stdin_pipe) < 0  ||  pipe(stdout_pipe) < 0 || pipe(stderr_pipe) < 0 )
	{
		log_process("Could not create pipes\n");
		return mmlFalse;
	}

#define PARENT_STDIN stdin_pipe[1]
#define CHILD_STDIN  stdin_pipe[0]

#define PARENT_STDOUT stdout_pipe[0]
#define CHILD_STDOUT  stdout_pipe[1]

#define PARENT_STDERR stderr_pipe[0]
#define CHILD_STDERR  stderr_pipe[1]
	mmlInt32 index;
	mmlChar ** new_argv = (mmlChar**)mmlAlloc( sizeof(mmlChar*) * ( 2 + argc) );
	new_argv[0] = (mmlChar*)executable->Get();
	for ( index = 0 ; index < argc ; index ++ )
	{
		new_argv[1+index] = (mmlChar*)argv[index];
	}
	new_argv[1+index] = mmlNULL;
	if ( (mProcessHandle = fork()) < 0)
	{
		log_process("fork failed\n");
		mmlFree(new_argv);
		return mmlFalse;
	}

	else if ( mProcessHandle == 0 )	/* in the child */
	{
		close(PARENT_STDIN);
		close(PARENT_STDOUT);
		close(PARENT_STDERR);

		dup2(CHILD_STDIN,  0);  close(CHILD_STDIN);
		dup2(CHILD_STDOUT, 1);  close(CHILD_STDOUT);
		dup2(CHILD_STDERR, 2);  close(CHILD_STDERR);
		//child
		if ( execv(executable->Get(), new_argv) == -1)
		{
			exit(errno);
		}
		exit(255);
	}
	else
	{
		mmlFree(new_argv);
		close(CHILD_STDIN);
		close(CHILD_STDOUT);
		close(CHILD_STDERR);
		//parent

		if (std_in != mmlNULL)
		{
			mSTDIN_Thread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
			mSTDIN_Thread->Create(new mmlProcessInputRedirect(new mmlProcessStreamHandler(PARENT_STDIN), std_in), PriorityNormal, MML_DEFAULT_STACK_SIZE);
		}
		else
		{
			close(PARENT_STDIN);
		}

		if ( std_out != mmlNULL )
		{
			mSTDOUT_Thread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
			mSTDOUT_Thread->Create(new mmlProcessOutputRedirect(new mmlProcessStreamHandler(PARENT_STDOUT), std_out), PriorityNormal, MML_DEFAULT_STACK_SIZE);
		}
		else
		{
			close(PARENT_STDOUT);
		}

		if ( std_err != mmlNULL )
		{
			mSTDERR_Thread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
			mSTDERR_Thread->Create(new mmlProcessOutputRedirect(new mmlProcessStreamHandler(PARENT_STDERR), std_err), PriorityNormal, MML_DEFAULT_STACK_SIZE);
		}
		else
		{
			close(PARENT_STDERR);
		}
		mProcessState = RUN;
		//usleep(100000);
	}
#endif
	return mmlTrue;
}

mmlBool mmlProcess::Wait ( const mmlInt32 timeout,
	                       mmlBool * finished,
	                       mmlInt32 * result)
{

	if ( mProcessState == IDLE )
	{
		return mmlFalse;
	}
	else if ( mProcessState == FINISHED )
	{
		if ( result )*result = -1;
		if ( finished ) *finished = mmlTrue;
		return mmlTrue;
	}
#ifdef MML_WIN
	DWORD res = WaitForSingleObject(mProcessHandle, timeout == -1 ? INFINITE : 0);
	if ( res == WAIT_OBJECT_0 )
	{
		BOOL res = TRUE;
		if ( result ) 
		{
			GetExitCodeProcess(mProcessHandle, (LPDWORD)result);
		}
		CloseHandle(mProcessHandle);
		mProcessHandle = INVALID_HANDLE_VALUE;
		mProcessGroupId = 0;
		mProcessState = FINISHED;
		if ( finished ) *finished = mmlTrue;
	}
	else if ( res == WAIT_TIMEOUT )
	{
		if ( finished ) *finished = mmlFalse;
	}
	else
	{
		CloseHandle(mProcessHandle);
		mProcessHandle = INVALID_HANDLE_VALUE;
		mProcessGroupId = 0;
		*finished = mmlTrue;
		mProcessState = FINISHED;
	}
#else
	int status;
	mml_timespec ts;
	mml_clock_gettime_monotonic(&ts);
	mml_clock_add(&ts, timeout);
	mmlSharedPtr < mmlISleep > sleep = mmlNewObject(MML_OBJECT_UUID(mmlISleep));
	for (;;)
	{
		if ( mProcessHandle == 0 )
		{
			return mmlTrue;
		}
		int rc = waitpid(mProcessHandle, &status, timeout == -1 ? 0 : WNOHANG);
		//log_process("rc %d\n", rc);
		if ( rc < 0 )
		{
			if ( result ) *result = -1;
			if (finished) *finished = mmlTrue;
			return mmlTrue;
		}
		if ( rc > 0 && (WIFEXITED(status) || WIFSIGNALED(status))) {
			if ( result ) *result = (status >> 8) & 0xFF;
			if (finished) *finished = mmlTrue;
			mProcessHandle = 0;
			return mmlTrue;
		}
		mml_timespec cts;
		mml_clock_gettime_monotonic(&cts);
		if ( mml_clock_cmp(&cts, &ts) >= 0 )
		{
			if ( finished) *finished = mmlFalse;
			return mmlTrue;
		}
		sleep->Do(5);
	}

#endif
	return mmlTrue;
}

mmlBool mmlProcess::Shutdown ( const mmlBool do_kill )
{
	if ( mProcessState == IDLE ||
		 mProcessState == FINISHED )
	{
		return mmlFalse;
	}
#ifdef MML_WIN
	if ( do_kill == mmlTrue )
	{
		BOOL res = TerminateProcess(mProcessHandle, 0xFFFFFFFF);
		if ( res == TRUE )
		{
			mProcessState = WAIT_FOR;
			return mmlTrue;
		}
	}
	else
	{
		BOOL res = GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, mProcessGroupId);
		if ( res == TRUE )
		{
			mProcessState = WAIT_FOR;
			return mmlTrue;
		}
	}
#else
	if ( mProcessHandle != 0 )
	{
		if ( do_kill == mmlTrue )
		{
			if ( kill(mProcessHandle, SIGKILL) == 0 )
			{
				return mmlTrue;
			}
		}
		else
		{
			if ( kill(mProcessHandle, SIGTERM) == 0 )
			{
				return mmlTrue;
			}
		}
	}
#endif
	return mmlFalse;
}

MML_OBJECT_IMPL1(mmlProcess, mmlIProcess)

#ifndef MML_WIN


#include <termios.h> 
#include <arpa/telnet.h>

static int getpty(char *pts)
{
	int p;

	p = getpt();
	if (p < 0) {
		close(p);
		return -1;
	}
	if (grantpt(p)<0 || unlockpt(p)<0) {
		close(p);
		return -1;
	}
	ptsname_r(p, pts, 32);
	return(p);
}


mmlBool mmlProcessPty::Exec(mmlICString * executable, mmlICString * dir, const mmlInt32 argc, const mmlChar ** argv, mmlIInputStream * std_in, mmlIOutputStream * std_out, mmlIOutputStream * std_err)
{
	if ( std_in == mmlNULL || std_out == mmlNULL )
	{
		return mmlFalse;
	}
	char tty_name[32];
	mmlChar ** new_argv = (mmlChar**)mmlAlloc( sizeof(mmlChar*) * ( 2 + argc) );

	new_argv[0] = (mmlChar*)executable->Get();
	for ( mmlInt32 index = 0 ; index < argc ; index ++ )
	{
		new_argv[1+index] = (mmlChar*)argv[index];
	}
	int pty = getpty(tty_name);
	if ( pty < 0 )
	{
		mmlFree(new_argv);
		return mmlFalse;
	}
	if ( (mProcessHandle = fork()) < 0)
	{
		mmlFree(new_argv);
		return mmlFalse;
	}
	else if ( mProcessHandle == 0 )	/* in the child */
	{
		close(0);
		setsid();
	        int t = open(tty_name, O_RDWR); /* becomes our ctty */
	        dup2(0, 1);
	        dup2(0, 2);
		
		tcsetpgrp(0, getpid());
		ioctl(t, TIOCSCTTY, NULL);

		struct termios termbuf;
		tcgetattr(t, &termbuf);
		termbuf.c_lflag |= ECHO;
		termbuf.c_oflag |= ONLCR|XTABS;
		termbuf.c_iflag |= ICRNL;
		termbuf.c_iflag &= ~IXOFF;
		tcsetattr(t, TCSANOW, &termbuf);

		execv(executable->Get(), new_argv);
		exit(-1);
	}
	else
	{
		mmlFree(new_argv);
		//parent
		if (std_in != mmlNULL)
		{
			mSTDIN_Thread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
			mSTDIN_Thread->Create(new mmlProcessInputRedirect(new mmlProcessStreamHandler(pty), std_in), PriorityNormal, MML_DEFAULT_STACK_SIZE);
		}
		if ( std_out != mmlNULL )
		{
			mSTDOUT_Thread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
			mSTDOUT_Thread->Create(new mmlProcessOutputRedirect(new mmlProcessStreamHandler(dup(pty)), std_out), PriorityNormal, MML_DEFAULT_STACK_SIZE);
		}
		mProcessState = RUN;
	}
	return mmlTrue;
}

MML_OBJECT_IMPL1(mmlProcessPty, mmlIProcess)

#endif
