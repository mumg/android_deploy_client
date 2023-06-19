#include <dlfcn.h>
#include <dirent.h>
#include <list>
#include <stdio.h>
#include <string.h>
class mmlILogger;

typedef void (*mmlInitComponent_t) (mmlILogger * logger);
typedef void (*mmlDestroyComponent_t) ();

static std::list < void * > mml_components; 

void mmlComponentLoaderLoad(mmlILogger * logger)
{
    DIR *dp;
    struct dirent *dirp;
    dp = opendir (".");
    if ( dp != 0 )
    {
        while ((dirp = readdir(dp)) != 0) 
        {
	    if ( strcmp(dirp->d_name , "..") == 0 ||
                 strcmp(dirp->d_name , ".") == 0 )
	    {
		continue;
            }
            int len = strlen(dirp->d_name);
            if ( len > 3 )
            {
		if ( dirp->d_name[len - 3] != '.' ||
                     dirp->d_name[len - 2] != 's' ||
                     dirp->d_name[len - 1] != 'o' )
		{
                     continue;
                }
	        //fprintf(stdout , "Loading %s\n" , dirp->d_name  ); fflush(stdout);
                void * module = dlopen(dirp->d_name , RTLD_LAZY | RTLD_LOCAL);
			if ( module != 0 )
			{
				mmlInitComponent_t init = (mmlInitComponent_t)dlsym(module , "mmlInitComponent");
				if ( init != 0 )
				{
					init(logger);
					mml_components.push_back(module);
				}
				else
				{
					dlclose(module);

				}
			}
	     }
        } 
        closedir(dp);
    }
}

void mmlComponentLoaderDestroy()
{
	for (std::list < void * > ::iterator module = mml_components.begin() ; module != mml_components.end() ; module ++)
	{
		mmlDestroyComponent_t destroy = (mmlDestroyComponent_t)dlsym(*module , "mmlDestroyComponent");
		if ( destroy != 0 )
		{
			destroy();
		}

	}
}

void mmlComponentLoaderUnload ()
{
	while (mml_components.size() > 0 )
	{
		dlclose(mml_components.front());
		mml_components.pop_front();
	}
}

