#ifndef MML_COMMANDLINE_HEADER_INCLUDED
#define MML_COMMANDLINE_HEADER_INCLUDED

#include "mmlIObject.h"

class mmlICommandLineEnumerator
{
public:
	virtual mmlBool OnArg ( const mmlChar * name, mmlICString * value ) = 0;
};

#define MML_COMMAND_LINE_UUID { 0x8AF083C2, 0x6C49, 0x1014, 0xADE3 , { 0x81, 0xFE, 0x26, 0xA9, 0x3E, 0x67 } }

class mmlICommandLine : public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_COMMAND_LINE_UUID)
public:

	virtual mmlBool Load ( const mmlInt32 argc, const mmlChar ** argv ) = 0;

	virtual mmlBool Exists ( const mmlChar * name ) = 0;

	virtual mmlBool Get ( const mmlChar * name , mmlInt32 & val ) = 0;

	virtual mmlBool Get ( const mmlChar * name , mmlInt64 & val ) = 0;

	virtual mmlBool Get ( const mmlChar * name , mmlBool & val ) = 0;

	virtual mmlBool Get ( const mmlChar * name, mmlFloat64 & val ) = 0;

	virtual mmlBool Get ( const mmlChar * name , mmlICString ** val ) = 0;

	virtual mmlBool Clone ( mmlICommandLine ** cmd_line) = 0;

	virtual mmlBool Set ( const mmlChar * name , const mmlInt32 & val ) = 0;

	virtual mmlBool Set ( const mmlChar * name , const mmlInt64 & val ) = 0;

	virtual mmlBool Set ( const mmlChar * name , const mmlBool & val ) = 0;

	virtual mmlBool Set ( const mmlChar * name, const mmlFloat64 & val ) = 0;

	virtual mmlBool Set ( const mmlChar * name , mmlICString * val ) = 0;

	virtual mmlBool Enumerate (mmlICommandLineEnumerator * en ) = 0;

};

#endif //MML_COMMANDLINE_HEADER_INCLUDED
