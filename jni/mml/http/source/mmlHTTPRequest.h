#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "mmlICStringStream.h"
#include "mmlIHTTPRequest.h"
#include <string>
#include <map>
#include <list>

class mmlHTTPRequestUri : public mmlIHTTPRequestUri
{
	MML_OBJECT_DECL
public:

	mmlHTTPRequestUri();

	const mmlChar * Get ();

	void Set ( const mmlChar * uri );

	const mmlChar * GetArgument ( const mmlChar * key );

	void SetArgument ( const mmlChar * key , const mmlChar * value );

	int Size ();

	mmlBool GetArgument (const mmlInt32 index , mmlICString ** key , mmlICString ** value );

	mmlBool Load ( mmlICStringInputStream * stream );

	void Serialize ( mmlICStringOutputStream * stream );

	mmlFloat64 GetVersion ();

	void SetVersion (const mmlFloat64 version);

private:

	mmlFloat64 mVersion;


	mmlAutoPtr < mmlICString > mURI;

	std::list <  std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > > > mArguments;


};

class mmlHTTPRequest : public mmlIHTTPRequest
{
	MML_OBJECT_DECL
public:

	mmlHTTPRequest ();

	static mmlBool Load ( void * data , const int data_len, int * data_decoded, mmlIHTTPRequest ** request );

	mmlBool Serialize ( mmlICStringOutputStream * stream );

	const mmlChar * GetHeader ( const mmlChar * name );

	void SetHeader ( const mmlChar * name , const mmlChar * value );
	
	void DeleteHeader ( const mmlChar * name );

	mmlIHTTPRequestUri * GetReqURI ();

	void SetReqURI(mmlIHTTPRequestUri * uri);

	void SetContent ( mmlIInputStream * content );

	mmlBool GetContent ( mmlIInputStream ** content );

	void SetMethod ( const mmlChar * method );

	const mmlChar * GetMethod ();


private:



	mmlAutoPtr < mmlICString > mMethod;

	mmlAutoPtr < mmlIHTTPRequestUri > mReqURI;

	std::list <  std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > > > mHeaders;

	double mVersion;

	mmlAutoPtr < mmlIInputStream > mContent;

};



#endif //HTTP_REQUEST_H
