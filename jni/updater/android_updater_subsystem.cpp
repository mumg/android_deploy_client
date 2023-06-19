#include "mmlIFileSystem.h"
#include "mmlIVariantSerializer.h"
#include "mmlIVariantDeserializer.h"
#include "android_http_fetch.h"
#include "mml_components.h"
#include "mml_services.h"
#include "mmlIProcess.h"
#include "android_utils.h"
#include <string>

void log_main( const mmlChar * formatter , ... );


void android_script_run (const mmlChar * dir, const mmlChar * file)
{
	mmlSharedPtr < mmlIProcess > proc = mmlNewObject(MML_OBJECT_UUID(mmlIProcess));
	if ( proc == mmlNULL )
	{
		return;
	}

	MML_CREATE_CSTRING_MEMORY_OUTPUT_STREAM(path);
	path->WriteFormatted("%s/%s", dir, file);
	MML_CLOSE_CSTRING_MEMORY_OUTPUT_STREAM(path);

	const mmlChar * argv[2] = {0};
	argv[0] = path_str->Get();

	if ( proc->Exec(mmlCStaticString("/system/bin/sh"), mmlNULL, 1 , argv, mmlNULL, android_log_output(), android_log_output()) == mmlFalse )
	{
		return;
	}
	mmlBool finished = mmlFalse;
	mmlInt32 result = -1;
	proc->Wait(PROCESS_WAIT_INFINITE, &finished, &result);
}


void android_subsystem_init ()
{
	mmlSharedPtr < mmlIVariantStruct > global_config = mmlGetService(MML_GLOBAL_CONFIG_UUID());
	mmlAutoPtr < mmlIVariantString > startup_dir;
	if ( global_config->Get("startup", mmlRelativePtrAddRef < mmlIVariantString , mmlIVariant >(startup_dir)) == mmlFalse )
	{
		return;
	}
	mmlSharedPtr < mmlIFileSystem > fs = mmlGetService(MML_OBJECT_UUID(mmlIFileSystem));
	if ( fs->IsDirExists(startup_dir->Get()->Get()) == mmlFalse )
	{
		log_main("Creating startup directory %s\n", startup_dir->Get()->Get());
		if ( fs->CreateDirectory(startup_dir->Get()->Get()) == mmlFalse )
		{
			log_main("Could not create startup directory\n");
			return;
		}
	}
	mmlAutoPtr < mmlIVariantString > shutdown_dir;
	if ( global_config->Get("shutdown", mmlRelativePtrAddRef < mmlIVariantString , mmlIVariant >(shutdown_dir)) == mmlFalse )
	{
		return;
	}
	if ( fs->IsDirExists(shutdown_dir->Get()->Get()) == mmlFalse )
	{
		log_main("Creating shutdown directory %s\n", shutdown_dir->Get()->Get());
		if ( fs->CreateDirectory(shutdown_dir->Get()->Get()) == mmlFalse )
		{
			log_main("Could not create shutdown directory\n");
			return;
		}
	}
}

mmlBool android_subsystem_start ()
{
	log_main("startup begin\n");
	android_subsystem_init();
	mmlSharedPtr < mmlIVariantStruct > global_config = mmlGetService(MML_GLOBAL_CONFIG_UUID());
	mmlAutoPtr < mmlIVariantString > startup_dir;
	if ( global_config->Get("startup", mmlRelativePtrAddRef < mmlIVariantString , mmlIVariant >(startup_dir)) == mmlFalse )
	{
		return mmlTrue;
	}
	mmlSharedPtr < mmlIFileSystem > fs = mmlGetService(MML_OBJECT_UUID(mmlIFileSystem));
	mmlAutoPtr < mmlIDirectory > dir;
	if ( fs->OpenDirectory(startup_dir->Get()->Get(), dir.getterAddRef()) == mmlTrue )
	{
		class android_startup_enumerator : public mmlIDirectoryEnumerator
		{
			MML_OBJECT_STATIC_DECL
		public:
			mmlBool OnFile ( const mmlChar * dir, const mmlChar * file )
			{
				android_script_run(dir, file);
				return mmlTrue;
			}
			mmlBool OnDirectory ( const mmlChar * directory )
			{
				return mmlTrue;
			}
		}se;
		dir->Enumerate(&se);
	}
	log_main("startup end\n");
	return mmlTrue;
}

void android_subsystem_stop ()
{
	log_main("shutdown begin\n");
	mmlSharedPtr < mmlIVariantStruct > global_config = mmlGetService(MML_GLOBAL_CONFIG_UUID());
	if ( global_config == mmlNULL )
	{
		return;
	}
	mmlAutoPtr < mmlIVariantString > shutdown_dir;
	if ( global_config->Get("shutdown", mmlRelativePtrAddRef < mmlIVariantString , mmlIVariant >(shutdown_dir)) == mmlFalse )
	{
		return;
	}
	mmlSharedPtr < mmlIFileSystem > fs = mmlGetService(MML_OBJECT_UUID(mmlIFileSystem));
	mmlAutoPtr < mmlIDirectory > dir;
	if ( fs->OpenDirectory(shutdown_dir->Get()->Get(), dir.getterAddRef()) == mmlTrue )
	{
		class android_shutdown_enumerator : public mmlIDirectoryEnumerator
		{
			MML_OBJECT_STATIC_DECL
		public:
			mmlBool OnFile ( const mmlChar * dir, const mmlChar * file )
			{
				android_script_run(dir, file);
				return mmlTrue;
			}
			mmlBool OnDirectory ( const mmlChar * directory )
			{
				return mmlTrue;
			}
		}se;
		dir->Enumerate(&se);
	}
	log_main("shutdown end\n");
}

ANDROID_STARTSTOP_MODULE_ID(android_subsystem_start,android_subsystem_stop );