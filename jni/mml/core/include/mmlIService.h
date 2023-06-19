#ifndef MML_SERVICE_HEADER_INLCUDED
#define MML_SERVICE_HEADER_INLCUDED

#include "mmlIObject.h"

#define MML_SERVICE_UUID { 0x2C91CF08, 0x6DB1, 0x1014, 0x945C , { 0xFE, 0xC2, 0xB7, 0x3D, 0xAB, 0x92 } }

class mmlIService : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_SERVICE_UUID)
public:
	virtual void SvcStop () = 0;
	virtual void SvcWaitForStop () = 0;
};


#endif //MML_SERVICE_HEADER_INLCUDED