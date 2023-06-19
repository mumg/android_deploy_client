/*************************************************************
*      This is automatically generated file, do not edit it  *
*************************************************************/
#ifndef MML_IXMLNODE_HEADER_INCLUDED
#define MML_IXMLNODE_HEADER_INCLUDED

#include "mmlIObject.h"

class mmlIXMLNode;

#define MML_IXMLNODEENUMERATOR_UUID { 0x2494b4ca , 0xcc5c , 0x11de , 0x9622 , { 0x00, 0x1c, 0xf0, 0x11, 0x74, 0x60 } }

class mmlIXMLNodeEnumerator: public mmlIObject
{
    MML_OBJECT_UUID_DECL(MML_IXMLNODEENUMERATOR_UUID)
public:
    virtual mmlBool HasMore ( mmlBool *has_more) = 0;
    virtual mmlBool GetNext ( mmlIXMLNode **node) = 0;
};


#define MML_IXMLNODEENUMERATOR_DECL                                                                                                         \
    MML_OBJECT_DECL                                                                                                                         \
        mmlBool HasMore ( mmlBool *has_more);                                                                                                   \
        mmlBool GetNext ( mmlIXMLNode **node);                                                                                                  \

#define MML_IXMLNODEATTRIBUTEENUMERATOR_UUID { 0x2494b4ca , 0xcc5c , 0x11de , 0x9622 , { 0x00, 0x1c, 0xf0, 0x11, 0x74, 0x61 } }

class mmlIXMLNodeAttributeEnumerator: public mmlIObject
{
	MML_OBJECT_UUID_DECL(MML_IXMLNODEATTRIBUTEENUMERATOR_UUID)
public:
	virtual mmlBool HasMore ( mmlBool *has_more) = 0;
	virtual mmlBool GetNext ( mmlICString ** name , mmlICString ** value ) = 0;
};

#define MML_IXMLNODE_UUID { 0x9e5596a8 , 0xcc5c , 0x11de , 0xaf6b , { 0x00, 0x1c, 0xf0, 0x11, 0x74, 0x60 } }

class mmlIXMLNode: public mmlIObject
{
    MML_OBJECT_UUID_DECL(MML_IXMLNODE_UUID)
public:
	virtual mmlInt32 Size () = 0;
	virtual mmlBool GetChild ( const mmlInt32 index, mmlIXMLNode ** node ) = 0;
	virtual mmlBool GetId ( mmlICString **id) = 0;
	virtual mmlBool SetAttribute ( const mmlChar *key, mmlICString *value) = 0;
	virtual mmlBool GetAttribute ( mmlICString *key, mmlICString **value) = 0;
	virtual mmlBool GetAttribute ( const mmlChar *key, mmlICString **value) = 0;
	virtual mmlBool SetAttribute ( mmlICString *key, mmlICString *value) = 0;
	virtual mmlBool AddSibling ( mmlIXMLNode *node) = 0;
	virtual mmlBool RemoveSibling ( mmlIXMLNode *node) = 0;
	virtual mmlBool GetEnumerator ( mmlIXMLNodeEnumerator **en) = 0;
	virtual mmlBool GetAttributeEnumerator ( mmlIXMLNodeAttributeEnumerator ** en) = 0;
	virtual mmlBool GetChild ( const mmlChar *path, mmlIXMLNode **node) = 0;
	virtual mmlBool AddChild ( const mmlChar *path, mmlIXMLNode *node) = 0;
	virtual mmlBool GetText ( mmlICString **text) = 0;
	virtual mmlBool SetText ( mmlICString *text) = 0;
	virtual mmlBool NewNode ( const mmlChar *name, mmlIXMLNode **node) = 0;
	virtual mmlBool Match ( const mmlChar * name) = 0;
};





#endif //MML_IXMLNODE_HEADER_INCLUDED
