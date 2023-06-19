#ifndef MML_RANDOM_HEADER_INCLUDED
#define MML_RANDOM_HEADER_INCLUDED

#include "mmlIRandom.h"

class mmlRandom : public mmlIRandom
{
	MML_OBJECT_DECL;
public:
	mmlRandom();
	mmlBool NewRandom ( mmlInt64 * value );
	mmlBool NewRandomFill ( void * buffer , const mmlInt32 len );
private:
	mmlUInt16 rand();
	mmlUInt16 lfsr;
	mmlUInt16 bit;
};

#endif //MML_RANDOM_HEADER_INCLUDED

