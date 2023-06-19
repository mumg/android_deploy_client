    
#include <stdio.h>
#include <string.h>
#ifndef WIN32
#include <unistd.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/wait.h>
#else
#include <Windows.h>
#endif
#include <stdio.h>
#include <fcntl.h>

#ifdef ANDROID
#include <android/log.h>
#endif

void _log(const char * formatter ,...)
{
	va_list arg;
	va_start(arg, formatter);
#ifdef ANDROID
	vfprintf(stdout , formatter , arg);
	fflush(stdout);

#else
	vfprintf(stdout , formatter, arg);
	fflush(stdout);
#endif
	va_end(arg);
}

typedef int(*android_updater_t)(int argc, char ** argv, const char * customer_id);
typedef const char * (*android_updater_version_t)();

android_updater_t android_updater = NULL;
android_updater_version_t android_updater_version = NULL;

#ifdef WIN32

HMODULE android_updater_module = NULL;

int android_updater_module_load()
{
	android_updater_module = LoadLibrary("android_updater_module.dll");
	if (android_updater_module == NULL)
	{
		return 0;
	}
	android_updater = (android_updater_t)GetProcAddress(android_updater_module, "android_updater");
	android_updater_version = (android_updater_version_t)GetProcAddress(android_updater_module, "android_updater_version");
	if (android_updater == NULL || android_updater_version == NULL)
	{
		android_updater_version = NULL;
		android_updater = NULL;
		FreeLibrary(android_updater_module);
		android_updater_module = NULL;
		return 0;
	}
	return 1;

}

int android_updater_module_unload()
{
	if (android_updater_module != NULL)
	{
		android_updater_version = NULL;
		android_updater = NULL;
		FreeLibrary(android_updater_module);
		android_updater_module = NULL;
		return 1;
	}
	return 0;
}


#else

void * android_updater_module = NULL;

int android_updater_module_load()
{
	android_updater_module = dlopen("/data/subsystem/libandroid_updater_module.so", RTLD_LAZY | RTLD_LOCAL);
	if (android_updater_module == NULL)
	{
		_log("could not load android updater module\n");
		return 0;
	}
	android_updater = (android_updater_t)dlsym(android_updater_module, "android_updater");
	android_updater_version = (android_updater_version_t)dlsym(android_updater_module, "android_updater_version");
	if (android_updater == NULL || android_updater_version == NULL)
	{
		_log("could not import api functions");
		android_updater_version = NULL;
		android_updater = NULL;
		dlclose(android_updater_module);
		android_updater_module = NULL;
		return 0;
	}
	return 1;
}

int android_updater_module_unload()
{
	if (android_updater_module != NULL)
	{
		android_updater_version = NULL;
		android_updater = NULL;
		dlclose(android_updater_module);
		android_updater_module = NULL;
		return 1;
	}
	return 0;
}

#endif


int main(int argc, char ** argv)
{
	char ver[16] = { 0 };
	if (android_updater_module_load())
	{
		_log("entering updater module with version %s\n", android_updater_version());
		android_updater(argc, argv, "000000000000000000000000");
	}
	else
	{
		_log("could not load module\n");
	}
	return 0;
}