#include "mmlDatabase.h"

class mmlDatabaseRow : public mmlIDatabaseRow
{
	MML_OBJECT_STATIC_DECL
private:
	sqlite3_stmt * mStmt;
public:

	mmlDatabaseRow(sqlite3_stmt *statement)
		:mStmt(statement)
	{

	}

	mmlBool GetValue ( const mmlInt32 index, mmlIVariant ** value )
	{
		if ( sqlite3_column_count(mStmt) < index )
		{
			return mmlFalse;
		}
		mmlInt32 type = sqlite3_column_type(mStmt, index);
		if ( type == SQLITE_INTEGER )
		{
			*value = mmlNewVariantInteger(sqlite3_column_int64(mStmt, index));
			MML_ADDREF(*value);
			return mmlTrue;
		}
		else if ( type == SQLITE_FLOAT )
		{
			*value = mmlNewVariantDouble(sqlite3_column_double(mStmt, index));
			MML_ADDREF(*value);
			return mmlTrue;
		}
		else if ( type == SQLITE_TEXT )
		{
			const mmlChar * text = (const mmlChar *)sqlite3_column_text(mStmt , index);
			if ( text != mmlNULL )
			{
				*value = mmlNewVariantString(mmlNewCString(text));
				MML_ADDREF(*value);
			}
			return mmlTrue;
		}
		else if ( type == SQLITE_BLOB )
		{
			mmlInt32 size = sqlite3_column_bytes(mStmt, index);
			const void * data = (const mmlChar *)sqlite3_column_blob(mStmt , index);
			if ( size > 0 && data != mmlNULL )
			{
				mmlAutoPtr < mmlIBuffer > buf = mmlAllocateBuffer(size);
				mmlMemoryCopy(buf->Get(), data, size);
				*value = mmlNewVariantRaw(buf);
				MML_ADDREF(*value);
			}
			return mmlTrue;
		}
		return mmlFalse;
	}
};

mmlDatabase::mmlDatabase()
	:mDB(mmlNULL)
{

}
mmlDatabase::~mmlDatabase()
{
	if ( mDB != mmlNULL )
	{
		sqlite3_close(mDB);
	}
}

mmlBool mmlDatabase::Construct (mmlIVariantStruct * config )
{
	return Construct(config, mmlNULL);
}

mmlBool mmlDatabase::Construct (mmlIVariantStruct * config , mmlIDatabaseScheme * scheme )
{
	mmlAutoPtr < mmlIVariantString > db_file;
	if ( config->Get("file", mmlRelativePtrAddRef < mmlIVariantString, mmlIVariant >(db_file)) == mmlFalse )
	{
		return mmlFalse;
	}
	if ( sqlite3_open(db_file->Get()->Get(), &mDB) != SQLITE_OK )
	{
		return mmlFalse;
	}
	if ( scheme != mmlNULL )
	{
		mmlAutoPtr < mmlIVariantInteger > current_version;
		Query("select value from scheme where key = 'version'" , mmlDatabaseObjectQuery < mmlIVariantInteger > (current_version) , mmlNULL , mmlNULL);
		mmlInt32 new_version = scheme->Ugrade(this, current_version == mmlNULL ? 0 : current_version->Get());
		if (current_version == mmlNULL &&  new_version > 0)
		{
			if ( sqlite3_exec(mDB , "create table if not exists scheme (key text not null , value text not null)" , NULL , NULL , NULL) != SQLITE_OK )
			{
				return mmlFalse;
			}
			mmlChar sqlite_query[256];
			mmlSprintf(sqlite_query , sizeof(sqlite_query) , "insert into scheme (key,value) values('version','%d')" , new_version);
			if ( sqlite3_exec(mDB , sqlite_query , NULL , NULL , NULL) != SQLITE_OK )
			{
				return mmlFalse;
			}
		}
		else if (new_version > current_version->Get())
		{
			mmlChar sqlite_query[256];
			mmlSprintf(sqlite_query , sizeof(sqlite_query), "update scheme set value = '%d' where key = 'version'" , new_version);
			if ( sqlite3_exec(mDB , sqlite_query , NULL , NULL , NULL) != SQLITE_OK )
			{
				return mmlFalse;
			}
		}
	}
	return mmlTrue;
}

mmlBool mml_database_bind (const mmlInt32 index,  mmlIVariant * value , sqlite3_stmt * stmt )
{
	MML_ADDREF(value);
	if ( value->GetType() == MML_ARRAY )
	{
		mmlSharedPtr < mmlIVariantArray > arr = value;
		if ( arr == mmlNULL )
		{
			MML_RELEASE(value);
			return mmlFalse;
		}
		for ( mmlInt32 idx = 0 ; idx < arr->Size(); idx ++ )
		{
			mmlAutoPtr < mmlIVariant > arr_val;
			if ( arr->Get(idx, arr_val.getterAddRef()) == mmlFalse )
			{
				MML_RELEASE(value);
				return mmlFalse;
			}
			if ( mml_database_bind(index + idx, arr_val, stmt) == mmlFalse )
			{
				MML_RELEASE(value);
				return mmlFalse;
			}
		}
	}
	else
	{
		if ( value->GetType() == MML_INTEGER)
		{
			mmlSharedPtr < mmlIVariantInteger > integer = value;
			if ( integer == mmlNULL )
			{
				MML_RELEASE(value);
				return mmlFalse;
			}
			if ( integer->Get() >  0x7FFFFFFF )
			{
				sqlite3_bind_int64(stmt , index + 1  , integer->Get());
			}
			else
			{
				sqlite3_bind_int(stmt , index + 1  , (mmlInt32)integer->Get());
			}
		}
		else if ( value->GetType() == MML_DOUBLE )
		{
			mmlSharedPtr < mmlIVariantDouble > dbl = value;
			if ( dbl == mmlNULL )
			{
				MML_RELEASE(value);
				return mmlFalse;
			}
			sqlite3_bind_double(stmt , index + 1  , dbl->Get());
		}
		else if ( value->GetType() == MML_BOOLEAN)
		{
			mmlSharedPtr < mmlIVariantBoolean > b = value;
			if ( b == mmlNULL )
			{
				MML_RELEASE(value);
				return mmlFalse;
			}
			sqlite3_bind_int(stmt , index + 1  , b->Get());
		}
		else if ( value->GetType() == MML_STRING )
		{
			mmlSharedPtr < mmlIVariantString > s = value;
			if ( s == mmlNULL )
			{
				MML_RELEASE(value);
				return mmlFalse;
			}
			sqlite3_bind_text(stmt, index + 1, s->Get()->Get() , -1, SQLITE_TRANSIENT);
		}
		else if ( value->GetType() == MML_RAW )
		{
			mmlSharedPtr < mmlIVariantRaw > r = value;
			if ( r == mmlNULL )
			{
				MML_RELEASE(value);
				return mmlFalse;
			}
			mmlAutoPtr < mmlIBuffer > b;
			r->Get(b.getterAddRef());
			if ( b == mmlNULL )
			{
				MML_RELEASE(value);
				return mmlFalse;
			}
			sqlite3_bind_blob(stmt, index + 1, b->Get(), b->Size(), mmlNULL);
		}
		else
		{
			MML_RELEASE(value);
			return mmlFalse;
		}
	}
	MML_RELEASE(value);
	return mmlTrue;
}

mmlBool mmlDatabase::Query ( const mmlChar * sql, mmlIDatabaseQueryHandler * handler , mmlIVariant * args, mmlInt64 * last_insert_id)
{
	sqlite3_stmt *statement;
	mmlBool result = mmlFalse;
	int err;
	if((err = sqlite3_prepare_v2(mDB, sql, -1, &statement, 0)) == SQLITE_OK)
	{
		result = mmlTrue;
		if ( args != NULL )
		{
			result = mml_database_bind(0, args, statement);
		}
		int index = 0;
		int sql_err = 0;
		while(result == mmlTrue && (sql_err = sqlite3_step(statement)) == SQLITE_ROW)
		{
			mmlDatabaseRow row(statement);
			if ( handler != mmlNULL && handler->OnRow(index, &row) == mmlFalse )
			{
				result = mmlFalse;
			}
		}
		if ( last_insert_id != mmlNULL) *last_insert_id = sqlite3_last_insert_rowid(mDB);
		sqlite3_finalize(statement);
	}
	return result;

}

MML_OBJECT_IMPL1(mmlDatabase, mmlIDatabase)