#include "mmlDirectory.h"
#include "mmlCString.h"
#include "mmlFileSystem.h"
#include "mml_components.h"
#include "mmlIMemoryStream.h"
#include "mmlIBuffer.h"
#include "mmlFileSystemOS.h"

#pragma comment(lib, "Kernel32.lib")

#ifdef MML_WIN
#undef CreateDirectory
#undef DeleteFile
#undef MoveFile
#undef DeleteFile
#undef GetCurrentDirectory
#undef SetCurrentDirectory
#endif


mmlFileHandle::mmlFileHandle()
:mFile(INVALID_HANDLE_VALUE), mMaxSize(0) ,mCurrentPos(0)
{

}

mmlFileHandle::~mmlFileHandle()
{
	if ( mFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle(mFile);
	}
	if ( mDoUnlink == mmlTrue )
	{
		DeleteFileA(mName->Get());
	}
}

mmlFileHandle * mmlFileHandle::Open ( const mmlChar * filename , const MODE_T mode, const mmlBool do_unlink, const mmlUInt32 mask)
{
	mmlFileHandle * handle = new mmlFileHandle();
	handle->mDoUnlink = do_unlink;
	if ( handle->mDoUnlink == mmlTrue )
	{
		handle->mName = mmlNewCString(filename);
	}
	if ( mode == MODE_READ )
	{
		handle->mFile = CreateFile ( filename, GENERIC_READ , FILE_SHARE_READ , mmlNULL , OPEN_EXISTING , 0, mmlNULL);
	}
	else if ( mode == MODE_RW )
	{
		handle->mFile = CreateFile ( filename, GENERIC_ALL, FILE_SHARE_READ , mmlNULL , CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL, mmlNULL);
	}
	else if ( mode == MODE_WRITE )
	{
		handle->mFile = CreateFile ( filename, GENERIC_WRITE, FILE_SHARE_READ , mmlNULL , CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL, mmlNULL);
	}
	else if( mode == MODE_WRITE_APPEND)
	{
		handle->mFile = CreateFile ( filename, GENERIC_WRITE | FILE_APPEND_DATA , FILE_SHARE_READ , mmlNULL , OPEN_ALWAYS , FILE_ATTRIBUTE_NORMAL, mmlNULL);
	}
	if ( handle->mFile == INVALID_HANDLE_VALUE )
	{
		delete handle;
		handle = mmlNULL;
	}
	return handle;
}

mmlInt64 mmlFileHandle::Size ()
{
	if ( mFile == INVALID_HANDLE_VALUE )
	{
		return -1;
	}
	return GetFileSize(mFile, mmlNULL);
}

mmlBool mmlFileHandle::Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
{
	if ( mFile == INVALID_HANDLE_VALUE )
	{
		return mmlFalse;
	}
	mmlInt64 pos = SetFilePointer(mFile, (DWORD)offset, NULL , mode == MML_SEEK_SET ? FILE_BEGIN : mode == MML_SEEK_CUR ? FILE_CURRENT : FILE_END) ? mmlFalse : mmlTrue;
	if ( pos == INVALID_SET_FILE_POINTER )
	{
		return mmlFalse;
	}
	mCurrentPos = pos;
	return mmlTrue;
}

mmlInt64 mmlFileHandle::Write ( const mmlInt64 size , const void * data )
{
	if ( mFile == INVALID_HANDLE_VALUE )
	{
		return -1;
	}
	mmlInt64 sz = size;
	if ( mMaxSize > 0 && mCurrentPos + sz > mMaxSize )
	{
		sz = mMaxSize - mCurrentPos;
	}
	DWORD written = 0;
	BOOL result = WriteFile(mFile, data, (DWORD)sz, &written, mmlNULL);
	if ( result == FALSE )
	{
		return -1;
	}
	mCurrentPos += sz;
	return (mmlInt64)sz;
}

mmlInt64 mmlFileHandle::Read ( const mmlInt64 size , void * data )
{
	DWORD read = 0;
	if ( ReadFile (mFile, data, (DWORD)size, &read, mmlNULL) == FALSE )
	{
		return -1;
	}
	return read;
}

mmlInt64 mmlFileHandle::GetPosition ()
{
	if ( mFile == INVALID_HANDLE_VALUE )
	{
		return -1;
	}
	return SetFilePointer(mFile,0,0,FILE_CURRENT);
}

mmlBool mmlFileHandle::IsFull()
{
	if ( mMaxSize != 0)
	{
		return mCurrentPos >= mMaxSize ? mmlTrue : mmlFalse;
	}
	return mmlFalse;
}

mmlBool mmlFileHandle::SetMaxSize ( const mmlInt64 size )
{
	mMaxSize = size;
	return mmlTrue;
}

mmlBool mmlFileHandle::Flush ()
{
	return mmlTrue;
}

MML_OBJECT_IMPL0(mmlFileHandle)


mmlBool mml_fs_create_dir ( void * arg, const mmlChar * path , const mmlChar * name )
{
	if ( CreateDirectoryA(path, NULL) == FALSE)
	{
		DWORD err = GetLastError();
		if ( err != ERROR_ALREADY_EXISTS )
		{
			return mmlFalse;
		}
	}
	return mmlTrue;
}

mmlBool mml_fs_create_temp_dir ( mmlICString ** dirname )
{
	mmlChar temp_path[MAX_PATH] = {0};
	mmlInt32 offset = GetTempPathA(sizeof(temp_path), temp_path);
	temp_path[offset] = '\\';
	offset ++;
	mmlUUID uuid;
	mmlUUIDGenerate(uuid);
	mmlUUIDToStr(uuid, temp_path + offset, sizeof(temp_path) - offset);
	if ( CreateDirectoryA(temp_path, NULL) == FALSE )
	{
		return mmlFalse;
	}
	*dirname = mmlNewCString(temp_path);
	MML_ADDREF(*dirname);
	return mmlTrue;
}

mmlBool mml_fs_delete_dir ( const mmlChar * dirname )
{
	mmlAutoPtr < mmlIDirectory > dir = new mmlDirectory();
	if ( dir->Open(dirname) == mmlTrue )
	{
		class mml_ds_del : public mmlIDirectoryEnumerator
		{
			MML_OBJECT_STATIC_DECL
		public:
			mmlBool OnFile ( const mmlChar * dir,  const mmlChar * file )
			{
				mmlChar path[MAX_PATH];
				mmlSprintf(path, MAX_PATH, "%s/%s", dir, file);
				return DeleteFileA(path) == FALSE ? mmlFalse : mmlTrue;
			}
			mmlBool OnDirectory ( const mmlChar * directory )
			{
				return mml_fs_delete_dir(directory);
			}
		}ds;
		if ( dir->Enumerate(&ds) == mmlTrue )
		{
			return RemoveDirectory(dirname) == FALSE ? mmlFalse : mmlTrue;
		}
	}
	return mmlFalse;
}


mmlBool mml_fs_delete_file ( const mmlChar * filename )
{
	return ::DeleteFileA(filename) == FALSE ? mmlFalse : mmlTrue;
}

mmlBool mml_fs_move_file ( const mmlChar * src_filename, const mmlChar * dst_filename)
{
	return ::MoveFileA(src_filename, dst_filename) == FALSE ? mmlFalse : mmlTrue;
}

mmlBool mml_fs_chown_file( const mmlChar * path, const mmlChar * uid, const mmlChar * gid )
{
	return mmlTrue;
}

mmlBool mml_fs_file_exists ( const mmlChar * filename )
{
	DWORD dwAttrib = GetFileAttributes(filename);
	return (  dwAttrib != INVALID_FILE_ATTRIBUTES && 
		!(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) ? mmlTrue : mmlFalse ;
}

mmlBool mml_fs_dir_exists ( const mmlChar * dirname )
{
	DWORD dwAttrib = GetFileAttributes(dirname);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
	       	dwAttrib & FILE_ATTRIBUTE_DIRECTORY) ? mmlTrue : mmlFalse ;
}

mmlBool mml_fs_get_current_dir ( mmlICString ** dirname )
{
	mmlChar dir[MAX_PATH + 1] = {0};
	::GetCurrentDirectoryA(sizeof(dir) , dir);
	*dirname = new mmlCString(dir, mmlStrLength(dir));
	MML_ADDREF(*dirname);
	return mmlTrue;
}

mmlBool mml_fs_set_current_dir ( const mmlChar * dirname )
{
	return ::SetCurrentDirectoryA(dirname) == FALSE ? mmlFalse : mmlTrue;
}

#include "mml_atomic.h"

mmlInt32 temp_file_index = 0;

mmlBool mml_fs_get_temp_filename ( mmlICString ** filename, const mmlChar * file_ext)
{
	mmlChar tmp_file_name[MAX_PATH] = {0};  
	mmlChar tmp_path[MAX_PATH] = {0};
	mmlInt32 res = GetTempPathA(MAX_PATH, tmp_path);
	if (res > MAX_PATH || (res == 0))
	{
		return mmlFalse;
	}

	mmlInt32 pid = GetCurrentProcessId();
	for ( mmlInt32 attempt = 0; ; attempt ++ )
	{
		mmlInt32 next_index = mmlAtomicAdd(&temp_file_index, 1);
		mmlSprintf(tmp_file_name, sizeof(tmp_file_name), "%s\\tmp_%d_%d.%s", tmp_path, pid, temp_file_index, file_ext == mmlNULL ? "tmp" : file_ext);
		HANDLE handle = CreateFile(tmp_file_name, GENERIC_WRITE, 0, mmlNULL, CREATE_NEW,FILE_ATTRIBUTE_NORMAL ,mmlNULL);
		if ( handle != INVALID_HANDLE_VALUE )
		{
			CloseHandle(handle);
			break;
		}
		if ( attempt > 100 )
		{
			return mmlFalse;
		}
	}
	*filename = mmlNewCString(tmp_file_name);
	MML_ADDREF(*filename);
	return mmlTrue;
}

