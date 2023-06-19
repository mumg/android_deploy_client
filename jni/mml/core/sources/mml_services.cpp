#ifdef MML_WIN
#include "portable/win32/mmlMutex.h"
#endif //MML_WIN

#ifdef MML_POSIX
#include "portable/posix/mmlMutex.h"
#endif //MML_WIN


#include "mml_services.h"
#include "mml_components.h"
#include "mmlPtr.h"
#include "mmlIService.h"

#include <map>
#include <list>

mmlMutex mml_services_mutex;

class mmlServicesUUIDContainer
{
public:
	mmlServicesUUIDContainer( const mmlUUID & uuid )
		:mUUID(uuid)
	{
	}

	mmlServicesUUIDContainer( const mmlServicesUUIDContainer & container )
		:mUUID(container.mUUID)
	{

	}

	mmlServicesUUIDContainer operator = ( const mmlServicesUUIDContainer & container )
	{
		mUUID = container.mUUID;
		return *this;
	}

	mmlInt32 operator == ( const mmlServicesUUIDContainer & container ) const
	{
		return mmlCompareUUID(mUUID , container.mUUID) == 0 ? mmlTrue : mmlFalse;
	}

	mmlInt32 operator < ( const mmlServicesUUIDContainer & container ) const
	{
		return mmlCompareUUID(mUUID , container.mUUID) < 0 ? mmlTrue : mmlFalse;
	}

private:
	mmlUUID mUUID;
};


std::map < mmlServicesUUIDContainer , mmlIObject * > mml_services_map;
std::list < mmlIObject * > mml_services_destory_sequence;


mml_core mmlIObject * mmlGetService ( const mmlUUID & uuid )
{
	mmlIObject * service = mmlNULL;
	mml_services_mutex.Lock();
	std::map < mmlServicesUUIDContainer , mmlIObject * >::iterator existing_service = mml_services_map.find(uuid);
	if ( existing_service == mml_services_map.end() )
	{
		service = mmlNewObject(uuid);
		if ( service != mmlNULL )
		{
			MML_ADDREF(service);
			mml_services_map[uuid] = service;
			mml_services_destory_sequence.push_front(service);
		}
	}
	else
	{
		service = (*existing_service).second;
	}
	mml_services_mutex.UnLock();
	return service;
}

mml_core void mmlSetService ( const mmlUUID & uuid , mmlIObject * service )
{
	mml_services_mutex.Lock();
	std::map < mmlServicesUUIDContainer , mmlIObject * >::iterator existing_service = mml_services_map.find(uuid);
	if ( existing_service != mml_services_map.end())
	{
		for ( std::list < mmlIObject * >::iterator service = mml_services_destory_sequence.begin();
			service != mml_services_destory_sequence.end();
			service ++ )
		{
			if ( *service == existing_service->second )
			{
				mml_services_destory_sequence.erase(service);
				break;
			}
		}
	}
	if ( service == mmlNULL )
	{
		std::map< mmlServicesUUIDContainer , mmlIObject * > ::iterator svc = mml_services_map.find(uuid);
		if (svc != mml_services_map.end() )
		{
			for ( std::list < mmlIObject * >::iterator service = mml_services_destory_sequence.begin();
				service != mml_services_destory_sequence.end();
				service ++ )
			{
				if ( svc->second == existing_service->second )
				{
					mml_services_destory_sequence.erase(service);
					break;
				}
			}
			mmlSharedPtr < mmlIService > svc_instance = svc->second;
			if ( svc_instance != mmlNULL )
			{
				svc_instance->SvcStop();
				svc_instance->SvcWaitForStop();
			}

			mml_services_map.erase(svc);
		}
	}
	else
	{
		MML_ADDREF(service);
		mml_services_map[uuid] = service;
		mml_services_destory_sequence.push_back(service);
	}
	mml_services_mutex.UnLock();
}

void mmlServicesInit ()
{

}

void mmlServicesShutdown()
{
	for ( std::list < mmlIObject * >::iterator service = mml_services_destory_sequence.begin();
		service != mml_services_destory_sequence.end();
		service ++ )
	{
		mmlSharedPtr < mmlIService > svc = (mmlIObject*)(*service);
		if ( svc != mmlNULL )
		{
			svc->SvcStop();
		}
	}
	for ( std::list < mmlIObject * >::iterator service = mml_services_destory_sequence.begin();
		service != mml_services_destory_sequence.end();
		service ++ )
	{
		mmlSharedPtr < mmlIService > svc = (mmlIObject*)(*service);
		if ( svc != mmlNULL )
		{
			svc->SvcWaitForStop();
		}
	}
}

void mmlServicesDestroy()
{

	for ( std::list < mmlIObject * >::iterator service = mml_services_destory_sequence.begin();
		  service != mml_services_destory_sequence.end();
		  service ++ )
	{
		MML_RELEASE((*service));
	}
	mml_services_map.clear();
}

