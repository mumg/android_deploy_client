#ifndef MML_STRUTILS_HEADER_INCLUDED
#define MML_STRUTILS_HEADER_INCLUDED

#include "mml_core.h"
#include <stdarg.h>

mml_core mmlUChar mmlToLower ( const mmlUChar chr );
mml_core mmlUChar mmlToUpper ( const mmlUChar chr );


typedef mmlUChar (*mml_char_conversion_t)  ( const mmlUChar chr );

#define MML_CASE_SENSITIVE (mml_char_conversion_t)mmlNULL
#define MML_CASE_INSENSITIVE mmlToLower


mml_core mmlInt32 mmlSprintf ( mmlChar * buffer , mmlInt32 len, const mmlChar * formatter , ... );
mml_core mmlInt32 mmlVSprintf ( mmlChar * buffer , mmlInt32 len , const mmlChar * formatter , va_list lst );

mml_core mmlInt32 mmlStrCompare ( const mmlChar * v1 , const mmlChar * v2 , mml_char_conversion_t rule = MML_CASE_SENSITIVE);
mml_core mmlInt32 mmlStrNCompare ( const mmlChar * v1 , const mmlChar * v2 , const mmlInt32 len , mml_char_conversion_t rule = MML_CASE_SENSITIVE);
mml_core void mmlStrCopy ( mmlChar * dst , const mmlChar * src , mmlInt32 len );

mml_core mmlBool mmlStrMatch ( const mmlChar * str, const mmlChar * pattern, mml_char_conversion_t rule = MML_CASE_SENSITIVE);

mml_core void * mmlStrMatchPrepare (const mmlChar * pattern);
mml_core void mmlStrMatchFree ( void * pattern );
mml_core mmlBool mmlStrMatchPrepared ( const mmlChar * str , void * pattern, mml_char_conversion_t rule = MML_CASE_SENSITIVE );

mml_core mmlInt32 mmlStrFind ( const mmlChar * str , const mmlChar * substr , mml_char_conversion_t rule = MML_CASE_SENSITIVE );

mml_core mmlInt64 mmlStoD ( const mmlChar * buffer , mmlBool * result );
mml_core mmlUInt64 mmlStoUD ( const mmlChar * buffer , mmlBool * result );
mml_core mmlFloat64 mmlStoF ( const mmlChar * buffer , mmlBool * result );

mml_core mmlInt32 mmlStrLength ( const mmlChar * str );
mml_core mmlInt32 mmlStrUTF8Length ( const mmlChar * str );

mml_core mmlInt32 mml_utf8_mbtowc (mmlUChar *pwc, const mmlUInt8 *s, const mmlInt32 n);

mml_core mmlInt32 mml_utf8_wctomb (mmlUInt8 *r, mmlUChar wc, mmlInt32 n);
mml_core mmlChar * mmlStrDup ( const mmlChar * val );

#endif //MML_STRUTILS_HEADER_INCLUDED

