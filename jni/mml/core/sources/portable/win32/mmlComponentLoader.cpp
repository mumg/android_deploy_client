#include <windows.h>
#include <list>
class mmlILogger;

typedef void (*mmlInitComponent_t) (mmlILogger * logger);
typedef void (*mmlDestroyComponent_t) ();

static std::list < HMODULE > mml_components; 

void mmlComponentLoaderLoad(mmlILogger * logger)
{
	char modules_dir[1024];
	GetModuleFileNameA(GetModuleHandle("mml_core.dll") , modules_dir , sizeof(modules_dir));
	int offset = strlen(modules_dir);
	while (modules_dir[offset] != '\\' && offset > 0)
	{offset --;}
	modules_dir[offset] = 0;

	std::string mask (modules_dir);
	mask += "\\*.dll";
	WIN32_FIND_DATA find_data;
	HANDLE find = FindFirstFile(mask.c_str(), &find_data);
	if ( find != INVALID_HANDLE_VALUE )
	{
		do 
		{
			std::string module_path (modules_dir);
			module_path += "\\";
			module_path += find_data.cFileName;
			HMODULE module = LoadLibraryA(module_path.c_str());
			if ( module != NULL )
			{
				mmlInitComponent_t init = (mmlInitComponent_t)GetProcAddress(module , "mmlInitComponent");
				if ( init != NULL )
				{
					init(logger);
					mml_components.push_back(module);
				}
				else
				{
					FreeLibrary(module);
				}
			}
		} while (FindNextFileA(find , &find_data) == TRUE);
		FindClose(find);
	}
	
}

void mmlComponentLoaderDestroy()
{
	for (std::list < HMODULE > ::iterator module = mml_components.begin() ; module != mml_components.end() ; module ++)
	{
		mmlDestroyComponent_t destroy = (mmlDestroyComponent_t)GetProcAddress(*module , "mmlDestroyComponent");
		if ( destroy != NULL )
		{
			destroy();
		}

	}
}

void mmlComponentLoaderUnload ()
{
	while (mml_components.size() > 0 )
	{
		FreeLibrary(mml_components.front());
		mml_components.pop_front();
	}
}
