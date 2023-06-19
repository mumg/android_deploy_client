#include "android_properties.h"
#include "mml_components.h"
#include "mmlIVariant.h"
#include "mml_services.h"
#include "android_settings.h"


#define MML_CONFIG_BEGIN() \
	mmlAutoPtr < mmlIVariantStruct > current = config;

#define MML_CONFIG_END() \


#define MML_CONFIG_BEGIN_SECTION(name) \
{ \
	mmlAutoPtr < mmlIVariantStruct > sub_section; \
	if ( current->Get(name, mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant >(sub_section)) == mmlFalse ) \
	{ \
		sub_section = mmlNewVariantStruct(); \
		current->Set(name, sub_section); \
	} \
	{ \
		mmlAutoPtr < mmlIVariantStruct > current = sub_section;


#define MML_CONFIG_BEGIN_ARRAY(name) \
{ \
	mmlAutoPtr < mmlIVariantArray > array = mmlNewVariantArray(); \
	current->Set(name, array);

#define MML_CONFIG_END_ARRAY() \
}

#define MML_CONFIG_BEGIN_ARRAY_SECTION() \
{\
	mmlAutoPtr < mmlIVariantStruct > current = mmlNewVariantStruct(); \
	array->Push(current); \

#define MML_CONFIG_END_ARRAY_SECTION() \
}

#define MML_CONFIG_END_SECTION() \
	} \
}

#define MML_CONFIG_ADD_PROPERTY(prop, name) \
	{ \
		if ( current->Has(name) == mmlFalse ) \
		{ \
			mmlAutoPtr < mmlICString > value; \
			if ( android_property_get(prop, value.getterAddRef()) == mmlFalse ) return mmlFalse; \
			current->Set(name, mmlNewVariantString(value)); \
		} \
	}

#define MML_CONFIG_INT32(name, value) \
	if ( current->Has(name) == mmlFalse ) current->Set(name, mmlNewVariantInteger(value));

#define MML_CONFIG_STRING(name, value) \
	if ( current->Has(name) == mmlFalse ) current->Set(name, mmlNewVariantString(mmlNewStaticCString(value)));

extern const mmlChar * customer_id;
extern const mmlChar * updater_version;
mmlBool android_config_init (mmlIVariantStruct * config)
{
	MML_CONFIG_BEGIN()
		MML_CONFIG_INT32("debug", 0)
		MML_CONFIG_STRING("updater", updater_version);
	MML_CONFIG_STRING("startup", "/data/subsystem/startup.d")
		MML_CONFIG_STRING("shutdown", "/data/subsystem/shutdown.d")
#ifdef MML_WIN
		MML_CONFIG_STRING("uuid", "id.dat")
#else
		MML_CONFIG_STRING("uuid", "/data/subsystem/id.dat")
#endif
		current->Set("settings", new android_settings());
		MML_CONFIG_BEGIN_SECTION("bootstrap")
			MML_CONFIG_INT32("install", 1)
			MML_CONFIG_INT32("connect", 300000)
			MML_CONFIG_INT32("response", 300000)
			MML_CONFIG_INT32("read", 300000)
			MML_CONFIG_INT32("write", 300000)
			MML_CONFIG_INT32("insecure", 1)
			MML_CONFIG_STRING("url", "https://bootstrap.trker.me/device/get")
		MML_CONFIG_END_SECTION()
	
		MML_CONFIG_BEGIN_SECTION("android")
			MML_CONFIG_ADD_PROPERTY(ANDROID_PROPERTY_SERIALNO, "sn")
			MML_CONFIG_ADD_PROPERTY(ANDROID_PROPERTY_MODEL, "model")
			MML_CONFIG_ADD_PROPERTY(ANDROID_PROPERTY_CPU, "cpu")
			MML_CONFIG_ADD_PROPERTY(ANDROID_PROPERTY_DEVICE, "device")
			MML_CONFIG_ADD_PROPERTY(ANDROID_PROPERTY_MANUFACTURER, "manufacturer")
			MML_CONFIG_ADD_PROPERTY(ANDROID_PROPERTY_RELEASE, "release")
			MML_CONFIG_ADD_PROPERTY(ANDROID_PROPERTY_SDK, "sdk")
			MML_CONFIG_ADD_PROPERTY(ANDROID_PROPERTY_HARDWARE, "hw")
			MML_CONFIG_ADD_PROPERTY(ANDROID_PROPERTY_IMEI, "imei")
			MML_CONFIG_ADD_PROPERTY(ANDROID_PROPERTY_ID, "android_id")
		MML_CONFIG_END_SECTION()
#ifdef MML_WIN
		MML_CONFIG_STRING("temp", "C:/TEMP")
#else
		MML_CONFIG_STRING("temp", "/data/local/tmp")
#endif
		MML_CONFIG_BEGIN_SECTION("database")
#ifdef MML_WIN
			MML_CONFIG_STRING("file", "android_updater.db")
#else
			MML_CONFIG_STRING("file", "/data/subsystem/android_updater_new.db")
#endif
		MML_CONFIG_END_SECTION()
		MML_CONFIG_BEGIN_SECTION("file")
			MML_CONFIG_INT32("insecure", 1)
			MML_CONFIG_INT32("connect", 10000)
			MML_CONFIG_INT32("response", 60000)
			MML_CONFIG_INT32("overall", 1800000)
			MML_CONFIG_INT32("read", 600000)
		MML_CONFIG_END_SECTION()
		MML_CONFIG_BEGIN_SECTION("scenario")
			MML_CONFIG_INT32("insecure", 1)
			MML_CONFIG_INT32("connect", 10000)
			MML_CONFIG_INT32("response", 60000)
			MML_CONFIG_INT32("overall", 180000)
			MML_CONFIG_INT32("read", 60000)
		MML_CONFIG_END_SECTION()
		MML_CONFIG_BEGIN_SECTION("query")
			MML_CONFIG_INT32("insecure", 1)
			MML_CONFIG_INT32("connect", 10000)
			MML_CONFIG_INT32("response", 60000)
			MML_CONFIG_INT32("overall", 180000)
			MML_CONFIG_INT32("read", 60000)
		MML_CONFIG_END_SECTION()
		MML_CONFIG_BEGIN_SECTION("websocket")
			MML_CONFIG_INT32("connect", 20000)
			MML_CONFIG_INT32("response", 30000)
			MML_CONFIG_INT32("insecure", 1)
		MML_CONFIG_END_SECTION()
		MML_CONFIG_BEGIN_SECTION("scheduler")
			MML_CONFIG_INT32("max", 1)
			MML_CONFIG_BEGIN_ARRAY("tasks")
				MML_CONFIG_BEGIN_ARRAY_SECTION()
					MML_CONFIG_STRING("name", "tracking")
					MML_CONFIG_STRING("rule", "periodic")
					MML_CONFIG_INT32("success",	180)
					MML_CONFIG_INT32("fail", 180)
				MML_CONFIG_END_ARRAY_SECTION()
				MML_CONFIG_BEGIN_ARRAY_SECTION()
					MML_CONFIG_STRING("name", "cleanup")
					MML_CONFIG_STRING("rule", "periodic")
					MML_CONFIG_INT32("success", 3600)
					MML_CONFIG_INT32("fail", 3600)
				MML_CONFIG_END_ARRAY_SECTION()
				MML_CONFIG_BEGIN_ARRAY_SECTION()
					MML_CONFIG_STRING("name", "reboot")
					MML_CONFIG_STRING("rule", "periodic")
					MML_CONFIG_INT32("success", 60)
					MML_CONFIG_INT32("fail", 60)
				MML_CONFIG_END_ARRAY_SECTION()
				MML_CONFIG_BEGIN_ARRAY_SECTION()
					MML_CONFIG_STRING("name", "check")
					MML_CONFIG_STRING("rule", "periodic")
					MML_CONFIG_INT32("success", 3600)
					MML_CONFIG_INT32("fail", 3600)
				MML_CONFIG_END_ARRAY_SECTION()
			MML_CONFIG_END_ARRAY_SECTION()
		MML_CONFIG_END_SECTION()
		MML_CONFIG_STRING("customer", customer_id);
	MML_CONFIG_END()
	return mmlTrue;
}
