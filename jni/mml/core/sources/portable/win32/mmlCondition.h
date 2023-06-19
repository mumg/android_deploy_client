#ifndef MML_CONDITION_HEADER_INCLUDED
#define MML_CONDITION_HEADER_INCLUDED

#include "mmlICondition.h"
#include <Windows.h>

class mmlCondition : public mmlICondition
{
	MML_OBJECT_DECL;
public:
	mmlCondition();

	~mmlCondition();

	mmlBool Wait ( const mmlInt32 timeout );

	mmlBool Signal ();

private:
	HANDLE mHandle;
	mmlInt32 mSignalCounter;
};

#endif //MML_CONDITION_HEADER_INCLUDED

