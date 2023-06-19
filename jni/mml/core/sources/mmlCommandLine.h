#ifndef MML_COMMAND_LINE_IMPL_HEADER_INCLUDED
#define MML_COMMAND_LINE_IMPL_HEADER_INCLUDED

#include "mmlICommandLine.h"
#include "mmlICString.h"
#include <map>

class mmlCommandLine : public mmlICommandLine
{
	MML_OBJECT_DECL
public:
	mmlCommandLine() {}

	mmlCommandLine(std::map < const mmlChar * , mmlAutoPtr < mmlICString > , mml_char_map_predicate_ci > & cmd_line);

	mmlBool Load ( const mmlInt32 argc, const mmlChar ** argv );

	mmlBool Exists ( const mmlChar * name );

	mmlBool Get ( const mmlChar * name , mmlInt32 & val );

	mmlBool Get ( const mmlChar * name , mmlInt64 & val );

	mmlBool Get ( const mmlChar * name , mmlBool & val );

	mmlBool Get ( const mmlChar * name, mmlFloat64 & val );

	mmlBool Get ( const mmlChar * name , mmlICString ** val );

	mmlBool Clone ( mmlICommandLine ** cmd_line);
	
	mmlBool Set ( const mmlChar * name , const mmlInt32 & val );

	mmlBool Set ( const mmlChar * name , const mmlInt64 & val );

	mmlBool Set ( const mmlChar * name , const mmlBool & val );

	mmlBool Set ( const mmlChar * name, const mmlFloat64 & val );

	mmlBool Set ( const mmlChar * name , mmlICString * val );

	mmlBool Enumerate (mmlICommandLineEnumerator * en );

private:

	std::map < const mmlChar * , mmlAutoPtr < mmlICString > , mml_char_map_predicate_ci > mCommandLine;
};


#endif //MML_COMMAND_LINE_IMPL_HEADER_INCLUDED
