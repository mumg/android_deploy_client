#ifndef MML_STATE_MACHINE_HEADER_INCLUDED
#define MML_STATE_MACHINE_HEADER_INCLUDED

#include "mmlIObject.h"
#include "mml_time.h"

class mmlIStateMachine;

#define MML_SM_STATE_ENTER_ARGS mmlIStateMachine * sm, mmlIObject * context, mmlUInt32 * next_state
#define MML_SM_STATE_ENTER_ARGS_FWD context, next_state
#define MML_SM_STATE_EVENT_ARGS  mmlIStateMachine * sm, mmlIObject * context, const mmlUInt32 event, mmlIObject * arg, void * data, const mmlInt32 data_size, mmlInt32 * data_processed, mmlUInt32 * next_state
#define MML_SM_STATE_EVENT_ARGS_FWD context, event, arg, data, data_size, data_processed, next_state
#define MML_SM_STATE_EXIT_ARGS  mmlIStateMachine * sm, mmlIObject * context
#define MML_SM_STATE_EXIT_ARGS_FWD  context

typedef mmlBool (*mml_sm_state_enter_t) (MML_SM_STATE_ENTER_ARGS);
typedef mmlBool (*mml_sm_state_event_process_t) (MML_SM_STATE_EVENT_ARGS);
typedef mmlBool (*mml_sm_state_exit_t) (MML_SM_STATE_EXIT_ARGS);

#define MML_SM_STATE_ENTER_FUNC(func) static mmlBool func (MML_SM_STATE_ENTER_ARGS)

#define MML_SM_STATE_EVENT_FUNC(func) static mmlBool func (MML_SM_STATE_EVENT_ARGS)

#define MML_SM_STATE_EXIT_FUNC(func) static mmlBool func (MML_SM_STATE_EXIT_ARGS)

class MML_SM_EVENT
{
public:
	MML_SM_EVENT(mmlUInt32 _event, mml_sm_state_event_process_t _func)
		:event(_event), func(_func), next(mmlNULL)
	{

	}

	~MML_SM_EVENT()
	{
		if ( next != mmlNULL) delete next;
	}

	void Append ( MML_SM_EVENT * entry )
	{
		if ( next == mmlNULL )
		{
			next = entry;
		}
		else
		{
			next->Append(entry);
		}
	}

	const mmlUInt32 event;
	mml_sm_state_event_process_t func;
	MML_SM_EVENT * next;
};


class MML_SM_STATE
{
public:
	class MML_SM_STATE_REF
	{
	public:
		MML_SM_STATE_REF(const mmlUInt32 _state = -1)
			:ref(1), state(_state), list(mmlNULL), enter(mmlNULL), exit(mmlNULL)
		{

		}

		~MML_SM_STATE_REF()
		{
			if ( list != mmlNULL )
			{
				delete list;
			}
		}

		mmlInt32 ref;
		mmlUInt32 state;
		MML_SM_EVENT * list;
		mml_sm_state_enter_t enter;
		mml_sm_state_exit_t exit;

		void AddRef()
		{
			ref ++;
		}

		void Release ()
		{
			ref --;
			if ( ref == 0 )
			{
				delete this;
			}
		}
	};

	MML_SM_STATE_REF * ref;


	MML_SM_STATE(const mmlUInt32 _state = -1)
	{
		ref = new MML_SM_STATE_REF(_state);
	}

	MML_SM_STATE(const MML_SM_STATE & src)
	{
		ref = src.ref;
		ref->AddRef();
	}

	~MML_SM_STATE()
	{
		ref->Release();
	}

	MML_SM_STATE& AddEvent ( const mmlUInt32 event, mml_sm_state_event_process_t proc )
	{
		MML_SM_EVENT * sm = new MML_SM_EVENT(event, proc);

		if ( ref->list == mmlNULL )
		{
			ref->list = sm;
		}
		else
		{
			ref->list->Append(sm);	
		}
		return *this;
	}

	MML_SM_STATE& SetEnter (mml_sm_state_enter_t _enter)
	{
		ref->enter =  _enter;
		return *this;
	}

	MML_SM_STATE& SetExit (mml_sm_state_exit_t _exit)
	{
		ref->exit = _exit;
		return *this;
	}
};



#define MML_SM_MAP_BEGIN(name) \
static const MML_SM_STATE name [] = {

#define MML_SM_BEGIN_STATE(state) \
	MML_SM_STATE(state)

#define MML_SM_STATE_ENTER(func) \
	.SetEnter(func)

#define MML_SM_STATE_EVENT(event, func) \
	.AddEvent(event, func)

#define MML_SM_STATE_EXIT(func) \
	.SetExit(func)

#define MML_SM_END_STATE() \
	,

#define MML_SM_MAP_END(name) \
	MML_SM_STATE() \
};

#define MML_SM_MAP(name) &name[0], (sizeof(name) / sizeof(MML_SM_STATE)-1)

#define MML_STATE_MACHINE_UUID { 0x9EBE636C, 0x6D45, 0x1014, 0x94C3 , { 0xC9, 0x91, 0x58, 0x7D, 0x05, 0x9F } }

class mmlIStateMachine : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_STATE_MACHINE_UUID)
public:
	virtual mmlBool Init (const mmlUInt32 initial_state, mmlIObject * context , const MML_SM_STATE * states, const mmlInt32 states_count) = 0;
	virtual mmlBool Process ( const mmlUInt32 event, mmlIObject * arg, void * data, const mmlInt32 data_size, mmlInt32 * data_processed, mmlUInt32 * result ) = 0;
	virtual mmlBool StartTimer(const mmlInt32 event, const mmlInt32 timeout) = 0;
	virtual mmlBool StopTimer(const mmlInt32 event) = 0;
	virtual void Tick() = 0;
};


#endif //MML_STATE_MACHINE_HEADER_INCLUDED
