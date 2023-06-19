#ifndef MML_INTERFACE_EVENT_HEADER_INCLUDED
#define MML_INTERFACE_EVENT_HEADER_INCLUDED

#include "mmlIObject.h"

#define MML_EVENT_UUID_IMPL(_class, _uuid)               \
	mmlBool _class::GetEventUUID ( mmlUUID & out_uuid )  \
	{                                                    \
	    static mmlUUID the_uuid = uuid;                  \
		return the_uuid;                                 \
	}                                                    \

#define MML_EVENT_UUID_DECL                              \
	mmlBool GetEventUUID ( mmlUUID & out_uuid );         \

#define MML_IEVENT_UUID {  0x5fb79a22 , 0x9070 , 0x11de , 0xb81e , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

class mmlIEvent : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_IEVENT_UUID)
public:
	virtual mmlBool GetEventUUID( mmlUUID & uuid ) = 0;

};

#endif //MML_INTERFACE_EVENT_HEADER_INCLUDED

