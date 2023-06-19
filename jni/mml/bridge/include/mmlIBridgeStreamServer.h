#ifndef MML_BRIDGE_SERVER_HEADER_INCLUDED
#define MML_BRIDGE_SERVER_HEADER_INCLUDED

#include "mmlIStream.h"

#define MML_BRIDGE_STREAM_SERVER_UUID { 0xE7841037, 0x6C60, 0x1014, 0x8362 , { 0xC1, 0x43, 0x1E, 0x19, 0x5B, 0xB5 } }

class mmlIBridgeStreamServer : public mmlIStreamServer
{
	MML_OBJECT_UUID_DECL(MML_BRIDGE_STREAM_SERVER_UUID)
public:
};


#endif// MML_BRIDGE_SERVER_HEADER_INCLUDED