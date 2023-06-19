#include "mmlDirectory.h"
#include <string>

MML_OBJECT_IMPL1(mmlDirectory , mmlIDirectory)

mmlDirectory::mmlDirectory()
:mFindHandle(INVALID_HANDLE_VALUE)
{

}

mmlDirectory::~mmlDirectory()
{
	if ( mFindHandle != INVALID_HANDLE_VALUE )
	{
		FindClose(mFindHandle);
		mFindHandle = INVALID_HANDLE_VALUE;
	}
}
mmlBool mmlDirectory::Open ( const mmlChar * directory )
{
	if ( directory != mmlNULL )
	{
		mDirectory = directory;
	}
	std::string mask = mDirectory;
	if ( mask.size() > 0 )
	{	
		mask += "\\";
	}
	mask += "*.*";
	mFindHandle = FindFirstFileA(mask.c_str(),&mFindData);
	if ( mFindHandle == INVALID_HANDLE_VALUE )
	{
		return mmlFalse;
	}
	return mmlTrue;
}

mmlBool mmlDirectory::Enumerate( mmlIDirectoryEnumerator * enumerator )
{
	if ( mFindHandle == INVALID_HANDLE_VALUE )
	{
		std::string mask = mDirectory;
		if ( mask.size() > 0 )
		{	
			mask += "\\";
		}
		mask += "*.*";
		mFindHandle = FindFirstFileA(mask.c_str(),&mFindData);
		if ( mFindHandle == INVALID_HANDLE_VALUE )
		{
			return mmlFalse;
		}
	}
	for (;;)
	{
		if ( mFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if ( strcmp(mFindData.cFileName , ".") != 0 &&
				 strcmp(mFindData.cFileName , "..") != 0 )
			{
				std::string subdir = mDirectory;
				subdir += "\\";
				subdir += mFindData.cFileName;
				if ( enumerator->OnDirectory(subdir.c_str()) == mmlFalse )
				{
					FindClose(mFindHandle);
					mFindHandle = INVALID_HANDLE_VALUE;
					return mmlFalse;
				}
			}
		}
		else
		{
			if ( enumerator->OnFile(mDirectory.c_str(), mFindData.cFileName) == mmlFalse )
			{
				FindClose(mFindHandle);
				mFindHandle = INVALID_HANDLE_VALUE;
				return mmlFalse;
			}
		}
		if ( FindNextFileA(mFindHandle , &mFindData) == FALSE )
		{
			break;
		}
	}
	FindClose(mFindHandle);
	mFindHandle = INVALID_HANDLE_VALUE;
	return mmlTrue;
}
