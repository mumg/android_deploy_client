#include "mmlDirectory.h"
#include "mmlCString.h"
#include "mmlFileSystem.h"
#include "mml_components.h"
#include "mmlFileSystemOS.h"
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

mmlFileHandle::mmlFileHandle()
	:mFile(mmlNULL), mMaxSize(0) ,mCurrentPos(0)
{
}

mmlFileHandle::~mmlFileHandle()
{
	if ( mFile != mmlNULL )
	{
		fclose(mFile);
	}
	if ( mDoUnlink == mmlTrue )
	{
		unlink(mName->Get());
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
		handle->mFile = fopen(filename, "rb");
	}
	else if ( mode == MODE_WRITE_APPEND )
	{
		handle->mFile = fopen(filename , "ab");
	}
	else
	{
		handle->mFile = fopen(filename , "wb");
	}
	if ( handle->mFile == mmlNULL )
	{
		delete handle;
		handle = mmlNULL;
		return mmlNULL;
	}
	if ( mode != MODE_READ )
	{
		fchmod(fileno(handle->mFile), mask);
	}
	return handle;
}

mmlInt64 mmlFileHandle::Size ()
{
	if ( mFile == mmlNULL )
	{
		return -1;
	}
	size_t cur_pos = ftell(mFile);
	fseek(mFile , 0 , SEEK_END);
	mmlInt64 size = (mmlInt64)ftell(mFile);
	fseek(mFile , cur_pos , SEEK_SET);
	return size;
}

mmlBool mmlFileHandle::Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
{
	if ( mFile == mmlNULL )
	{
		return mmlFalse;
	}
	return fseek(mFile, offset, mode == MML_SEEK_SET ? SEEK_SET : mode == MML_SEEK_CUR ? SEEK_CUR : SEEK_END) != 0 ? mmlFalse : mmlTrue;
}

mmlInt64 mmlFileHandle::Write ( const mmlInt64 size , const void * data )
{
	if ( mFile == mmlNULL )
	{
		return -1;
	}
	mmlInt64 sz = size;
	if ( mMaxSize > 0 && mCurrentPos + sz > mMaxSize )
	{
		sz = mMaxSize - mCurrentPos;
	}
	sz = fwrite(data, 1, sz, mFile);
	mCurrentPos += sz;
	return (mmlInt64)sz;
}

mmlInt64 mmlFileHandle::Read ( const mmlInt64 size , void * data )
{
	if ( mFile == mmlNULL )
	{
		return -1;
	}
	return (mmlInt64)fread(data, 1 , size , mFile);
}

mmlInt64 mmlFileHandle::GetPosition ()
{
	if ( mFile == mmlNULL )
	{
		return -1;
	}
	return (mmlInt64)ftell(mFile);
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
	if ( mFile == mmlNULL )
	{
		return mmlFalse;;
	}
	fflush(mFile);
	return mmlTrue;
}

MML_OBJECT_IMPL0(mmlFileHandle)

void log_core(const mmlChar * formatter , ... );

mmlBool mml_fs_create_dir ( void * arg, const mmlChar * path , const mmlChar * name )
{
	if ( mmlStrLength(path) == 0 ) return mmlTrue;
	log_core("mkdir %s\n", path);
	if ( mml_fs_dir_exists(path) == mmlTrue ) return mmlTrue;
	umask(0022);
	return mkdir(path, 0755) == 0 ? mmlTrue : mmlFalse;
}


void log_core(const mmlChar * formatter , ... );

mmlBool mml_fs_create_temp_dir ( mmlICString ** dirname )
{
	mmlChar temp_path[PATH_MAX] = {0};
	umask(0022);
#if MML_ANDROID
    mmlInt32 offset = mmlSprintf(temp_path, sizeof(temp_path), "%s", "/data/local/tmp/");
#else
	mmlInt32 offset = mmlSprintf(temp_path, sizeof(temp_path), "%s", P_tmpdir);
#endif


	temp_path[offset] = '/';
	offset ++;
	mmlUUID uuid;
	mmlUUIDGenerate(uuid);
	mmlUUIDToStr(uuid, temp_path + offset, sizeof(temp_path) - offset);
	
	    log_core("create temp dir %s\n", temp_path);
		
	if ( mkdir(temp_path, 0777) != 0 )
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
			mmlBool OnFile ( const mmlChar * dir, const mmlChar * file )
			{
				return unlink(file) != 0 ? mmlFalse : mmlTrue;
			}
			mmlBool OnDirectory ( const mmlChar * directory )
			{
				return mml_fs_delete_dir(directory);
			}
		}ds;
		if ( dir->Enumerate(&ds) == mmlTrue )
		{
			return rmdir(dirname) != 0 ? mmlFalse : mmlTrue;
		}
	}
	return mmlFalse;
}

mmlBool mml_fs_delete_file ( const mmlChar * filename )
{
	return ::unlink(filename) != 0 ? mmlFalse : mmlTrue;
}

mmlBool mml_fs_move_file ( const mmlChar * src_filename, const mmlChar * dst_filename)
{
	return ::rename(src_filename, dst_filename) == 0 ? mmlFalse : mmlTrue;
}

#include <pwd.h>
#include <grp.h>

int isnumeric(const mmlChar *str)
{
  while(*str)
  {
    if(!isdigit(*str))
      return 0;
    str++;
  }

  return 1;
}

mmlBool mml_fs_chown_file ( const mmlChar * path, const mmlChar * uid, const mmlChar * gid )
{
	uid_t _uid;
	gid_t _gid;
	if (isnumeric(uid) == 1)
	{
		_uid = mmlStoD(uid, mmlNULL);
	}
	else
	{
		struct passwd * pwd = getpwnam(uid);
		if ( pwd == NULL)
		{
			return mmlFalse;
		}
		_uid = pwd->pw_uid;
	}
	if ( isnumeric(gid) == 1 )
	{
		_gid = mmlStoD(gid, mmlNULL);
	}
	else
	{
		struct group * grp = getgrnam(gid);
		if ( grp == NULL)
		{
			return mmlFalse;
		}
		_gid = grp->gr_gid;
	}
	if ( chown(path, _uid, _gid) == 0 )
	{
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mml_fs_file_exists ( const mmlChar * filename )
{
	struct stat s;
	mmlInt32 stat_result = stat(filename, &s);
	if ( stat_result == 0 )
	{
		if ( (s.st_mode & S_IFMT) == S_IFREG)
		{
			return mmlTrue;
		}
	}
	return mmlFalse;
}

mmlBool mml_fs_dir_exists ( const mmlChar * dirname )
{
	struct stat s;
        mmlInt32 stat_result = stat(dirname, &s);
        if ( stat_result == 0 )
        {       
                if ( (s.st_mode & S_IFMT) == S_IFDIR)
                {       
                        return mmlTrue;
                }       
        }       
        return mmlFalse;
}

mmlBool mml_fs_get_current_dir ( mmlICString ** dirname )
{
	mmlChar dir[PATH_MAX + 1] = {0};
	::getcwd(dir, PATH_MAX);
	*dirname = new mmlCString(dir, mmlStrLength(dir));
	MML_ADDREF(*dirname);
	return mmlTrue;
}

mmlBool mml_fs_set_current_dir ( const mmlChar * dirname )
{
	return ::chdir(dirname) != 0 ? mmlFalse : mmlTrue;
}

mmlBool mml_fs_get_temp_filename ( mmlICString ** filename , const mmlChar * file_ext)
{
	mmlChar tmp_file_name[PATH_MAX] = {0};  
	snprintf(tmp_file_name, sizeof(tmp_file_name) , "/tmp/tmp.XXXXXX");
	if ( mktemp(tmp_file_name) == mmlNULL)
	{
		return mmlFalse;
	}
	*filename = mmlNewCString(tmp_file_name);
	MML_ADDREF(*filename);
	return mmlTrue;
}

