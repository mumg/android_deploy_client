#ifndef ANDROID_UPDATER_DATABASE_HEADER_INCLUDED
#define ANDROID_UPDATER_DATABASE_HEADER_INCLUDED

#include "mmlIDatabase.h"
#include "mmlIObject.h"

mmlBool android_database_init ();

mmlBool android_database_get (mmlIDatabase ** db);


#endif //ANDROID_UPDATER_DATABASE_HEADER_INCLUDED
