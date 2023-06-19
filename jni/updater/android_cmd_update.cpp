#include "mmlIVariant.h"
#include "android_cmd_dispatcher.h"
#include "android_http_update.h"
#include "android_updater_database.h"
#include "mmlILogger.h"
#include "mml_services.h"
#include "mml_components.h"
#include "mmlIThread.h"
#include "mmlIVariantSerializer.h"
#include "mmlIVariantDeserializer.h"
#include "mmlIMemoryStream.h"
#include "mmlIBuffer.h"
#include "android_reboot.h"

static mmlUUID update_queue_uuid = { 0xB84DA9A3, 0x6F34, 0x1014, 0x99E5 , { 0xC6, 0x1B, 0x38, 0x1D, 0xF8, 0x76 } };

static mmlUUID update_worker_uuid = { 0xBC3EDD50, 0x6F34, 0x1014, 0xB773 , { 0xC6, 0x88, 0xB8, 0x20, 0x60, 0xA0 } };

#define MML_BINARY_VARIANT_SERIALIZER_UUID { 0x6392D37F, 0x6C6E, 0x1014, 0xAE5B , { 0xDC, 0x81, 0x31, 0xDC, 0xEA, 0xA1 } }

#define MML_BINARY_VARIANT_DESERIALIZER_UUID { 0x6E1257B1, 0x6C6E, 0x1014, 0x8067 , { 0x8F, 0x47, 0x3C, 0xAD, 0xC9, 0x8C } }

#define ANDROID_UPDATE_REQUEST_UUID { 0x09A8D6B7, 0x6C07, 0x1014, 0xADF1 , { 0xFD, 0xEF, 0x5E, 0x3D, 0x8B, 0x32 } }

void log_main(const mmlChar * formatter, ...);



class cmd_update_request : public mmlIObject
{
	MML_OBJECT_DECL
		MML_OBJECT_UUID_DECL(ANDROID_UPDATE_REQUEST_UUID)
public:
	cmd_update_request(android_cmd_request * _responder, androidHTTP_Update * _update)
		:responder(_responder), update(_update)
	{

	}
	mmlAutoPtr < android_cmd_request > responder;
	mmlAutoPtr < androidHTTP_Update > update;
};

MML_OBJECT_IMPL1(cmd_update_request, cmd_update_request)


CMD_HANDLER_BEGIN(cmd_update)
{
	mmlSharedPtr < mmlIVariantStruct > update_request;
	request->GetRequest(update_request.getterAddRef());
	CMD_HANDLER_ASSERT(update_request != mmlNULL, -1, "Could not cast request");
	mmlAutoPtr < androidHTTP_Update > update;
	CMD_HANDLER_ASSERT(androidHTTP_Update::Create(update_request, update.getterAddRef()) == mmlTrue, -1, "Could not parse request");
	mmlSharedPtr < mmlIObjectQueue > queue = mmlGetService(update_queue_uuid);
	queue->Post(new cmd_update_request(request, update));
}
CMD_HANDLER_END()

static mmlBool updater_is_idle = mmlTrue;

class android_update_worker : public mmlIThreadFunction
{
	MML_OBJECT_DECL
		MML_THREAD_DECLARE_SHUTDOWN_FLAG
private:
	mmlAutoPtr < mmlIObjectQueue > mQueue;
public:
	android_update_worker(mmlIObjectQueue * queue)
		:mQueue(queue)
	{

	}


	void Execute(mmlIThread * thread)
	{
		mmlSharedPtr < mmlIVariantStruct > config = mmlGetService(MML_GLOBAL_CONFIG_UUID());
		mmlBool immediate_reboot = mmlFalse;
		while (IsShutdown() == mmlFalse)
		{
			mmlAutoPtr < cmd_update_request > update;
			if (mQueue->Get(5000, update.getterAddRef()) == mmlTrue)
			{
				if (update != mmlNULL)
				{
					updater_is_idle = mmlFalse;
					androidHTTP_Update::UPDATE_RESULT_T res = androidHTTP_Update::ERROR;
					mmlAutoPtr < mmlIVariantStruct > response_struct = mmlNewVariantStruct();

					mmlAutoPtr < mmlICString > file;
					mmlAutoPtr < mmlICString > scenario;
					mmlAutoPtr < mmlICString > logs;
					mmlInt32 result_code = 0;
					androidHTTP_Update::UPDATE_SCOPE_T scope = androidHTTP_Update::DOWNLOAD;
					{
						log_main("got update task\n");
						
						for (mmlInt32 attempt = 0; attempt < 30; attempt++)
						{
							log_main("update attempt %d in progress\n", attempt);
							res = update->update->Download(file.getterAddRef());
							if (res == androidHTTP_Update::SUCCESS)
							{
								break;
							}
							log_main("Downloading aborted, sleeping\n");
							mml_sleep(60000);
						}

						if (res == androidHTTP_Update::SUCCESS)
						{
							log_main("install begin\n");
							res = update->update->Run(&scope, file.getterAddRef(), scenario.getterAddRef(), logs.getterAddRef(), &result_code);
							log_main("install end\n");
							if (res != androidHTTP_Update::SUCCESS)
							{
								result_code = -1;
							}
							if (logs != mmlNULL)
							{
								log_main(logs->Get());
							}
						}
						update->update->Delete();
					}
					response_struct->Set("log", mmlNewVariantString(logs));
					if (res == androidHTTP_Update::SUCCESS && result_code == 0)
					{
						log_main("install success\n");
						update->responder->RespondOK(response_struct);
					}
					else
					{

						const mmlChar * errorStr = scope == androidHTTP_Update::INITIALIZE ? "initialize" :
								scope == androidHTTP_Update::DOWNLOAD ? "download" :
								scope == androidHTTP_Update::UPDATE ? "update" : "scenario";
						if (file != mmlNULL)response_struct->Set("file", mmlNewVariantString(file));
						if (scenario != mmlNULL) response_struct->Set("scenario", mmlNewVariantString(scenario));
						log_main("install failed scope=%s  result=%d file=<%s> scenario=<%s>\n", errorStr, result_code, file == mmlNULL ? "-" : file->Get(), scenario == mmlNULL ? "-" : scenario->Get());
						update->responder->RespondError(result_code, errorStr, response_struct);
					}
					if (res == androidHTTP_Update::SUCCESS)
					{
						mmlBool reboot_imediate;
						mmlInt32 reboot_begin;
						mmlInt32 reboot_end;
						if (update->update->IsRebootRequired(&reboot_imediate, &reboot_begin, &reboot_end) == mmlTrue)
						{
							log_main("Reboot is required\n");
							if (reboot_imediate == mmlTrue)
							{
								android_updater_reboot();
							}
							else
							{
								log_main("Schedule reboot\n");
								android_updater_reboot_schedule(reboot_begin, reboot_end);
							}
						}
					}
					updater_is_idle = mQueue->Size() == 0 ? mmlTrue : mmlFalse;
				}
			}
		}
	}
};

MML_OBJECT_IMPL0(android_update_worker)

mmlBool cmd_updater_is_idle()
{
	return updater_is_idle;
}

mmlBool cmd_update_init()
{
	android_updater_reboot_register_checker(cmd_updater_is_idle);
	mmlSharedPtr < mmlIObjectQueue > update_queue = mmlNewObject(MML_OBJECT_UUID(mmlIObjectQueue));

	if (update_queue == mmlNULL)
	{
		return mmlFalse;
	}

	mmlSetService(update_queue_uuid, update_queue);

	mmlSharedPtr < mmlIThread > update_thread = mmlNewObject(MML_OBJECT_UUID(mmlIThread));
	if (update_thread->Create(new android_update_worker(update_queue), PriorityNormal, MML_DEFAULT_STACK_SIZE) == mmlFalse)
	{
		return mmlFalse;
	}
	mmlSetService(update_worker_uuid, update_thread);

	return mmlTrue;
}


ANDROID_CMD_MODULE_I(update, "io.appservice.module.UPDATE", cmd_update, cmd_update_init);