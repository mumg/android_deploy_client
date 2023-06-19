#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "mmlIHTTPResponse.h"
#include "mmlIList.h"
#include <list>

#define MML_HTTP_RESPONSE_IMPL_UUID { 0xCC3B2A4B, 0x6CA7, 0x1014, 0xB4E3 , { 0x84, 0x74, 0xC8, 0x5C, 0xDB, 0x17 } }

class mmlHTTPResponse : public mmlIHTTPResponse
{
	MML_OBJECT_DECL
	MML_OBJECT_UUID_DECL(MML_HTTP_RESPONSE_IMPL_UUID)
public:

	mmlHTTPResponse();

	mmlHTTPResponse (const mmlInt32 code, mmlICString * str , const mmlFloat64 version);

	static mmlBool Load ( void * data , const mmlInt32 data_len, mmlInt32 * data_decoded, mmlIHTTPResponse ** response );

	void Serialize ( mmlICStringOutputStream * stream );

	void SetResponseCode ( const mmlInt32 response_code );

	mmlInt32 GetResponseCode ();

	void SetResponseStr ( const mmlChar * response_str );

	const mmlChar * GetResponseStr ();

	const mmlChar * GetHeader ( const mmlChar * name );

	void SetHeader ( const mmlChar *name , const mmlChar *value );

	void SetVersion ( const mmlFloat64 version );

	mmlFloat64 GetVersion ();

private:

	std::list <   std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > > > mHeaders;

	mmlFloat64 mVersion;

	mmlInt32 mCode;

	mmlAutoPtr < mmlICString > mStr;

};

#endif
