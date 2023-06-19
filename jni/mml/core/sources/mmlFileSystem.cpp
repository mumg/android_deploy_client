#include "mmlDirectory.h"
#include "mmlCString.h"
#include "mmlFileSystem.h"
#include "mml_components.h"
#include "mmlIMemoryStream.h"
#include "mmlIBuffer.h"
#include "mmlFileSystemOS.h"


#ifdef MML_WIN
#undef CreateDirectory
#undef DeleteFile
#undef MoveFile
#undef DeleteFile
#undef GetCurrentDirectory
#undef SetCurrentDirectory
#endif

class mmlFileOutputStreamControl : public mmlIOutputStreamControl
{
	MML_OBJECT_DECL
public:
	mmlFileOutputStreamControl(mmlFileHandle * handle)
		:mFile(handle)
	{

	}

	mmlBool SetSignal(mmlIStreamSignal * signal)
	{
		return mmlFalse;
	}

	mmlBool SetTimeout( const mmlInt32 timeout )
	{
		return mmlFalse;
	}

	mmlBool GetTimeout ( mmlInt32 * timeout )
	{
		return mmlFalse;
	}

	mmlInt64 Size ()
	{
		if ( mFile == mmlNULL )
		{
			return -1;
		}
		return mFile->Size();
	}

	mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
	{
		if ( mFile == mmlNULL )
		{
			return mmlFalse;
		}
		return mFile->Seek(mode, offset);
	}

	mmlBool Close ()
	{
		if ( mFile != mmlNULL )
		{
			mFile = mmlNULL;
			return mmlTrue;
		}
		return mmlFalse;
	}



	mmlBool SetMaxSize ( const mmlInt64 size )
	{
		if ( mFile == mmlNULL )
		{
			return mmlFalse;
		}
		return mFile->SetMaxSize(size);
	}

	mmlInt64 GetMaxSize ()
	{
		if ( mFile == mmlNULL )
		{
			return -1;
		}
		return mFile->GetMaxSize();
	}

	mmlInt64 GetPosition()
	{
		if ( mFile == mmlNULL )
		{
			return -1;
		}
		return mFile->GetPosition();
	}

private:

	mmlAutoPtr < mmlFileHandle > mFile;
};

MML_OBJECT_IMPL2(mmlFileOutputStreamControl, mmlIOutputStreamControl, mmlIStreamControl)

class mmlFileOutputStream : public mmlIOutputStream
{
	MML_OBJECT_DECL
public:

	mmlFileOutputStream()
	{

	}

	mmlBool Open ( const mmlChar * filename , const mmlBool append , const mmlUInt32 mode)
	{
		mFile = mmlFileHandle::Open(filename, append == mmlTrue ? mmlFileHandle::MODE_WRITE_APPEND: mmlFileHandle::MODE_WRITE, mmlFalse, mode);
		if ( mFile == mmlNULL )
		{
			return mmlFalse;
		}
		return mmlTrue;
	}
	

	mmlInt64 Write ( const mmlInt64 size , const void * data )
	{
		if ( mFile == mmlNULL )
		{
			return -1;
		}
		return mFile->Write(size, data);
	}

	mmlBool IsFull()
	{
		return mFile->IsFull();
	}

	mmlBool GetControl (  mmlIStreamControl ** control )
	{
		if ( mFile == mmlNULL )
		{
			return mmlFalse;
		}
		*control = new mmlFileOutputStreamControl(mFile);
		MML_ADDREF(*control);
		return mmlTrue;
	}

	mmlBool Close ()
	{
		mFile = mmlNULL;
		return mmlTrue;
	}

	mmlInt64 Size ()
	{
		if ( mFile == mmlNULL) return 0;
		return mFile->Size();
	}

	mmlBool Flush ()
	{
		return mFile->Flush();
	}

protected:

	mmlAutoPtr < mmlFileHandle > mFile;

};

MML_OBJECT_IMPL1(mmlFileOutputStream , mmlIOutputStream)

mmlBool mmlFileSystem::Construct ( mmlICString * current_dir )
{
	mCurrentDir = current_dir;
	return mmlTrue;
}

mmlBool mmlFileSystem::OpenFile ( const mmlChar * filename , const mmlBool append , mmlIOutputStream ** stream , const mmlUInt32 mode)
{
	mmlAutoPtr < mmlFileOutputStream > file = new mmlFileOutputStream();
	if ( file->Open(filename, append, mode) == mmlTrue )
	{
		*stream = file;
		MML_ADDREF(*stream);
		return mmlTrue;
	}
	return mmlFalse;
}

class mmlFileInputStreamControl : public mmlIStreamControl
{
	MML_OBJECT_DECL
public:

	mmlFileInputStreamControl(mmlFileHandle * handle)
		:mFile(handle)
	{

	}

	mmlBool SetSignal(mmlIStreamSignal * signal)
	{
		return mmlFalse;
	}

	mmlBool SetTimeout( const mmlInt32 timeout )
	{
		return mmlFalse;
	}

	mmlBool GetTimeout ( mmlInt32 * timeout )
	{
		return mmlFalse;
	}

	mmlInt64 Size ()
	{
		if ( mFile == mmlNULL )
		{
			return -1;
		}
		return mFile->Size();
	}

	mmlBool Seek ( const MML_SEEK_T mode , const mmlInt64 offset )
	{
		if ( mFile == mmlNULL )
		{
			return mmlFalse;
		}

		return mFile->Seek(mode, offset);
	}

	mmlInt64 GetPosition ()
	{
		if ( mFile == mmlNULL )
		{
			return -1;
		}
		return mFile->GetPosition();
	}

	mmlBool Close ()
	{
		if ( mFile != mmlNULL )
		{
			mFile = mmlNULL;
			return mmlTrue;
		}
		return mmlFalse;
	}

private:

	mmlAutoPtr < mmlFileHandle > mFile;

};

MML_OBJECT_IMPL1(mmlFileInputStreamControl, mmlIStreamControl)

class mmlFileInputStream : public mmlIInputStream
{
	MML_OBJECT_DECL
public:

	mmlBool Open ( const mmlChar * filename )
	{
		mFile = mmlFileHandle::Open(filename, mmlFileHandle::MODE_READ, mmlFalse, MML_ACCESS_DEFAULT);

		return mFile == mmlNULL ? mmlFalse : mmlTrue;
	}

	mmlInt64 Read ( const mmlInt64 size , void * data )
	{
		if ( mFile == mmlNULL )
		{
			return -1;
		}
		return mFile->Read(size, data);
	}

	mmlBool IsEmpty()
	{
		return mFile->GetPosition() >= mFile->Size() ? mmlTrue : mmlFalse;
	}

	mmlBool GetControl ( mmlIStreamControl ** control )
	{
		if ( mFile == mmlNULL )
		{
			return mmlFalse;
		}
		*control = new mmlFileInputStreamControl(mFile);
		MML_ADDREF(*control);
		return mmlTrue;
	}

	mmlInt64 Size ()
	{
		if ( mFile == mmlNULL )
		{
			return 0;
		}
		return mFile->Size();
	}

	mmlBool Close ()
	{
		mFile = mmlNULL;
		return mmlTrue;
	}

	mmlBool Wait ( const mmlInt32 timeout ) { return mmlTrue; }

protected:

	mmlAutoPtr < mmlFileHandle > mFile;
};

MML_OBJECT_IMPL1(mmlFileInputStream , mmlIInputStream)

mmlBool mmlFileSystem::OpenFile ( const mmlChar * filename , mmlIInputStream ** stream )
{
	mmlAutoPtr < mmlFileInputStream > file = new mmlFileInputStream();
	if ( file->Open(filename) == mmlTrue )
	{
		*stream = file;
		MML_ADDREF(*stream);
		return mmlTrue;
	}
	return mmlFalse;
}
mmlBool mmlFileSystem::OpenDirectory ( const mmlChar * dirname , mmlIDirectory ** dir )
{
	mmlAutoPtr < mmlIDirectory > _dir = new mmlDirectory();
	if ( _dir->Open(dirname) == mmlTrue )
	{
		*dir = _dir;
		MML_ADDREF(*dir);
		return mmlTrue;
	}
	return mmlFalse;
}

typedef mmlBool (*path_handle) (void * arg, const mmlChar * path , const mmlChar * name );

static mmlBool mml_fs_process_path ( const mmlChar * path, void * arg, path_handle handle )
{
	mmlInt32 path_size = mmlStrLength(path);
	mmlChar * path_copy = (mmlChar*)mmlAlloc (path_size + 1);
	mmlMemoryCopy(path_copy, path , path_size);
	mmlChar * path_ptr = path_copy;
	mmlChar * name_ptr = path_copy;
	while (*path_ptr != 0)
	{
		if ( *path_ptr == '\\' ||
			 *path_ptr == '/')
		{
			*path_ptr = 0;
			if ( handle (arg, path_copy, name_ptr) == mmlFalse )
			{
				mmlFree(path_copy);
				return mmlFalse;
			}
#ifdef MML_WIN
			*path_ptr = '\\';
#else
			*path_ptr = '/';
#endif
			path_ptr ++;
			name_ptr = path_ptr;
		}
		else
		{
			path_ptr ++;
		}
	}
	if ( handle(arg, path_copy, name_ptr) == mmlFalse )
	{
		mmlFree(path_copy);
		return mmlFalse;
	}
	mmlFree(path_copy);
	return mmlTrue;
}

mmlBool mmlFileSystem::CreateDirectory ( const mmlChar * dirname )
{
	return mml_fs_process_path(dirname, mmlNULL , mml_fs_create_dir);
}

mmlBool mmlFileSystem::DeleteDirectory ( const mmlChar * dirname )
{
	return mml_fs_delete_dir(dirname);
}
mmlBool mmlFileSystem::CreateTempDirectory ( mmlICString ** dirname )
{
	return mml_fs_create_temp_dir(dirname);
}
mmlBool mmlFileSystem::DeleteFile ( const mmlChar * filename )
{
	return mml_fs_delete_file(filename);
}

mmlBool mmlFileSystem::MoveFile ( const mmlChar * src_filename, const mmlChar * dst_filename)
{
	return mml_fs_move_file(src_filename, dst_filename);
}

mmlBool mmlFileSystem::ChownFile ( const mmlChar * filename, const mmlChar * UID, const mmlChar * GID )
{
	return mml_fs_chown_file(filename, UID, GID);
}

mmlBool mmlFileSystem::IsFileExists ( const mmlChar * filename )
{
	return mml_fs_file_exists(filename);
}

mmlBool mmlFileSystem::IsDirExists ( const mmlChar * dirname )
{
	return mml_fs_dir_exists(dirname);
}

mmlBool mmlFileSystem::GetCurrentDirectory ( mmlICString ** dirname )
{
	return mml_fs_get_current_dir(dirname);
}

mmlBool mmlFileSystem::SetCurrentDirectory ( const mmlChar * dirname )
{
	return mml_fs_set_current_dir(dirname);
}

class mmlTempFile : public mmlIObject
{
	MML_OBJECT_DECL
public:
	mmlTempFile(mmlICString * filename, const mmlBool unlink = mmlTrue)
		:mFileName(filename), mUnlink(unlink)
	{

	}

	~mmlTempFile()
	{
		if ( mUnlink == mmlTrue ) mml_fs_delete_file(mFileName->Get());
	}

	const mmlChar * Get ()
	{
		return mFileName->Get();
	}

private:
	mmlAutoPtr < mmlICString > mFileName;
	mmlBool mUnlink;
};

MML_OBJECT_IMPL0(mmlTempFile)

class mmlTempFileOutputStream : public mmlFileOutputStream
{
	MML_OBJECT_DECL
public:
	mmlTempFileOutputStream(mmlTempFile * temp_file)
		:mTempFile(temp_file)
	{
	}

	~mmlTempFileOutputStream()
	{
		Close();
		mTempFile = mmlNULL;
	}

	mmlBool IsClosed ()
	{
		return mFile == mmlNULL ? mmlTrue : mmlFalse;
	}



private:

	mmlAutoPtr < mmlTempFile > mTempFile;

};

MML_OBJECT_IMPL1(mmlTempFileOutputStream, mmlIOutputStream)

#define TEMP_CHECK_AND_OPEN_FILE(res) \
	if ( mOutput->IsClosed() == mmlFalse ) return res; \
	if ( mFile == mmlNULL && (mFile = mmlFileHandle::Open(mTempFile->Get() , mmlFileHandle::MODE_READ, mmlTrue, MML_ACCESS_DEFAULT)) == mmlNULL ) return res; \


class mmlTempFileInputStream : public mmlFileInputStream
{
	MML_OBJECT_DECL
public:
	mmlTempFileInputStream(mmlTempFile * temp_file, mmlTempFileOutputStream * output)
		:mOutput(output), mTempFile(temp_file)
	{
	}

	~mmlTempFileInputStream()
	{
		if ( mFile != mmlNULL )
		{
			mFile = mmlNULL;
		}
		mTempFile = mmlNULL;
	}

	mmlInt64 Read ( const mmlInt64 size , void * data )
	{
		TEMP_CHECK_AND_OPEN_FILE(-1);
		return mmlFileInputStream::Read(size, data);
	}

	mmlBool IsEmpty()
	{
		TEMP_CHECK_AND_OPEN_FILE(mmlFalse);
		return mFile->GetPosition() >= mFile->Size() ? mmlTrue : mmlFalse;
	}

	mmlBool GetControl( mmlIStreamControl ** control )
	{
		TEMP_CHECK_AND_OPEN_FILE(mmlFalse);
		return mmlFileInputStream::GetControl(control);
	}

private:

	mmlAutoPtr < mmlTempFile > mTempFile;
	mmlAutoPtr < mmlTempFileOutputStream > mOutput;
};

MML_OBJECT_IMPL1(mmlTempFileInputStream, mmlIInputStream)


mmlFileSystem::mmlFileSystem()
{
	mMutex = mmlNewObject(MML_OBJECT_UUID(mmlIMutex));
}


class mmlMemoryCacheInputStream : public mmlIInputStream
{
	MML_OBJECT_DECL
public:

	void SetStream ( mmlIInputStream * stream )
	{
		mInput = stream;
	}


	mmlInt64 Read ( const mmlInt64 size , void * data )
	{
		if ( mInput == mmlNULL ) return -1; 
		return mInput->Read(size, data);
	}

	mmlBool IsEmpty ()
	{
		if ( mInput == mmlNULL ) return mmlTrue;
		return mInput->IsEmpty();

	}


	mmlBool GetControl ( mmlIStreamControl ** control )
	{
		if ( mInput == mmlNULL ) return mmlFalse;
		return mInput->GetControl(control);
	}

	mmlInt64 Size ()
	{
		if ( mInput == mmlNULL ) return 0;
		return mInput->Size();
	}

	mmlBool Close ()
	{
		if ( mInput == mmlNULL ) return mmlFalse;
		return mInput->Close();
	}


private:
	mmlAutoPtr < mmlIInputStream > mInput;
};

MML_OBJECT_IMPL1(mmlMemoryCacheInputStream, mmlIInputStream)


static mmlBool mml_fs_open_temp_stream (mmlIOutputStream ** out_stream, mmlIInputStream ** in_stream)
{
	mmlAutoPtr < mmlICString > tmp_file_name;
	if ( mml_fs_get_temp_filename(tmp_file_name.getterAddRef()) == mmlFalse )
	{
		return mmlFalse;
	}

	mmlAutoPtr < mmlTempFile > tmp_file = new mmlTempFile(tmp_file_name);

	mmlAutoPtr < mmlTempFileOutputStream > out = new mmlTempFileOutputStream(tmp_file);
	if ( out->Open(tmp_file->Get(), mmlFalse, MML_ACCESS_DEFAULT) == mmlFalse )
	{
		return mmlFalse;
	}

	*out_stream = out;
	MML_ADDREF(*out_stream);

	*in_stream = new mmlTempFileInputStream(tmp_file, out);
	MML_ADDREF(*in_stream);

	return mmlTrue;
}

mmlBool mmlFileSystem::OpenTempFile ( mmlIOutputStream ** out_stream, mmlICString ** file_name, const mmlChar * file_ext )
{
	mmlAutoPtr < mmlICString > tmp_file_name;
	if ( mml_fs_get_temp_filename(tmp_file_name.getterAddRef(), file_ext) == mmlFalse )
	{
		return mmlFalse;
	}

	mmlAutoPtr < mmlTempFile > tmp_file = new mmlTempFile(tmp_file_name, mmlFalse);

	mmlAutoPtr < mmlTempFileOutputStream > out = new mmlTempFileOutputStream(tmp_file);
	if ( out->Open(tmp_file->Get(), mmlFalse, MML_ACCESS_DEFAULT) == mmlFalse )
	{
		return mmlFalse;
	}
	*file_name = tmp_file_name;
	MML_ADDREF(*file_name);
	*out_stream = out;
	MML_ADDREF(*out_stream);
	return mmlTrue;
}

MML_OBJECT_IMPL1(mmlFileSystem, mmlIFileSystem);
