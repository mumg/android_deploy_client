#ifndef MML_FILE_SYSTEM_OS_HEADER_INCLUDED
#define MML_FILE_SYSTEM_OS_HEADER_INCLUDED

#include "mmlPtr.h"
#include "mmlIMutex.h"

class mmlFileHandle : public mmlIObject
{
	MML_OBJECT_DECL
public:
	mmlFileHandle();

	~mmlFileHandle();

	typedef enum
	{
		MODE_READ,
		MODE_WRITE,
		MODE_WRITE_APPEND,
		MODE_RW
	}MODE_T;

	static mmlFileHandle * Open ( const mmlChar * filename , const MODE_T mode, const mmlBool do_unlink, const mmlUInt32 mask);

	mmlInt64 Size ();

	mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset );

	mmlInt64 Write ( const mmlInt64 size , const void * data );

	mmlInt64 Read ( const mmlInt64 size , void * data );

	mmlInt64 GetPosition ();

	mmlBool IsFull();

	mmlBool SetMaxSize ( const mmlInt64 size );

	mmlInt64 GetMaxSize () { return mMaxSize; }

	mmlBool Flush ();

private:
	mmlInt64 mMaxSize;
	mmlInt64 mCurrentPos;
	HANDLE mFile;
	mmlBool mDoUnlink;
	mmlAutoPtr < mmlICString > mName;
};


mmlBool mml_fs_create_dir ( void * arg, const mmlChar * path , const mmlChar * name );

mmlBool mml_fs_chown_file ( const mmlChar * path, const mmlChar * uid, const mmlChar * gid );

mmlBool mml_fs_create_temp_dir ( mmlICString ** dirname );

mmlBool mml_fs_delete_dir ( const mmlChar * dirname );

mmlBool mml_fs_delete_file ( const mmlChar * filename );

mmlBool mml_fs_move_file ( const mmlChar * src_filename, const mmlChar * dst_filename);

mmlBool mml_fs_file_exists ( const mmlChar * filename );

mmlBool mml_fs_dir_exists ( const mmlChar * dirname );

mmlBool mml_fs_get_current_dir ( mmlICString ** dirname );

mmlBool mml_fs_set_current_dir ( const mmlChar * dirname );

mmlBool mml_fs_get_temp_filename ( mmlICString ** filename , const mmlChar * file_ext = mmlNULL);


#endif //MML_FILE_SYSTEM_IMPL_HEADER_INCLUDED
