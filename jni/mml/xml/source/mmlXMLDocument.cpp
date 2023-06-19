#include "mmlXMLDocument.h"
#include "mmlXMLNode.h"
#include "mmlICStringStream.h"
#include "mmlICStringUtils.h"
#include "mml_components.h"

MML_OBJECT_IMPL1(mmlXMLDocument, mmlIXMLDocument);

mmlXMLDocument::mmlXMLDocument()
:mVerMajor(1) , mVerMinor(0) , mEncoding(mmlNewStaticCString("utf-8"))
{
}

mmlBool mmlXMLDocument::GetVersion(mmlUInt32 *major, mmlUInt32 *minor)
{
	*major = mVerMajor;
	*minor = mVerMinor;
	return mmlTrue;
}

mmlBool mmlXMLDocument::SetVersion(const mmlUInt32 major, const mmlUInt32 minor)
{
	mVerMajor = major;
	mVerMinor = minor;
	return mmlTrue;
}

mmlBool mmlXMLDocument::GetEncoding(mmlICString ** encoding)
{
	*encoding = mEncoding;
	MML_ADDREF(*encoding);
	return mmlTrue;
}

mmlBool mmlXMLDocument::SetEncoding(mmlICString * encoding)
{
	mEncoding = encoding;
	return mmlTrue;
}

mmlBool mmlXMLDocument::Get(const mmlChar * path, mmlIXMLNode **node)
{
	if ( path == mmlNULL ||
		*path == 0 ||
		 mRoot == mmlNULL )
	{
		return mmlFalse;
	}
	mmlInt32 separator = 0;
	while ( path[separator] != 0 &&
		    path[separator] != '/' &&
			path[separator] != '.' )
	{
		separator ++;
	}
	if ( path[separator] == 0 )
	{
		mmlAutoPtr < mmlICString > root_node_id;
		mRoot->GetId(root_node_id.getterAddRef());
		mmlBool equal;
		if ( root_node_id->CompareStr(MML_EQUAL , path , equal, MML_CASE_INSENSITIVE) == mmlTrue && equal == mmlTrue )
		{
			*node = mRoot;
			MML_ADDREF(*node);
			return mmlTrue;

		}
	}
	else
	{
		if ( mRoot == mmlNULL ) return mmlFalse;
		mmlAutoPtr < mmlICString > root_node_id;
		mRoot->GetId(root_node_id.getterAddRef());

		mmlBool equal = mmlFalse;
		mmlAutoPtr < mmlICString > path_str = mmlNewCString(path , separator);
		if ( root_node_id->CompareStr(MML_EQUAL , path_str->Get() , equal, MML_CASE_INSENSITIVE) == mmlTrue &&
			equal == mmlTrue )
		{
			return mRoot->GetChild(path + separator + 1 , node );
		}
	}
	return mmlFalse;
}

mmlBool mmlXMLDocument::Set(const mmlChar * path, mmlIXMLNode *node)
{
	if ( path == mmlNULL ||
		*path == mmlNULL )
	{
		mRoot = node;
		return mmlTrue;
	}
	mmlInt32 separator = 0;
	while ( path[separator] != 0 &&
		    path[separator] != '/' )
	{
		separator ++;
	}
	if ( path[separator] == 0 )
	{
		if ( mRoot == mmlNULL )
		{
			mmlSharedPtr < mmlICString > str_path = mmlNewObject(MML_OBJECT_UUID(mmlICString));
			str_path->Assign(path);
			mRoot = new mmlXMLNode(str_path);
			return mRoot->AddChild(mmlNULL , node);
		}
		mmlAutoPtr < mmlICString > root_node_id;
		mRoot->GetId(root_node_id.getterAddRef());
		mmlBool equal;
		if ( root_node_id->CompareStr(MML_EQUAL , path , equal, MML_CASE_INSENSITIVE) == mmlTrue &&
			 equal == mmlTrue)
		{
			return mRoot->AddChild(mmlNULL , node);
		}
	}
	else 
	{
		mmlBool equal = mmlFalse;
		mmlAutoPtr < mmlICString > path_str = mmlNewCString(path , separator);
		if ( mRoot == mmlNULL )
		{
			mRoot = new mmlXMLNode(path_str);
			return mRoot->AddChild(mmlNULL , node);
		}
		mmlAutoPtr < mmlICString > root_node_id;
		mRoot->GetId(root_node_id.getterAddRef());
		if ( root_node_id->CompareStr(MML_EQUAL , path_str->Get() , equal, MML_CASE_INSENSITIVE) == mmlTrue &&
			equal == mmlTrue )
		{
			return mRoot->AddChild(path + separator + 1 , node );
		}
	}
	return mmlFalse;
}

mmlBool mmlXMLDocument::Load(mmlICStringInputStream *stream)
{
	mmlInt32 count;
	mmlInt32 restore_point;
	mmlAutoPtr < mmlICString > token;
	mmlBool equal;
	stream->GetRestorePoint(&restore_point);
	mmlUInt8 header;
	stream->ReadChar((mmlChar&)header, mmlFalse);
	if ( header == 0xEF)
	{
		stream->Seek(1, mmlFalse);
		stream->ReadChar((mmlChar&)header, mmlTrue);
		if ( header != 0xBB )
		{
			return mmlFalse;
		}
		stream->ReadChar((mmlChar&)header, mmlTrue);
		if ( header != 0xBF )
		{
			return mmlFalse;
		}
	}



	if ( stream->Skip("<", &count) == mmlTrue &&
		 count == 1 &&
		 stream->Skip("?" , &count) == mmlTrue &&
		 count == 1 &&
		 stream->ReadToken(" ?>" , mmlTrue , token.getterAddRef()) == mmlTrue )
	{
		if ( token->CompareStr(MML_EQUAL , "xml" , equal) != mmlTrue ||
			 equal != mmlTrue )
		{
			return mmlFalse;
		}
		for (;;)
		{
			stream->Skip(" \r\n\t", &count);
			if ( stream->Skip("?" , &count) == mmlTrue &&
				 count == 1 &&
				 stream->Skip(">" , &count) == mmlTrue &&
				 count == 1 )
			{
				break;
			}
			if ( stream->ReadToken(" =?>" , mmlTrue , token.getterAddRef()) == mmlFalse ) return mmlFalse;
			if ( stream->Skip("=" , &count) == mmlFalse || count == 0 ) return mmlFalse;
			if ( token->CompareStr(MML_EQUAL , "version" , equal, MML_CASE_INSENSITIVE) == mmlTrue &&
				 equal == mmlTrue )
			{
				stream->Skip("\"" , &count);
				if ( stream->ReadInteger32(&mVerMajor) == mmlFalse ) return mmlFalse;
				if ( stream->Skip("." , &count) == mmlFalse || count == 0 ) return mmlFalse;
				if ( stream->ReadInteger32(&mVerMinor) == mmlFalse ) return mmlFalse;
				stream->Skip("\"", &count);

			}
			else if ( token->CompareStr(MML_EQUAL , "encoding" , equal, MML_CASE_INSENSITIVE) == mmlTrue &&
				 equal == mmlTrue )
			{
				stream->Skip("\"" , &count);
				if ( stream->ReadToken(" \"" , mmlTrue , mEncoding.getterAddRef()) == mmlFalse ) return mmlFalse;
				stream->Skip("\"" , &count);
			}
			else
			{
				stream->SkipToken(" ?>");
			}
		}
	}
	else
	{
		stream->Restore(restore_point);
	}
	mRoot = new mmlXMLNode();
	if ( mRoot->Load(stream) == mmlFalse )
	{
		mRoot = mmlNULL;
	}
	return mmlTrue;
}

mmlBool mmlXMLDocument::Save(mmlICStringOutputStream *stream)
{
	stream->WriteStr("<?xml version=\"");
	stream->WriteInteger32(mVerMajor, mmlFalse , 0);
	stream->WriteChar('.');
	stream->WriteInteger32(mVerMinor , mmlFalse , 0);
	stream->WriteStr("\" encoding=\"");
	stream->WriteStr(mEncoding->Get());
	stream->WriteStr("\"?>\n");
	mmlInt32 depth = 0;
	if ( mRoot != mmlNULL )
	{
		return mRoot->Save(stream , depth);
	}
	return mmlTrue;
}

mmlBool mmlXMLDocument::NewNode ( mmlIXMLNode *parent , const mmlChar * name, mmlIXMLNode **node)
{
	mmlSharedPtr < mmlICString > id = mmlNewObject(MML_OBJECT_UUID(mmlICString));
	id->Assign(name);
	*node = new mmlXMLNode (id);
	MML_ADDREF(*node);
	if ( parent == mmlNULL )
	{
		mRoot = *node;
	}
	else
	{
		parent->AddChild(mmlNULL , *node);
	}
	return mmlTrue;
}

