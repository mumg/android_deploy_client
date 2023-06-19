#ifndef MML_DATABASE_HEADER_INCLUDED
#define MML_DATABASE_HEADER_INCLUDED

#include "mmlICString.h"
#include "mmlICommandLine.h"
#include "mmlIStream.h"

class mmlIDatabase;

class mmlIDatabaseRow : public mmlIObject
{
public:

	virtual mmlBool GetValue ( const mmlInt32 index, mmlIVariant ** value ) = 0;

};

class mmlIDatabaseQueryHandler : public mmlIObject
{
public:
	virtual mmlBool OnRow ( const mmlInt32 index, mmlIDatabaseRow * row ) = 0;
};

#define MML_DATABASE_SCHEME_UUID { 0x80DE037C, 0x6DDA, 0x1014, 0x8188 , { 0xB2, 0x2D, 0x0F, 0x89, 0xD0, 0xAE } }

class mmlIDatabaseScheme : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_DATABASE_SCHEME_UUID)
public:
	virtual mmlInt32 Ugrade ( mmlIDatabase * database , const mmlInt32 current_version ) = 0;
};

#define MML_DATABASE_UUID { 0xEE3AD4A1, 0x6D3A, 0x1014, 0x8818 , { 0x91, 0x69, 0xCC, 0x39, 0x18, 0xA3 } }

class mmlIDatabase : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_DATABASE_UUID)
public:
	virtual mmlBool Query ( const mmlChar * sql, mmlIDatabaseQueryHandler * handler , mmlIVariant * args , mmlInt64 * last_insert_id = mmlNULL) = 0;
};

template < class T >
class mmlDatabaseObjectQuery : public mmlIDatabaseQueryHandler
{
	MML_OBJECT_STATIC_DECL
private:
	mmlAutoPtr < T > & mValue;
public:
	mmlDatabaseObjectQuery ( mmlAutoPtr < T > & value )
		:mValue(value)
	{

	}
	mmlBool OnRow ( const mmlInt32 index, mmlIDatabaseRow * row )
	{
		if ( index > 0 )
		{
			return mmlFalse;
		}
		return row->GetValue(0, mmlRelativePtrAddRef < T , mmlIVariant > (mValue));
	}

	operator mmlIDatabaseQueryHandler * ()
	{
		return this;
	}
};

#endif //MML_DATABASE_HEADER_INCLUDED
