#ifndef MML_DIRECTORY_HEADER_INCLUDED
#define MML_DIRECTORY_HEADER_INCLUDED

#include "mmlIFileSystem.h"
#include <dirent.h>
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
	std::string mDirectory;
	DIR * mHandle;
};


#endif //MML_DIRECTORY_HEADER_INCLUDED

