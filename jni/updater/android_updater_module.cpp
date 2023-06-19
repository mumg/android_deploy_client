#include "mml_library.h"
#include "mml_services.h"
#include "mmlIFileSystem.h"
#include "mmlIVariantSerializer.h"
#include "mmlIVariantDeserializer.h"
#include "mmlIThread.h"
#include "android_http_fetch.h"
#include "android_utils.h"
#include <stdio.h>
#include <signal.h>
#include "mmlIStateMachine.h"
#include "mmlIRegexp.h"
#ifndef MML_WIN
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <errno.h>
#endif
#include <map>
#ifdef MML_ANDROID
#include <android/log.h>
#endif
#include "android_utils.h"
#include "android_cmd_dispatcher.h"
#include "android_updater_database.h"

mmlBool do_log = mmlFalse;




MML_COMPONENT_LOGGER_IMPL(main)

class mmlLoggerConsole : public mmlILoggerOutput
{
	MML_OBJECT_DECL;
public:
	mmlLoggerConsole()
	{
	}

	void DoLog ( const mmlChar * str )
	{
		if ( do_log == mmlTrue )
		{
#ifdef MML_ANDROID
		__android_log_print(ANDROID_LOG_VERBOSE, "android_updater", "%s", str);
#endif
		mml_timespec ts;
		mml_clock_gettime_realtime(&ts);
		mml_tm tm;
		mml_clock_localtime(&ts, &tm);
		fprintf(stdout , 
			"%d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d:%0.3d ",
			tm.tm_year , tm.tm_mon , tm.tm_mday, tm.tm_hour , tm.tm_min , tm.tm_sec , tm.tm_msec);
		fprintf(stdout , "%s" , str);
        fflush(stdout);
		}
	}
private:
};

MML_OBJECT_IMPL0(mmlLoggerConsole)

mmlBool android_config_init(mmlIVariantStruct * config);

MML_COMPONENT_DECL(network)
MML_COMPONENT_DECL(crc)
MML_COMPONENT_DECL(md5)
MML_COMPONENT_DECL(base64)
MML_COMPONENT_DECL(sha1)
MML_COMPONENT_DECL(http)
MML_COMPONENT_DECL(process)
MML_COMPONENT_DECL(scheduler)
MML_COMPONENT_DECL(json_serialization)
MML_COMPONENT_DECL(binary_serialization)
MML_COMPONENT_DECL(websocket)
MML_COMPONENT_DECL(regexp)
MML_COMPONENT_DECL(database)

#define MML_JSON_VARIANT_DESERIALIZER_UUID { 0x4383190F, 0x6C70, 0x1014, 0x9A99 , { 0x9C, 0xA2, 0x29, 0x60, 0xE2, 0xE9 } }

const mmlChar * config_file_name = "/data/local/tmp/android_updater.json";

mmlBool main_ctor ()
{
#ifdef MML_WIN
	do_log = mmlTrue;
#endif
	mmlSharedPtr < mmlILogger > logger = mmlGetService(MML_OBJECT_UUID(mmlILogger));
	logger->Attach(new mmlLoggerConsole, mmlNULL);
	log_init_main(logger);

	MML_COMPONENT_INIT(network, logger);
	MML_COMPONENT_INIT(crc, logger);
	MML_COMPONENT_INIT(md5, logger);
	MML_COMPONENT_INIT(base64, logger);
	MML_COMPONENT_INIT(sha1, logger);
	MML_COMPONENT_INIT(http, logger);
	MML_COMPONENT_INIT(process, logger);
	MML_COMPONENT_INIT(scheduler, logger);
	MML_COMPONENT_INIT(json_serialization, logger);
	MML_COMPONENT_INIT(binary_serialization, logger);
	MML_COMPONENT_INIT(websocket, logger);
	MML_COMPONENT_INIT(regexp, logger);
	MML_COMPONENT_INIT(database, logger);
	mmlAutoPtr < mmlIVariantStruct > config;
	mmlSharedPtr < mmlIFileSystem > fs = mmlGetService(MML_OBJECT_UUID(mmlIFileSystem));
	mmlAutoPtr < mmlIInputStream > config_file;
	if (config_file_name != mmlNULL && fs->OpenFile(config_file_name, config_file.getterAddRef()) == mmlTrue )
	{
		mmlUUID uuid = MML_JSON_VARIANT_DESERIALIZER_UUID;
		mmlSharedPtr < mmlIVariantDeserializer > json_deserializer = mmlNewObject(uuid);
		if ( json_deserializer == mmlNULL )
		{
			return mmlFalse;
		}
		if ( json_deserializer->Read(config_file, mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant>(config)) == mmlFalse )
		{
			return mmlFalse;
		}
		if ( config == mmlNULL )
		{
			return mmlFalse;
		}
	}
	else 
	{
		config = mmlNewVariantStruct();
	}


	if ( android_config_init(config) == mmlFalse )
	{
		return mmlFalse;
	}
	mmlAutoPtr < mmlIVariantInteger > debug;
	config->Get("debug", mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant >(debug));
	if ( debug != mmlNULL && debug->Get() == 1 )
	{
		do_log = mmlTrue;
	}
	mmlSetService(MML_GLOBAL_CONFIG_UUID(), config);
	return android_updater_init();
}

void main_dctor()
{
	android_updater_destroy();
	MML_COMPONENT_DESTROY(database);
	MML_COMPONENT_DESTROY(regexp);
	MML_COMPONENT_DESTROY(network);
	MML_COMPONENT_DESTROY(crc);
	MML_COMPONENT_DESTROY(md5);
	MML_COMPONENT_DESTROY(base64);
	MML_COMPONENT_DESTROY(sha1);
	MML_COMPONENT_DESTROY(http);
	MML_COMPONENT_DESTROY(process);
	MML_COMPONENT_DESTROY(scheduler);
	MML_COMPONENT_DESTROY(json_serialization);
	MML_COMPONENT_DESTROY(binary_serialization);
	MML_COMPONENT_DESTROY(websocket);
	log_destroy_main();
}

mmlBool gshutdown = mmlFalse;

void android_terminate_proc(int)
{
	gshutdown = mmlTrue;
}

void android_updater_set_terminate_proc ()
{
#ifndef MML_WIN
	signal (SIGHUP,   SIG_IGN);
	signal (SIGQUIT , android_terminate_proc);
	signal (SIGPIPE, SIG_IGN);
#else
	signal (SIGBREAK, android_terminate_proc);

#endif

	signal (SIGILL, android_terminate_proc);
	signal (SIGTERM, android_terminate_proc);
	signal (SIGINT , android_terminate_proc);
}

#define MML_JSON_VARIANT_SERIALIZER_UUID { 0x407065FF, 0x6C70, 0x1014, 0xBDA0 , { 0xF9, 0xC2, 0xF4, 0x90, 0x1F, 0xFA } }

#define MML_JSON_VARIANT_DESERIALIZER_UUID { 0x4383190F, 0x6C70, 0x1014, 0x9A99 , { 0x9C, 0xA2, 0x29, 0x60, 0xE2, 0xE9 } }

const mmlChar * updater_version = GIT_VERSION;

#ifdef MML_WIN
extern "C" __declspec(dllexport)
#else
extern "C" __attribute__((visibility("default")))
#endif 
const mmlChar * android_updater_version ()
{
	return updater_version;
}

const mmlChar * customer_id;

class android_database_logs_output : public mmlILoggerOutput
{
	MML_OBJECT_DECL
	mmlAutoPtr < mmlIDatabase > mDB;
public:

	android_database_logs_output()
	{
		android_database_get(mDB.getterAddRef());
	}

	void DoLog(const mmlChar * str)
	{
		if (mDB != mmlNULL)
		{
			mml_timespec ts;
			mml_clock_gettime_realtime(&ts);
			mmlAutoPtr < mmlIVariantString > var_str = mmlNewVariantString(mmlNewCString(str));
			mDB->Query("INSERT INTO logs (timestamp, data) VALUES (?,?)", mmlNULL, mmlNewVariantArrayFill(mmlNewVariantInteger(ts.tv_sec), var_str), mmlNULL);
		}
	}
};

MML_OBJECT_IMPL0(android_database_logs_output)

mmlInt32 db_log_id = 0;

void android_database_log_init()
{
	mmlAutoPtr < android_database_logs_output > output = new android_database_logs_output();
	mmlSharedPtr < mmlILogger > logger = mmlGetService(MML_OBJECT_UUID(mmlILogger));
	logger->Attach(output, &db_log_id);
}

void android_database_log_destroy()
{
	mmlSharedPtr < mmlILogger > logger = mmlGetService(MML_OBJECT_UUID(mmlILogger));
	logger->Detach(db_log_id);
}

#ifdef MML_WIN
extern "C" __declspec(dllexport)
#else
extern "C" __attribute__((visibility("default")))
#endif 
int android_updater ( int argc , char ** argv , const char * _customer_id )
{
	customer_id = _customer_id;
	for ( mmlInt32 index = 1 ; index < argc; index ++ )
	{
		if ( mmlStrCompare (argv[index], "-l") == 0 )
		{
			do_log = mmlTrue;
		}
		else if (argv[index][0] != '-')
		{
			config_file_name = argv[index];
		}
	}
	if ( mmlInitialize( mmlNULL, 0, mmlNULL , mmlNULL , main_ctor , main_dctor , mmlNULL) == mmlTrue )
	{
		android_updater_set_terminate_proc();
		mmlSharedPtr < mmlIVariantStruct > config = mmlGetService(MML_GLOBAL_CONFIG_UUID());
		if ( android_database_init() == mmlTrue )
		{
			android_database_log_init();
			log_main("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");

			MML_CREATE_CSTRING_MEMORY_OUTPUT_STREAM(cmd);
#ifndef MML_WIN
			cmd->WriteFormatted("/system/bin/sh /system/bin/");
#endif
			cmd->WriteFormatted("pm uninstall io.appservice.module");
			MML_CLOSE_CSTRING_MEMORY_OUTPUT_STREAM(cmd);
			mmlInt32 exit_code;
			if (android_run_process(
				cmd_str->Get(),
				android_log_output(),
				&exit_code) == mmlFalse || exit_code != 0)
			{
				log_main("Couldn't uninstall service %d\n", exit_code);
			}
			else
			{
				log_main("Service has been uninstalled\n");
			}



			android_updater_start();
			android_cmd_dispatcher_run(&gshutdown);
			android_updater_stop();
			android_database_log_destroy();
		}
		else
		{
			log_main("Could not initialize database\n");
		}
	}
	mmlDestroy();
	return 0;
}
