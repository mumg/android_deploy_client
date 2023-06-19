#ifndef ANDROID_CMD_DISPATCHER_HEADER_INCLUDED
#define ANDROID_CMD_DISPATCHER_HEADER_INCLUDED

#include "mmlICString.h"
#include "mmlIVariant.h"

#define CMD_HANDLER_ASSERT(c,e,emsg) \
	if (!(c)){ \
		request->RespondError(e, emsg, mmlNULL); \
		break; \
	}

#define CMD_HANDLER_BEGIN(name) \
void name(android_cmd_request * request) \
{ \
	do \



#define CMD_HANDLER_END() \
	while (0); \
}

#define ANDROID_CMD_REQUEST_UUID { 0x5DB5ACDD, 0x8C27, 0x1014, 0x82D6 , { 0xB5, 0xDB, 0xFC, 0xCC, 0xDF, 0x5C } }

class android_cmd_request: public mmlIObject
{
	MML_OBJECT_UUID_DECL(ANDROID_CMD_REQUEST_UUID);
public:
	virtual void GetRequest(mmlIVariantStruct ** request) = 0;
	virtual void RespondOK(mmlIVariantStruct * data) = 0;
	virtual void RespondError(mmlInt32 errorCode, const mmlChar * errorStr, mmlIVariantStruct * data) = 0;

};

typedef void (*cmd_handler_t) (android_cmd_request * request);

typedef mmlBool(*cmd_init_handler_t)();

typedef void(*cmd_destroy_handler_t)();

typedef struct
{
	const mmlChar * event;
	cmd_init_handler_t init;
	cmd_handler_t handler;
	cmd_destroy_handler_t destroy;
}CMD_MODULE_T;

void android_register_cmd(CMD_MODULE_T * module);

class android_cmd_module
{
private:
	CMD_MODULE_T module;
public:
	android_cmd_module(const mmlChar * event, cmd_handler_t handler, cmd_init_handler_t init, cmd_destroy_handler_t destroy)
	{
		module.event = event;
		module.handler = handler;
		module.init = init;
		module.destroy = destroy;
		android_register_cmd(&module);
	}
};

#define ANDROID_CMD_MODULE(name, event, handler) \
	static android_cmd_module name(event, handler , mmlNULL, mmlNULL );

#define ANDROID_CMD_MODULE_I(name, event, handler, init) \
	static android_cmd_module name(event, handler , init , mmlNULL );

#define ANDROID_CMD_MODULE_D(name, event, handler, destroy) \
	static android_cmd_module name(event, handler , mmlNULL , destroy );

#define ANDROID_CMD_MODULE_ID(name, event, handler, init, destroy) \
	static android_cmd_module name(event, handler , init , destroy );


void android_cmd(mmlICString * url);

void android_cmd_dispatcher_run(mmlBool * terminate);


#endif //ANDROID_CMD_DISPATCHER_HEADER_INCLUDED
