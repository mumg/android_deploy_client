#ifndef MML_FILE_SYSTEM_IMPL_HEADER_INCLUDED
#define MML_FILE_SYSTEM_IMPL_HEADER_INCLUDED

#include "mmlIFileSystem.h"
#include "mmlPtr.h"
#include "mmlIMutex.h"

#ifdef MML_WIN
#undef CreateDirectory
#undef DeleteFile
#undef MoveFile
#undef DeleteFile
#undef GetCurrentDirectory
#undef SetCurrentDirectory
#endif
/*
Constructors:
1. default, no args
2. filesystem with top level
   0 - mmlICString
*/

class mmlFileSystem : public mmlIFileSystem
{
	MML_OBJECT_DECL
public:
	mmlFileSystem();
	mmlBool Construct ( mmlICString * current_dir );
	mmlBool OpenFile ( const mmlChar * filename , const mmlBool append , mmlIOutputStream ** stream , const mmlUInt32 mode);
	mmlBool OpenFile ( const mmlChar * filename , mmlIInputStream ** stream );
	mmlBool OpenDirectory ( const mmlChar * dirname , mmlIDirectory ** dir );
	mmlBool CreateDirectory ( const mmlChar * dirname );
	mmlBool DeleteDirectory ( const mmlChar * dirname );
	mmlBool CreateTempDirectory ( mmlICString ** dirname );
	mmlBool DeleteFile ( const mmlChar * filename );
	mmlBool MoveFile ( const mmlChar * src_filename, const mmlChar * dst_filename);
	mmlBool ChownFile ( const mmlChar * filename, const mmlChar * UID, const mmlChar * GID );
	mmlBool IsFileExists ( const mmlChar * filename );
	mmlBool IsDirExists ( const mmlChar * dirname );
	mmlBool GetCurrentDirectory ( mmlICString ** dirname );
	mmlBool SetCurrentDirectory ( const mmlChar * dirname );
	mmlBool OpenTempFile ( mmlIOutputStream ** out_stream, mmlICString ** file_name, const mmlChar * file_ext );

private:
	mmlAutoPtr < mmlICString > mCurrentDir;
	mmlSharedPtr < mmlIMutex > mMutex;
};

#endif //MML_FILE_SYSTEM_IMPL_HEADER_INCLUDED
