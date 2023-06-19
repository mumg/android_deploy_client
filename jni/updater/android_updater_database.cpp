#include "android_updater_database.h"
#include "mml_services.h"
#include "mml_components.h"

#define VERSION_BEGIN() \
	version ++; \
	if ( current_version < version ) { \

#define VERSION_END() \
	} \

#define QUERY(q) if ( database->Query(q, mmlNULL, mmlNULL, mmlNULL ) == mmlFalse ) return mmlFalse;


static mmlUUID android_db_uuid = { 0x28B2E9F3, 0x6EAA, 0x1014, 0xB079 , { 0xA8, 0x2A, 0xCE, 0x1E, 0xB1, 0x10 } };

class android_database_scheme : public mmlIDatabaseScheme
{
	MML_OBJECT_DECL
public:
	 mmlInt32 Ugrade ( mmlIDatabase * database , const mmlInt32 current_version )
	 {
		 mmlInt32 version = 0;
		 VERSION_BEGIN()
			QUERY("CREATE TABLE IF NOT EXISTS logs (timestamp INT NOT NULL, data TEXT NOT NULL)")
			QUERY("CREATE TABLE IF NOT EXISTS runners (id INTEGER PRIMARY KEY AUTOINCREMENT, timestamp INT NOT NULL, uuid TEXT NOT NULL, data BLOB NOT NULL)")
			QUERY("CREATE TABLE IF NOT EXISTS settings (key TEXT PRIMARY KEY , data BLOB NOT NULL)")
		    QUERY("CREATE TABLE IF NOT EXISTS reboot_schedule(timestamp INT NOT NULL, begin INT NOT NULL, end INT NOT NULL)")
		VERSION_END()
		 return version;
	 }
};

MML_OBJECT_IMPL1(android_database_scheme, mmlIDatabaseScheme)

mmlBool android_database_init ()
{
	mmlSharedPtr < mmlIVariantStruct > global_config = mmlGetService(MML_GLOBAL_CONFIG_UUID());

	mmlAutoPtr < mmlIVariantStruct > db_config;
	if ( global_config->Get("database", mmlRelativePtrAddRef < mmlIVariantStruct, mmlIVariant >(db_config)) == mmlFalse )
	{
		return mmlFalse;
	}

	mmlSharedPtr < mmlIDatabase > db = mmlNewObject(MML_OBJECT_UUID(mmlIDatabase), db_config, new android_database_scheme());
	if ( db == mmlNULL )
	{
		return mmlFalse;
	}
	mmlSetService(android_db_uuid, db);
	return mmlTrue;
}

mmlBool android_database_get (mmlIDatabase ** db)
{
	mmlSharedPtr < mmlIDatabase > svc_db = mmlGetService(android_db_uuid);
	if ( svc_db == mmlNULL )
	{
		return mmlFalse;
	}
	*db = svc_db;
	MML_ADDREF(*db);
	return mmlTrue;
}


