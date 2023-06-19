#ifndef MML_FILE_SYSTEM_HEADER_INCLUDED
#define MML_FILE_SYSTEM_HEADER_INCLUDED

#include "mmlIObject.h"
#include "mmlIStream.h"
#include "mmlICString.h"

#ifdef MML_WIN
#undef CreateDirectory
#undef DeleteFile
#undef MoveFile
#undef DeleteFile
#undef GetCurrentDirectory
#undef SetCurrentDirectory
#endif

class mmlIDirectoryEnumerator : public mmlIObject
{
public:
	virtual mmlBool OnFile ( const mmlChar * dir, const mmlChar * file ) = 0;
	virtual mmlBool OnDirectory ( const mmlChar * directory ) = 0;
};

#define MML_IDIRECTORY_UUID {  0x992e96d2 , 0x6f4c , 0x11df , 0x83dc , { 0x73 , 0x5c , 0xaa , 0x6a , 0x15 , 0xfa } }


class mmlIDirectory : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_IDIRECTORY_UUID);
public:
	virtual mmlBool Open ( const mmlChar * directory ) = 0;
	virtual mmlBool Enumerate ( mmlIDirectoryEnumerator * enumerator ) = 0;
};

#define MML_FILE_SYSTEM_UUID { 0x3E8CD537, 0x6C0C, 0x1014, 0x91A5 , { 0x8F, 0xFE, 0xBA, 0x41, 0x35, 0x6F } }

class mmlIFileSystem : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_FILE_SYSTEM_UUID)
public:

#define MML_ACCESS_ALL_EXEC    0x001
#define MML_ACCESS_ALL_WRITE   0x002
#define MML_ACCESS_ALL_READ    0x004
#define MML_ACCESS_GROUP_EXEC  0x008
#define MML_ACCESS_GROUP_WRITE 0x010
#define MML_ACCESS_GROUP_READ  0x020
#define MML_ACCESS_OWNER_EXEC  0x040
#define MML_ACCESS_OWNER_WRITE 0x080
#define MML_ACCESS_OWNER_READ  0x100

#define MML_ACCESS_DEFAULT MML_ACCESS_OWNER_READ | \
	                       MML_ACCESS_OWNER_WRITE | \
						   MML_ACCESS_GROUP_READ | \
						   MML_ACCESS_GROUP_WRITE | \
						   MML_ACCESS_ALL_READ | \
						   MML_ACCESS_ALL_WRITE

	virtual mmlBool OpenFile ( const mmlChar * filename , const mmlBool append , mmlIOutputStream ** stream , const mmlUInt32 mode = MML_ACCESS_DEFAULT) = 0;
	virtual mmlBool OpenFile ( const mmlChar * filename , mmlIInputStream ** stream ) = 0;
	virtual mmlBool OpenDirectory ( const mmlChar * dirname , mmlIDirectory ** dir ) = 0;
	virtual mmlBool CreateDirectory ( const mmlChar * dirname ) = 0;
	virtual mmlBool DeleteDirectory ( const mmlChar * dirname ) = 0;
	virtual mmlBool CreateTempDirectory ( mmlICString ** dirname ) = 0;
	virtual mmlBool DeleteFile ( const mmlChar * filename ) = 0;
	virtual mmlBool MoveFile ( const mmlChar * src_filename, const mmlChar * dst_filename) = 0;
	virtual mmlBool ChownFile ( const mmlChar * filename, const mmlChar * UID, const mmlChar * GID ) = 0;
	virtual mmlBool IsFileExists ( const mmlChar * filename ) = 0;
	virtual mmlBool IsDirExists ( const mmlChar * dirname ) = 0;
	virtual mmlBool GetCurrentDirectory ( mmlICString ** dirname ) = 0;
	virtual mmlBool SetCurrentDirectory ( const mmlChar * dirname ) = 0;
	virtual mmlBool OpenTempFile ( mmlIOutputStream ** out_stream, mmlICString ** file_name , const mmlChar * file_ext = mmlNULL) = 0;
};

mml_core mmlBool mml_fs_parse ( const mmlChar * path, mmlICString ** dirname, mmlICString ** filename, mmlICString ** basename, mmlICString ** extension);

#endif //MML_FILE_SYSTEM_HEADER_INCLUDED
