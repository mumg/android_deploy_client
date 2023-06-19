#ifndef MML_XML_NODE_HEADER_INCLUDED
#define MML_XML_NODE_HEADER_INCLUDED

#include "mmlIXMLNode.h"
#include "mmlICStringStream.h"
#include "mmlPtr.h"
#include "mmlICString.h"
#include <map>
#include <string>
#include <string.h>

#define MML_XML_NODE_IID {  0x945974dc , 0xccf5 , 0x11de , 0xbafa , { 0x00 , 0x1c , 0xf0 , 0x11 , 0x74 , 0x60 } }

#include "mmlIStream.h"


class mmlXMLNode : public mmlIXMLNode
{
	MML_OBJECT_DECL
public:

	MML_OBJECT_UUID_DECL(MML_XML_NODE_IID)

	mmlBool Construct ( mmlICStringInputStream * stream );

	mmlBool GetChild ( const mmlInt32 index, mmlIXMLNode ** node );

	mmlBool GetId ( mmlICString **id);

	mmlBool SetAttribute ( const mmlChar *key, mmlICString *value);

	mmlBool GetAttribute ( const mmlChar *key, mmlICString **value);

	mmlBool SetAttribute ( mmlICString *key, mmlICString *value);

	mmlBool GetAttribute ( mmlICString *key, mmlICString **value);

	mmlBool AddSibling ( mmlIXMLNode *node);

	mmlBool RemoveSibling ( mmlIXMLNode *node);

	mmlBool GetEnumerator ( mmlIXMLNodeEnumerator **en);

	mmlBool GetAttributeEnumerator ( mmlIXMLNodeAttributeEnumerator ** en);

	mmlBool GetChild ( const mmlChar *path, mmlIXMLNode **node);

	mmlBool AddChild ( const mmlChar *path, mmlIXMLNode *node);

	mmlBool GetText ( mmlICString **text);

	mmlBool SetText ( mmlICString *text);

	mmlBool NewNode ( const mmlChar *name, mmlIXMLNode **node);

	mmlXMLNode ( mmlICString * id = mmlNULL);

	mmlBool Save ( mmlICStringOutputStream * stream,
		           mmlInt32 & depth );

	mmlInt32 Size ();

	mmlBool Load ( mmlICStringInputStream * stream );

	mmlBool Match ( const mmlChar * name);

private:

	friend class mmlXMLNodeEnumerator;
	friend class mmlXMLNodeAttributeEnumerator;

	mmlSharedPtr < mmlXMLNode > mSibling;

	mmlSharedPtr < mmlXMLNode > mChildren;

	mmlAutoPtr < mmlICString > mId;

	mmlAutoPtr < mmlICString > mText;

	class map_compare
	{
	public:
		bool operator()(const mmlAutoPtr < mmlICString >& _Left, const mmlAutoPtr < mmlICString >& _Right) const
		{	
			mmlBool res = mmlFalse;
			_Left->CompareStr(MML_LESS, _Right->Get(), res, MML_CASE_INSENSITIVE);
			return res == mmlTrue ? 1 : 0 ;
		}
	};

	std::map < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > , map_compare> mAttributes;

};

#endif //MML_XML_NODE_HEADER_INCLUDED

