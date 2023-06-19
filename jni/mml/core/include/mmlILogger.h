#ifndef MML_INTERFACE_LOGGER_HEADER_INCLUDED
#define MML_INTERFACE_LOGGER_HEADER_INCLUDED

#include "mmlIObject.h"

class mmlILoggerOutput : public mmlIObject
{
public:
	virtual void DoLog ( const mmlChar * str ) = 0;
};

#define MML_ILOGGER_MODULE_UUID {  0xbd79c2ba , 0x88fb , 0x11de , 0xad6a , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

class mmlILoggerModule : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_ILOGGER_MODULE_UUID);
public:

	virtual mmlBool Lock ( mmlChar ** buffer , mmlInt32 * maxLen ) = 0;

	virtual mmlBool UnLock () = 0;

};

#define MML_ILOGGER_UUID {  0x2ebfc9e2 , 0x88de , 0x11de , 0xbde2 , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

class mmlILogger : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_ILOGGER_UUID);
public:
	virtual mmlBool Create ( const mmlChar * component , mmlILoggerModule ** module ) = 0;

	virtual mmlBool Attach ( mmlILoggerOutput * output , mmlInt32 * output_id ) = 0;

	virtual mmlBool Detach ( const mmlInt32 output_id ) = 0;

	virtual mmlBool Configure ( const mmlChar * cfg ) = 0;

	virtual mmlBool DoLog ( const mmlChar * str ) = 0;

};


typedef void (*_logger_func)(const mmlChar * formatter , ...);

#define MML_IBINARY_LOGGER_UUID {  0xbc221db6 , 0x89a2 , 0x11de , 0xbae0 , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

class mmlIBinaryLogger : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_IBINARY_LOGGER_UUID)
public:
	virtual void DoLog (const mmlChar * prefix , void * data , const mmlInt32 data_len , _logger_func func) = 0;
};

#define MML_IMULTILINE_LOGGER_UUID {  0x7abdafac , 0x9190 , 0x11de , 0xa379 , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

class mmlIMultilineLogger : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_IMULTILINE_LOGGER_UUID)
public:
	virtual void DoLog (const mmlChar * prefix , void * data , const mmlInt32 data_len , _logger_func func) = 0;
};

#define MML_COMPONENT_LOGGER_IMPL(_component)                                                               \
static mmlILoggerModule * _logger_##_component = mmlNULL;                                                   \
void log_init_##_component(mmlILogger * logger)                                                                                \
{                                                                                                           \
	logger->Create(#_component, &_logger_##_component);                                                     \
}                                                                                                           \
void log_##_component ( const mmlChar * formatter , ... )                                                   \
{                                                                                                           \
	if ( _logger_##_component != mmlNULL )                                                                  \
	{                                                                                                       \
		mmlChar * buffer;                                                                                   \
		mmlInt32 buffer_len;                                                                                \
		if ( _logger_##_component->Lock(&buffer , &buffer_len) == mmlTrue )                                 \
		{                                                                                                   \
			va_list vp;                                                                                     \
			va_start(vp , formatter );                                                                      \
			mmlVSprintf(buffer , buffer_len , formatter , vp);                                              \
			va_end(vp);                                                                                     \
			_logger_##_component->UnLock();                                                                 \
		}                                                                                                   \
	}                                                                                                       \
}                                                                                                           \
void log_##_component##_v ( const mmlChar * formatter , va_list vp )                                        \
{                                                                                                           \
	if ( _logger_##_component != mmlNULL )                                                                  \
	{                                                                                                       \
		mmlChar * buffer;                                                                                   \
		mmlInt32 buffer_len;                                                                                \
		if ( _logger_##_component->Lock(&buffer , &buffer_len) == mmlTrue )                                 \
		{                                                                                                   \
			mmlVSprintf(buffer , buffer_len , formatter , vp);                                              \
			_logger_##_component->UnLock();                                                                 \
		}                                                                                                   \
	}                                                                                                       \
}                                                                                                           \
void log_##_component##_o_v ( const mmlChar offset, const mmlChar * formatter , va_list vp )                \
{                                                                                                           \
	if ( _logger_##_component != mmlNULL )                                                                  \
	{                                                                                                       \
		mmlChar * buffer;                                                                                   \
		mmlInt32 buffer_len;                                                                                \
		if ( _logger_##_component->Lock(&buffer , &buffer_len) == mmlTrue )                                 \
		{                                                                                                   \
			mmlMemorySet(buffer , ' ', offset);                                                             \
			mmlVSprintf(buffer + offset, buffer_len - offset, formatter , vp);                              \
			_logger_##_component->UnLock();                                                                 \
		}                                                                                                   \
	}                                                                                                       \
}                                                                                                           \
void log_destroy_##_component()                                                                             \
{                                                                                                           \
    MML_RELEASE(_logger_##_component);                                                                      \
}                                                                                                           \



#endif //MML_INTERFACE_LOGGER_HEADER_INCLUDED

