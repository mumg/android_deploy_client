#include "android_settings.h"
#include "android_updater_database.h"
#include "mml_components.h"
#include "mmlIMemoryStream.h"
#include "mmlIVariantDeserializer.h"
#include "mmlIVariantSerializer.h"

#define MML_BINARY_VARIANT_SERIALIZER_UUID { 0x6392D37F, 0x6C6E, 0x1014, 0xAE5B , { 0xDC, 0x81, 0x31, 0xDC, 0xEA, 0xA1 } }

#define MML_BINARY_VARIANT_DESERIALIZER_UUID { 0x6E1257B1, 0x6C6E, 0x1014, 0x8067 , { 0x8F, 0x47, 0x3C, 0xAD, 0xC9, 0x8C } }

void android_settings::_load()
{
	if (mSettings != mmlNULL)
	{
		return;
	}
	mSettings = mmlNewVariantStruct();
	mmlAutoPtr < mmlIDatabase > db;
	android_database_get(db.getterAddRef());
	class handler : public mmlIDatabaseQueryHandler
	{
		MML_OBJECT_STATIC_DECL
	private:
		mmlAutoPtr < mmlIVariantStruct > mSettings;
		mmlSharedPtr < mmlIVariantDeserializer > mDeserializer;
	public:
		handler(mmlIVariantStruct * settings)
			:mSettings(settings)
		{
			static mmlUUID binary_deserializer_uuid = MML_BINARY_VARIANT_DESERIALIZER_UUID;
			mDeserializer = mmlNewObject(binary_deserializer_uuid);
		}

		mmlBool OnRow(const mmlInt32 index, mmlIDatabaseRow * row)
		{
			mmlAutoPtr < mmlIVariantString > key;
			mmlAutoPtr < mmlIVariantRaw > data;
			if (row->GetValue(0, mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(key)) == mmlFalse)
			{
				return mmlTrue;
			}
			if (row->GetValue(1, mmlRelativePtrAddRef < mmlIVariantRaw, mmlIVariant >(data)) == mmlFalse)
			{
				return mmlTrue;
			}
			mmlAutoPtr < mmlIBuffer > blob;
			data->Get(blob.getterAddRef());
			mmlSharedPtr < mmlIInputStream > stream = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream), blob);

			mmlAutoPtr < mmlIVariant > value;
			if (mDeserializer->Read(stream, value.getterAddRef()) == mmlFalse)
			{
				return mmlTrue;
			}
			mSettings->Set(key->Get(), value);
			return mmlTrue;
		}
	}handler(mSettings);
	db->Query("SELECT key, data FROM settings", &handler, mmlNULL, mmlNULL);
}

void android_settings::_save(const mmlChar * name, mmlIVariant * data)
{
	mmlSharedPtr < mmlIMemoryOutputStream > blob = mmlNewObject(MML_OBJECT_UUID(mmlIMemoryOutputStream));
	static mmlUUID binary_serializer_uuid = MML_BINARY_VARIANT_SERIALIZER_UUID;
	mmlSharedPtr < mmlIVariantSerializer > serializer = mmlNewObject(binary_serializer_uuid);
	if (serializer == mmlNULL)
	{
		return;
	}
	if (serializer->Write(blob, data) == mmlFalse)
	{
		return;
	}
	blob->Close();
	mmlAutoPtr < mmlIBuffer > blob_data;
	blob->GetData(blob_data.getterAddRef());
	mmlAutoPtr < mmlIDatabase > db;
	android_database_get(db.getterAddRef());
	mmlAutoPtr < mmlIVariantInteger > cnt;
	mmlAutoPtr < mmlIVariantString > key = mmlNewVariantString(mmlNewCString(name));
	if ( (db->Query("SELECT count(*) FROM settings WHERE key = ?", mmlDatabaseObjectQuery<mmlIVariantInteger>(cnt), key, mmlNULL) == mmlTrue) &&
		 cnt->Get() == 0)
	{
		db->Query("INSERT INTO settings (key, data) VALUES(?,?)", mmlNULL,
			mmlNewVariantArrayFill(
				mmlNewVariantString(mmlNewCString(name)),
				mmlNewVariantRaw(blob_data)
			));
	}
	else 
	{
		db->Query("UPDATE settings SET data = ? WHERE key = ?", mmlNULL,
			mmlNewVariantArrayFill(
				mmlNewVariantRaw(blob_data),
				mmlNewVariantString(mmlNewCString(name))
			));
	}
}
void android_settings::Set(const mmlChar * name, mmlIVariant * value)
{
	_load();
	mSettings->Set(name, value);
	_save(name, value);
}
void android_settings::Set(mmlICString * name, mmlIVariant * value)
{
	_load();
	mSettings->Set(name, value);
	_save(name->Get(), value);
}
mmlBool android_settings::Has(const mmlChar * name)
{
	_load();
	return mSettings->Has(name);
}
mmlBool android_settings::Get(const mmlChar * name, mmlIVariant ** value, const mmlBool create)
{
	_load();
	return mSettings->Get(name, value, create);
}
mmlInt32 android_settings::Size()
{
	_load();
	return mSettings->Size();
}
mmlBool android_settings::Enumerate(mmlIVariantStructEnumerator * en)
{
	_load();
	return mSettings->Enumerate(en);
}
void android_settings::Inherit(mmlIVariantStruct * str)
{
	_load();
	mSettings->Inherit(str);
}
mml_variant_type android_settings::GetType()
{
	_load();
	return mSettings->GetType();
}
void android_settings::Dump(const mmlInt32 depth, void * arg, mml_variant_log_func func)
{
	_load();
	mSettings->Dump(depth, arg, func);
}

mmlBool android_settings::Compare(mmlIVariant * object,
	const mmlCompareOperator mode,
	mmlBool * _retval)
{
	_load();
	return mSettings->Compare(object, mode, _retval);
}

mmlBool android_settings::IsChanged()
{
	_load();
	return mSettings->IsChanged();
}
void android_settings::ResetChanged()
{
	_load();
	mSettings->ResetChanged();
}
MML_OBJECT_IMPL1(android_settings, mmlIVariantStruct)