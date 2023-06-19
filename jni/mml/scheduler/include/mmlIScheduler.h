#ifndef MML_SCHEDULER_HEADER_INCLUDED
#define MML_SCHEDULER_HEADER_INCLUDED

#include "mml_time.h"
#include "mmlIService.h"
#include "mmlIVariant.h"

#define MML_SHEDULER_RULE_UUID { 0xC2D6028D, 0x6C81, 0x1014, 0x9F8E , { 0xB3, 0x81, 0xCF, 0xEA, 0xA1, 0x8D } }

class mmlISchedulerRule : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_SHEDULER_RULE_UUID)
public:


	typedef enum
	{
		TASK_FIRST_START,
		TASK_FAILED,
		TASK_SUCCESS,
		TASK_FINISHED
	}TASK_RESULT_T;
	
	virtual void GetNextFireTime (const TASK_RESULT_T result, const mml_timespec & current_time , mml_timespec * next_time) = 0;
	virtual mmlBool Configure ( mmlIVariantStruct * config ) = 0;
};

#define MML_SCHEDULER_TASK_UUID { 0xBE7259EE, 0x6C81, 0x1014, 0x9048 , { 0x81, 0x85, 0x8E, 0x8F, 0x75, 0x9F } }

class mmlISchedulerTask : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_SCHEDULER_TASK_UUID)
public:


	virtual mmlISchedulerRule::TASK_RESULT_T Run (mmlIVariantStruct * config) = 0;
};

#define MML_SCHEDULER_UUID { 0xC712C779, 0x6C19, 0x1014, 0xA940 , { 0x9C, 0x12, 0x96, 0x93, 0x3B, 0x95 } }

class mmlIScheduler : public mmlIService
{
	MML_OBJECT_UUID_DECL(MML_SCHEDULER_UUID)
public:
	virtual mmlBool Add ( mmlIVariantStruct * config ,mmlISchedulerTask * task , mmlISchedulerRule * rule, mmlInt32 * handle) = 0;

	virtual mmlBool Suspend ( const mmlInt32 task_hdl ) = 0;

	virtual mmlBool Resume ( const mmlInt32 task_hdl ) = 0;

	virtual mmlBool Remove ( const mmlInt32 task_hdl ) = 0;

	virtual mmlBool Run ( const mmlInt32 task_hdl ) = 0;

	virtual void SetMaxSize (const mmlInt32 max) = 0;

};

#endif //MML_SCHEDULER_HEADER_INCLUDED
