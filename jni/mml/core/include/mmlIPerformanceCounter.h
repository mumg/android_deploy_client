#ifndef MML_INTERFACE_PERFORMACE_COUNTER_HEADER_INCLUDED
#define MML_INTERFACE_PERFORMACE_COUNTER_HEADER_INCLUDED

#include "mmlIObject.h"

#define MML_IPERFORMANCECOUNTER_UUID {  0x134b273a , 0xab9a , 0x11df , 0xb819 , { 0x97 , 0xa3 , 0x55 , 0x5c , 0x93 , 0x89 } }

class mmlIPerformanceCounter : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_IPERFORMANCECOUNTER_UUID)
public:
	virtual mmlBool Start () = 0;
	virtual mmlBool Stop (mmlInt32 * time) = 0;
	virtual mmlBool GetReport (mmlInt64 * active_time,
		                       mmlInt64 * total_time ) = 0;
};


#endif //MML_INTERFACE_PERFORMACE_COUNTER_HEADER_INCLUDED
