#include "mmlDirectory.h"
#include <string>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

MML_OBJECT_IMPL1(mmlDirectory , mmlIDirectory)

mmlDirectory::mmlDirectory()
:mHandle(mmlNULL)
{

}

mmlDirectory::~mmlDirectory()
{
	if ( mHandle != mmlNULL )
	{
		closedir(mHandle);
	}
}
mmlBool mmlDirectory::Open ( const mmlChar * directory )
{
	if ( directory != mmlNULL )
	{
		mDirectory = directory;
	}
	else
	{
		mDirectory = ".";
	}
	mHandle = opendir (mDirectory.c_str());
	if ( mHandle == mmlNULL )
	{
		return mmlFalse;
	}
	return mmlTrue;
}

mmlBool mmlDirectory::Enumerate( mmlIDirectoryEnumerator * enumerator )
{
	struct dirent *dir_entry;
	while ( (dir_entry = readdir(mHandle)) != mmlNULL)
	{
		struct stat file_info;
		std::string entry = mDirectory;
		entry += "/";
		entry += dir_entry->d_name;
		stat(entry.c_str(),&file_info);
		switch (file_info.st_mode & S_IFMT) 
		{
			case S_IFDIR:
			{
				if ( strcmp(dir_entry->d_name , ".") != 0 &&
				     strcmp(dir_entry->d_name , "..") != 0 )
				{
					if ( enumerator->OnDirectory(entry.c_str()) == mmlFalse )
					{
						return mmlFalse;
					}
				}
				break;
			}
			case S_IFREG: 
			{
				if ( enumerator->OnFile(mDirectory.c_str(), dir_entry->d_name) == mmlFalse )
				{
					return mmlFalse;
				}
				break;
			}
		}
	}
	return mmlTrue;
}

