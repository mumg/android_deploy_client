#ifdef MML_WIN
#include "portable/win32/mmlMutex.h"
#endif //MML_WIN

#ifdef MML_POSIX
#include "portable/posix/mmlMutex.h"
#endif

#include <list>
#include <map>
#include "mmlPtr.h"
#include "mml_strutils.h"
#include "mml_components.h"
#include <stdio.h>
#include "mmlIObject.h"

mmlMutex mml_components_mutex;

class mmlUUIDContainer
{
public:
	mmlUUIDContainer( const mmlUUID & uuid )
		:mUUID(uuid)
	{
	}

	mmlUUIDContainer( const mmlUUIDContainer & container )
		:mUUID(container.mUUID)
	{

	}

	mmlUUIDContainer operator = ( const mmlUUIDContainer & container )
	{
		mUUID = container.mUUID;
		return *this;
	}

	mmlInt32 operator == ( const mmlUUIDContainer & container ) const
	{
		return mmlCompareUUID(mUUID , container.mUUID) == 0 ? mmlTrue : mmlFalse;
	}

	mmlInt32 operator < ( const mmlUUIDContainer & container ) const
	{
		return mmlCompareUUID(mUUID , container.mUUID) < 0 ? mmlTrue : mmlFalse;
	}

private:
	mmlUUID mUUID;
};


class mml_object_constructor_holder
{
public:

	const mmlInt32 signature_size;
	const mmlUUID * signature;
	mml_object_constructor constructor;
	mml_object_constructor_holder(const mmlInt32 ss, const mmlUUID * s, mml_object_constructor c)
		:signature_size(ss), signature(s), constructor(c)
	{
	
	}

	mml_object_constructor_holder ( const mml_object_constructor_holder & dup)
		:signature_size(dup.signature_size),
	     signature(dup.signature),
	     constructor (dup.constructor)
	{
	}

	mmlInt32 operator == (const mml_object_constructor_holder & src) const
	{
		if ( constructor != src.constructor )
		{
			return 0;
		}
		if ( signature_size != src.signature_size )
		{
			return 0;
		}
		for ( mmlInt32 index = 0 ; index < signature_size; index ++ )
		{
			if ( mmlCompareUUID(signature[index], src.signature[index]) != 0)
			{
				return 0;
			}
		}

		return 1;
	}

};

std::map < mmlUUIDContainer , std::list < mml_object_constructor_holder > > mml_factory_map;


class mmlStringContainer
{
public:
	mmlStringContainer( const mmlChar * component )
		:mStr(component)
	{
	}

	mmlStringContainer( const mmlStringContainer & container )
		:mStr(container.mStr)
	{

	}

	mmlStringContainer operator = ( const mmlStringContainer & container )
	{
		mStr = container.mStr;
		return *this;
	}

	mmlInt32 operator == ( const mmlStringContainer & container ) const
	{
		return mmlStrCompare(mStr , container.mStr) == 0 ? mmlTrue : mmlFalse;
	}

	mmlInt32 operator < ( const mmlStringContainer & container ) const
	{
		return mmlStrCompare(mStr , container.mStr) < 0 ? mmlTrue : mmlFalse;
	}

private:
	const mmlChar * mStr;
};


std::map < mmlUInt32 , std::map < mmlStringContainer , mmlUUID > > mml_component2uuid_map;

void mmlComponentsInit (mmlObjectFactory * factory_map , mmlInt32 size )
{
	for ( mmlInt32 count = 0 ; count < size ; count ++ )
	{
		mmlRegisterComponentFactory(&(factory_map[count]));
	}
#ifdef MML_SHARED

#endif //MML_SHARED
}

static mmlUUID null_uuid = {0};

static mmlBool mml_construct_dep ( const mmlUUID & uuid );

static void mml_destruct_dep ( const mmlUUID & uuid );

static mmlBool mml_add_dep ( const mmlUUID & where , const mmlUUID & who );

class mml_component_object
{
public:

	mml_component_object ( const mmlChar * name , mml_component_constructor constr , mml_component_destructor destr , mmlObjectFactory * factory, const mmlInt32 factory_size, mmlUUID * before_deps , mmlUUID * after_deps )
		:mName(name), mConstructed(mmlFalse), mDestructed(mmlFalse), mConstructor(constr), mDestructor(destr)
	{
		mmlUUIDGenerate(mUUID);
		for ( mmlInt32 index = 0 ; index < factory_size ; index ++ )
		{
			mComponents.push_back(factory[index].uuid);
		}
		if ( before_deps != mmlNULL )
		{
			while ( mmlCompareUUID(*before_deps , null_uuid) != 0 )
			{
				mBeforeDeps.push_back(*before_deps);
				before_deps ++;
			}
		}
		if ( after_deps != mmlNULL )
		{
			while ( mmlCompareUUID(*after_deps , null_uuid) != 0 )
			{
				mAfterDeps.push_back(*after_deps);
				after_deps ++;
			}
		}
	}

	mmlBool AddDep ( const mmlUUID & dep )
	{
		mAfterDeps.push_back(dep);
		return mmlTrue;
	}

	mmlBool Construct ()
	{
		mDestructed = mmlFalse;
		if ( mConstructor == mmlNULL )
		{
			return mmlTrue;
		}
		if ( mConstructed == mmlTrue )
		{
			return mmlTrue;
		}
		mConstructed = mmlTrue;
		for ( std::list < mmlUUID > ::iterator dep_uuid = mAfterDeps.begin(); dep_uuid != mAfterDeps.end(); dep_uuid ++ )
		{
			if ( mml_construct_dep(*dep_uuid) == mmlFalse )
			{
				return mmlFalse;
			}
		}
		if ( mConstructor() == mmlFalse )
		{
			return mmlFalse;
		}
		for ( std::list < mmlUUID > ::iterator dep_uuid = mBeforeDeps.begin(); dep_uuid != mBeforeDeps.end(); dep_uuid ++ )
		{
			if ( mml_construct_dep(*dep_uuid) == mmlFalse )
			{
				return mmlFalse;
			}
		}

		return mmlTrue;
	}

	void Destruct ()
	{
		if ( mDestructed == mmlTrue )
		{
			return;
		}
		mDestructed = mmlTrue;
		mConstructed = mmlFalse;
		for ( std::list < mmlUUID > ::iterator dep_uuid = mBeforeDeps.begin(); dep_uuid != mBeforeDeps.end(); dep_uuid ++ )
		{
			mml_destruct_dep(*dep_uuid);
		}
		if ( mDestructor != mmlNULL ) mDestructor();
		for ( std::list < mmlUUID > ::iterator dep_uuid = mAfterDeps.begin(); dep_uuid != mAfterDeps.end(); dep_uuid ++ )
		{
			mml_destruct_dep(*dep_uuid);
		}
	}

	mmlBool Equal (const mmlUUID &uuid)
	{
		if ( mmlCompareUUID(mUUID , uuid) == 0 )
		{
			return mmlTrue;
		}
		for ( std::list < mmlUUID > ::iterator my_uuid = mComponents.begin(); my_uuid != mComponents.end(); my_uuid ++ )
		{
			if ( mmlCompareUUID(*my_uuid, uuid) == 0 )
			{
				return mmlTrue;
			}
		}
		return mmlFalse;
	}


	mmlBool Link ()
	{
		for ( std::list < mmlUUID > ::iterator dep_uuid = mBeforeDeps.begin(); dep_uuid != mBeforeDeps.end(); dep_uuid ++ )
		{
			if ( mml_add_dep(*dep_uuid , mUUID) == mmlFalse )
			{
				return mmlFalse;
			}
		}
		return mmlTrue;
	}


private:

	mmlUUID mUUID;

	const mmlChar * mName;

	mmlBool mConstructed;

	mmlBool mDestructed;

	mml_component_constructor mConstructor;

	mml_component_destructor mDestructor;

	std::list < mmlUUID > mComponents;

	std::list < mmlUUID > mBeforeDeps;

	std::list < mmlUUID > mAfterDeps;

};


std::list < mml_component_object > mml_components_list;


static mmlBool mml_construct_dep ( const mmlUUID & uuid )
{
	for (std::list < mml_component_object >::iterator ctr = mml_components_list.begin(); ctr != mml_components_list.end(); ctr ++ )
	{
		if ( ctr->Equal(uuid) == mmlTrue )
		{
			return ctr->Construct();
		}
	}
	return mmlFalse;
}

static void mml_destruct_dep ( const mmlUUID & uuid )
{
	for (std::list < mml_component_object >::iterator ctr = mml_components_list.begin(); ctr != mml_components_list.end(); ctr ++ )
	{
		if ( ctr->Equal(uuid) == mmlTrue )
		{
			ctr->Destruct();
			break;
		}
	}
}

static mmlBool mml_add_dep ( const mmlUUID & uuid , const mmlUUID & dep )
{
	for (std::list < mml_component_object >::iterator ctr = mml_components_list.begin(); ctr != mml_components_list.end(); ctr ++ )
	{
		if ( ctr->Equal(uuid) == mmlTrue )
		{
			ctr->AddDep(dep);
			return mmlTrue;
		}
	}
	return mmlFalse;
}

mmlBool mmlComponentsConstruct ()
{
	for (std::list < mml_component_object >::iterator ctr = mml_components_list.begin(); ctr != mml_components_list.end(); ctr ++ )
	{
		if ( (*ctr).Link() == mmlFalse )
		{
			return mmlFalse;
		}
	}
	for (std::list < mml_component_object >::iterator ctr = mml_components_list.begin(); ctr != mml_components_list.end(); ctr ++ )
	{
		if ( (*ctr).Construct() == mmlFalse )
		{
			return mmlFalse;
		}
	}
	return mmlTrue;
}

void mmlComponentsDestruct ()
{
	for (std::list < mml_component_object >::reverse_iterator dtr = mml_components_list.rbegin(); dtr != mml_components_list.rend(); dtr ++ )
	{
		(*dtr).Destruct();
	}
	mml_components_list.clear();
}

mml_core mmlBool mmlRegisterComponent ( const mmlChar * name , mml_component_constructor constr , mml_component_destructor destr , mmlObjectFactory * factory, const mmlInt32 factory_size , mmlUUID * bdeps , mmlUUID * adeps)
{
	mml_components_mutex.Lock();
	if ( constr != mmlNULL || destr != mmlNULL )
	{
		mml_components_list.push_back(mml_component_object(name , constr , destr , factory , factory_size , bdeps , adeps));
	}
	mml_components_mutex.UnLock();
	return mmlTrue;
}


static mmlUInt32 mmlComputeHash( const mmlChar * str )
{
	mmlUInt32 hash = 0;
	for ( mmlUInt32 index = 0 ; str[index] != 0; index ++ )
	{
		hash += (mmlUInt32)str[index];
	}
	return hash;
}

mml_core mmlBool mmlGetObjectUUID (const mmlChar * component, mmlUUID & uuid )
{
	mmlBool result = mmlFalse;
	if ( component != mmlNULL )
	{
		mml_components_mutex.Lock();
		mmlUInt32 hash = mmlComputeHash(component);
		std::map < mmlUInt32 , std::map < mmlStringContainer , mmlUUID > >::iterator hash_row = mml_component2uuid_map.find(hash);
		if ( hash_row != mml_component2uuid_map.end() )
		{
			std::map < mmlStringContainer , mmlUUID >::iterator row_iter = (*hash_row).second.find(mmlStringContainer(component));
			if ( row_iter != (*hash_row).second.end() )
			{
				uuid = (*row_iter).second;
				result = mmlTrue;
			}
		}
		mml_components_mutex.UnLock();
	}
	return result;
}

mml_core mmlBool mmlRegisterComponentFactory ( mmlObjectFactory * factory )
{
	mmlBool result = mmlFalse;
	if ( factory != mmlNULL )
	{
		mml_components_mutex.Lock();
		if ( factory->name != mmlNULL )
		{
			mmlUInt32 hash = mmlComputeHash(factory->name);
			mml_component2uuid_map[hash][mmlStringContainer(factory->name)] = factory->uuid;
		}
		mml_factory_map[factory->uuid].push_back(mml_object_constructor_holder(factory->signature_size, factory->signature, factory->constructor));
		mml_components_mutex.UnLock();
	}
	return result;
}

mml_core mmlBool mmlUnRegisterComponentFactory (mmlObjectFactory * factory)
{
	mml_components_mutex.Lock();
	if ( factory->name != mmlNULL )
	{
		mmlUInt32 hash = mmlComputeHash(factory->name);
		std::map< mmlUInt32 , std::map< mmlStringContainer , mmlUUID > > ::iterator hash_map = mml_component2uuid_map.find(hash);
		if ( hash_map != mml_component2uuid_map.end())
		{
			hash_map->second.erase(factory->name);
		}
	}
	std::map< mmlUUIDContainer , std::list< mml_object_constructor_holder > >::iterator factory_instance =  mml_factory_map.find(factory->uuid);
	if ( factory_instance != mml_factory_map.end() )
	{
		mml_object_constructor_holder h(factory->signature_size, factory->signature, factory->constructor);
		for ( std::list< mml_object_constructor_holder >::iterator holder = factory_instance->second.begin(); holder != factory_instance->second.end(); holder ++ )
		{
			if ( (*holder) == h)
			{
				factory_instance->second.erase(holder);
				break;
			}
		}
		if ( factory_instance->second.size() == 0 )
		{
			mml_factory_map.erase(factory_instance);
		}
	}
	mml_components_mutex.UnLock();
	return mmlTrue;
}

mml_core mmlIObject * mmlNewObject ( const mmlUUID & uuid,
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
									 mmlIObject * arg10,
									 mmlIObject * arg11,
									 mmlIObject * arg12,
									 mmlIObject * arg13,
									 mmlIObject * arg14,
									 mmlIObject * arg15)
{
	mmlInt32 args = 0;
	mmlIObject * objects[16];
	if ( arg0 != mmlNULL) 
	{ 
		objects[args] = arg0;
		args ++;
		if ( arg1 != mmlNULL) 
		{
			objects[args] = arg1;
			args ++;
			if ( arg2 != mmlNULL) 
			{
				objects[args] = arg2;
				args ++;
				if ( arg3 != mmlNULL) 
				{
					objects[args] = arg3;
					args ++;
					if ( arg4 != mmlNULL) 
					{
						objects[args] = arg4;
						args ++;
						if ( arg5 != mmlNULL) 
						{
							objects[args] = arg5;
							args ++;
							if ( arg6 != mmlNULL) 
							{
								objects[args] = arg6;
								args ++;
								if ( arg7 != mmlNULL) 
								{
									objects[args] = arg7;
									args ++;
									if ( arg8 != mmlNULL) 
									{
										objects[args] = arg8;
										args ++;
										if ( arg9 != mmlNULL) 
										{
											objects[args] = arg9;
											args ++;
											if ( arg10 != mmlNULL) 
											{
												objects[args] = arg10;
												args ++;
												if ( arg11 != mmlNULL) 
												{
													objects[args] = arg11;
													args ++;
													if ( arg12 != mmlNULL) 
													{
														objects[args] = arg12;
														args ++;
														if ( arg13 != mmlNULL) 
														{
															objects[args] = arg13;
															args ++;
															if ( arg14 != mmlNULL) 
															{
																objects[args] = arg14;
																args ++;
																if ( arg15 != mmlNULL) 
																{
																	objects[args] = arg15;
																	args ++;
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	mml_components_mutex.Lock();
	std::map < mmlUUIDContainer , std::list < mml_object_constructor_holder > >::iterator constructor_iter = mml_factory_map.find(uuid);
	if ( constructor_iter == mml_factory_map.end() )
	{
		mml_components_mutex.UnLock();
		return mmlNULL;
	}
	mml_components_mutex.UnLock();
	for ( mmlInt32 index = 0 ; index < args; index ++ )
	{
		MML_ADDREF(objects[index]);
	}
	mmlIObject * result = mmlNULL;
	for ( std::list < mml_object_constructor_holder > ::iterator holder = constructor_iter->second.begin(); holder != constructor_iter->second.end(); holder ++)
	{
		if ( holder->signature_size == args )
		{
			if ( args >= 1 && (arg0 == mmlNULL || arg0->IsRelative(holder->signature[0]) == mmlFalse) )
			{
				continue;
			}
			if ( args >= 2 && (arg1 == mmlNULL || arg1->IsRelative(holder->signature[1]) == mmlFalse) )
			{
				continue;
			}
			if ( args >= 3 && (arg2 == mmlNULL || arg2->IsRelative(holder->signature[2]) == mmlFalse) )
			{
				continue;
			}
			if ( args >= 4 && (arg3 == mmlNULL || arg3->IsRelative(holder->signature[3]) == mmlFalse) )
			{
				continue;
			}
			if ( args >= 5 && (arg4 == mmlNULL || arg4->IsRelative(holder->signature[4]) == mmlFalse) )
			{
				continue;
			}
			if ( args >= 6 && (arg5 == mmlNULL || arg5->IsRelative(holder->signature[5]) == mmlFalse) )
			{
				continue;
			}
			if ( args >= 7 && (arg6 == mmlNULL || arg6->IsRelative(holder->signature[6]) == mmlFalse) )
			{
				continue;
			}
			if ( args >= 8 && (arg7 == mmlNULL || arg7->IsRelative(holder->signature[7]) == mmlFalse) )
			{
				continue;
			}
			if ( args >= 9 && (arg8 == mmlNULL || arg8->IsRelative(holder->signature[8]) == mmlFalse) )
			{
				continue;
			}
			if ( args >= 10 && (arg9 == mmlNULL || arg9->IsRelative(holder->signature[9]) == mmlFalse) )
			{
				continue;
			}
			if ( args >= 11 && (arg10 == mmlNULL || arg10->IsRelative(holder->signature[10]) == mmlFalse) )
			{
				continue;
			}
			if ( args >= 12 && (arg11 == mmlNULL || arg11->IsRelative(holder->signature[11]) == mmlFalse) )
			{
				continue;
			}
			if ( args >= 13 && (arg12 == mmlNULL || arg12->IsRelative(holder->signature[12]) == mmlFalse) )
			{
				continue;
			}
			if ( args >= 14 && (arg13 == mmlNULL || arg13->IsRelative(holder->signature[13]) == mmlFalse) )
			{
				continue;
			}
			if ( args >= 15 && (arg14 == mmlNULL || arg14->IsRelative(holder->signature[14]) == mmlFalse) )
			{
				continue;
			}
			if ( args == 16 && (arg15 == mmlNULL || arg15->IsRelative(holder->signature[15]) == mmlFalse) )
			{
				continue;
			}
			result = holder->constructor (uuid, arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, arg11, arg12, arg13, arg14, arg15);
			if ( result != mmlNULL )
			{
				break;
			}
		}
	}
	for ( mmlInt32 index = 0 ; index < args; index ++ )
	{
		MML_RELEASE(objects[index]);
	}
	return result;
}

