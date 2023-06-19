#ifndef ANDROID_HTTP_DOWNLOAD_HEADER_INCLUDED
#define ANDROID_HTTP_DOWNLOAD_HEADER_INCLUDED

#include "mmlICString.h"
#include "mmlIVariant.h"
#include "mmlPtr.h"
#include "mmlIDatabase.h"
#include "mmlIObjectQueue.h"
#include "android_utils.h"
#include "android_http_fetch.h"
#include <list>
#include <map>

class androidHTTP_File : public mmlIObject
{
	MML_OBJECT_DECL
public:
	androidHTTP_File (mmlICString * dir, mmlIVariantString * url, mmlIVariantString * file, mmlIVariantString * md5, mmlIVariantInteger * mode );

	~androidHTTP_File();

	HTTP_PROCESS_RESULT_T Download ();

	mmlBool GetPath (mmlICString ** path );

	void Delete ();

	mmlBool Match ( mmlICString * url );

	mmlBool GetURL(mmlICString ** url);

	mmlBool GetFile ( mmlICString ** name);

private:

	mmlAutoPtr < mmlICString > mDir;

	mmlAutoPtr < mmlIVariantString > mFileName;

	mmlAutoPtr < mmlIVariantString > mMD5;

	mmlAutoPtr < mmlIVariantString > mURL;

	mmlAutoPtr < mmlIVariantInteger > mMode;

};

class androidHTTP_FilesList : public mmlIObject
{
	MML_OBJECT_DECL
public:

	static androidHTTP_FilesList * Create (mmlICString * dir);

	mmlBool Add ( mmlIVariantStruct * file_desc , androidHTTP_File ** file );

	HTTP_PROCESS_RESULT_T Download (mmlICString ** url);

	mmlBool Delete ();

private:

	androidHTTP_FilesList(mmlICString * dir);

	std::list < mmlAutoPtr < androidHTTP_File > > mFiles;

	mmlAutoPtr < mmlICString > mDir;

};

class androidHTTP_Scenario : public mmlIObject
{
	MML_OBJECT_DECL
public:
	androidHTTP_Scenario();

	~androidHTTP_Scenario();

	mmlBool Load (androidHTTP_FilesList * file_list,  mmlIVariantStruct * str );

	mmlBool Run (mmlICString * path, mmlICString ** scenario, mmlIOutputStream * logs, mmlInt32 * result_code);

	mmlBool GetFile(mmlICString ** name)
	{
		return mFile->GetFile(name);
	}
	
private:
	mmlAutoPtr < mmlIVariantString > mArgs;
	mmlAutoPtr < androidHTTP_File > mFile;
	mmlAutoPtr < mmlIVariantString > mShell;
};


class androidHTTP_Installation : public mmlIObject
{
	MML_OBJECT_DECL
public:
	androidHTTP_Installation ();

	~androidHTTP_Installation();

	mmlBool Load (androidHTTP_FilesList * file_list,  mmlIVariantStruct * str);

	mmlBool Install (mmlICString ** file, mmlICString ** scenarion, mmlIOutputStream * logs, mmlInt32 * error_code);

private:

	enum
	{
		READY,
		INSTALLED,
		COMMITED
	}mState;

	mmlAutoPtr < androidHTTP_File > mFile;

	mmlAutoPtr < androidHTTP_Scenario > mInstall;

};

#define ANDROID_HTTP_UPDATE_UUID { 0x3B8EF259, 0x6E9A, 0x1014, 0xB5B3 , { 0x9E, 0x58, 0xCB, 0x4E, 0x2A, 0xC0 } }

class androidHTTP_Update : public mmlIObject
{
	MML_OBJECT_DECL
	MML_OBJECT_UUID_DECL(ANDROID_HTTP_UPDATE_UUID)
public:

	typedef enum
	{
		SUCCESS,
		ABORTED,
		ERROR
	}UPDATE_RESULT_T;

	typedef enum
	{
		INITIALIZE,
		DOWNLOAD,
		UPDATE,
		SCENARIO
	}UPDATE_SCOPE_T;

	~androidHTTP_Update();

	static mmlBool Create (mmlIVariantStruct * update_request  , androidHTTP_Update ** update);

	UPDATE_RESULT_T Download ( mmlICString ** file );

	UPDATE_RESULT_T Run (UPDATE_SCOPE_T * scope, mmlICString ** file, mmlICString ** scenario, mmlICString ** logs, mmlInt32 * result_code);

	void Delete () { mFilesList->Delete(); }

	mmlBool IsRebootRequired ( mmlBool * immediate, mmlInt32 * begin, mmlInt32 * end )
	{
		*immediate = mRebootImmediate;
		*begin = mRebootBegin;
		*end = mRebootEnd;
		return mReboot;
	}

private:

	mmlBool mReboot;
	mmlBool mRebootImmediate;
	mmlInt32 mRebootBegin;
	mmlInt32 mRebootEnd;

	androidHTTP_Update();

	mmlAutoPtr < mmlICString > mDirName;

	mmlAutoPtr < androidHTTP_FilesList > mFilesList;

	std::list < mmlAutoPtr < androidHTTP_Installation > > mInstallations;

	std::list < mmlAutoPtr < androidHTTP_Scenario > > mScenarios;
};


#endif //ANDROID_HTTP_DOWNLOAD_HEADER_INCLUDED
