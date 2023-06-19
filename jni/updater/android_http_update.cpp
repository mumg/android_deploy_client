#include "android_http_update.h"
#include "mmlICStringStream.h"
#include "mmlIFileSystem.h"
#include "mml_md5_uuids.h"
#include "mml_services.h"
#include "mml_components.h"
#include "mmlIProcess.h"
#include "android_http_fetch.h"
#include "mmlIStreamHash.h"
#include "android_updater_database.h"
#include "mmlIThread.h"
#include <string>

void log_main ( const mmlChar * formatter , ... );
void log_main_v( const mmlChar * formatter , va_list vp );

void log_main_variant_log_func( const mmlInt32 level, void * arg, const mmlChar * formatter , ... )
{
	mmlChar _formatter[64];
	mmlMemorySet(_formatter, ' ', level * 2);
	mmlMemoryCopy(_formatter + level * 2, formatter, mmlStrLength(formatter) + 1 );
	va_list va;
	va_start(va, formatter);
	log_main_v(_formatter, va);
	va_end(va);
}

androidHTTP_File::androidHTTP_File (mmlICString * dir, mmlIVariantString * url, mmlIVariantString * file, mmlIVariantString * md5, mmlIVariantInteger * mode)
	:mURL(url), mFileName(file), mMD5(md5), mMode(mode), mDir(dir)
{

}

androidHTTP_File::~androidHTTP_File()
{

}



HTTP_PROCESS_RESULT_T androidHTTP_File::Download ()
{
	log_main("Downloading %s\n", mFileName->Get()->Get());
	MML_CREATE_CSTRING_MEMORY_OUTPUT_STREAM(path_stream);
	path_stream->WriteFormatted("%s/%s", mDir->Get(), mFileName->Get()->Get());
	MML_CLOSE_CSTRING_MEMORY_OUTPUT_STREAM(path_stream);

	mmlSharedPtr < mmlIFileSystem > fs = mmlGetService(MML_OBJECT_UUID(mmlIFileSystem));

	if ( fs->IsFileExists(path_stream_str->Get()) == mmlTrue )
	{
		if ( android_file_check_md5 (path_stream_str, mMD5->Get()) == mmlTrue )
		{
			log_main("File already downloaded\n");
			return HTTP_SUCCESS;
		}
	}


	mmlAutoPtr < mmlIOutputStream > file_body;

	if ( fs->OpenFile(path_stream_str->Get(), mmlTrue, file_body.getterAddRef(), mMode->Get()) == mmlFalse )
	{
		log_main("Could not open file for writing\n");
		return HTTP_ERROR;
	}

	mmlInt32 offset = file_body->Size();

	mmlSharedPtr < mmlIVariantStruct > config = mmlGetService(MML_GLOBAL_CONFIG_UUID());
	mmlAutoPtr < mmlIVariantStruct > file_config;
	if ( config->Get("file", mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant>(file_config)) == mmlFalse )
	{
		return HTTP_ERROR;
	}
	if ( file_config == mmlNULL )
	{
		return HTTP_ERROR;
	}
	HTTP_PROCESS_RESULT_T res;
	if ( (res = android_http_fetch(offset, file_config, mURL->Get(), mmlNULL, mmlNULL, mmlNULL, file_body)) != HTTP_SUCCESS)
	{
		log_main("ERROR: Download failed\n");
		return res;
	}
	log_main("Download complete\n");
	if ( file_body->Close() == mmlFalse )
	{
		return HTTP_ERROR;
	}
	if ( android_file_check_md5 (path_stream_str, mMD5->Get()) != mmlTrue )
	{
		return HTTP_ERROR;
	}
	return HTTP_SUCCESS;
}


void androidHTTP_File::Delete ()
{
	mmlSharedPtr < mmlIFileSystem > fs = mmlGetService(MML_OBJECT_UUID(mmlIFileSystem));
	mmlAutoPtr < mmlICString > path;
	GetPath(path.getterAddRef());
	fs->DeleteFile(path->Get());
}

mmlBool androidHTTP_File::GetPath (mmlICString ** path )
{
	MML_CREATE_CSTRING_MEMORY_OUTPUT_STREAM(path_stream);
	path_stream->WriteFormatted("%s/%s", mDir->Get(), mFileName->Get()->Get());
	MML_CLOSE_CSTRING_MEMORY_OUTPUT_STREAM(path_stream);
	*path = path_stream_str;
	MML_ADDREF(*path);
	return mmlTrue;
}

mmlBool androidHTTP_File::Match ( mmlICString * url )
{
	mmlBool eq;
	if ( mURL->Get()->CompareStr(MML_EQUAL, url->Get(), eq) == mmlTrue )
	{
		if ( eq == mmlTrue )
		{
			return mmlTrue;
		}
	}
	return mmlFalse;
}

mmlBool androidHTTP_File::GetURL(mmlICString ** url)
{
	*url = mURL->Get();
	MML_ADDREF(*url);
	return mmlTrue;
}

mmlBool androidHTTP_File::GetFile ( mmlICString ** name)
{
	*name = mFileName->Get();
	MML_ADDREF(*name);
	return mmlTrue;
}

MML_OBJECT_IMPL0(androidHTTP_File)

androidHTTP_FilesList * androidHTTP_FilesList::Create (mmlICString * dir)
{
	mmlSharedPtr < mmlIFileSystem > fs = mmlGetService(MML_OBJECT_UUID(mmlIFileSystem));
	if ( fs->IsDirExists(dir->Get()) == mmlFalse )
	{
		if ( fs->CreateDirectory(dir->Get()) == mmlFalse )
		{
			return mmlNULL;
		}
	}
	return new androidHTTP_FilesList(dir);
}

androidHTTP_FilesList::androidHTTP_FilesList(mmlICString * dir)
:mDir(dir)
{

}

mmlBool androidHTTP_FilesList::Add ( mmlIVariantStruct * file_desc , androidHTTP_File ** file )
{
	
	mmlAutoPtr < mmlIVariantString > url;
	if ( file_desc->Get("url", mmlRelativePtrAddRef < mmlIVariantString , mmlIVariant >(url)) == mmlFalse )
	{
		log_main("url doesn't exists in response\n");
		return mmlFalse;
	}
	mmlAutoPtr < mmlIVariantString > md5;
	if ( file_desc->Get("md5", mmlRelativePtrAddRef < mmlIVariantString , mmlIVariant >(md5)) == mmlFalse )
	{
		log_main("md5 doesn't exists in response\n");
		return mmlFalse;
	}
	mmlAutoPtr < mmlIVariantString > filename;
	if ( file_desc->Get("file", mmlRelativePtrAddRef < mmlIVariantString , mmlIVariant >(filename)) == mmlFalse )
	{
		log_main("file doesn't exists in response\n");
		return mmlFalse;
	}
	mmlAutoPtr  < mmlIVariantInteger > mode;
	if ( file_desc->Get("mode", mmlRelativePtrAddRef < mmlIVariantInteger , mmlIVariant >(mode)) != mmlTrue )
	{
		mode = mmlNewVariantInteger(MML_ACCESS_DEFAULT);
	}
	for (std::list < mmlAutoPtr < androidHTTP_File > > ::iterator f = mFiles.begin(); f != mFiles.end(); f ++ )
	{
		if ( (*f)->Match(url->Get()) == mmlTrue )
		{
			*file = (*f);
			MML_ADDREF(*file);
			return mmlTrue;
		}
	}
	*file = new androidHTTP_File(mDir, url, filename, md5, mode);
	MML_ADDREF(*file);
	mFiles.push_back(*file);
	return mmlTrue;
}

HTTP_PROCESS_RESULT_T androidHTTP_FilesList::Download (mmlICString ** url)
{
	for (std::list < mmlAutoPtr < androidHTTP_File > > ::iterator f = mFiles.begin(); f != mFiles.end(); f ++ )
	{
		HTTP_PROCESS_RESULT_T res = (*f)->Download();
		if ( res != HTTP_SUCCESS )
		{
			(*f)->GetURL(url);
			return res;
		}
	}
	return HTTP_SUCCESS;
}

mmlBool androidHTTP_FilesList::Delete ()
{
	for (std::list < mmlAutoPtr < androidHTTP_File > > ::iterator f = mFiles.begin(); f != mFiles.end(); f ++ )
	{
		(*f)->Delete();
	}
	return mmlTrue;
}

MML_OBJECT_IMPL0(androidHTTP_FilesList)

androidHTTP_Update::androidHTTP_Update()
:mReboot(mmlFalse), mRebootImmediate(mmlFalse), mRebootBegin(-1), mRebootEnd(-1)
{
}

androidHTTP_Update::~androidHTTP_Update()
{
}


mmlBool androidHTTP_Update::Create (mmlIVariantStruct * update_request  , androidHTTP_Update ** update )
{
	mmlSharedPtr < mmlIVariantStruct > global_config = mmlGetService(MML_GLOBAL_CONFIG_UUID());

	mmlAutoPtr < mmlIVariantString > temp;

	if ( global_config->Get("temp", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(temp)) == mmlFalse )
	{
		log_main("temp directory has not been defined\n");
		return mmlFalse;
	}

	mmlAutoPtr < androidHTTP_Update > http_update = new androidHTTP_Update();


	mmlAutoPtr < mmlIVariantStruct > reboot;
	if ( update_request->Get("reboot", mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant>(reboot)) == mmlTrue )
	{
		mmlAutoPtr < mmlIVariantString > type;
		if ( reboot->Get("type", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant>(type)) == mmlTrue )
		{
			if ( type != mmlNULL && type->Get() != mmlNULL )
			{
				mmlBool eq;
				if ( type->Get()->CompareStr(MML_EQUAL, "immediate", eq) == mmlTrue && eq == mmlTrue )
				{
					http_update->mReboot = mmlTrue;
					http_update->mRebootImmediate = mmlTrue;
				}
				else if ( type->Get()->CompareStr(MML_EQUAL, "scheduled", eq) == mmlTrue && eq == mmlTrue )
				{
					mmlAutoPtr < mmlIVariantStruct > period;
					if ( reboot->Get("period", mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant>(period)) == mmlTrue )
					{
						mmlAutoPtr < mmlIVariantInteger > begin;
						mmlAutoPtr < mmlIVariantInteger > end;
						if ( period->Get("begin", mmlRelativePtrAddRef < mmlIVariantInteger , mmlIVariant>(begin)) == mmlTrue &&
							period->Get("end", mmlRelativePtrAddRef < mmlIVariantInteger, mmlIVariant>(end)) == mmlTrue )
						{
							if ( end != mmlNULL && 
								begin != mmlNULL )
							{
								if ( begin->Get() > end->Get() )
								{
									return mmlFalse;
								}
								http_update->mReboot = mmlTrue;
								http_update->mRebootImmediate = mmlFalse;
								http_update->mRebootBegin = begin->Get();
								http_update->mRebootEnd = end->Get();
							}
						}
						else
						{
							return mmlFalse;
						}

					}
					else
					{
						return mmlFalse;
					}

				}
				else
				{
					return mmlFalse;
				}
			}
		}
	}

	log_main("update directory %s\n", temp->Get()->Get());
	http_update->mFilesList = androidHTTP_FilesList::Create(temp->Get());
	if ( http_update->mFilesList == mmlNULL )
	{
		log_main("Could not create file list\n");
		return mmlFalse;
	}

	mmlAutoPtr < mmlIVariantArray > json_update;
	if ( update_request->Get("update", mmlRelativePtrAddRef < mmlIVariantArray, mmlIVariant>(json_update)) == mmlTrue )
	{
		for ( mmlInt32 index = 0 ; index < json_update->Size(); index ++ )
		{
			mmlAutoPtr < mmlIVariantStruct > update_item;
			if ( json_update->Get(index, mmlRelativePtrAddRef < mmlIVariantStruct , mmlIVariant > (update_item)) == mmlTrue )
			{
				mmlAutoPtr < androidHTTP_Installation > installation = new androidHTTP_Installation();
				if ( installation->Load(http_update->mFilesList, update_item) == mmlTrue )
				{
					http_update->mInstallations.push_back(installation);
				}
			}
		}
	}


	mmlAutoPtr < mmlIVariantArray > json_scenario;
	if ( update_request->Get("scenario", mmlRelativePtrAddRef < mmlIVariantArray, mmlIVariant>(json_scenario)) == mmlTrue )
	{
		for ( mmlInt32 index = 0 ; index < json_scenario->Size(); index ++ )
		{
			mmlAutoPtr < mmlIVariantStruct > update_item;
			if ( json_scenario->Get(index, mmlRelativePtrAddRef < mmlIVariantStruct , mmlIVariant > (update_item)) == mmlTrue )
			{
				mmlAutoPtr < androidHTTP_Scenario > http_scenario = new androidHTTP_Scenario();
				if ( http_scenario->Load(http_update->mFilesList, update_item) == mmlTrue )
				{
					http_update->mScenarios.push_back(http_scenario);
				}
			}
		}
	}

	*update = http_update;
	MML_ADDREF(*update);

	return mmlTrue;

}


androidHTTP_Update::UPDATE_RESULT_T androidHTTP_Update::Download (mmlICString ** url)
{
	HTTP_PROCESS_RESULT_T res;
	if ( (res = mFilesList->Download(url)) != HTTP_SUCCESS )
	{
		if ( res == HTTP_ABORTED )
		{
			return ABORTED;
		}
		return ERROR;
	}
	return SUCCESS;
}
	
androidHTTP_Update::UPDATE_RESULT_T androidHTTP_Update::Run (UPDATE_SCOPE_T * scope, mmlICString ** file, mmlICString ** scenario_file, mmlICString ** logs, mmlInt32 * result_code)
{
	*scope = UPDATE;
	mmlSharedPtr < mmlIMemoryOutputStream > output = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));
	class finalizer 
	{
	private:
		mmlIMemoryOutputStream  * mOutput;
		mmlICString ** mLogs;
	public:
		finalizer(mmlIMemoryOutputStream * output, mmlICString ** logs)
			:mOutput(output), mLogs(logs)
		{

		}
		~finalizer()
		{
			mOutput->Close();
			mmlAutoPtr < mmlIBuffer > buffer;
			mOutput->GetData(buffer.getterAddRef());
			*mLogs = mmlNewCStringFromBuffer(buffer);
			MML_ADDREF(*mLogs);
		}
	}f(output, logs);
	for ( std::list < mmlAutoPtr < androidHTTP_Installation > >::iterator inst = mInstallations.begin(); inst != mInstallations.end(); inst ++ )
	{
		if ( (*inst)->Install(file, scenario_file, output, result_code) == mmlFalse )
		{
			return ERROR;
		}
	}
	*scope = SCENARIO;
	for ( std::list < mmlAutoPtr < androidHTTP_Scenario > >::iterator scenario = mScenarios.begin(); scenario != mScenarios.end(); scenario ++ )
	{
		if ( (*scenario)->Run(mmlNULL, scenario_file, output, result_code) == mmlFalse )
		{
			return ERROR;
		}
	}
	return SUCCESS;
}

MML_OBJECT_IMPL0(androidHTTP_Update);

androidHTTP_Installation::androidHTTP_Installation ()
									 :mState(READY)
{

}

mmlBool androidHTTP_Installation::Load (androidHTTP_FilesList * file_list,  mmlIVariantStruct * str)
{
	if ( file_list->Add(str, mFile.getterAddRefNoCast()) == mmlFalse )
	{
		return mmlFalse;
	}
	mmlAutoPtr < mmlIVariantStruct > script;
	if ( str->Get("install", mmlRelativePtrAddRef < mmlIVariantStruct , mmlIVariant >(script)) == mmlTrue )
	{
		mmlAutoPtr < androidHTTP_Scenario > install_scenario = new androidHTTP_Scenario();
		if ( install_scenario->Load(file_list, script) == mmlFalse )
		{
			return mmlFalse;
		}
		mInstall = install_scenario;
	}
	return mmlTrue;
}


androidHTTP_Installation::~androidHTTP_Installation()
{
}


mmlBool androidHTTP_Installation::Install (mmlICString ** file, mmlICString ** scenario, mmlIOutputStream * logs, mmlInt32 * error_code)
{
	mmlAutoPtr < mmlICString > file_path;
	mFile->GetPath(file_path.getterAddRef());
	if ( mInstall != mmlNULL )
	{
		log_main("Run install scenario\n");
		mmlBool res = mInstall->Run(file_path, scenario, logs, error_code);
		if ( res == mmlTrue )
		{
			log_main("Install scenario successes\n");
			mState = INSTALLED;
		}
		else
		{
			mFile->GetFile(file);
			log_main("Install scenario failed\n");
		}
		return res;
	}
	return mmlFalse;
}


MML_OBJECT_IMPL0(androidHTTP_Installation);


androidHTTP_Scenario::androidHTTP_Scenario()
{

}



mmlBool androidHTTP_Scenario::Load (androidHTTP_FilesList * file_list,  mmlIVariantStruct * str )
{
	if ( file_list->Add(str, mFile.getterAddRefNoCast()) == mmlFalse )
	{
		return mmlFalse;
	}
	str->Get("arguments", mmlRelativePtrAddRef < mmlIVariantString , mmlIVariant >(mArgs));
	if ( str->Get("shell", mmlRelativePtrAddRef < mmlIVariantString , mmlIVariant >(mShell)) == mmlFalse )
	{
		return mmlFalse;
	}
	return mmlTrue;
}


androidHTTP_Scenario::~androidHTTP_Scenario()
{
}

#include <stdio.h>

class androidScenarioOutput : public mmlIOutputStream
{
	MML_OBJECT_DECL
public:

	virtual mmlInt64 Write ( const mmlInt64 size , const void * data )
	{
		std::string s( (const mmlChar*) data, size);
		log_main("[stdout]%s\n", s.c_str());
		return size;
	}

	virtual mmlBool IsFull () { return mmlFalse; }

	virtual mmlBool GetControl ( mmlIStreamControl ** control ) { return mmlTrue; }

	virtual mmlBool Close () { return mmlTrue; }

	virtual mmlInt64 Size () { return 0; }

	virtual mmlBool Flush () { return mmlTrue; }
};

MML_OBJECT_IMPL0(androidScenarioOutput)

mmlBool androidHTTP_Scenario::Run (mmlICString * path, mmlICString ** scenario, mmlIOutputStream * logs, mmlInt32 * result_code)
{
	mmlAutoPtr < mmlICString > file_path;
	mFile->GetPath(file_path.getterAddRef());
	log_main("[script] %s\n", file_path->Get());
	log_main("[args] %s\n", path == mmlNULL ? "<empty>" : path->Get());

	MML_CREATE_CSTRING_MEMORY_OUTPUT_STREAM(cmd_line);
	cmd_line->WriteStr(mShell->Get()->Get());
	cmd_line->WriteChar(' ');
	android_cmd_escape_string(cmd_line, file_path);
	if ( path != mmlNULL )
	{
		cmd_line->WriteChar(' ');
		android_cmd_escape_string(cmd_line, path);
	}
	if (mArgs != mmlNULL && mArgs->Get()->Length() > 0)
	{
		cmd_line->WriteChar(' ');
		cmd_line->WriteStr(mArgs->Get()->Get());
	}
	cmd_line->WriteFormatted(" 2>&1");

	MML_CLOSE_CSTRING_MEMORY_OUTPUT_STREAM(cmd_line);

	log_main("exec %s\n", cmd_line_str->Get());
	mmlBool result = android_run_process(cmd_line_str->Get(), logs, result_code);
	if (result == mmlFalse )
	{
		*result_code = -1;
		mFile->GetFile(scenario);
	}
	return mmlTrue;
}

MML_OBJECT_IMPL0(androidHTTP_Scenario)

