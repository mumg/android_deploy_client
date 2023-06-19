#ifdef MML_WIN
#include "portable/win32/mmlMutex.h"
#include "portable/win32/mmlThread.h"
#include "portable/win32/mmlCondition.h"
#include "portable/win32/mmlRandom.h"
#include "portable/win32/mmlSleep.h"
#include "portable/win32/mmlTimer.h"
#include "portable/win32/mmlPerformanceCounter.h"
#ifdef MML_SHARED
#include "portable/win32/mmlComponentLoader.h"
#endif //MML_SHARED
#endif //MML_WIN

#ifdef MML_POSIX
#include "portable/posix/mmlMutex.h"
#include "portable/posix/mmlThread.h"
#include "portable/posix/mmlCondition.h"
#include "portable/posix/mmlRandom.h"
#include "portable/posix/mmlSleep.h"
#include "portable/posix/mmlTimer.h"
#include "portable/posix/mmlPerformanceCounter.h"
#ifdef MML_SHARED
#include "portable/posix/mmlComponentLoader.h"
#endif //MML_SHARED
#endif // MML_POSIX

#include "mmlVariant.h"


#include "mmlObjectQueue.h"
#include "mml_components.h"
#include "mml_services.h"
#include "mmlList.h"
#include "mmlPtr.h"
#include "mmlLogger.h"
#include "mmlCStringUtils.h"
#include "mmlCStringStream.h"
#include "mmlCString.h"
#include "mmlMemoryStream.h"
#include "mmlCommandLine.h"
#include "mmlFileSystem.h"
#include "mmlStateMachine.h"
#include "mmlBufferedStream.h"
#include "mmlStreamCopy.h"
#include "mmlMemoryBufferedQueue.h"
#include <deque>


MML_CONSTRUCTOR_IMPL(mmlMutex);
MML_CONSTRUCTOR_IMPL(mmlThread);
MML_CONSTRUCTOR_IMPL(mmlCondition);
MML_CONSTRUCTOR_IMPL0(mmlObjectQueue);
MML_CONSTRUCTOR_IMPL1(mmlObjectQueue, mmlICondition);
MML_CONSTRUCTOR_IMPL(mmlRandom);
MML_CONSTRUCTOR_IMPL(mmlList);
MML_CONSTRUCTOR_IMPL(mmlLogger);
MML_CONSTRUCTOR_IMPL(mmlBinaryLogger);
MML_CONSTRUCTOR_IMPL(mmlMultilineLogger);
MML_CONSTRUCTOR_IMPL(mmlCStringUtils);
MML_CONSTRUCTOR_IMPL1(mmlCStringInputStream, mmlIInputStream);
MML_CONSTRUCTOR_IMPL1(mmlCStringOutputStream, mmlIOutputStream);
MML_CONSTRUCTOR_IMPL(mmlSleep);
MML_CONSTRUCTOR_IMPL(mmlCString);
MML_CONSTRUCTOR_IMPL(mmlTimer);
MML_CONSTRUCTOR_IMPL(mmlPerformanceCounter);
MML_CONSTRUCTOR_IMPL(mmlVariantStruct);
MML_CONSTRUCTOR_IMPL(mmlVariantInteger);
MML_CONSTRUCTOR_IMPL(mmlVariantDouble);
MML_CONSTRUCTOR_IMPL(mmlVariantBoolean);
MML_CONSTRUCTOR_IMPL(mmlVariantString);
MML_CONSTRUCTOR_IMPL(mmlVariantArray);
MML_CONSTRUCTOR_IMPL(mmlVariantRaw); 
MML_CONSTRUCTOR_IMPL(mmlVariantDateTime);
MML_CONSTRUCTOR_IMPL(mmlCommandLine);
MML_CONSTRUCTOR_IMPL(mmlStateMachine);
MML_CONSTRUCTOR_IMPL(mmlStreamCopy);
MML_CONSTRUCTOR_IMPL1(mmlMemoryInputStream, mmlICString);
MML_CONSTRUCTOR_IMPL1(mmlMemoryInputStream, mmlIBuffer);
MML_CONSTRUCTOR_IMPL1(mmlMemoryInputStream, mmlIMemoryOutputStream);
MML_CONSTRUCTOR_IMPL1(mmlMemoryOutputStreamData, mmlIBuffer);
MML_CONSTRUCTOR_IMPL1(mmlMemoryOutputStreamCString, mmlICString);
MML_CONSTRUCTOR_IMPL(mmlMemoryOutputStreamMutable);
MML_CONSTRUCTOR_IMPL1(mmlMemoryOutputCachedStream, mmlIOutputStream);

MML_CONSTRUCTOR_IMPL1(mmlBufferedInputStream, mmlIInputStream)
MML_CONSTRUCTOR_IMPL1(mmlBufferedOutputStream, mmlIOutputStream)

MML_CONSTRUCTOR_IMPL(mmlFileSystem);
MML_CONSTRUCTOR_IMPL1(mmlFileSystem, mmlICString);

MML_CONSTRUCTOR_IMPL(mmlOutputMemoryBufferedQueue);
MML_CONSTRUCTOR_IMPL1(mmlInputMemoryBufferedQueue, mmlIInputMemoryBufferedQueueReader);
MML_CONSTRUCTOR_IMPL1(mmlOutputMemoryBufferedQueueStream, mmlIOutputMemoryBufferedQueue);

static mmlObjectFactory mml_components [] =
{	
	{
		MML_ICSTRING_UUID,
		"@mml/string",
		MML_CONSTRUCTOR(mmlCString)
	},
	{
		MML_IMUTEX_UUID,
		"@mml/ipc/mutex",
		MML_CONSTRUCTOR(mmlMutex)
	},
	{
		MML_ITHREAD_UUID,
		"@mml/ipc/thread",
		MML_CONSTRUCTOR(mmlThread)
	},
	{
		MML_ICONDITION_UUID,
		"@mml/ipc/condition",
		MML_CONSTRUCTOR(mmlCondition)
	},
	{
		MML_IOBJECT_QUEUE_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlObjectQueue)
	},
	{
		MML_IOBJECT_QUEUE_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlObjectQueue, mmlICondition)
	},
	{
		MML_ISLEEP_UUID,
		"@mml/ipc/sleep",
		MML_CONSTRUCTOR(mmlSleep)
	},
	{
		MML_IRANDOM_UUID,
		"@mml/random",
		MML_CONSTRUCTOR(mmlRandom)
	},
	{
		MML_ILIST_UUID,
		"@mml/list",
		MML_CONSTRUCTOR(mmlList)
	},
	{
		MML_ILOGGER_UUID,
		"@mml/logger",
		MML_CONSTRUCTOR(mmlLogger)
	},
	{
		MML_IBINARY_LOGGER_UUID,
		"@mml/binarylogger",
		MML_CONSTRUCTOR(mmlBinaryLogger)
	},
	{
		MML_IMULTILINE_LOGGER_UUID,
		"@mml/multilinelogger",
		MML_CONSTRUCTOR(mmlMultilineLogger)
	},
	{
		MML_ICSTRING_UTILS_UUID,
		"@mml/stringutils",
		MML_CONSTRUCTOR(mmlCStringUtils)
	},
	{
		MML_ICSTRING_INPUT_STREAM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlCStringInputStream, mmlIInputStream)
	},
	{
		MML_ICSTRING_OUTPUT_STREAM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlCStringOutputStream, mmlIOutputStream)
	},
	{
		MML_ITIMER_UUID,
		"@mml/timer",
		MML_CONSTRUCTOR(mmlTimer)
	},
	{
		MML_IPERFORMANCECOUNTER_UUID,
		"@mml/performancecounter",
		MML_CONSTRUCTOR(mmlPerformanceCounter)
	},
	{
		MML_VARIANT_STRUCT_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlVariantStruct)
	},
	{
		MML_VARIANT_INTEGER_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlVariantInteger)
	},
	{
		MML_VARIANT_DOUBLE_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlVariantDouble)
	},
	{
		MML_VARIABLE_BOOLEAN_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlVariantBoolean)
	},
	{
		MML_VARIANT_STRING_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlVariantString)
	},
	{
		MML_VARIANT_ARRAY_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlVariantArray)
	},
	{
		MML_VARIANT_RAW_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlVariantRaw)
	},
	{
		MML_VARIANT_DATETIME_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlVariantDateTime)
	},
	{
		MML_MEMORY_INPUT_STREAM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlMemoryInputStream, mmlICString)
	},
	{
		MML_MEMORY_INPUT_STREAM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlMemoryInputStream, mmlIBuffer)
	},
	{
		MML_MEMORY_INPUT_STREAM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlMemoryInputStream, mmlIMemoryOutputStream)
	},
	{
		MML_MEMORY_OUTPUT_STREAM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlMemoryOutputStreamData, mmlIBuffer)
	},
	{
		MML_MEMORY_OUTPUT_STREAM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlMemoryOutputStreamCString, mmlICString)
	},
	{
		MML_MEMORY_OUTPUT_STREAM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlMemoryOutputStreamMutable)
	},
	{
		MML_MEMORY_OUTPUT_CACHED_STREAM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlMemoryOutputCachedStream, mmlIOutputStream)
	},
	{
		MML_BUFFERED_INPUT_STREAM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlBufferedInputStream, mmlIInputStream)
	},
	{
		MML_BUFFERED_OUTPUT_STREAM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlBufferedOutputStream, mmlIOutputStream)
	},
	{
		MML_FILE_SYSTEM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlFileSystem)
	},
	{
		MML_FILE_SYSTEM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlFileSystem, mmlICString)
	},
	{
		MML_COMMAND_LINE_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlCommandLine)
	},
	{
		MML_STATE_MACHINE_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlStateMachine)
	},
	{
		MML_STREAM_COPY_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlStreamCopy)
	},
	{
		MML_INPUT_MEMORY_BUFFERED_QUEUE_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlInputMemoryBufferedQueue,mmlIInputMemoryBufferedQueueReader)
	},
	{
		MML_OUTPUT_MEMORY_BUFFERED_QUEUE_UUID,
		mmlNULL,
		MML_CONSTRUCTOR(mmlOutputMemoryBufferedQueue)
	},
	{
		MML_OUTPUT_MEMORY_BUFFERED_QUEUE_STREAM_UUID,
		mmlNULL,
		MML_CONSTRUCTOR1(mmlOutputMemoryBufferedQueueStream,mmlIOutputMemoryBufferedQueue)
	}
};

void mmlAtomicInit ();
void mmlAtomicDestroy ();
void mmlComponentsInit (mmlObjectFactory * factory_map , mmlInt32 size);
mmlBool mmlComponentsConstruct ();
void mmlComponentsDestruct();
void mmlServicesInit ();
void mmlServicesDestroy();
void mmlServicesShutdown();

typedef mmlBool (* mml_components_reg_ext) ( mmlILogger * logger, void * args);
typedef void (*mml_components_unreg_ext)();

mml_components_unreg_ext mml_unreg_ext = mmlNULL;

MML_COMPONENT_LOGGER_IMPL(core);

static mml_component_destructor g_main_dctor = mmlNULL;

mml_core mmlBool mmlInitialize(mmlChar ** argv,
	                           const mmlInt32 argc,
							   mml_components_reg_ext reg,
							   mml_components_unreg_ext unreg,
							   mml_component_constructor main_ctor,
							   mml_component_destructor main_dctor,
                               void * args)
{

	

	mmlBool res = mmlTrue;
	mmlAtomicInit();
	mmlComponentsInit(mml_components , sizeof(mml_components) / sizeof(mmlObjectFactory));

	mmlServicesInit();

	mmlSharedPtr < mmlICommandLine > cmd_line = mmlGetService(MML_OBJECT_UUID(mmlICommandLine));
	if ( cmd_line->Load(argc, (const mmlChar **)argv) == mmlFalse )
	{
		return mmlFalse;
	}
	mmlAutoPtr < mmlICString > logging;
	if ( cmd_line->Get("logging", logging.getterAddRef()) == mmlFalse )
	{
		 logging = mmlCStaticString("+all");
	}

	mmlSetService(MML_OBJECT_UUID(mmlICommandLine), cmd_line);

	mmlSharedPtr < mmlILogger > logger = mmlGetService(MML_OBJECT_UUID(mmlILogger));
	if ( logger != mmlNULL )
	{
		logger->Configure(logging->Get());
	}
	log_init_core(logger);
	mml_unreg_ext = unreg;
	g_main_dctor = main_dctor;
	if ( reg != mmlNULL )
	{
		res = reg(logger, args);
	}
#ifdef MML_SHARED
	mmlComponentLoaderLoad(logger);
#endif
	if ( res == mmlTrue && main_ctor != mmlNULL)
	{
		res = main_ctor();
	}
	if ( res == mmlTrue )
	{
		res = mmlComponentsConstruct();
	}
	return res;
}

mml_core void mmlDestroy ()
{
	mmlServicesShutdown();

	mmlComponentsDestruct();
	if ( g_main_dctor != mmlNULL )
	{
		g_main_dctor();
	}
	if ( mml_unreg_ext != mmlNULL )
	{
		mml_unreg_ext();
	}
#ifdef MML_SHARED
	mmlComponentLoaderDestroy();
#endif
	log_destroy_core();
	mmlServicesDestroy();
	mmlAtomicDestroy();

#ifdef MML_SHARED
	mmlComponentLoaderUnload();
#endif
}


#ifdef WIN32
#pragma comment(lib, "Rpcrt4.lib")
#include <Rpc.h>
#elif MML_ANDROID

#else
#include <uuid/uuid.h>
#endif 

mml_core void mmlUUIDGenerate (mmlUUID & uuid)
{
#ifdef MML_WIN
	UUID new_uuid;
	UuidCreate(&new_uuid);
	uuid.d1 = new_uuid.Data1;
	uuid.d2 = new_uuid.Data2;
	uuid.d3 = new_uuid.Data3;
	uuid.d4 = (mmlUInt16)new_uuid.Data4[0] << 8 | (mmlUInt16)new_uuid.Data4[1];
	mmlMemoryCopy(uuid.b , new_uuid.Data4+2 , sizeof(uuid.b));
#elif MML_ANDROID
	FILE * uuid_file = fopen ("/proc/sys/kernel/random/uuid", "r");
	if ( uuid_file != mmlNULL )
	{
		mmlChar uuid_str[37] = {0};
		if ( fread(uuid_str, 1 , 36, uuid_file) == 36 )
		{
			mmlStrToUUID(uuid_str, uuid);
		}
		fclose(uuid_file);
	}
#else
	uuid_t new_uuid;
	uuid_generate(new_uuid);
	uuid.d1 = ((mmlUInt32)new_uuid[0] << 24) | ((mmlUInt32)new_uuid[1] << 16) | ((mmlUInt32)new_uuid[2] << 8) | ((mmlUInt32)new_uuid[3]);
	uuid.d2 = ((mmlUInt32)new_uuid[4] << 8) | (mmlUInt32)new_uuid[5];
	uuid.d3 = ((mmlUInt32)new_uuid[6] << 8) | (mmlUInt32)new_uuid[7];
	uuid.d4 = ((mmlUInt32)new_uuid[8] << 8) | (mmlUInt32)new_uuid[9];
	mmlMemoryCopy(uuid.b , new_uuid + 11 , sizeof(uuid.b));
#endif
}

mmlChar mmlInt2Hex ( mmlUInt8 integer )
{
	if ( integer >= 0 &&
		 integer < 10 )
	{
		return (mmlChar)integer + '0';
	}
	return (mmlChar)integer - 10 + 'a';
}

void mmlPutHEX ( mmlChar ** cursor , const mmlUInt8 integer )
{
	**cursor = mmlInt2Hex((integer >> 4) & 0x0F);
	(*cursor) ++;
	**cursor = mmlInt2Hex(integer & 0x0F);
	(*cursor) ++;
}

void mmlPutHEXCollection ( mmlChar ** cursor , mmlUInt8 ** collection , const mmlInt32 size )
{
	for ( mmlInt32 index = 0 ; index < size ; index ++ )
	{
		mmlPutHEX(cursor , **collection);
		(*collection) ++;
	}
}

void mmlPutChar ( mmlChar ** cursor , const mmlChar character )
{
	**cursor = character;
	(*cursor) ++;
}

mml_core mmlBool mmlUUIDToStr ( const mmlUUID & uuid , mmlChar * str , const mmlInt32 str_len )
{
	if ( str_len < 38 )
	{
		return mmlFalse;
	}
	mmlChar * str_ptr = str;
	mmlUInt8 * uuid_bytes = (mmlUInt8*) &uuid;
	mmlPutHEXCollection(&str_ptr , &uuid_bytes , 4);
	mmlPutChar(&str_ptr , '-');
	mmlPutHEXCollection(&str_ptr , &uuid_bytes , 2);
	mmlPutChar(&str_ptr , '-');
	mmlPutHEXCollection(&str_ptr , &uuid_bytes , 2);
	mmlPutChar(&str_ptr , '-');
	mmlPutHEXCollection(&str_ptr , &uuid_bytes , 2);
	mmlPutChar(&str_ptr , '-');
	mmlPutHEXCollection(&str_ptr , &uuid_bytes , 6);
	mmlPutChar(&str_ptr , 0);
	return mmlTrue;
}

mmlUInt8 mmlHex2Int ( mmlChar character )
{
	if ( character >= '0' &&
		 character <= '9' )
	{
		return character - '0';
	}
	else if ( character >= 'a' &&
		      character <= 'f' )
	{
		return character + 10 - 'a';
	}
	else if ( character >= 'A' &&
		      character <= 'F' )
	{
		return character + 10 - 'A';
	}
	return mmlUInt8(-1);
}

mmlUInt8 mmlGetHEX ( mmlChar ** cursor )
{
	mmlUInt8 p1,p2;
	p1 = mmlHex2Int(**cursor);
	(*cursor) ++;
	p2 = mmlHex2Int(**cursor);
	(*cursor) ++;
	if ( p1 != mmlUInt8(-1) &&
		 p2 != mmlUInt8(-1) )
	{
		return ((p1 << 4) & 0xF0) |
			   (p2 & 0x0F); 
	}
	return mmlUInt8(-1);
}

mmlBool mmlGetHEXCollection ( mmlChar ** cursor ,  mmlUInt8 ** collection , const mmlInt32 size )
{
	for ( mmlInt32 index = 0 ; index < size ; index ++ )
	{
		mmlUInt8 hex = mmlGetHEX(cursor);
		**collection = hex;
		(*collection) ++;
	}
	return mmlTrue;
}

mmlBool mmlValidateChar ( mmlChar ** cursor , const mmlChar character )
{
	if ( **cursor == character )
	{
		(*cursor) ++;
		return mmlTrue;
	}
	return mmlFalse;
}

mml_core mmlBool mmlStrToUUID ( const mmlChar * str , mmlUUID & uuid )
{
	if ( mmlStrLength(str) < 36 )
	{
		return mmlFalse;
	}
	mmlChar * str_ptr = (mmlChar *)str;
	mmlUInt8 * uuid_bytes = (mmlUInt8*) &uuid;
	if ( mmlGetHEXCollection(&str_ptr , &uuid_bytes , 4) == mmlFalse ) return mmlFalse;
	if ( mmlValidateChar(&str_ptr , '-')  == mmlFalse ) return mmlFalse;
	if ( mmlGetHEXCollection(&str_ptr , &uuid_bytes , 2)  == mmlFalse ) return mmlFalse;
	if ( mmlValidateChar(&str_ptr , '-')  == mmlFalse ) return mmlFalse;
	if ( mmlGetHEXCollection(&str_ptr , &uuid_bytes , 2)  == mmlFalse ) return mmlFalse;
	if ( mmlValidateChar(&str_ptr , '-')  == mmlFalse ) return mmlFalse;
	if ( mmlGetHEXCollection(&str_ptr , &uuid_bytes , 2)  == mmlFalse ) return mmlFalse;
	if ( mmlValidateChar(&str_ptr , '-')  == mmlFalse ) return mmlFalse;
	if ( mmlGetHEXCollection(&str_ptr , &uuid_bytes , 6)  == mmlFalse ) return mmlFalse;
	if ( mmlValidateChar(&str_ptr , 0)  == mmlFalse ) return mmlFalse;
	return mmlTrue;
}

