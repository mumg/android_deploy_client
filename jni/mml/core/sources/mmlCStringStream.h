#ifndef MML_CSTRING_STREAM_HEADER_INCLUDED
#define MML_CSTRING_STREAM_HEADER_INCLUDED

#include "mmlICStringStream.h"
#include <stdio.h>

class mmlCStringInputStream : public mmlICStringInputStream
{
	MML_OBJECT_DECL;
public:

	mmlCStringInputStream();

	~mmlCStringInputStream();

	mmlBool Construct ( mmlIInputStream * stream );

	mmlBool ReadStr ( const mmlInt32 len,
		              mmlICString ** value );

	mmlBool ReadChar ( mmlChar & ch , const mmlBool seek);

	mmlBool ReadToken ( const mmlChar * seps,
		                mmlBool seek,
		                mmlICString ** value );

	mmlBool SkipToken ( const mmlChar * seps );

	mmlBool ReadUntil ( const mmlChar * terminators_list,
		                const mmlChar * remove_from_begin,
		                const mmlChar * remove_from_end,
						mmlInt32 * removed,
		                mmlICString ** value );

	mmlBool ReadInteger64 ( mmlInt64 * value );

	mmlBool ReadInteger32 ( mmlInt32 * value );

	mmlBool ReadInteger16 ( mmlInt16 * value );

	mmlBool ReadUInteger64 ( mmlUInt64 * value );

	mmlBool ReadUInteger32 ( mmlUInt32 * value );

	mmlBool ReadUInteger16 ( mmlUInt16 * value );

	mmlBool ReadFloat ( mmlFloat64 * value );

	mmlBool Seek ( const mmlInt32 value , const mmlBool abs );

	mmlBool Skip ( const mmlChar * characters,
		           mmlInt32 * value );


	mmlBool GetRestorePoint ( mmlInt32 * value );

	mmlBool Restore ( const mmlInt32 value );

	 mmlBool ReadEscaped (const mmlChar * terminators_list,
		 const mmlChar escapeChar,
		 mmlICString ** value );

private:
	mmlChar * mBuffer;
	mmlInt32 mBufferLen;
	mmlInt32 mBufferOffset;
};

class mmlCStringOutputStream : public mmlICStringOutputStream
{
	MML_OBJECT_DECL;
public:
	mmlCStringOutputStream();
	~mmlCStringOutputStream();

	mmlBool Construct ( mmlIOutputStream * stream );

	mmlBool WriteChar ( const mmlChar value );
	mmlBool WriteStr   ( const mmlChar * value , const mmlInt32 size);
	mmlBool WriteInteger16 ( const mmlInt16 value , const mmlBool hex, const mmlInt32 min );
	mmlBool WriteInteger32 ( const mmlInt32 value , const mmlBool hex, const mmlInt32 min );
	mmlBool WriteInteger64 ( const mmlInt64 value , const mmlBool hex, const mmlInt32 min );
	mmlBool WriteUInteger16 ( const mmlUInt16 value , const mmlBool hex, const mmlInt32 min );
	mmlBool WriteUInteger32 ( const mmlUInt32 value , const mmlBool hex, const mmlInt32 min );
	mmlBool WriteUInteger64 ( const mmlUInt64 value , const mmlBool hex, const mmlInt32 min );
	mmlBool WriteFloat ( const mmlFloat64 value , const mmlInt32 min, const mmlInt32 precision );
	mmlBool WriteFormatted ( const mmlChar * formatter , ... );
	mmlBool WriteFormattedV ( const mmlChar * formatter , va_list args );
	mmlBool Close();

private:

	mmlAutoPtr < mmlIOutputStream > mStream;

};


#endif //MML_CSTRING_STREAM_HEADER_INCLUDED

