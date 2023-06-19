#ifndef ANDROID_UTILS_HEADER_INCLUDED
#define ANDROID_UTILS_HEADER_INCLUDED

#include "mmlICString.h"
#include "mmlIStream.h"
#include "mmlPtr.h"
#include <list>
#include "mmlICStringStream.h"

void log_variant_log_func( const mmlInt32 level, void * arg, const mmlChar * formatter , ... );


#define ENSURE(c) if (!(c)) {brk(); return mmlFalse;}

mmlBool android_args_parse ( const mmlChar * args, std::list < mmlAutoPtr < mmlICString>  > & args_list );

mmlBool android_get_packages ( std::list < mmlAutoPtr < mmlICString > > & packages );

mmlBool android_run_process(const mmlChar * args, mmlIOutputStream * result, mmlInt32 * exit_code = mmlNULL);

mmlBool android_run_process ( const mmlChar * args , mmlIInputStream ** result , mmlInt32 * exit_code = mmlNULL);

mmlBool android_run_process(const mmlChar * args, mmlICString ** result, mmlInt32 * exit_code = mmlNULL);

mmlBool android_run_process_v(const mmlChar * executable, std::list < mmlAutoPtr < mmlICString > > & args_list, mmlIOutputStream * output, mmlInt32 * result_code);

mmlBool android_file_check_md5(mmlICString * path, mmlICString * md5);

mmlBool android_file_get_md5(mmlICString * path, mmlICString ** md5);

mmlBool android_file_download(mmlICString * url, mmlICString * filename, mmlICString * md5, mmlICString ** fullpath);

mmlBool android_run_process_match(const mmlChar * args, const mmlChar * regexp, const mmlBool full);

mmlBool android_run_process_find(const mmlChar * args, const mmlChar * regexp, mmlICString ** substring);

mmlBool android_run_process_find(const mmlChar * args, const mmlChar * regexp , mmlIList ** sublist);

mmlBool android_query_server(mmlICString * url, mmlIVariantStruct * config, mmlIVariantStruct * args, mmlIVariant ** result);

void android_wake_lock();

void android_wake_unlock();

mmlBool android_is_package_installed(mmlICString * package);

typedef mmlBool (*start_handler_t)();

typedef void (*stop_handler_t)();

mmlBool android_json_serialize (mmlIVariant * object , mmlIBuffer ** data);
mmlBool android_json_deserialize(mmlIBuffer * data, mmlIVariant ** object);

void android_cmd_escape_string(mmlICStringOutputStream * stream, mmlICString * string);

typedef struct  
{
	start_handler_t start;
	stop_handler_t stop;
}START_STOP_MODULE_T;

void android_updater_register_init ( START_STOP_MODULE_T * module );
mmlBool android_updater_init();
void android_updater_destroy();

class android_updater_init_reg
{
private:
	START_STOP_MODULE_T module;
public:
	android_updater_init_reg(start_handler_t init, stop_handler_t destroy )
	{
		module.start = init;
		module.stop = destroy;
		android_updater_register_init(&module);
	}
};

#define ANDROID_INIT_MODULE_I(init) \
	static android_updater_init_reg name(init , mmlNULL );

#define ANDROID_INIT_MODULE_D(destroy) \
	static android_updater_init_reg name(mmlNULL , destroy );

#define ANDROID_INIT_MODULE_ID(init, destroy) \
	static android_updater_init_reg name(init , destroy );

void android_updater_register_startstop ( START_STOP_MODULE_T * module );
mmlBool android_updater_start();
void android_updater_stop();


class android_updater_startstop_reg
{
private:
	START_STOP_MODULE_T module;
public:
	android_updater_startstop_reg(start_handler_t start, stop_handler_t stop )
	{
		module.start = start;
		module.stop = stop;
		android_updater_register_init(&module);
	}
};

mmlIOutputStream * android_log_output();

#define ANDROID_STARTSTOP_MODULE_I(init) \
	static android_updater_startstop_reg name(init , mmlNULL );

#define ANDROID_STARTSTOP_MODULE_D(destroy) \
	static android_updater_startstop_reg name(mmlNULL , destroy );

#define ANDROID_STARTSTOP_MODULE_ID(init, destroy) \
	static android_updater_startstop_reg name(init , destroy );

#define ADD_PROPERTY(source, prop, name, dst) \
	{ \
		mmlAutoPtr < mmlIVariant > var; \
		if (source->Get(prop, var.getterAddRef()) != mmlFalse) \
		{ \
			dst->Set(name, var); \
		} \
	}

void android_get_device_id(mmlICString ** id);



#endif //ANDROID_UTILS_HEADER_INCLUDED
