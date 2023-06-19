#include "mmlIScheduler.h"
#include "mmlIFileSystem.h"
#include "mmlIVariantSerializer.h"
#include "mmlIVariantDeserializer.h"
#include "android_http_update.h"
#include "android_http_fetch.h"
#include "mml_components.h"
#include "android_utils.h"
#include "android_updater_database.h"
#include "mmlIThread.h"
#include "android_reboot.h"

void log_main( const mmlChar * formatter , ... );

static std::list < android_reboot_check_t > checkers;


void android_updater_reboot_register_checker(android_reboot_check_t checker)
{
	checkers.push_back(checker);
}

void android_updater_reboot_schedule(const mmlInt32 begin, const mmlInt32 end)
{
	mml_timespec ts;
	mml_clock_gettime_realtime(&ts);

	mmlAutoPtr < mmlIDatabase > db;
	android_database_get(db.getterAddRef());
	if (db != mmlNULL)
	{
		db->Query("INSERT INTO reboot_schedule (timestamp, begin, end) VALUES(?,?,?)", mmlNULL, mmlNewVariantArrayFill(mmlNewVariantInteger(ts.tv_sec),
			mmlNewVariantInteger(begin),
			mmlNewVariantInteger(end)));
	}
}

void android_updater_reboot()
{
	android_updater_reboot_schedule(0, 24);
}

mmlBool android_updater_is_idle()
{
	for (std::list < android_reboot_check_t >::iterator checker = checkers.begin(); checker != checkers.end(); checker++)
	{
		if ((*checker)() == mmlFalse)
		{
			return mmlFalse;
		}
	}
	return mmlTrue;
}

void android_updater_reboot_tick()
{
	if (android_updater_is_idle() == mmlFalse)
	{
		return;
	}

	mml_timespec ts;
	mml_clock_gettime_realtime(&ts);
	mml_tm tm;
	mml_clock_localtime(&ts, &tm);

	mmlAutoPtr < mmlIDatabase > db;
	android_database_get(db.getterAddRef());
	if (db != mmlNULL)
	{
		mml_timespec uptime;
		mml_clock_gettime_uptime(&uptime);
		db->Query("DELETE FROM reboot_schedule WHERE timestamp < ?", mmlNULL, mmlNewVariantInteger(ts.tv_sec - uptime.tv_sec));

		mmlAutoPtr < mmlIVariantInteger > do_reboot;

		if (db->Query("SELECT count(*) FROM reboot_schedule WHERE ? >= begin and ? <= end", mmlDatabaseObjectQuery<mmlIVariantInteger>(do_reboot), mmlNewVariantArrayFill(mmlNewVariantInteger(tm.tm_hour), mmlNewVariantInteger(tm.tm_hour))) == mmlTrue)
		{
			if (do_reboot != mmlNULL && do_reboot->Get() > 0)
			{
				log_main("Delete all reboot schedules\n");
				mmlAutoPtr < mmlIDatabase > db;
				android_database_get(db.getterAddRef());
				if (db != mmlNULL)
				{
					db->Query("DELETE FROM reboot_schedule", mmlNULL, mmlNULL);
				}
				log_main("Stopping subsystem\n");
				android_updater_stop();
				log_main("Doing reboot\n");
				system("/system/bin/am start -a android.intent.action.REBOOT --ez nowait 1 --ez interval 1 --ez window 0 --activity-clear-task");
				mml_sleep(20000);
				system("/system/bin/reboot");
				exit(0);
			}
		}
	}
}


class AndroidRebootTask : public mmlISchedulerTask
{
	MML_OBJECT_DECL
public:

	mmlISchedulerRule::TASK_RESULT_T Run (mmlIVariantStruct * config)
	{
		//log_main("reboot tick\n");
		android_updater_reboot_tick();
		return mmlISchedulerRule::TASK_SUCCESS;
	}

};

MML_OBJECT_IMPL1(AndroidRebootTask, mmlISchedulerTask)

#include "mml_components.h"

MML_CONSTRUCTOR_IMPL(AndroidRebootTask);

static mmlObjectFactory android_reboot_task=
{
	{ 0x63AB4904, 0x6FC0, 0x1014, 0x9EE8 , { 0xAE, 0x9B, 0xD1, 0xF9, 0xC5, 0xE5 } },
	"mml/scheduler/tasks/reboot",
	MML_CONSTRUCTOR(AndroidRebootTask)
};

mmlBool register_android_reboot_task ()
{
	mmlRegisterComponentFactory(&android_reboot_task);
	return mmlTrue;
}

ANDROID_INIT_MODULE_I(register_android_reboot_task);
