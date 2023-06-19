#include "mmlStateMachine.h"
#include "mml_components.h"
#include <list>

void log_core(const mmlChar * formatter, ...);

mmlStateMachine::mmlStateMachine()
	:mCurrentState(mmlNULL), mStatesQueueSize(0)
{
	mMutex = mmlNewObject(MML_OBJECT_UUID(mmlIMutex));
}

mmlBool mmlStateMachine::Init (const mmlUInt32 initial_state, mmlIObject * context , const MML_SM_STATE * states, const mmlInt32 states_count)
{
	mContext= context;
	for ( mmlInt32 index = 0; index < states_count; index ++ )
	{
		std::map < mmlUInt32 , MML_SM_STATE_INT >::iterator st = mStates.find(states[index].ref->state);
		if ( st != mStates.end())
		{
			return mmlFalse;
		}
		MML_SM_STATE_INT state(states[index].ref->state, states[index].ref->enter, states[index].ref->exit);
		for ( MML_SM_EVENT * event = states[index].ref->list; event != mmlNULL; event = event->next )
		{
			std::map < mmlUInt32 , mml_sm_state_event_process_t >::iterator existing = state.events.find(event->event);
			if ( existing != state.events.end() )
			{
				return mmlFalse;
			}
			state.events[event->event] = event->func;
		}
		mStates[state.state] = state;
	}
	mCurrentState = _ChangeState(initial_state);
	if ( mCurrentState == mmlNULL )
	{
		return mmlFalse;
	}
	return mmlTrue;
}

mmlBool mmlStateMachine::Process ( const mmlUInt32 event, mmlIObject * arg, void * data, const mmlInt32 data_size, mmlInt32 * data_processed, mmlUInt32 * current_state )
{
	mmlMutexGuard g(mMutex);
	if ( mCurrentState == mmlNULL )
	{
		return mmlFalse;
	}
	mmlUInt32 next_state = mCurrentState->state;
	std::map < mmlUInt32 , mml_sm_state_event_process_t >::iterator process = mCurrentState->events.find(event);
	if ( process == mCurrentState->events.end())
	{
		return mmlFalse;
	}
	if ( process->second(this, mContext, event, arg, data, data_size, data_processed, &next_state) == mmlFalse )
	{
		return mmlFalse;
	}
	if ( next_state != mCurrentState->state )
	{
		if ( _ChangeState(next_state) == mmlNULL )
		{
			return mmlFalse;
		}
	}
	if ( current_state != mmlNULL ) *current_state = mCurrentState->state;
	return mmlTrue;
}

void mmlStateMachine::_queue_push(mmlUInt32 state)
{
	mStatesQueue[mStatesQueueSize] = state;
	mStatesQueueSize ++;
}

mmlBool mmlStateMachine::_queue_pop(mmlUInt32 * state)
{
	if ( mStatesQueueSize > 0 )
	{
		*state = mStatesQueue[0];
		for ( mmlInt32 idx = 0 ; idx < mStatesQueueSize; idx ++ )
		{
			mStatesQueue[idx] = mStatesQueue[idx + 1];
		}
		mStatesQueueSize --;
		return mmlTrue;
	}
	return mmlFalse;
}

mmlInt32 mmlStateMachine::_queue_size ()
{
	return mStatesQueueSize;
}

MML_SM_STATE_INT * mmlStateMachine::_ChangeState (const mmlUInt32 state)
{
	_queue_push(state);
	if ( _queue_size() > 1 )
	{
		return mCurrentState;
	}
	mmlUInt32 next_state = state;
	mmlBool do_pop = mmlTrue;
	for(;;)
	{
		if (mCurrentState == mmlNULL || next_state != mCurrentState->state )
		{
			for(;;) 
			{
				if ( mCurrentState != mmlNULL && mCurrentState->exit != mmlNULL)
				{
					mCurrentState->exit(this, mContext);
				}
				std::map < mmlUInt32 , MML_SM_STATE_INT > ::iterator st = mStates.find(next_state);
				if ( st == mStates.end())
				{
					return mmlNULL;
				}
				mCurrentState = &(st->second);
				next_state = st->first;
				if ( st->second.enter != mmlNULL )
				{
					if ( st->second.enter(this, mContext, &next_state) == mmlFalse )
					{
						return mmlNULL;
					}
					if ( next_state == mCurrentState->state)
					{
						break;
					}
				}
				else
				{
					break;
				}
			} 
		}
		if ( do_pop == mmlTrue )
		{
			do_pop = mmlFalse;
			_queue_pop(&next_state);
		}
		if ( _queue_pop(&next_state) == mmlFalse )
		{
			return mCurrentState;
		}
	}
	return mCurrentState;
}

mmlBool mmlStateMachine::StartTimer(const mmlInt32 event, const mmlInt32 timeout)
{
	mml_timespec ts;
	mml_clock_gettime_monotonic(&ts);
	mml_clock_add(&ts, timeout);
	mTimers[event] = ts;
	log_core("StartTimer %d %d sec\n", event, timeout / 1000);
	return mmlTrue;
}

mmlBool mmlStateMachine::StopTimer(const mmlInt32 event)
{
	log_core("StopTimer %d\n", event);
	mTimers.erase(event);
	return mmlTrue;
}

void mmlStateMachine::Tick()
{
	mml_timespec ts;
	mml_clock_gettime_monotonic(&ts);
	std::list < mmlInt32 > timers;
	for (std::map < mmlInt32, mml_timespec >::iterator timer = mTimers.begin(); timer != mTimers.end(); )
	{
		//log_core("timer %d seconds left %d\n", timer->first, mml_clock_diff(&timer->second, &ts) / 1000);
		if ( mml_clock_cmp(&ts, &timer->second) >= 0 )
		{
			log_core("Expired timer %d\n", timer->first);
			timers.push_front(timer->first);
			mTimers.erase(timer++);
		}
		else
		{
			++timer;
		}
	}
	for (std::list < mmlInt32 >::iterator expired_timer = timers.begin(); expired_timer != timers.end(); expired_timer++)
	{
		log_core("Notify timer %d\n", *expired_timer);
		Process(*expired_timer, mmlNULL, mmlNULL, 0, mmlNULL, mmlNULL);
	}
}

MML_OBJECT_IMPL1(mmlStateMachine, mmlIStateMachine)