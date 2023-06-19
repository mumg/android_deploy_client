#ifndef MML_DATABASE_IMPL_HEADER_INCLUDED
#define MML_DATABASE_IMPL_HEADER_INCLUDED

#include "mmlIDatabase.h"
#include <sqlite3.h>

class mmlDatabase : public mmlIDatabase
{
	MML_OBJECT_DECL
public:
	mmlDatabase();
	~mmlDatabase();
	mmlBool Construct (mmlIVariantStruct * config );
	mmlBool Construct (mmlIVariantStruct * config , mmlIDatabaseScheme * scheme );
	mmlBool Query ( const mmlChar * sql, mmlIDatabaseQueryHandler * handler , mmlIVariant * args, mmlInt64 * last_insert_id);
private:
	sqlite3 * mDB;
};

#endif //MML_PROCESS_IMPL_HEADER_INCLUDED
