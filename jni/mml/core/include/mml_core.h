#ifndef MML_CORE_HEADER_INCLUDED
#define MML_CORE_HEADER_INCLUDED

#ifdef MML_WIN
#pragma execution_character_set("utf-8")
#define _MML_WIN_WINNT 0x0500
#endif

#ifdef MML_WIN
#define MML_INT64 "lld"
#define MML_UINT64 "llu"
#else
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#define MML_INT64 PRId64
#define MML_UINT64 PRIu64
#endif

#ifdef MML_SHARED
#ifdef MML_WIN
#define mml_export extern "C" __declspec(dllexport)
#ifdef MML_CORE
	#define mml_core extern "C" __declspec(dllexport)
#else
	#define mml_core extern "C" __declspec(dllimport)
#endif // MMLC_CORE
#else
#define mml_export extern "C" __attribute__((visibility("default")))
#ifdef MML_CORE
	#define mml_core extern "C" __attribute__((visibility("default")))
#else
	#define mml_core extern "C" __attribute__((visibility("default")))
#endif // MMLC_CORE
#endif //MML_WIN
#else
	#define mml_export
	#define mml_core
#endif

#ifdef MML_WIN
#define mml_module extern "C" __declspec(dllexport)
#else
#define mml_module extern "C" __attribute__((visibility("default")))
#endif 
typedef char mmlInt8;
typedef unsigned char mmlUInt8;
typedef short mmlInt16;
typedef unsigned short mmlUInt16;
typedef int mmlInt32;
typedef unsigned int mmlUInt32;
typedef double mmlFloat64;
#ifdef MML_WIN
typedef __int64 mmlInt64;
typedef unsigned __int64 mmlUInt64;
#pragma warning(disable: 4800)
#else
#ifdef MML_64
typedef long mmlInt64;
typedef unsigned long mmlUInt64;
#else
typedef long long mmlInt64;
typedef unsigned long long mmlUInt64;
#endif //MML_64


#endif //MML_WIN

class mmlRef
{
public:
	mmlRef() : mRef (0) {}

	mmlInt32 * ptr ()
	{
		return &mRef;
	}

private:
	mmlInt32 mRef;
};

typedef mmlInt8 mmlChar;

typedef wchar_t mmlUChar;

typedef enum
{
	mmlFalse,
	mmlTrue
}mmlBool;

#ifdef MML_WIN
#pragma pack(push, 1)
#endif
typedef struct
{
	mmlUInt32 d1;
	mmlUInt16 d2;
	mmlUInt16 d3;
	mmlUInt16 d4;
	mmlUInt8  b[6];
}
#ifndef MML_WIN
__attribute__((packed))
#endif
mmlUUID;
#ifdef MML_WIN
#pragma pack(pop)
#endif


#define mmlNULL 0

inline mmlInt32 mmlCompareUUID ( const mmlUUID & uuid1, const mmlUUID & uuid2 )
{
	const mmlUInt32 * p_uuid1 = (const mmlUInt32*)&uuid1;
	const mmlUInt32 * p_uuid2 = (const mmlUInt32*)&uuid2;
	if ( p_uuid1[0] == p_uuid2[0] )
	{
		if ( p_uuid1[1] == p_uuid2[1] )
		{
			if ( p_uuid1[2] == p_uuid2[2] )
			{
				if ( p_uuid1[3] == p_uuid2[3] )
				{
					return 0;
				}
				else if ( p_uuid1[3] > p_uuid2[3] )
				{
					return 1;
				}
				return -1;
		
			}
			else if ( p_uuid1[2] > p_uuid2[2] )
			{
				return 1;
			}
			return -1;
		}
		else if ( p_uuid1[1] > p_uuid2[1] )
		{
			return 1;
		}
		return -1;
	}
	else if ( p_uuid1[0] > p_uuid2[0] )
	{
		return 1;
	}
	return -1;
}

class mml_uuid_predicate
{
public:
	mmlInt32 operator () ( const mmlUUID & left,
		const mmlUUID & right) const
	{
		return mmlCompareUUID(left, right) < 0 ? 1 : 0;
	}
};

#define MML_U16TON(value) \
	(((value >> 8) &0x00FF | (value << 8)&0xFF00))

#define MML_NTOU16(value) \
	MML_U16TON(value)

#define MML_U32TON(value) \
	(((value >> 24) & 0x000000FF) | \
     ((value >>  8) & 0x0000FF00) | \
     ((value <<  8) & 0x00FF0000) | \
	 ((value << 24) & 0xFF000000) )

#define MML_NTOU32(value) \
	MML_U32TON(value)

#define MML_U64TON(value) \
	(((value >> 56) & 0x00000000000000FFLL)     |  \
     ((value >> 40) & 0x000000000000FF00LL)     |  \
     ((value >> 24) & 0x0000000000FF0000LL)     |  \
	 ((value >>  8) & 0x00000000FF000000LL)     |  \
	 ((value <<  8) & 0x000000FF00000000LL)     |  \
	 ((value << 24) & 0x0000FF0000000000LL)     |  \
	 ((value << 40) & 0x00FF000000000000LL)     |  \
	 ((value << 56) & 0xFF00000000000000LL))       

#define MML_NTOU64(value) \
	MML_U64TON(value)


typedef enum
{
	MML_EQUAL,
	MML_MORE,
	MML_MORE_OR_EQUAL,
	MML_LESS,
	MML_LESS_OR_EQUAL,
	MML_NOT_EQUAL,
	MML_ANY
} mmlCompareOperator;

mml_core void mmlUUIDGenerate (mmlUUID & uuid);

mml_core mmlBool mmlUUIDToStr ( const mmlUUID & uuid , mmlChar * str , const mmlInt32 str_len );

mml_core mmlBool mmlStrToUUID ( const mmlChar * str , mmlUUID & uuid );



#endif //MML_CORE_HEADER_INCLUDED

