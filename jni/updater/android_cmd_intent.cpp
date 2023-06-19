#include "mmlICString.h"
#include "mmlICStringStream.h"
#include "mml_components.h"
#include "mmlIVariant.h"
#include "mmlIProcess.h"
#include "android_utils.h"
#include "android_cmd_dispatcher.h"

void log_main( const mmlChar * formatter , ... );



void cmd_intent (mmlICString * id, mmlIVariant * request, android_responder * responder)
{
	mmlSharedPtr < mmlIVariantStruct > intent_request = request;
	if ( intent_request == mmlNULL )
	{
		return;
	}
	mmlAutoPtr < mmlIVariantString > action;
	if ( intent_request->Get("action", mmlRelativePtrAddRef < mmlIVariantString , mmlIVariant >(action)) == mmlFalse )
	{
		return;
	}
	mmlAutoPtr < mmlIVariantArray > require;
	if ( intent_request->Get("require", mmlRelativePtrAddRef < mmlIVariantArray , mmlIVariant >(require)) == mmlTrue )
	{
		if ( require != mmlNULL )
		{
			
			for ( mmlInt32 index = 0 ; index < require->Size(); index ++ )
			{
				mmlAutoPtr < mmlIVariantString > pkg;
				require->Get(index, mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(pkg));
				if ( pkg == mmlNULL )
				{
					return;
				}
				if ( android_is_package_installed(pkg->Get()) == mmlFalse )
				{
					RESPOND_ERROR("require_failed");
				}
			}
		}
	}
	mmlAutoPtr < mmlIVariantString > method;
	if ( intent_request->Get("method", mmlRelativePtrAddRef < mmlIVariantString , mmlIVariant >(method)) == mmlFalse )
	{
		RESPOND_INVALID_ARGS();
	}
	
	MML_CREATE_CSTRING_MEMORY_OUTPUT_STREAM(cmd_line);
	cmd_line->WriteFormatted("/system/bin/sh /system/bin/am %s -a %s", method->Get(), action->Get());


	mmlAutoPtr < mmlIVariantStruct > args;
	if ( intent_request->Get("args", mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant >(args)) == mmlTrue )
	{
		class args_enumerator : public mmlIVariantStructEnumerator
		{
			MML_OBJECT_STATIC_DECL
				mmlICStringOutputStream * mArgs;
		public:
			args_enumerator(mmlICStringOutputStream * args)
				:mArgs(args)
			{

			}
			mmlBool OnMember ( const mmlInt32 tag, mmlICString * name , mmlIVariant * val )
			{
				if ( val->GetType() == MML_STRING )
				{
					mArgs->WriteStr(" -e ");
				}
				else if ( val->GetType() == MML_INTEGER )
				{
					mArgs->WriteStr(" --ei ");
				}
				else if ( val->GetType() == MML_DOUBLE )
				{
					mArgs->WriteStr(" --ef ");
				}
				else if ( val->GetType() == MML_BOOLEAN )
				{
					mArgs->WriteStr(" --ez ");
				}
				else
				{
					return mmlFalse;
				}
				mArgs->WriteStr(name->Get());
				mArgs->WriteChar(' ');
				mmlSharedPtr < mmlIVariantString > s = val;
				if ( s == mmlNULL )
				{
					return mmlFalse;
				}
				android_cmd_escape_string(mArgs, s->Get());
				return mmlTrue;
			}
		}ae(cmd_line);
		if ( args->Enumerate(&ae) == mmlFalse )
		{
			RESPOND_INVALID_ARGS();
		}
	}

	MML_CLOSE_CSTRING_MEMORY_OUTPUT_STREAM(cmd_line);
	log_main("running %s\n", cmd_line_str->Get());
	mmlAutoPtr < mmlIInputStream > result;
	if ( android_run_process(cmd_line_str->Get(), result.getterAddRef()) != 0 )
	{
		return ERROR;
	}
	return OK;
}

ANDROID_WEBSOCKET_CMD_MODULE(intent, "com.mplore.cmd.intent", cmd_intent);