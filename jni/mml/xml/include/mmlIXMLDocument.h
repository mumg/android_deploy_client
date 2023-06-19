/*************************************************************
*      This is automatically generated file, do not edit it  *
*************************************************************/
#ifndef MML_IXMLDOCUMENT_HEADER_INCLUDED
#define MML_IXMLDOCUMENT_HEADER_INCLUDED

#include "mmlIObject.h"

class mmlIXMLNode;
class mmlIXMLStream;
class mmlICStringInputStream;
class mmlICStringOutputStream;

#define MML_IXMLDOCUMENT_UUID { 0x02776010 , 0xcc5a , 0x11de , 0x8946 , { 0x00, 0x1c, 0xf0, 0x11, 0x74, 0x60 } }

class mmlIXMLDocument: public mmlIObject
{
    MML_OBJECT_UUID_DECL(MML_IXMLDOCUMENT_UUID)
public:
    virtual mmlBool NewNode ( mmlIXMLNode *parent, const mmlChar *name, mmlIXMLNode **node) = 0;
    virtual mmlBool GetVersion ( mmlUInt32 *major, mmlUInt32 *minor) = 0;
    virtual mmlBool SetVersion ( const mmlUInt32 major, const mmlUInt32 minor) = 0;
    virtual mmlBool GetEncoding ( mmlICString **encoding) = 0;
    virtual mmlBool SetEncoding ( mmlICString *encoding) = 0;
    virtual mmlBool Get ( const mmlChar *path, mmlIXMLNode **node) = 0;
    virtual mmlBool Set ( const mmlChar *path, mmlIXMLNode *node) = 0;
    virtual mmlBool Load ( mmlICStringInputStream *stream) = 0;
    virtual mmlBool Save ( mmlICStringOutputStream *stream) = 0;
};


#define MML_IXMLDOCUMENT_DECL                                                                                                               \
    MML_OBJECT_DECL                                                                                                                         \
        mmlBool NewNode ( mmlIXMLNode *parent, const mmlChar *name, mmlIXMLNode **node);                                                        \
        mmlBool GetVersion ( mmlUInt32 *major, mmlUInt32 *minor);                                                                               \
        mmlBool SetVersion (  const mmlUInt32 major,  const mmlUInt32 minor);                                                                   \
        mmlBool GetEncoding ( mmlICString **encoding);                                                                                          \
        mmlBool SetEncoding ( mmlICString *encoding);                                                                                           \
        mmlBool Get ( const mmlChar *path, mmlIXMLNode **node);                                                                                 \
        mmlBool Set ( const mmlChar *path, mmlIXMLNode *node);                                                                                  \
        mmlBool Load ( mmlICStringInputStream *stream);                                                                                         \
        mmlBool Save ( mmlICStringOutputStream *stream);                                                                                        \



#define MML_IXMLDOCUMENT_IMPL(_class)                                                                                                       \
MML_OBJECT_IMPL_EXECUTE_BEGIN(_class)                                                                                                       \
MML_OBJECT_IMPL_EXECUTE_END(_class)                                                                                                         \
MML_OBJECT_IMPL_PREPARE_EXECUTE_BEGIN(_class)                                                                                               \
MML_OBJECT_IMPL_PREPARE_EXECUTE_END(_class)                                                                                                 \

#define MML_IXMLDOCUMENT_COMMON_IMPL                                                                                                        \



#endif //MML_IXMLDOCUMENT_HEADER_INCLUDED
