#ifndef MML_INTERFACE_OBJECT_HEADER_INCLUDED
#define MML_INTERFACE_OBJECT_HEADER_INCLUDED

#include "mml_core.h"
#include "mml_memory.h"
#include "mml_atomic.h"

#define mmlIObjectUUID \
      { 0 }

class mmlICString;

class mmlIObject
{
public:

	virtual void AddRef () = 0;

	virtual mmlInt32 GetRef () = 0;
	
	virtual void Release () = 0;

	virtual mmlBool IsRelative ( const mmlUUID & uuid ) = 0;
	
	virtual mmlBool QueryInterface ( const mmlUUID & uuid,
	                                 void ** ptr ) = 0;
};

#define MML_OBJECT_UUID_DECL(the_uuid)                                        \
public:                                                                       \
	static const mmlUUID& GetUIID()                                           \
	{static const mmlUUID uuid = the_uuid; return uuid;}                      \

#define MML_OBJECT_DECL                                                       \
public:                                                                       \
	virtual void AddRef ();                                                   \
	virtual void Release ();                                                  \
	virtual mmlInt32 GetRef ();                                               \
	virtual mmlBool IsRelative ( const mmlUUID & uuid );                      \
	virtual mmlBool QueryInterface ( const mmlUUID & uuid,                    \
	                                 void ** ptr );                           \
private:                                                                      \
	mmlRef mRef;                                                              \
public:                                                                       \

#define MML_ADDREF(_object) if ( _object ) (_object)->AddRef();

#define MML_RELEASE(_object) if ( _object ) { (_object)->Release (); (_object) = mmlNULL ; }


#define MML_OBJECT_STATIC_DECL \
	void AddRef () {} \
    mmlInt32 GetRef () {return 1;} \
	void Release () {} \
    mmlBool IsRelative ( const mmlUUID & uuid ) { return mmlFalse; } \
    mmlBool QueryInterface ( const mmlUUID & uuid, void ** ptr ) { return mmlFalse; }

#define MML_OBJECT_UUID(_class) _class::GetUIID()

#define MML_OBJECT_RELATIVE_MAP_BEGIN(_class)                                 \
mmlBool _class::IsRelative(const mmlUUID & uuid)                              \
{                                                                             \

#define MML_OBJECT_RELATIVE_MAP_ENTRY(_interface)                             \
	if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(_interface)) == 0)              \
         return mmlTrue;                                                      \

#define MML_OBJECT_RELATIVE_MAP_END(_class)                                   \
	static mmlUUID mml_object_uuid = mmlIObjectUUID;                          \
	if ( mmlCompareUUID(uuid , mml_object_uuid ) == 0) return mmlTrue;        \
	return mmlFalse;                                                          \
}  

#define MML_OBJECT_INTERFACE_MAP_BEGIN(_class)                                \
mmlBool _class::QueryInterface(const mmlUUID & uuid, void ** ptr)             \
{                                                                             \
  mmlIObject * foundInterface = mmlNULL;                                      \
  static mmlUUID mml_object_uuid = mmlIObjectUUID;                            \
  
#define MML_OBJECT_INTERFACE_MAP_ENTRY(_interface)                            \
  if ( mmlCompareUUID(uuid ,MML_OBJECT_UUID(_interface)) == 0)                \
    foundInterface = static_cast<_interface*>(this);                          \
  else                                                                        \

#define MML_OBJECT_INTERFACE_MAP_END(_class)                                  \
  if ( mmlCompareUUID(uuid , mml_object_uuid ) == 0)                          \
    foundInterface = static_cast<mmlIObject*>(this);                          \
  *ptr = foundInterface;                                                      \
  MML_ADDREF(foundInterface);                                                 \
  if ( ptr != mmlNULL ) return mmlTrue;                                       \
  return mmlFalse;                                                            \
}                                                                             \

#define MML_OBJECT_IMPL_ADDREF(_class)                                        \
void _class::AddRef()                                                         \
{                                                                             \
  mmlAtomicAdd( mRef.ptr() , 1 );                                             \
}                                                                             \

#define MML_OBJECT_IMPL_RELEASE(_class)                                       \
void _class::Release()                                                        \
{                                                                             \
  mmlInt32 count = mmlAtomicAdd(mRef.ptr() , -1);                             \
  if ( count == 0 )                                                           \
  {                                                                           \
     delete this;                                                             \
  }                                                                           \
}                                                                             \

#define MML_OBJECT_IMPL_GETREF(_class)                                        \
mmlInt32 _class::GetRef()                                                     \
{                                                                             \
  return  mmlAtomicGet(mRef.ptr());                                           \
}                                                                             \

#define MML_TPL_IMPL0(_class)                                                 \
	template <>                                                               \
	MML_OBJECT_IMPL_ADDREF(_class)                                            \
	template <>                                                               \
	MML_OBJECT_IMPL_RELEASE(_class)                                           \
	template <>                                                               \
	MML_OBJECT_IMPL_GETREF(_class)                                            \
	template <>                                                               \
	MML_OBJECT_INTERFACE_MAP_BEGIN(_class)                                    \
	MML_OBJECT_INTERFACE_MAP_END(_class)                                      \
	template <>                                                               \
	MML_OBJECT_RELATIVE_MAP_BEGIN(_class)                                     \
	MML_OBJECT_RELATIVE_MAP_END(_class)                                       \


#define MML_OBJECT_IMPL0(_class)                                              \
	MML_OBJECT_IMPL_ADDREF(_class)                                            \
	MML_OBJECT_IMPL_RELEASE(_class)                                           \
	MML_OBJECT_IMPL_GETREF(_class)                                            \
	MML_OBJECT_INTERFACE_MAP_BEGIN(_class)                                    \
	MML_OBJECT_INTERFACE_MAP_END(_class)                                      \
	MML_OBJECT_RELATIVE_MAP_BEGIN(_class)                                     \
	MML_OBJECT_RELATIVE_MAP_END(_class)                                       \

#define MML_OBJECT_IMPL1(_class , c1)                                         \
	MML_OBJECT_IMPL_ADDREF(_class)                                            \
	MML_OBJECT_IMPL_RELEASE(_class)                                           \
	MML_OBJECT_IMPL_GETREF(_class)                                            \
	MML_OBJECT_INTERFACE_MAP_BEGIN(_class)                                    \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c1)                                        \
	MML_OBJECT_INTERFACE_MAP_END(_class)                                      \
	MML_OBJECT_RELATIVE_MAP_BEGIN(_class)                                     \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c1)                                         \
	MML_OBJECT_RELATIVE_MAP_END(_class)                                       \

#define MML_OBJECT_IMPL2(_class , c1 , c2 )                                   \
	MML_OBJECT_IMPL_ADDREF(_class)                                            \
	MML_OBJECT_IMPL_RELEASE(_class)                                           \
	MML_OBJECT_IMPL_GETREF(_class)                                            \
	MML_OBJECT_INTERFACE_MAP_BEGIN(_class)                                    \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c1)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c2)                                        \
	MML_OBJECT_INTERFACE_MAP_END(_class)                                      \
	MML_OBJECT_RELATIVE_MAP_BEGIN(_class)                                     \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c1)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c2)                                         \
	MML_OBJECT_RELATIVE_MAP_END(_class)                                       \

#define MML_OBJECT_IMPL3(_class, c1 , c2 , c3 )                               \
	MML_OBJECT_IMPL_ADDREF(_class)                                            \
	MML_OBJECT_IMPL_RELEASE(_class)                                           \
	MML_OBJECT_IMPL_GETREF(_class)                                            \
	MML_OBJECT_INTERFACE_MAP_BEGIN(_class)                                    \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c1)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c2)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c3)                                        \
	MML_OBJECT_INTERFACE_MAP_END(_class)                                      \
	MML_OBJECT_RELATIVE_MAP_BEGIN(_class)                                     \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c1)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c2)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c3)                                         \
	MML_OBJECT_RELATIVE_MAP_END(_class)                                       \

#define MML_OBJECT_IMPL4(_class, c1 , c2 , c3 , c4 )                          \
	MML_OBJECT_IMPL_ADDREF(_class)                                            \
	MML_OBJECT_IMPL_RELEASE(_class)                                           \
	MML_OBJECT_IMPL_GETREF(_class)                                            \
	MML_OBJECT_INTERFACE_MAP_BEGIN(_class)                                    \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c1)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c2)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c3)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c4)                                        \
	MML_OBJECT_INTERFACE_MAP_END(_class)                                      \
	MML_OBJECT_RELATIVE_MAP_BEGIN(_class)                                     \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c1)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c2)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c3)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c4)                                         \
	MML_OBJECT_RELATIVE_MAP_END(_class)                                       \

#define MML_OBJECT_IMPL5(_class , c1 , c2 , c3 , c4 , c5 )                    \
	MML_OBJECT_IMPL_ADDREF(_class)                                            \
	MML_OBJECT_IMPL_RELEASE(_class)                                           \
	MML_OBJECT_IMPL_GETREF(_class)                                            \
	MML_OBJECT_INTERFACE_MAP_BEGIN(_class)                                    \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c1)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c2)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c3)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c4)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c5)                                        \
	MML_OBJECT_INTERFACE_MAP_END(_class)                                      \
	MML_OBJECT_RELATIVE_MAP_BEGIN(_class)                                     \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c1)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c2)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c3)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c4)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c5)                                         \
	MML_OBJECT_RELATIVE_MAP_END(_class)                                       \

#define MML_OBJECT_IMPL6(_class , c1 , c2 , c3 , c4 , c5 , c6)                \
	MML_OBJECT_IMPL_ADDREF(_class)                                            \
	MML_OBJECT_IMPL_RELEASE(_class)                                           \
	MML_OBJECT_IMPL_GETREF(_class)                                            \
	MML_OBJECT_INTERFACE_MAP_BEGIN(_class)                                    \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c1)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c2)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c3)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c4)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c5)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c6)                                        \
	MML_OBJECT_INTERFACE_MAP_END(_class)                                      \
	MML_OBJECT_RELATIVE_MAP_BEGIN(_class)                                     \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c1)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c2)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c3)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c4)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c5)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c6)                                         \
	MML_OBJECT_RELATIVE_MAP_END(_class)                                       \

#define MML_OBJECT_IMPL7(_class , c1 , c2 , c3 , c4 , c5 , c6 , c7)           \
	MML_OBJECT_IMPL_ADDREF(_class)                                            \
	MML_OBJECT_IMPL_RELEASE(_class)                                           \
	MML_OBJECT_IMPL_GETREF(_class)                                            \
	MML_OBJECT_INTERFACE_MAP_BEGIN(_class)                                    \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c1)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c2)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c3)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c4)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c5)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c6)                                        \
	MML_OBJECT_INTERFACE_MAP_ENTRY(c7)                                        \
	MML_OBJECT_INTERFACE_MAP_END(_class)                                      \
	MML_OBJECT_RELATIVE_MAP_BEGIN(_class)                                     \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c1)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c2)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c3)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c4)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c5)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c6)                                         \
	MML_OBJECT_RELATIVE_MAP_ENTRY(c7)                                         \
	MML_OBJECT_RELATIVE_MAP_END(_class)                                       \


#endif //MML_INTERFACE_OBJECT_HEADER_INCLUDED

