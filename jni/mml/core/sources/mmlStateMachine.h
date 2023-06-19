#ifndef MML_STATE_MACHINE_IMPL_HEADER_INCLUDED
#define MML_STATE_MACHINE_IMPL_HEADER_INCLUDED

#include "mmlIStateMachine.h"
#include "mmlPtr.h"
#include "mmlIMutex.h"
#include <map>

class MML_SM_STATE_INT
{
public:
	mmlUInt32 state;
	mml_sm_state_enter_t enter;
	mml_sm_state_exit_t exit;
	std::map < mmlUInt32 , mml_sm_state_event_process_t > events;

	MML_SM_STATE_INT()
		:state(-1), enter(mmlNULL), exit(mmlNULL)
	{

	}

	MML_SM_STATE_INT(mmlUInt32 _state, mml_sm_state_enter_t _enter, mml_sm_state_exit_t _exit)
		:state(_state), enter(_enter), exit(_exit)
	{
		
	}

	MML_SM_STATE_INT( const MML_SM_STATE_INT & src)
		:state (src.state), enter(src.enter), exit(src.exit), events(src.events)
	{

	}
};

class mmlStateMachine : public mmlIStateMachine
{
	MML_OBJECT_DECL
public:
	mmlStateMachine();
	mmlBool Init (const mmlUInt32 initial_state, mmlIObject * context , const MML_SM_STATE * states, const mmlInt32 states_count);
	mmlBool Process ( const mmlUInt32 event, mmlIObject * arg, void * data, const mmlInt32 data_size, mmlInt32 * data_processed, mmlUInt32 * current_state );
	mmlBool StartTimer(const mmlInt32 event, const mmlInt32 timeout);
	mmlBool StopTimer(const mmlInt32 event);
	void Tick();
private:
	void _queue_push(mmlUInt32 state);

	mmlBool _queue_pop(mmlUInt32 * state);

	mmlInt32 _queue_size ();

	MML_SM_STATE_INT * _ChangeState (const mmlUInt32 state);

	MML_SM_STATE_INT * mCurrentState;

	std::map < mmlUInt32 , MML_SM_STATE_INT > mStates;

	mmlAutoPtr < mmlIObject > mContext;

	mmlSharedPtr < mmlIMutex > mMutex;

	mmlUInt32 mStatesQueue[32];

	mmlInt32 mStatesQueueSize;

	std::map < mmlInt32, mml_timespec > mTimers;
};



#endif //MML_STATE_MACHINE_IMPL_HEADER_INCLUDED
