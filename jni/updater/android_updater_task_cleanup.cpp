#include "mmlIScheduler.h"
#include "mmlIFileSystem.h"
#include "android_http_update.h"
#include "android_http_fetch.h"
#include "mml_components.h"
#include "android_updater_database.h"


class AndroidCleanupTask : public mmlISchedulerTask
{
	MML_OBJECT_DECL
public:

	mmlISchedulerRule::TASK_RESULT_T Run (mmlIVariantStruct * config)
	{
		mmlAutoPtr < mmlIDatabase > db;
		android_database_get(db.getterAddRef());
		if ( db != mmlNULL )
		{
			mml_timespec ts;
			mml_clock_gettime_realtime(&ts);
			ts.tv_sec -= 24 * 60 * 60;
			db->Query("DELETE FROM logs WHERE timestamp < ? ", mmlNULL, mmlNewVariantInteger(ts.tv_sec), mmlNULL);
		}
		return mmlISchedulerRule::TASK_SUCCESS;
	}

};

MML_OBJECT_IMPL1(AndroidCleanupTask, mmlISchedulerTask)

#include "mml_components.h"

MML_CONSTRUCTOR_IMPL(AndroidCleanupTask);

static mmlObjectFactory android_cleanup_task=
{
	{ 0xD556FBD1, 0x6C81, 0x1014, 0xBA21 , { 0x80, 0x7E, 0x21, 0xBA, 0xF5, 0xAA } },
	"mml/scheduler/tasks/cleanup",
	MML_CONSTRUCTOR(AndroidCleanupTask)
};

mmlBool register_android_cleanup_task ()
{
	mmlRegisterComponentFactory(&android_cleanup_task);
	return mmlTrue;
}


ANDROID_INIT_MODULE_I(register_android_cleanup_task);