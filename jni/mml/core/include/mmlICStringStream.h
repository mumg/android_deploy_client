#ifndef MML_ICSTRING_STREAM_HEADER_INCLUDED
#define MML_ICSTRING_STREAM_HEADER_INCLUDED

#include "mmlICString.h"
#include "mmlIBuffer.h"
#include "mmlIStream.h"
#include "mmlIMemoryStream.h"

class mmlIArray;

#define MML_ICSTRING_INPUT_STREAM_UUID {  0x05a2ed4e , 0x8e0d , 0x11de , 0x91e0 , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

/* arg0 - mmlIInputStream */

class mmlICStringInputStream : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_ICSTRING_INPUT_STREAM_UUID)
public:

	virtual mmlBool ReadStr ( const mmlInt32 len,
		                      mmlICString ** value ) = 0;

	virtual mmlBool ReadChar ( mmlChar & ch , const mmlBool seek) = 0;

	virtual mmlBool ReadToken ( const mmlChar * seps,
								mmlBool seek,
								mmlICString ** value ) = 0;

	virtual mmlBool SkipToken ( const mmlChar * seps ) = 0;

	virtual mmlBool ReadUntil ( const mmlChar * terminators_list,
		                        const mmlChar * remove_from_begin,
								const mmlChar * remove_from_end,
								mmlInt32 * removed,
		                        mmlICString ** value ) = 0;

	virtual mmlBool ReadEscaped (const mmlChar * terminators_list,
								 const mmlChar escapeChar,
		                         mmlICString ** value ) = 0;

	virtual mmlBool ReadInteger64 ( mmlInt64 * value ) = 0;

	virtual mmlBool ReadInteger32 ( mmlInt32 * value ) = 0;

	virtual mmlBool ReadInteger16 ( mmlInt16 * value ) = 0;

	virtual mmlBool ReadUInteger64 ( mmlUInt64 * value ) = 0;

	virtual mmlBool ReadUInteger32 ( mmlUInt32 * value ) = 0;

	virtual mmlBool ReadUInteger16 ( mmlUInt16 * value ) = 0;

	virtual mmlBool ReadFloat ( mmlFloat64 * value ) = 0;

	virtual mmlBool Seek ( const mmlInt32 value , const mmlBool abs ) = 0;

	virtual mmlBool Skip ( const mmlChar * characters,
		                   mmlInt32 * value ) = 0;

	virtual mmlBool GetRestorePoint ( mmlInt32 * value ) = 0;

	virtual mmlBool Restore ( const mmlInt32 value ) = 0;
};

#define MML_ICSTRING_OUTPUT_STREAM_UUID {  0x8c3a3dc0 , 0x8e0e , 0x11de , 0x94d0 , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

/* arg0 - mmlIOutputStream */

class mmlICStringOutputStream : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_ICSTRING_OUTPUT_STREAM_UUID)
public:
	virtual mmlBool WriteChar ( const mmlChar value ) = 0;
	virtual mmlBool WriteStr   ( const mmlChar * value , const mmlInt32 size = 0) = 0;
	virtual mmlBool WriteInteger16 ( const mmlInt16 value , const mmlBool hex, const mmlInt32 min ) = 0;
	virtual mmlBool WriteInteger32 ( const mmlInt32 value , const mmlBool hex, const mmlInt32 min ) = 0;
	virtual mmlBool WriteInteger64 ( const mmlInt64 value , const mmlBool hex, const mmlInt32 min ) = 0;
	virtual mmlBool WriteUInteger16 ( const mmlUInt16 value , const mmlBool hex, const mmlInt32 min ) = 0;
	virtual mmlBool WriteUInteger32 ( const mmlUInt32 value , const mmlBool hex, const mmlInt32 min ) = 0;
	virtual mmlBool WriteUInteger64 ( const mmlUInt64 value , const mmlBool hex, const mmlInt32 min ) = 0;
	virtual mmlBool WriteFloat ( const mmlFloat64 value , const mmlInt32 min, const mmlInt32 precision ) = 0;
	virtual mmlBool WriteFormatted ( const mmlChar * formatter , ... ) = 0;
	virtual mmlBool WriteFormattedV ( const mmlChar * formatter , va_list args ) = 0;
	virtual mmlBool Close () = 0;
};

#define MML_CREATE_CSTRING_MEMORY_INPUT_STREAM_STRING(name, str) \
	mmlSharedPtr < mmlICStringInputStream > name = mmlNewObject(MML_OBJECT_UUID(mmlICStringInputStream), mmlObjectPtr < mmlIMemoryInputStream >(str));

#define MML_CREATE_CSTRING_MEMORY_OUTPUT_STREAM(name) \
	mmlSharedPtr < mmlICString > name##_str = mmlNewObject(MML_OBJECT_UUID(mmlICString)); \
	mmlSharedPtr < mmlICStringOutputStream > name = mmlNewObject(MML_OBJECT_UUID(mmlICStringOutputStream), mmlObjectPtr < mmlIMemoryOutputStream > (name##_str)); 

#define MML_CLOSE_CSTRING_MEMORY_OUTPUT_STREAM(name) \
	name->Close();
	
#endif //MML_ICSTRING_STREAM_HEADER_INCLUDED

