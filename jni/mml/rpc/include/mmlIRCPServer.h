#ifndef MML_RPC_SERVER_HEADER_INCLUDED
#define MML_RPC_SERVER_HEADER_INCLUDED

#include "mmlIStream.h"

#define MML_RPC_SERVER_AUTHENTICATOR_UUID { 0x3A624BAE, 0x6C93, 0x1014, 0x8C2E , { 0xFE, 0xEF, 0x17, 0x8F, 0x37, 0x6E } }

class mmlIRPCServerAuthenticator : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_RPC_SERVER_AUTHENTICATOR_UUID)
public:
	virtual mmlBool Authenticate(const mmlChar * source, const mmlChar * iface, const mmlChar * token) = 0;
};

#define MML_RPC_SERVER_FUNCTION_UUID { 0x405244EF, 0x6C93, 0x1014, 0x8AFD , { 0xCA, 0x93, 0xC1, 0x0C, 0xE6, 0x9F } }

class mmlIRPCServerFunction : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_RPC_SERVER_FUNCTION_UUID)
public:
	virtual mmlBool run(mmlIVariant * request, mmlIVariant ** response, mmlIVariantString ** exception_desc) = 0;
};

#define MML_RCP_SERVER_STREAM_HANDLER_UUID { 0x44192050, 0x6C93, 0x1014, 0xA08E , { 0xBF, 0x78, 0x2C, 0x58, 0x45, 0x13 } }

/*
Constructors:
1. arg0 - mmlIRPCServerAuthenticator
2. default, public RPC server
*/

class mmlIRPCServerStreamHandler : public mmlIStreamServerHandler
{
	MML_OBJECT_UUID_DECL(MML_RCP_SERVER_STREAM_HANDLER_UUID)
public:
	virtual void Add(const mmlChar * name, mmlIRPCServerFunction * function) = 0;
	virtual void SetWorkers(const mmlInt32 count) = 0;
};

#endif //MML_RPC_SERVER_HEADER_INCLUDED
