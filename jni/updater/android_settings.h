#ifndef ANDROID_SETTINGS_HEADER_INCLUDED
#define ANDROID_SETTINGS_HEADER_INCLUDED

#include "mmlIVariant.h"
#include "mmlIObject.h"

class android_settings : public mmlIVariantStruct
{
	MML_OBJECT_DECL
private:
	mmlSharedPtr < mmlIVariantStruct > mSettings;
	void _load();
	void _save(const mmlChar * name, mmlIVariant * data);
public:
	void Set(const mmlChar * name, mmlIVariant * value);
	void Set(mmlICString * name, mmlIVariant * value);
	mmlBool Has(const mmlChar * name);
	mmlBool Get(const mmlChar * name, mmlIVariant ** value, const mmlBool create = mmlFalse);
	mmlInt32 Size();
	mmlBool Enumerate(mmlIVariantStructEnumerator * en);
	void Inherit(mmlIVariantStruct * str);
	mml_variant_type GetType();
	void Dump(const mmlInt32 depth, void * arg, mml_variant_log_func func);

	mmlBool Compare(mmlIVariant * object,
		const mmlCompareOperator mode,
		mmlBool * _retval);

	mmlBool IsChanged();
	void ResetChanged();
};



#endif //ANDROID_SETTINGS_HEADER_INCLUDED
