#ifndef MML_DIRECTORY_HEADER_INCLUDED
#define MML_DIRECTORY_HEADER_INCLUDED

#include "mmlIFileSystem.h"
#include <Windows.h>
#include <string>

class mmlDirectory : public mmlIDirectory
{
	MML_OBJECT_DECL;
public:
	mmlDirectory();
	~mmlDirectory();
	mmlBool Open ( const mmlChar * directory );
	mmlBool Enumerate( mmlIDirectoryEnumerator * enumerator );
private:
	WIN32_FIND_DATAA mFindData;
	HANDLE mFindHandle;
	std::string mDirectory;
};


#endif //MML_DIRECTORY_HEADER_INCLUDED

