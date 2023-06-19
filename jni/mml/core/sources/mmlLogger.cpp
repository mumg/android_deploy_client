#include "mmlLogger.h"
#include "mml_memory.h"
#include "mml_strutils.h"
#include <stdio.h>
#if defined(__FreeBSD__)
#include <pthread_np.h>
#endif

#define MML_LOGGER_MAX_MESSAGE_LEN 512

class mmlLoggerModule : public mmlILoggerModule
{
	MML_OBJECT_DECL;
public:

	mmlLoggerModule( mmlILogger * logger , const mmlChar * component )
		:mLogger(logger)
	{
		mBuffer = (mmlChar*)mmlAlloc(MML_LOGGER_MAX_MESSAGE_LEN + 2);
		mBuffer[MML_LOGGER_MAX_MESSAGE_LEN  ] = '\n';
        mBuffer[MML_LOGGER_MAX_MESSAGE_LEN+1] = '0';
	}

	~mmlLoggerModule()
	{
		mmlFree(mBuffer);
	}

	mmlBool Lock ( mmlChar ** buffer , mmlInt32 * maxLen )
	{
		mMutex.Lock();
		*buffer = mBuffer;
		*maxLen = MML_LOGGER_MAX_MESSAGE_LEN;
		return mmlTrue;
	}

	mmlBool UnLock ()
	{
		mLogger->DoLog(mBuffer);
		mMutex.UnLock();
		return mmlTrue;
	}

private:
	mmlMutex mMutex;
	mmlAutoPtr < mmlILogger > mLogger;
	mmlChar * mBuffer;
	mmlInt32 mOffset;
};

MML_OBJECT_IMPL1(mmlLoggerModule , mmlILoggerModule)

MML_OBJECT_IMPL1(mmlLogger , mmlILogger)

mmlLogger::mmlLogger()
:mNextOutputId(0), mShowTime(mmlTrue)
{

}

mmlBool mmlLogger::Create ( const mmlChar * component , mmlILoggerModule ** module )
{
	mmlBool plus_all = (mCfg.find("+all") != std::string::npos) ? mmlTrue : mmlFalse;
	mmlBool minus_all = (mCfg.find("-all") != std::string::npos) ? mmlTrue : mmlFalse;
	std::string component_plus = std::string("+");
	component_plus += component;
	std::string component_minus = std::string("-");
	component_minus += component;
	mmlBool plus_mod = (mCfg.find(component_plus) != std::string::npos) ? mmlTrue : mmlFalse;
	mmlBool minus_mod = (mCfg.find(component_minus) != std::string::npos) ? mmlTrue : mmlFalse;
	mmlBool component_log_enabled = mmlFalse;
	if ( minus_mod == mmlTrue ||              
		(minus_all == mmlTrue && plus_mod == mmlFalse))
	{
		component_log_enabled = mmlFalse;
	}
	else if ( plus_mod == mmlTrue ||
		     (plus_all == mmlTrue && minus_mod == mmlFalse ))
	{
		component_log_enabled = mmlTrue;
	}
	if ( component_log_enabled == mmlTrue)
	{
		*module = new mmlLoggerModule(this, component);
		MML_ADDREF(*module);
	}
	return mmlTrue;
}

mmlBool mmlLogger::Attach ( mmlILoggerOutput * output , mmlInt32 * output_id )
{
	mMutex.Lock();
	mOutputList[mNextOutputId]=output;
	if ( output_id != mmlNULL ) *output_id = mNextOutputId;
	mNextOutputId ++;
	mMutex.UnLock();
	return mmlTrue;
}

mmlBool mmlLogger::Detach( const mmlInt32 output_id )
{
	mMutex.Lock();
	std::map< mmlInt32 , mmlAutoPtr< mmlILoggerOutput > > ::iterator output = mOutputList.find(output_id);
	if ( output != mOutputList.end())
	{
		mOutputList.erase(output);
	}
	mMutex.UnLock();
	return mmlTrue;
}


mmlBool mmlLogger::Configure ( const mmlChar * cfg )
{
	mCfg = cfg;
	return mmlTrue;
}

mmlBool mmlLogger::DoLog ( const mmlChar * str )
{
	mMutex.Lock();
	for ( std::map < mmlInt32 , mmlAutoPtr < mmlILoggerOutput > >::iterator output = mOutputList.begin(); output != mOutputList.end(); output ++ )
	{
		(*output).second->DoLog(str);
	}
	mMutex.UnLock();
	return mmlTrue;
}

MML_OBJECT_IMPL1(mmlBinaryLogger, mmlIBinaryLogger)


static mmlChar ascii [] = 
{
	'.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.',
	'.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.',
	' ' , '!' , '\"', '#' , '$' , '%' , '&', '\'' , '(' , ')' , '*', '+' , ',' , '-' , '.', '/',
	'0' , '1' , '2', '3' , '4' , '5' , '6', '7' , '8' , '9' , ':', ';' , '<' , '=' , '>', '?',
	'@' , 'A' , 'B', 'C' , 'D' , 'E' , 'F', 'G' , 'H' , 'I' , 'J', 'K' , 'L' , 'M' , 'N', 'O',
	'P' , 'Q' , 'R', 'S' , 'T' , 'U' , 'V', 'W' , 'X' , 'Y' , 'Z', '[' , '\\' , ']' , '^', '?',
	'`' , 'a' , 'b', 'c' , 'd' , 'e' , 'f', 'g' , 'h' , 'i' , 'j', 'k' , 'l' , 'm' , 'n', 'o',
	'p' , 'q' , 'r', 's' , 't' , 'u' , 'v', 'w' , 'x' , 'y' , 'z', '{' , ':' , '}' , '~', '.',
	'.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.',
	'.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.',
	'.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.',
	'.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.',
	'.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.',
	'.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.',
	'.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.',
	'.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.' , '.' , '.' , '.', '.'
};


void mmlBinaryLogger::DoLog (const mmlChar * prefix, void * data , const mmlInt32 data_len , _logger_func func)
{
	mmlUInt8 * u_data = (mmlUInt8*) data;
	mmlInt32 bytes_left = data_len;
	while ( bytes_left >= 8 )
	{
		func("%s%0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X %0.2X  %c%c%c%c%c%c%c%c\n",
			 prefix,
			 u_data[0], u_data[1], u_data[2] , u_data[3],
			 u_data[4], u_data[5], u_data[6] , u_data[7],
			 ascii[u_data[0]], ascii[u_data[1]] , ascii[u_data[2]] , ascii[u_data[3]],
			 ascii[u_data[4]], ascii[u_data[5]] , ascii[u_data[6]] , ascii[u_data[7]]);
		u_data += 8;
		bytes_left -= 8;
	}
	if (bytes_left > 0 )
	{
		mmlChar buffer[64] = {0};
		mmlChar * buffer_ptr = buffer;
		mmlInt32 index = 0;
		buffer_ptr += mmlSprintf(buffer , sizeof(buffer) , "%s" , prefix);
		for ( index = 0; index < bytes_left; index ++ )
		{
			buffer_ptr += mmlSprintf(buffer_ptr, 4 , "%0.2X " , u_data[index]);
		}
		for ( index = 0; index < 8 - bytes_left ; index ++ )
		{
			*buffer_ptr = ' ';
			buffer_ptr ++;
			*buffer_ptr = ' ';
			buffer_ptr ++;
			*buffer_ptr = ' ';
			buffer_ptr ++;
		}
		*buffer_ptr = ' ';
		buffer_ptr ++;
		for ( index = 0; index < bytes_left; index ++ )
		{
			buffer_ptr += mmlSprintf(buffer_ptr, 2 , "%c" , ascii[u_data[index]]);
		}
		func("%s\n" , buffer);
	}
}

MML_OBJECT_IMPL1(mmlMultilineLogger, mmlIMultilineLogger)

#define MAX_LEN 256

void mmlMultilineLogger::DoLog (const mmlChar * prefix, void * data , const mmlInt32 data_len , _logger_func func)
{
	mmlChar pre_buffer[MAX_LEN];
	mmlInt32 pre_buffer_index = 0;
	for ( mmlInt32 data_index = 0 ; data_index < data_len ; data_index ++ )
	{
		if ( ((mmlChar*)data)[data_index] == '\r' ||
			 ((mmlChar*)data)[data_index] == '\n' )
		{
			if ( data_index + 1 < data_len )
			{
				if (((mmlChar*)data)[data_index+1] == '\r' ||
					((mmlChar*)data)[data_index+1] == '\n' )
				{
					data_index ++;
				}

			}
			if ( pre_buffer_index > 0 )
			{
				pre_buffer[pre_buffer_index] = 0;
				func("%s%s\n" , prefix , pre_buffer);
				pre_buffer_index = 0;
			}
			else
			{
				func("%s\n" , prefix);
			}
		}
		else
		{
			if ( pre_buffer_index < MAX_LEN-1 )
			{
				pre_buffer[pre_buffer_index] = ascii[((mmlUInt8*)data)[data_index]];
				pre_buffer_index ++;
			}
		}
	}
}

