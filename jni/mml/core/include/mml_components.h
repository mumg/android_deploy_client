#ifndef MML_COMPONENTS_HEADER_INCLUDED
#define MML_COMPONENTS_HEADER_INCLUDED

#include "mmlIObject.h"
#include "mmlILogger.h"
#include "mml_strutils.h"
#include "mmlPtr.h"
#include <stdlib.h>
#include <stdarg.h>

inline mmlUUID MML_GLOBAL_CONFIG_UUID ()
{
	mmlUUID uuid = { 0xC3D1CCE0, 0x6C04, 0x1014, 0x90EB , { 0xA0, 0x19, 0x1C, 0xD1, 0x6A, 0x07 } };
	return uuid;
};

typedef mmlIObject * (*mml_object_constructor)(const mmlUUID & class_uuid,
	                                           mmlIObject * arg0, 
											   mmlIObject * arg1,
											   mmlIObject * arg2,
											   mmlIObject * arg3,
											   mmlIObject * arg4,
											   mmlIObject * arg5,
											   mmlIObject * arg6,
											   mmlIObject * arg7,
											   mmlIObject * arg8,
											   mmlIObject * arg9,
											   mmlIObject * arga,
											   mmlIObject * argb,
											   mmlIObject * argc,
											   mmlIObject * argd,
											   mmlIObject * arge,
											   mmlIObject * argf);

typedef struct  
{
	mmlUUID uuid;
	const mmlChar * name;
	const mmlInt32 signature_size;
	const mmlUUID * signature;
	mml_object_constructor constructor;
}mmlObjectFactory;

mml_core mmlBool mmlGetObjectUUID (const mmlChar * component, mmlUUID & uuid );

mml_core mmlBool mmlRegisterComponentFactory ( mmlObjectFactory * factory );

mml_core mmlBool mmlUnRegisterComponentFactory (mmlObjectFactory * factory);

mml_core mmlIObject * mmlNewObject ( const mmlUUID & uuid , 
									 mmlIObject * arg0 = mmlNULL, 
									 mmlIObject * arg1 = mmlNULL,
									 mmlIObject * arg2 = mmlNULL,
									 mmlIObject * arg3 = mmlNULL,
									 mmlIObject * arg4 = mmlNULL,
									 mmlIObject * arg5 = mmlNULL,
									 mmlIObject * arg6 = mmlNULL,
									 mmlIObject * arg7 = mmlNULL,
									 mmlIObject * arg8 = mmlNULL,
									 mmlIObject * arg9 = mmlNULL,
									 mmlIObject * arga = mmlNULL,
									 mmlIObject * argb = mmlNULL,
									 mmlIObject * argc = mmlNULL,
									 mmlIObject * argd = mmlNULL,
									 mmlIObject * arge = mmlNULL,
									 mmlIObject * argf = mmlNULL);

typedef mmlBool (*mml_component_constructor)();

typedef void (*mml_component_destructor)();

mml_core mmlBool mmlRegisterComponent ( const mmlChar * name , mml_component_constructor constr , mml_component_destructor destr , mmlObjectFactory * factory, const mmlInt32 factory_size , mmlUUID * before_deps , mmlUUID * after_deps);

#ifdef MML_SHARED   

#define MML_COMPONENT_IMPL_LOCAL(_component, _factories_list)                                               \
	MML_COMPONENT_LOGGER_IMPL(_component)                                                                   \
	mml_export void mmlInitComponent_##_component (mmlILogger * logger)                                     \
{                                                                                                           \
	log_init_##_component(logger);                                                                          \
	for ( mmlInt32 index = 0 ; index < (sizeof(_factories_list) / sizeof(mmlObjectFactory)) ; index ++ )    \
	mmlRegisterComponentFactory(&(_factories_list[index]));                                                 \
}                                                                                                           \
	mml_export void mmlDestroyComponent_##_component ()                                                     \
{                                                                                                           \
	log_destroy_##_component();                                                                             \
}    

#define MML_COMPONENT_IMPL(_component, _factories_list)                                                     \
MML_COMPONENT_LOGGER_IMPL(_component)                                                                       \
	                                                                                                        \
mml_export void mmlInitComponent (mmlILogger * logger)                                                      \
{                                                                                                           \
	log_init_##_component(logger);                                                                          \
	for ( mmlUInt32 index = 0 ; index < (sizeof(_factories_list) / sizeof(mmlObjectFactory)) ; index ++ )    \
		mmlRegisterComponentFactory(&(_factories_list[index]));                                             \
}                                                                                                           \
mml_export void mmlDestroyComponent ()                                                                      \
{                                                                                                           \
    log_destroy_##_component();                                                                             \
}                                                                                                           \

#define MML_COMPONENT_IMPL_C(_component, _factories_list, _ctor,_bdeps, _adeps)                             \
	MML_COMPONENT_LOGGER_IMPL(_component)                                                                   \
	mml_export void mmlInitComponent (mmlILogger * logger)                                                  \
{                                                                                                           \
	log_init_##_component(logger);                                                                          \
	for ( mmlUInt32 index = 0 ; index < (sizeof(_factories_list) / sizeof(mmlObjectFactory)) ; index ++ )    \
	mmlRegisterComponentFactory(&(_factories_list[index]));                                                 \
	mmlRegisterComponent(#_component, _ctor, mmlNULL , _factories_list ,                                    \
                                   (sizeof(_factories_list) / sizeof(mmlObjectFactory)),                    \
								   _bdeps,                                                                  \
                                   _adeps);                                                                 \
}                                                                                                           \
	mml_export void mmlDestroyComponent ()                                                                  \
{                                                                                                           \
	log_destroy_##_component();                                                                             \
} 


#define MML_COMPONENT_IMPL_EMPTY_C(_component, _ctor,_bdeps, _adeps)                                        \
	MML_COMPONENT_LOGGER_IMPL(_component)                                                                   \
	mml_export void mmlInitComponent (mmlILogger * logger)                                                  \
{                                                                                                           \
	log_init_##_component(logger);                                                                          \
	mmlRegisterComponent(#_component, _ctor, mmlNULL , mmlNULL ,                                            \
                                   0,                                                                       \
								   _bdeps,                                                                  \
                                   _adeps);                                                                 \
}                                                                                                           \
	mml_export void mmlDestroyComponent ()                                                                  \
{                                                                                                           \
	log_destroy_##_component();                                                                             \
} 


#define MML_COMPONENT_IMPL_D(_component, _factories_list, _dtor)                                            \
	MML_COMPONENT_LOGGER_IMPL(_component)                                                                   \
	mml_export void mmlInitComponent (mmlILogger * logger)                                                  \
{                                                                                                           \
	log_init_##_component(logger);                                                                          \
	for ( mmlUInt32 index = 0 ; index < (sizeof(_factories_list) / sizeof(mmlObjectFactory)) ; index ++ )    \
	mmlRegisterComponentFactory(&(_factories_list[index]));                                                 \
	mmlRegisterComponent(#_component, mmlNULL, _dtor , mmlNULL , 0 ,  mmlNULL , mmlNULL);                   \
}                                                                                                           \
	mml_export void mmlDestroyComponent ()                                                                  \
{                                                                                                           \
	log_destroy_##_component();                                                                             \
}  

#define MML_COMPONENT_IMPL_CD(_component, _factories_list, _ctor, _dtor, _bdeps , _adeps)                   \
	MML_COMPONENT_LOGGER_IMPL(_component)                                                                   \
	mml_export void mmlInitComponent (mmlILogger * logger)                                                  \
{                                                                                                           \
	log_init_##_component(logger);                                                                          \
	for ( mmlUInt32 index = 0 ; index < (sizeof(_factories_list) / sizeof(mmlObjectFactory)) ; index ++ )    \
	mmlRegisterComponentFactory(&(_factories_list[index]));                                                 \
	mmlRegisterComponent(#_component, _ctor, _dtor , _factories_list ,                                      \
                                   (sizeof(_factories_list) / sizeof(mmlObjectFactory)),                    \
								   _bdeps,                                                                  \
                                   _adeps);                                                                 \
}                                                                                                           \
	mml_export void mmlDestroyComponent ()                                                                  \
{                                                                                                           \
	log_destroy_##_component();                                                                             \
} 

#define MML_COMPONENT_IMPL_EMPTY_CD(_component, _ctor, _dtor, _bdeps , _adeps)                              \
	MML_COMPONENT_LOGGER_IMPL(_component)                                                                   \
	mml_export void mmlInitComponent (mmlILogger * logger)                                                  \
{                                                                                                           \
	log_init_##_component(logger);                                                                          \
	mmlRegisterComponent(#_component, _ctor, _dtor , mmlNULL ,                                              \
                                   0,                                                                       \
								   _bdeps,                                                                  \
                                   _adeps);                                                                 \
}                                                                                                           \
	mml_export void mmlDestroyComponent ()                                                                  \
{                                                                                                           \
	log_destroy_##_component();                                                                             \
} 

#else                                                                                            
#define MML_COMPONENT_IMPL(_component, _factories_list)                                                     \
MML_COMPONENT_LOGGER_IMPL(_component)                                                                       \
mml_export void mmlInitComponent_##_component (mmlILogger * logger)                                         \
{                                                                                                           \
	log_init_##_component(logger);                                                                          \
	for ( mmlUInt32 index = 0 ; index < (sizeof(_factories_list) / sizeof(mmlObjectFactory)) ; index ++ )    \
	mmlRegisterComponentFactory(&(_factories_list[index]));                                                 \
}                                                                                                           \
mml_export void mmlDestroyComponent_##_component ()                                                         \
{                                                                                                           \
	log_destroy_##_component();                                                                             \
}                                                                                                           \

#define MML_COMPONENT_IMPL_LOCAL MML_COMPONENT_IMPL

#define MML_COMPONENT_IMPL_C(_component, _factories_list, _ctor,   _bdeps , _adeps)                         \
MML_COMPONENT_LOGGER_IMPL(_component)                                                                       \
mml_export void mmlInitComponent_##_component (mmlILogger * logger)                                         \
{                                                                                                           \
	log_init_##_component(logger);                                                                          \
	for ( mmlUInt32 index = 0 ; index < (sizeof(_factories_list) / sizeof(mmlObjectFactory)) ; index ++ )    \
	mmlRegisterComponentFactory(&(_factories_list[index]));                                                 \
	mmlRegisterComponent(#_component, _ctor, mmlNULL , _factories_list ,                                    \
                                   (sizeof(_factories_list) / sizeof(mmlObjectFactory)),                    \
								   _bdeps, _adeps);                                                         \
}                                                                                                           \
mml_export void mmlDestroyComponent_##_component ()                                                         \
{                                                                                                           \
	log_destroy_##_component();                                                                             \
}                                                                                                           \
	

#define MML_COMPONENT_IMPL_D(_component, _factories_list, _dtor)                                            \
MML_COMPONENT_LOGGER_IMPL(_component)                                                                       \
mml_export void mmlInitComponent_##_component (mmlILogger * logger)                                         \
{                                                                                                           \
	log_init_##_component(logger);                                                                          \
	for ( mmlUInt32 index = 0 ; index < (sizeof(_factories_list) / sizeof(mmlObjectFactory)) ; index ++ )    \
	mmlRegisterComponentFactory(&(_factories_list[index]));                                                 \
	mmlRegisterComponent(#_component, mmlNULL, _dtor , mmlNULL ,                                            \
                                   0,                                                                       \
								   mmlNULL);                                                                \
}                                                                                                           \
mml_export void mmlDestroyComponent_##_component ()                                                         \
{                                                                                                           \
	log_destroy_##_component();                                                                             \
}                                                                                                           \

#define MML_COMPONENT_IMPL_CD(_component, _factories_list, _ctor, _dtor, _bdeps, _adeps)                    \
MML_COMPONENT_LOGGER_IMPL(_component)                                                                       \
mml_export void mmlInitComponent_##_component (mmlILogger * logger)                                         \
{                                                                                                           \
	log_init_##_component(logger);                                                                          \
	for ( mmlUInt32 index = 0 ; index < (sizeof(_factories_list) / sizeof(mmlObjectFactory)) ; index ++ )    \
	mmlRegisterComponentFactory(&(_factories_list[index]));                                                 \
	mmlRegisterComponent(#_component, _ctor, _dtor , _factories_list ,                                      \
                                   (sizeof(_factories_list) / sizeof(mmlObjectFactory)),                    \
								   _bdeps, _adeps);                                                         \
}                                                                                                           \
mml_export void mmlDestroyComponent_##_component ()                                                         \
{                                                                                                           \
	log_destroy_##_component();                                                                             \
}                                                                                                           \

#define MML_COMPONENT_IMPL_EMPTY_C(_component, _ctor,_bdeps, _adeps)                                        \
	MML_COMPONENT_LOGGER_IMPL(_component)                                                                   \
	mml_export void mmlInitComponent_##_component (mmlILogger * logger)                                                  \
{                                                                                                           \
	log_init_##_component(logger);                                                                          \
	mmlRegisterComponent(#_component, _ctor, mmlNULL , mmlNULL ,                                            \
	0,                                                                       \
	_bdeps,                                                                  \
	_adeps);                                                                 \
}                                                                                                           \
	mml_export void mmlDestroyComponent_##_component ()                                                                  \
{                                                                                                           \
	log_destroy_##_component();                                                                             \
}

#endif //MML_SHARED

#define MML_CONSTRUCTOR_UNUSED_ARG(num)                                                                     \
	(void)arg##num;

#define MML_CONSTRUCTOR_ARG(num, type)                                                                      \
	mmlSharedPtr < type > c##num = arg##num;                                                                \


#define MML_CONSTRUCTOR_IMPL(_class)                                                                        \
	mmlIObject * _class##c                                                                                  \
                                     (const mmlUUID & class_uuid,                                           \
                                      mmlIObject * arg0,                                                    \
                                      mmlIObject * arg1,                                                    \
                                      mmlIObject * arg2,                                                    \
                                      mmlIObject * arg3,                                                    \
                                      mmlIObject * arg4,                                                    \
                                      mmlIObject * arg5,                                                    \
                                      mmlIObject * arg6,                                                    \
                                      mmlIObject * arg7,                                                    \
                                      mmlIObject * arg8,                                                    \
                                      mmlIObject * arg9,                                                    \
                                      mmlIObject * arg10,                                                   \
                                      mmlIObject * arg11,                                                   \
                                      mmlIObject * arg12,                                                   \
                                      mmlIObject * arg13,                                                   \
                                      mmlIObject * arg14,                                                   \
                                      mmlIObject * arg15)                                                   \
{                                                                                                           \
	MML_CONSTRUCTOR_UNUSED_ARG(0);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(1);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(2);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(3);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(4);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(5);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(6);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(7);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(8);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(9);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(10);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(11);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(12);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(13);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	return new _class;                                                                                      \
}                                                                                                           \

#define MML_CONSTRUCTOR_IMPL0(_class)                                                                        \
	mmlIObject * _class##c                                                                                  \
                                     (const mmlUUID & class_uuid,                                           \
                                      mmlIObject * arg0,                                                    \
                                      mmlIObject * arg1,                                                    \
                                      mmlIObject * arg2,                                                    \
                                      mmlIObject * arg3,                                                    \
                                      mmlIObject * arg4,                                                    \
                                      mmlIObject * arg5,                                                    \
                                      mmlIObject * arg6,                                                    \
                                      mmlIObject * arg7,                                                    \
                                      mmlIObject * arg8,                                                    \
                                      mmlIObject * arg9,                                                    \
                                      mmlIObject * arg10,                                                   \
                                      mmlIObject * arg11,                                                   \
                                      mmlIObject * arg12,                                                   \
                                      mmlIObject * arg13,                                                   \
                                      mmlIObject * arg14,                                                   \
                                      mmlIObject * arg15)                                                   \
{                                                                                                           \
	MML_CONSTRUCTOR_UNUSED_ARG(0);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(1);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(2);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(3);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(4);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(5);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(6);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(7);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(8);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(9);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(10);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(11);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(12);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(13);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct() == mmlFalse ) { delete object; return mmlNULL; }                               \
	return object;                                                                                          \
}                                                                                                           \

#define MML_CONSTRUCTOR_IMPL1(_class, t0)                                                                   \
	static mmlUUID _class##s##t0 [] =                                                                       \
	{                                                                                                       \
		t0::GetUIID()                                                                                       \
	};                                                                                                      \
	mmlIObject * _class##c##t0                                                                              \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
    MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_UNUSED_ARG(1);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(2);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(3);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(4);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(5);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(6);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(7);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(8);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(9);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(10);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(11);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(12);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(13);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0) == mmlFalse ) { delete object; return mmlNULL; }                             \
	return object;                                                                                          \
}

#define MML_CONSTRUCTOR_IMPL2(_class, t0, t1)                                                               \
	static mmlUUID _class##s##t0##t1 [] =                                                                   \
	{                                                                                                       \
		t0::GetUIID(),                                                                                      \
		t1::GetUIID()                                                                                       \
	};                                                                                                      \
	mmlIObject * _class##c##t0##t1                                                                          \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
	MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_ARG(1 , t1);                                                                            \
	MML_CONSTRUCTOR_UNUSED_ARG(2);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(3);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(4);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(5);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(6);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(7);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(8);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(9);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(10);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(11);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(12);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(13);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0, c1) == mmlFalse ) { delete object; return mmlNULL; }                         \
	return object;                                                                                          \
}


#define MML_CONSTRUCTOR_IMPL3(_class, t0, t1, t2)                                                           \
	static mmlUUID _class##s##t0##t1##t2 [] =                                                               \
	{                                                                                                       \
		t0::GetUIID(),                                                                                      \
		t1::GetUIID(),                                                                                      \
		t2::GetUIID()                                                                                       \
	};                                                                                                      \
	mmlIObject * _class##c##t0##t1##t2                                                                      \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
	MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_ARG(1 , t1);                                                                            \
	MML_CONSTRUCTOR_ARG(2 , t2);                                                                            \
	MML_CONSTRUCTOR_UNUSED_ARG(3);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(4);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(5);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(6);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(7);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(8);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(9);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(10);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(11);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(12);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(13);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0,                                                                              \
                           c1,                                                                              \
						   c2) == mmlFalse ) { delete object; return mmlNULL; }                             \
	return object;                                                                                          \
}


#define MML_CONSTRUCTOR_IMPL4(_class, t0, t1, t2, t3)                                                       \
	static mmlUUID _class##s##t0##t1##t2##t3 [] =                                                           \
	{                                                                                                       \
		t0::GetUIID(),                                                                                      \
		t1::GetUIID(),                                                                                      \
		t2::GetUIID(),                                                                                      \
		t3::GetUIID()                                                                                       \
	};                                                                                                      \
	mmlIObject * _class##c##t0##t1##t2##t3                                                                  \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
	MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_ARG(1 , t1);                                                                            \
	MML_CONSTRUCTOR_ARG(2 , t2);                                                                            \
	MML_CONSTRUCTOR_ARG(3 , t3);                                                                            \
	MML_CONSTRUCTOR_UNUSED_ARG(4);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(5);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(6);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(7);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(8);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(9);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(10);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(11);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(12);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(13);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0,                                                                              \
                           c1,                                                                              \
						   c2,                                                                              \
                           c3) == mmlFalse ) { delete object; return mmlNULL; }                             \
	return object;                                                                                          \
}

#define MML_CONSTRUCTOR_IMPL5(_class, t0, t1, t2, t3, t4)                                                   \
	static mmlUUID _class##s##t0##t1##t2##t3##t4 [] =                                                       \
	{                                                                                                       \
		t0::GetUIID(),                                                                                      \
		t1::GetUIID(),                                                                                      \
		t2::GetUIID(),                                                                                      \
		t3::GetUIID(),                                                                                      \
		t4::GetUIID()                                                                                       \
	};                                                                                                      \
	mmlIObject * _class##c##t0##t1##t2##t3##t4                                                              \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
	MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_ARG(1 , t1);                                                                            \
	MML_CONSTRUCTOR_ARG(2 , t2);                                                                            \
	MML_CONSTRUCTOR_ARG(3 , t3);                                                                            \
	MML_CONSTRUCTOR_ARG(4 , t4);                                                                            \
	MML_CONSTRUCTOR_UNUSED_ARG(5);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(6);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(7);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(8);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(9);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(10);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(11);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(12);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(13);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0,                                                                              \
                           c1,                                                                              \
						   c2,                                                                              \
                           c3,                                                                              \
						   c4) == mmlFalse ) { delete object; return mmlNULL; }                             \
	return object;                                                                                          \
}


#define MML_CONSTRUCTOR_IMPL6(_class, t0, t1, t2, t3, t4, t5)                                               \
	static mmlUUID _class##s##t0##t1##t2##t3##t4##t5 [] =                                                   \
	{                                                                                                       \
		t0::GetUIID(),                                                                                      \
		t1::GetUIID(),                                                                                      \
		t2::GetUIID(),                                                                                      \
		t3::GetUIID(),                                                                                      \
		t4::GetUIID(),                                                                                      \
		t5::GetUIID()                                                                                       \
	};                                                                                                      \
	mmlIObject * _class##c##t0##t1##t2##t3##t4##t5                                                          \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
	MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_ARG(1 , t1);                                                                            \
	MML_CONSTRUCTOR_ARG(2 , t2);                                                                            \
	MML_CONSTRUCTOR_ARG(3 , t3);                                                                            \
	MML_CONSTRUCTOR_ARG(4 , t4);                                                                            \
	MML_CONSTRUCTOR_ARG(5 , t5);                                                                            \
	MML_CONSTRUCTOR_UNUSED_ARG(6);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(7);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(8);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(9);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(10);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(11);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(12);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(13);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0,                                                                              \
                           c1,                                                                              \
						   c2,                                                                              \
                           c3,                                                                              \
						   c4,                                                                              \
						   c5) == mmlFalse ) { delete object; return mmlNULL; }                             \
	return object;                                                                                          \
}

#define MML_CONSTRUCTOR_IMPL7(_class, t0, t1, t2, t3, t4, t5, t6)                                           \
	static mmlUUID _class##s##t0##t1##t2##t3##t4##t5##t6 [] =                                               \
	{                                                                                                       \
		t0::GetUIID(),                                                                                      \
		t1::GetUIID(),                                                                                      \
		t2::GetUIID(),                                                                                      \
		t3::GetUIID(),                                                                                      \
		t4::GetUIID(),                                                                                      \
		t5::GetUIID(),                                                                                      \
		t6::GetUIID()                                                                                       \
	};                                                                                                      \
	mmlIObject * _class##c##t0##t1##t2##t3##t4##t5##t6                                                      \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
	MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_ARG(1 , t1);                                                                            \
	MML_CONSTRUCTOR_ARG(2 , t2);                                                                            \
	MML_CONSTRUCTOR_ARG(3 , t3);                                                                            \
	MML_CONSTRUCTOR_ARG(4 , t4);                                                                            \
	MML_CONSTRUCTOR_ARG(5 , t5);                                                                            \
	MML_CONSTRUCTOR_ARG(6 , t6);                                                                            \
	MML_CONSTRUCTOR_UNUSED_ARG(7);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(8);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(9);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(10);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(11);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(12);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(13);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0,                                                                              \
                           c1,                                                                              \
						   c2,                                                                              \
                           c3,                                                                              \
						   c4,                                                                              \
						   c5,                                                                              \
                           c6) == mmlFalse ) { delete object; return mmlNULL; }                             \
	return object;                                                                                          \
}

#define MML_CONSTRUCTOR_IMPL8(_class, t0, t1, t2, t3, t4, t5, t6, t7)                                       \
	static mmlUUID _class##s##t0##t1##t2##t3##t4##t5##t6##t7 [] =                                           \
	{                                                                                                       \
		t0::GetUIID(),                                                                                      \
		t1::GetUIID(),                                                                                      \
		t2::GetUIID(),                                                                                      \
		t3::GetUIID(),                                                                                      \
		t4::GetUIID(),                                                                                      \
		t5::GetUIID(),                                                                                      \
		t6::GetUIID(),                                                                                      \
		t7::GetUIID()                                                                                       \
	};                                                                                                      \
	mmlIObject * _class##c##t0##t1##t2##t3##t4##t5##t6##t7                                                  \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
	MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_ARG(1 , t1);                                                                            \
	MML_CONSTRUCTOR_ARG(2 , t2);                                                                            \
	MML_CONSTRUCTOR_ARG(3 , t3);                                                                            \
	MML_CONSTRUCTOR_ARG(4 , t4);                                                                            \
	MML_CONSTRUCTOR_ARG(5 , t5);                                                                            \
	MML_CONSTRUCTOR_ARG(6 , t6);                                                                            \
	MML_CONSTRUCTOR_ARG(7 , t7);                                                                            \
	MML_CONSTRUCTOR_UNUSED_ARG(8);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(9);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(10);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(11);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(12);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(13);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0,                                                                              \
                           c1,                                                                              \
						   c2,                                                                              \
                           c3,                                                                              \
						   c4,                                                                              \
						   c5,                                                                              \
                           c6,                                                                              \
                           c7) == mmlFalse ) { delete object; return mmlNULL; }                             \
	return object;                                                                                          \
}

#define MML_CONSTRUCTOR_IMPL9(_class, t0, t1, t2, t3, t4, t5, t6, t7, t8)                                   \
	static mmlUUID _class##s##t0##t1##t2##t3##t4##t5##t6##t7##t8 [] =                                       \
	{                                                                                                       \
		t0::GetUIID(),                                                                                      \
		t1::GetUIID(),                                                                                      \
		t2::GetUIID(),                                                                                      \
		t3::GetUIID(),                                                                                      \
		t4::GetUIID(),                                                                                      \
		t5::GetUIID(),                                                                                      \
		t6::GetUIID(),                                                                                      \
		t7::GetUIID(),                                                                                      \
		t8::GetUIID()                                                                                       \
	};                                                                                                      \
	mmlIObject * _class##c##t0##t1##t2##t3##t4##t5##t6##t7##t8                                              \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
	MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_ARG(1 , t1);                                                                            \
	MML_CONSTRUCTOR_ARG(2 , t2);                                                                            \
	MML_CONSTRUCTOR_ARG(3 , t3);                                                                            \
	MML_CONSTRUCTOR_ARG(4 , t4);                                                                            \
	MML_CONSTRUCTOR_ARG(5 , t5);                                                                            \
	MML_CONSTRUCTOR_ARG(6 , t6);                                                                            \
	MML_CONSTRUCTOR_ARG(7 , t7);                                                                            \
	MML_CONSTRUCTOR_ARG(8 , t8);                                                                            \
	MML_CONSTRUCTOR_UNUSED_ARG(9);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(10);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(11);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(12);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(13);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0,                                                                              \
                           c1,                                                                              \
						   c2,                                                                              \
                           c3,                                                                              \
						   c4,                                                                              \
						   c5,                                                                              \
                           c6,                                                                              \
                           c7,                                                                              \
                           c8) == mmlFalse ) { delete object; return mmlNULL; }                             \
	return object;                                                                                          \
}


#define MML_CONSTRUCTOR_IMPL10(_class, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9)                              \
	static mmlUUID _class##s##t0##t1##t2##t3##t4##t5##t6##t7##t8##t9 [] =                                   \
	{                                                                                                       \
		t0::GetUIID(),                                                                                      \
		t1::GetUIID(),                                                                                      \
		t2::GetUIID(),                                                                                      \
		t3::GetUIID(),                                                                                      \
		t4::GetUIID(),                                                                                      \
		t5::GetUIID(),                                                                                      \
		t6::GetUIID(),                                                                                      \
		t7::GetUIID(),                                                                                      \
		t8::GetUIID(),                                                                                      \
		t9::GetUIID()                                                                                       \
	};                                                                                                      \
	mmlIObject * _class##c##t0##t1##t2##t3##t4##t5##t6##t7##t8##t9                                          \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
	MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_ARG(1 , t1);                                                                            \
	MML_CONSTRUCTOR_ARG(2 , t2);                                                                            \
	MML_CONSTRUCTOR_ARG(3 , t3);                                                                            \
	MML_CONSTRUCTOR_ARG(4 , t4);                                                                            \
	MML_CONSTRUCTOR_ARG(5 , t5);                                                                            \
	MML_CONSTRUCTOR_ARG(6 , t6);                                                                            \
	MML_CONSTRUCTOR_ARG(7 , t7);                                                                            \
	MML_CONSTRUCTOR_ARG(8 , t8);                                                                            \
	MML_CONSTRUCTOR_ARG(9 , t9);                                                                            \
	MML_CONSTRUCTOR_UNUSED_ARG(10);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(11);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(12);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(13);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0,                                                                              \
                           c1,                                                                              \
						   c2,                                                                              \
                           c3,                                                                              \
						   c4,                                                                              \
						   c5,                                                                              \
                           c6,                                                                              \
                           c7,                                                                              \
                           c8,                                                                              \
                           c9) == mmlFalse ) { delete object; return mmlNULL; }                             \
	return object;                                                                                          \
}

#define MML_CONSTRUCTOR_IMPL11(_class, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10)                         \
	static mmlUUID _class##s##t0##t1##t2##t3##t4##t5##t6##t7##t8##t9##t10 [] =                              \
	{                                                                                                       \
		t0::GetUIID(),                                                                                      \
		t1::GetUIID(),                                                                                      \
		t2::GetUIID(),                                                                                      \
		t3::GetUIID(),                                                                                      \
		t4::GetUIID(),                                                                                      \
		t5::GetUIID(),                                                                                      \
		t6::GetUIID(),                                                                                      \
		t7::GetUIID(),                                                                                      \
		t8::GetUIID(),                                                                                      \
		t9::GetUIID(),                                                                                      \
		t10::GetUIID()                                                                                      \
	};                                                                                                      \
	mmlIObject * _class##c##t0##t1##t2##t3##t4##t5##t6##t7##t8##t9##t10                                     \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
	MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_ARG(1 , t1);                                                                            \
	MML_CONSTRUCTOR_ARG(2 , t2);                                                                            \
	MML_CONSTRUCTOR_ARG(3 , t3);                                                                            \
	MML_CONSTRUCTOR_ARG(4 , t4);                                                                            \
	MML_CONSTRUCTOR_ARG(5 , t5);                                                                            \
	MML_CONSTRUCTOR_ARG(6 , t6);                                                                            \
	MML_CONSTRUCTOR_ARG(7 , t7);                                                                            \
	MML_CONSTRUCTOR_ARG(8 , t8);                                                                            \
	MML_CONSTRUCTOR_ARG(9 , t9);                                                                            \
	MML_CONSTRUCTOR_ARG(10 , t10);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(11);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(12);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(13);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0,                                                                              \
                           c1,                                                                              \
						   c2,                                                                              \
                           c3,                                                                              \
						   c4,                                                                              \
						   c5,                                                                              \
                           c6,                                                                              \
                           c7,                                                                              \
                           c8,                                                                              \
                           c9,                                                                              \
						   c10) == mmlFalse ) { delete object; return mmlNULL; }                            \
	return object;                                                                                          \
}

#define MML_CONSTRUCTOR_IMPL12(_class, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11)                    \
	static mmlUUID _class##s##t0##t1##t2##t3##t4##t5##t6##t7##t8##t9##t10##t11 [] =                         \
	{                                                                                                       \
		t0::GetUIID(),                                                                                      \
		t1::GetUIID(),                                                                                      \
		t2::GetUIID(),                                                                                      \
		t3::GetUIID(),                                                                                      \
		t4::GetUIID(),                                                                                      \
		t5::GetUIID(),                                                                                      \
		t6::GetUIID(),                                                                                      \
		t7::GetUIID(),                                                                                      \
		t8::GetUIID(),                                                                                      \
		t9::GetUIID(),                                                                                      \
		t10::GetUIID(),                                                                                     \
		t11::GetUIID()                                                                                      \
	};                                                                                                      \
	mmlIObject * _class##c##t0##t1##t2##t3##t4##t5##t6##t7##t8##t9##t10##t11                                \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
	MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_ARG(1 , t1);                                                                            \
	MML_CONSTRUCTOR_ARG(2 , t2);                                                                            \
	MML_CONSTRUCTOR_ARG(3 , t3);                                                                            \
	MML_CONSTRUCTOR_ARG(4 , t4);                                                                            \
	MML_CONSTRUCTOR_ARG(5 , t5);                                                                            \
	MML_CONSTRUCTOR_ARG(6 , t6);                                                                            \
	MML_CONSTRUCTOR_ARG(7 , t7);                                                                            \
	MML_CONSTRUCTOR_ARG(8 , t8);                                                                            \
	MML_CONSTRUCTOR_ARG(9 , t9);                                                                            \
	MML_CONSTRUCTOR_ARG(10 , t10);                                                                          \
	MML_CONSTRUCTOR_ARG(11 , t11);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(12);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(13);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0,                                                                              \
                           c1,                                                                              \
						   c2,                                                                              \
                           c3,                                                                              \
						   c4,                                                                              \
						   c5,                                                                              \
                           c6,                                                                              \
                           c7,                                                                              \
                           c8,                                                                              \
                           c9,                                                                              \
						   c10,                                                                             \
						   c11) == mmlFalse ) { delete object; return mmlNULL; }                            \
	return object;                                                                                          \
}


#define MML_CONSTRUCTOR_IMPL13(_class, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12)               \
	static mmlUUID _class##s##t0##t1##t2##t3##t4##t5##t6##t7##t8##t9##t10##t11##t12 [] =                    \
	{                                                                                                       \
		t0::GetUIID(),                                                                                      \
		t1::GetUIID(),                                                                                      \
		t2::GetUIID(),                                                                                      \
		t3::GetUIID(),                                                                                      \
		t4::GetUIID(),                                                                                      \
		t5::GetUIID(),                                                                                      \
		t6::GetUIID(),                                                                                      \
		t7::GetUIID(),                                                                                      \
		t8::GetUIID(),                                                                                      \
		t9::GetUIID(),                                                                                      \
		t10::GetUIID(),                                                                                     \
		t11::GetUIID(),                                                                                     \
		t12::GetUIID()                                                                                      \
	};                                                                                                      \
	mmlIObject * _class##c##t0##t1##t2##t3##t4##t5##t6##t7##t8##t9##t10##t11##t12                           \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
	MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_ARG(1 , t1);                                                                            \
	MML_CONSTRUCTOR_ARG(2 , t2);                                                                            \
	MML_CONSTRUCTOR_ARG(3 , t3);                                                                            \
	MML_CONSTRUCTOR_ARG(4 , t4);                                                                            \
	MML_CONSTRUCTOR_ARG(5 , t5);                                                                            \
	MML_CONSTRUCTOR_ARG(6 , t6);                                                                            \
	MML_CONSTRUCTOR_ARG(7 , t7);                                                                            \
	MML_CONSTRUCTOR_ARG(8 , t8);                                                                            \
	MML_CONSTRUCTOR_ARG(9 , t9);                                                                            \
	MML_CONSTRUCTOR_ARG(10 , t10);                                                                          \
	MML_CONSTRUCTOR_ARG(11 , t11);                                                                          \
	MML_CONSTRUCTOR_ARG(12 , t12);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(13);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0,                                                                              \
                           c1,                                                                              \
						   c2,                                                                              \
                           c3,                                                                              \
						   c4,                                                                              \
						   c5,                                                                              \
                           c6,                                                                              \
                           c7,                                                                              \
                           c8,                                                                              \
                           c9,                                                                              \
						   c10,                                                                             \
						   c11,                                                                             \
						   c12) == mmlFalse ) { delete object; return mmlNULL; }                            \
	return object;                                                                                          \
}

#define MML_CONSTRUCTOR_IMPL14(_class, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13)          \
	static mmlUUID _class##s##t0##t1##t2##t3##t4##t5##t6##t7##t8##t9##t10##t11##t12##t13 [] =               \
	{                                                                                                       \
		t0::GetUIID(),                                                                                      \
		t1::GetUIID(),                                                                                      \
		t2::GetUIID(),                                                                                      \
		t3::GetUIID(),                                                                                      \
		t4::GetUIID(),                                                                                      \
		t5::GetUIID(),                                                                                      \
		t6::GetUIID(),                                                                                      \
		t7::GetUIID(),                                                                                      \
		t8::GetUIID(),                                                                                      \
		t9::GetUIID(),                                                                                      \
		t10::GetUIID(),                                                                                     \
		t11::GetUIID(),                                                                                     \
		t12::GetUIID(),                                                                                     \
		t13::GetUIID()                                                                                      \
	};                                                                                                      \
	mmlIObject * _class##c##t0##t1##t2##t3##t4##t5##t6##t7##t8##t9##t10##t11##t12##t13                      \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
	MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_ARG(1 , t1);                                                                            \
	MML_CONSTRUCTOR_ARG(2 , t2);                                                                            \
	MML_CONSTRUCTOR_ARG(3 , t3);                                                                            \
	MML_CONSTRUCTOR_ARG(4 , t4);                                                                            \
	MML_CONSTRUCTOR_ARG(5 , t5);                                                                            \
	MML_CONSTRUCTOR_ARG(6 , t6);                                                                            \
	MML_CONSTRUCTOR_ARG(7 , t7);                                                                            \
	MML_CONSTRUCTOR_ARG(8 , t8);                                                                            \
	MML_CONSTRUCTOR_ARG(9 , t9);                                                                            \
	MML_CONSTRUCTOR_ARG(10 , t10);                                                                          \
	MML_CONSTRUCTOR_ARG(11 , t11);                                                                          \
	MML_CONSTRUCTOR_ARG(12 , t12);                                                                          \
	MML_CONSTRUCTOR_ARG(13 , t13);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(14);                                                                         \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0,                                                                              \
                           c1,                                                                              \
						   c2,                                                                              \
                           c3,                                                                              \
						   c4,                                                                              \
						   c5,                                                                              \
                           c6,                                                                              \
                           c7,                                                                              \
                           c8,                                                                              \
                           c9,                                                                              \
						   c10,                                                                             \
						   c11,                                                                             \
						   c12,                                                                             \
						   c13) == mmlFalse ) { delete object; return mmlNULL; }                            \
	return object;                                                                                          \
}

#define MML_CONSTRUCTOR_IMPL15(_class, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14)     \
	static mmlUUID _class##s##t0##t1##t2##t3##t4##t5##t6##t7##t8##t9##t10##t11##t12##t13##t14 [] =          \
	{                                                                                                       \
		t0::GetUIID(),                                                                                      \
		t1::GetUIID(),                                                                                      \
		t2::GetUIID(),                                                                                      \
		t3::GetUIID(),                                                                                      \
		t4::GetUIID(),                                                                                      \
		t5::GetUIID(),                                                                                      \
		t6::GetUIID(),                                                                                      \
		t7::GetUIID(),                                                                                      \
		t8::GetUIID(),                                                                                      \
		t9::GetUIID(),                                                                                      \
		t10::GetUIID(),                                                                                     \
		t11::GetUIID(),                                                                                     \
		t12::GetUIID(),                                                                                     \
		t13::GetUIID(),                                                                                     \
		t14::GetUIID()                                                                                      \
	};                                                                                                      \
	mmlIObject * _class##c##t0##t1##t2##t3##t4##t5##t6##t7##t8##t9##t10##t11##t12##t13##t14                 \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
	MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_ARG(1 , t1);                                                                            \
	MML_CONSTRUCTOR_ARG(2 , t2);                                                                            \
	MML_CONSTRUCTOR_ARG(3 , t3);                                                                            \
	MML_CONSTRUCTOR_ARG(4 , t4);                                                                            \
	MML_CONSTRUCTOR_ARG(5 , t5);                                                                            \
	MML_CONSTRUCTOR_ARG(6 , t6);                                                                            \
	MML_CONSTRUCTOR_ARG(7 , t7);                                                                            \
	MML_CONSTRUCTOR_ARG(8 , t8);                                                                            \
	MML_CONSTRUCTOR_ARG(9 , t9);                                                                            \
	MML_CONSTRUCTOR_ARG(10 , t10);                                                                          \
	MML_CONSTRUCTOR_ARG(11 , t11);                                                                          \
	MML_CONSTRUCTOR_ARG(12 , t12);                                                                          \
	MML_CONSTRUCTOR_ARG(13 , t13);                                                                          \
	MML_CONSTRUCTOR_ARG(14 , t14);                                                                          \
	MML_CONSTRUCTOR_UNUSED_ARG(15);                                                                         \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0,                                                                              \
                           c1,                                                                              \
						   c2,                                                                              \
                           c3,                                                                              \
						   c4,                                                                              \
						   c5,                                                                              \
                           c6,                                                                              \
                           c7,                                                                              \
                           c8,                                                                              \
                           c9,                                                                              \
						   c10,                                                                             \
						   c11,                                                                             \
						   c12,                                                                             \
						   c13,                                                                             \
						   c14) == mmlFalse ) { delete object; return mmlNULL; }                            \
	return object;                                                                                          \
}

#define MML_CONSTRUCTOR_IMPL16(_class, t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15)\
	static mmlUUID _class##s##t0##t1##t2##t3##t4##t5##t6##t7##t8##t9##t10##t11##t12##t13##t14##t15 [] =     \
	{                                                                                                       \
		t0::GetUIID(),                                                                                      \
		t1::GetUIID(),                                                                                      \
		t2::GetUIID(),                                                                                      \
		t3::GetUIID(),                                                                                      \
		t4::GetUIID(),                                                                                      \
		t5::GetUIID(),                                                                                      \
		t6::GetUIID(),                                                                                      \
		t7::GetUIID(),                                                                                      \
		t8::GetUIID(),                                                                                      \
		t9::GetUIID(),                                                                                      \
		t10::GetUIID(),                                                                                     \
		t11::GetUIID(),                                                                                     \
		t12::GetUIID(),                                                                                     \
		t13::GetUIID(),                                                                                     \
		t14::GetUIID(),                                                                                     \
		t15::GetUIID()                                                                                      \
	};                                                                                                      \
	mmlIObject * _class##c##t0##t1##t2##t3##t4##t5##t6##t7##t8##t9##t10##t11##t12##t13##t14##t15            \
                                      (const mmlUUID & class_uuid,                                          \
                                       mmlIObject * arg0,                                                   \
									   mmlIObject * arg1,                                                   \
									   mmlIObject * arg2,                                                   \
									   mmlIObject * arg3,                                                   \
									   mmlIObject * arg4,                                                   \
									   mmlIObject * arg5,                                                   \
									   mmlIObject * arg6,                                                   \
									   mmlIObject * arg7,                                                   \
									   mmlIObject * arg8,                                                   \
									   mmlIObject * arg9,                                                   \
									   mmlIObject * arg10,                                                  \
									   mmlIObject * arg11,                                                  \
									   mmlIObject * arg12,                                                  \
									   mmlIObject * arg13,                                                  \
									   mmlIObject * arg14,                                                  \
									   mmlIObject * arg15)                                                  \
{                                                                                                           \
	MML_CONSTRUCTOR_ARG(0 , t0);                                                                            \
	MML_CONSTRUCTOR_ARG(1 , t1);                                                                            \
	MML_CONSTRUCTOR_ARG(2 , t2);                                                                            \
	MML_CONSTRUCTOR_ARG(3 , t3);                                                                            \
	MML_CONSTRUCTOR_ARG(4 , t4);                                                                            \
	MML_CONSTRUCTOR_ARG(5 , t5);                                                                            \
	MML_CONSTRUCTOR_ARG(6 , t6);                                                                            \
	MML_CONSTRUCTOR_ARG(7 , t7);                                                                            \
	MML_CONSTRUCTOR_ARG(8 , t8);                                                                            \
	MML_CONSTRUCTOR_ARG(9 , t9);                                                                            \
	MML_CONSTRUCTOR_ARG(10 , t10);                                                                          \
	MML_CONSTRUCTOR_ARG(11 , t11);                                                                          \
	MML_CONSTRUCTOR_ARG(12 , t12);                                                                          \
	MML_CONSTRUCTOR_ARG(13 , t13);                                                                          \
	MML_CONSTRUCTOR_ARG(14 , t14);                                                                          \
	MML_CONSTRUCTOR_ARG(15 , t15);                                                                          \
	_class * object = new _class;                                                                           \
	if ( object->Construct(c0,                                                                              \
                           c1,                                                                              \
						   c2,                                                                              \
                           c3,                                                                              \
						   c4,                                                                              \
						   c5,                                                                              \
                           c6,                                                                              \
                           c7,                                                                              \
                           c8,                                                                              \
                           c9,                                                                              \
						   c10,                                                                             \
						   c11,                                                                             \
						   c12,                                                                             \
						   c13,                                                                             \
						   c14,                                                                             \
                           c15) == mmlFalse ) { delete object; return mmlNULL; }                            \
	return object;                                                                                          \
}


#define MML_CONSTRUCTOR(_class)                                                                             \
	0 , mmlNULL , _class##c                                                                               

#define MML_CONSTRUCTOR1(_class, a0)                                                                        \
	1 ,                                                                                                     \
	_class##s##a0,                                                                                          \
	_class##c##a0

#define MML_CONSTRUCTOR2(_class, a0, a1)                                                                    \
	2 ,                                                                                                     \
    _class##s##a0##a1,                                                                                      \
	_class##c##a0##a1

#define MML_CONSTRUCTOR3(_class, a0, a1, a2)                                                                \
	3 ,                                                                                                     \
	_class##s##a0##a1##a2,                                                                                  \
	_class##c##a0##a1##a2

#define MML_CONSTRUCTOR4(_class, a0, a1, a2, a3)                                                            \
	4 ,                                                                                                     \
	_class##s##a0##a1##a2##a3,                                                                              \
	_class##c##a0##a1##a2##a3

#define MML_CONSTRUCTOR5(_class, a0, a1, a2, a3, a4)                                                        \
	5 ,                                                                                                     \
	_class##s##a0##a1##a2##a3##a4,                                                                          \
	_class##c##a0##a1##a2##a3##a4

#define MML_CONSTRUCTOR6(_class, a0, a1, a2, a3, a4, a5)                                                    \
	6 ,                                                                                                     \
	_class##s##a0##a1##a2##a3##a4##a5,                                                                      \
	_class##c##a0##a1##a2##a3##a4##a5

#define MML_CONSTRUCTOR7(_class, a0, a1, a2, a3, a4, a5, a6)                                                \
	7 ,                                                                                                     \
	_class##s##a0##a1##a2##a3##a4##a5##a6,                                                                  \
	_class##c##a0##a1##a2##a3##a4##a5##a6

#define MML_CONSTRUCTOR8(_class, a0, a1, a2, a3, a4, a5, a6, a7)                                            \
	8 ,                                                                                                     \
	_class##s##a0##a1##a2##a3##a4##a5##a6##a7,                                                              \
	_class##c##a0##a1##a2##a3##a4##a5##a6##a7

#define MML_CONSTRUCTOR9(_class, a0, a1, a2, a3, a4, a5, a6, a7, a8)                                        \
	9 ,                                                                                                     \
	_class##s##a0##a1##a2##a3##a4##a5##a6##a7##a8,                                                          \
	_class##c##a0##a1##a2##a3##a4##a5##a6##a7##a8

#define MML_CONSTRUCTOR10(_class, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9)                                   \
	10 ,                                                                                                    \
	_class##s##a0##a1##a2##a3##a4##a5##a6##a7##a8##a9,                                                      \
	_class##c##a0##a1##a2##a3##a4##a5##a6##a7##a8##a9

#define MML_CONSTRUCTOR11(_class, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)                              \
	11 ,                                                                                                    \
	_class##s##a0##a1##a2##a3##a4##a5##a6##a7##a8##a9##a10,                                                 \
	_class##c##a0##a1##a2##a3##a4##a5##a6##a7##a8##a9##a10

#define MML_CONSTRUCTOR12(_class, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11)                         \
	12 ,                                                                                                    \
	_class##s##a0##a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11,                                            \
	_class##c##a0##a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11

#define MML_CONSTRUCTOR13(_class, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12)                    \
	13 ,                                                                                                    \
	_class##s##a0##a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12,                                       \
	_class##c##a0##a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12

#define MML_CONSTRUCTOR14(_class, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13)               \
	14 ,                                                                                                    \
	_class##s##a0##a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13,                                  \
	_class##c##a0##a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13

#define MML_CONSTRUCTOR15(_class, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14)          \
	15 ,                                                                                                    \
	_class##s##a0##a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14,                             \
	_class##c##a0##a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14

#define MML_CONSTRUCTOR16(_class, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15)     \
	16 ,                                                                                                    \
	_class##s##a0##a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a15,                        \
	_class##c##a0##a1##a2##a3##a4##a5##a6##a7##a8##a9##a10##a11##a12##a13##a14##a16


#ifdef MML_SHARED
#define MML_COMPONENT_INIT(_component, _logger)
#define MML_COMPONENT_DESTROY(_component)
#define MML_COMPONENT_INIT_LOCAL(_component, _logger) mmlInitComponent_##_component(_logger);
#define MML_COMPONENT_DESTROY_LOCAL(_component) mmlDestroyComponent_##_component();

#else
#define MML_COMPONENT_INIT(_component, _logger) mmlInitComponent_##_component(_logger);
#define MML_COMPONENT_DESTROY(_component) mmlDestroyComponent_##_component();
#define MML_COMPONENT_INIT_LOCAL(_component, _logger) mmlInitComponent_##_component(_logger);
#define MML_COMPONENT_DESTROY_LOCAL(_component) mmlDestroyComponent_##_component();
#endif

#ifdef MML_SHARED
#define MML_COMPONENT_DECL(_component)
#else
#define MML_COMPONENT_DECL(_component) \
	void mmlInitComponent_##_component(mmlILogger*logger); \
	void mmlDestroyComponent_##_component(); 
#endif


#define MML_OBJECT_VARGS \
	mmlIObject * a0 = mmlNULL,  \
	mmlIObject * a1 = mmlNULL,  \
	mmlIObject * a2 = mmlNULL,  \
	mmlIObject * a3 = mmlNULL,  \
	mmlIObject * a4 = mmlNULL,  \
	mmlIObject * a5 = mmlNULL,  \
	mmlIObject * a6 = mmlNULL,  \
	mmlIObject * a7 = mmlNULL,  \
	mmlIObject * a8 = mmlNULL,  \
	mmlIObject * a9 = mmlNULL,  \
	mmlIObject * a10 = mmlNULL, \
	mmlIObject * a11 = mmlNULL, \
	mmlIObject * a12 = mmlNULL, \
	mmlIObject * a13 = mmlNULL, \
	mmlIObject * a14 = mmlNULL, \
	mmlIObject * a15 = mmlNULL

#define MML_OBJECT_VARGS_FORWARD \
a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15

template < class T >
class mmlObjectPtr 
{
public:
	mmlObjectPtr(MML_OBJECT_VARGS)
	{
		mObject = mmlNewObject(MML_OBJECT_UUID(T),
			MML_OBJECT_VARGS_FORWARD);
	}

	mmlObjectPtr( const mmlUUID & uuid )
	{
		mObject = mmlNewObject(uuid);
	}

	operator T * ()
	{
		return mObject;
	}

private:

	mmlSharedPtr < T > mObject;
};

#endif //MML_COMPONENTS_HEADER_INCLUDED


