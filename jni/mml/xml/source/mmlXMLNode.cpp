#include "mmlXMLNode.h"
#include "mmlICStringUtils.h"
#include "mml_components.h"

MML_OBJECT_IMPL2(mmlXMLNode,mmlXMLNode,mmlIXMLNode);

mmlXMLNode::mmlXMLNode ( mmlICString * id )
:mId(id)
{
	
}

mmlBool mmlXMLNode::GetId(mmlICString ** aId)
{
	*aId = mId;
	MML_ADDREF(*aId);
	return mmlTrue;
}

mmlBool mmlXMLNode::SetAttribute ( const mmlChar *key, mmlICString *value)
{
	mmlAutoPtr < mmlICString > _key = mmlNewCString(key);
	mAttributes[_key] = value;
	return mmlTrue;
}

mmlBool mmlXMLNode::GetAttribute ( const mmlChar *key, mmlICString **value)
{
	mmlAutoPtr < mmlICString > _key = (mmlICString*)mmlCStaticString(key);
	std::map < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > , map_compare >::iterator val = mAttributes.find(_key);
	if ( val != mAttributes.end() )
	{
		*value = val->second;
		MML_ADDREF(*value);
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlXMLNode::GetAttribute(mmlICString * key , mmlICString ** value)
{
	std::map < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > , map_compare >::iterator val = mAttributes.find(key);
	if ( val != mAttributes.end() )
	{
		*value = val->second;
		MML_ADDREF(*value);
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlXMLNode::SetAttribute(mmlICString * key , mmlICString * value)
{
	mAttributes[key] = value;
	return mmlTrue;
}

mmlBool mmlXMLNode::AddSibling(mmlIXMLNode *node)
{
	if ( mSibling == mmlNULL )
	{
		mSibling = node;
		return mmlTrue;
	}
	return mSibling->AddSibling(node);
}

mmlBool mmlXMLNode::RemoveSibling(mmlIXMLNode *node)
{
	if ( (mmlIXMLNode*)mSibling == node )
	{
		mSibling = mmlNULL;
		return mmlTrue;
	}
	else if ( mSibling != mmlNULL )
	{
		return mSibling->RemoveSibling(node);
	}
	return mmlFalse;
}

class mmlXMLNodeEnumerator : public mmlIXMLNodeEnumerator
{
public:

	MML_OBJECT_DECL;

	mmlXMLNodeEnumerator ( mmlXMLNode * node )
		:mNode(node)
	{

	}

	mmlBool HasMore(mmlBool *_retval)
	{
		*_retval = mNode == mmlNULL ? mmlFalse : mmlTrue;
		return mmlTrue;
	}

	mmlBool GetNext(mmlIXMLNode **node)
	{
		*node = mNode;
		if ( *node != mmlNULL )
		{
			MML_ADDREF(*node);
			mNode = mNode->mSibling;
			return mmlTrue;
		}
		return mmlFalse;
	}

private:

	mmlSharedPtr < mmlXMLNode > mNode;
};

MML_OBJECT_IMPL1(mmlXMLNodeEnumerator, mmlIXMLNodeEnumerator);

mmlBool mmlXMLNode::GetChild ( const mmlInt32 index, mmlIXMLNode ** node )
{
	mmlInt32 size = 0;
	mmlSharedPtr < mmlXMLNode > child;
	child.dynamicCast(mChildren);
	while (child != mmlNULL)
	{
		if ( index == size )
		{
			*node = child;
			MML_ADDREF(*node);
			return mmlTrue;
		}
		size ++;
		child.dynamicCast(child->mSibling);
	}
	return mmlFalse;
}

mmlBool mmlXMLNode::GetEnumerator(mmlIXMLNodeEnumerator **enumerator)
{
	*enumerator = new mmlXMLNodeEnumerator(mChildren);
	MML_ADDREF(*enumerator);
	return mmlTrue;
}


class mmlXMLNodeAttributeEnumerator : public mmlIXMLNodeAttributeEnumerator
{
	MML_OBJECT_DECL
public:
	mmlXMLNodeAttributeEnumerator(mmlXMLNode * node)
		:mNode(node)
	{
		mIterator = mNode->mAttributes.begin();
	}
	mmlBool HasMore ( mmlBool *has_more)
	{
		*has_more = mIterator == mNode->mAttributes.end() ? mmlFalse : mmlTrue;
		return mmlTrue;
	}
	mmlBool GetNext ( mmlICString ** name , mmlICString ** value )
	{
		if ( mIterator == mNode->mAttributes.end() )
		{
			return mmlFalse;
		}

		*name = mIterator->first;
		MML_ADDREF(*name);
		*value = mIterator->second;
		MML_ADDREF(*value);
		mIterator ++;
		return mmlTrue;
	}
private:
	std::map < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > , mmlXMLNode::map_compare> ::iterator mIterator;
	mmlAutoPtr < mmlXMLNode > mNode;
};

MML_OBJECT_IMPL1(mmlXMLNodeAttributeEnumerator, mmlIXMLNodeAttributeEnumerator);

mmlBool mmlXMLNode::GetAttributeEnumerator(mmlIXMLNodeAttributeEnumerator **enumerator)
{
	*enumerator = new mmlXMLNodeAttributeEnumerator(this);
	MML_ADDREF(*enumerator);
	return mmlTrue;
}

mmlBool mmlXMLNode::GetChild(const mmlChar * path, mmlIXMLNode **node)
{
	if ( path == mmlNULL ||
		 *path == 0 )
	{
		return mmlFalse;
	}
	mmlInt32 separator = 0;
	while ( path[separator] != 0 &&
		    path[separator] != '/' &&
			path[separator] != '.')
	{
		separator ++;
	}
	if ( path[separator] == 0 )
	{
		mmlAutoPtr < mmlXMLNode > xml_node = (mmlXMLNode*)mChildren;
		while ( xml_node != mmlNULL )
		{
			mmlAutoPtr < mmlICString > xml_node_id;
			mmlBool equal = mmlFalse;
			xml_node->GetId(xml_node_id.getterAddRef());
			if ( xml_node_id->CompareStr(MML_EQUAL , path , equal, MML_CASE_INSENSITIVE) == mmlTrue &&
				 equal == mmlTrue )
			{
				*node = xml_node;
				MML_ADDREF(*node);
				return mmlTrue;
			}
			xml_node = xml_node->mSibling;
		}
	}
	else
	{
		mmlAutoPtr < mmlXMLNode > xml_node = (mmlXMLNode*)mChildren;
		while ( xml_node != mmlNULL )
		{
			mmlAutoPtr < mmlICString > xml_node_id;
			xml_node->GetId(xml_node_id.getterAddRef());
			mmlBool equal = mmlFalse;
			mmlAutoPtr < mmlICString > path_str = mmlNewCString(path , separator);
			if ( xml_node_id->CompareStr(MML_EQUAL , path_str->Get() , equal, MML_CASE_INSENSITIVE) == mmlTrue &&
				 equal == mmlTrue )
			{
				return xml_node->GetChild(path + separator + 1 , node );
			}
			xml_node = xml_node->mSibling;
		}
	}
	return mmlFalse;
}

mmlBool mmlXMLNode::AddChild(const mmlChar * path, mmlIXMLNode *node)
{
	mmlBool equal;
	if ( path == mmlNULL ||
		*path == 0 )
	{
		if ( mChildren == mmlNULL )
		{
			mChildren = node;
			return mmlTrue;
		}
		return mChildren->AddSibling(node);
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
		mmlAutoPtr < mmlIXMLNode > xml_node = (mmlXMLNode*)mChildren;
		while ( xml_node != mmlNULL )
		{
			mmlAutoPtr < mmlICString > xml_node_id;
			xml_node->GetId(xml_node_id.getterAddRef());
			if ( xml_node_id->CompareStr( MML_EQUAL , path , equal , MML_CASE_INSENSITIVE) == mmlTrue &&
				 equal == mmlTrue)
			{
				return xml_node->AddChild(mmlNULL , node);
			}
		}
		xml_node = new mmlXMLNode(mmlNewCString(path));
		if ( xml_node->AddChild(mmlNULL , node) == mmlTrue )
		{
			if ( mChildren == mmlNULL )
			{
				mChildren = xml_node;
				return mmlTrue;
			}
			else
			{
				return mChildren->AddSibling(xml_node);
			}
		}
	}
	else
	{
		mmlAutoPtr < mmlIXMLNode > xml_node = (mmlXMLNode*)mChildren;
		while ( xml_node != mmlNULL )
		{
			mmlAutoPtr < mmlICString > xml_node_id;
			xml_node->GetId(xml_node_id.getterAddRef());
			mmlBool equal = mmlFalse;
			mmlAutoPtr < mmlICString > path_str = mmlNewCString(path , separator);
			if ( xml_node_id->CompareStr(MML_EQUAL , path_str->Get() , equal, MML_CASE_INSENSITIVE) == mmlTrue &&
				equal == mmlTrue )
			{
				return xml_node->AddChild(path + separator + 1 , node );
			}
		}
		mmlAutoPtr < mmlICString > path_str= mmlNewCString(path , separator);
		xml_node = new mmlXMLNode(path_str);
		if ( xml_node->AddChild(path + separator + 1 , node) == mmlTrue )
		{
			if ( mChildren == mmlNULL )
			{
				mChildren = xml_node;
				return mmlTrue;
			}
			else
			{
				return mChildren->AddSibling(xml_node);
			}
		}
	}
	return mmlFalse;
}

mmlBool mmlXMLNode::GetText(mmlICString ** str)
{
	if ( mChildren != mmlNULL )
	{
		return mmlFalse;
	}
	*str = mText;
	MML_ADDREF(*str);
	return mmlTrue;
}

mmlBool mmlXMLNode::SetText(mmlICString * text)
{
	if ( mChildren != mmlNULL )
	{
		return mmlFalse;
	}
	mText = text;
	return mmlTrue;
}

mmlInt32 mmlXMLNode::Size ()
{
	mmlInt32 size = 0;
	mmlSharedPtr < mmlXMLNode > node;
	node.dynamicCast(mChildren);
	while (node != mmlNULL)
	{
		size ++;
		node.dynamicCast(node->mSibling);
	}
	return size;
}

static void mml_xml_node_write_depth(mmlICStringOutputStream * stream , const mmlInt32 depth )
{
	if ( depth == -1 ) return;
	for ( mmlInt32 count = 0 ; count < depth ; count ++ )
	{
		stream->WriteChar('\t');
	}
}

mmlBool mmlXMLNode::Save ( mmlICStringOutputStream * stream,
						   mmlInt32 & depth )
{
	
	mml_xml_node_write_depth(stream , depth); stream->WriteChar('<'); stream->WriteStr(mId->Get());
	if (mAttributes.size() > 0 )
	{
		for ( std::map < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > , map_compare>::iterator attr = mAttributes.begin(); attr != mAttributes.end(); attr ++ )
		{
			stream->WriteChar(' ');
			stream->WriteStr(attr->first->Get());
			stream->WriteStr("=\"");
			if ( attr->second != mmlNULL )
			{
				stream->WriteStr(attr->second->Get());
			}
			stream->WriteChar('\"');
		}
	}
	if ( mText != mmlNULL || mChildren != mmlNULL )
	{
		if ( mText != mmlNULL )
		{
			stream->WriteStr(">");
			if ( stream->WriteStr(mText->Get()) != mmlTrue )
			{
				return mmlFalse;
			}
			stream->WriteStr("</");
			stream->WriteStr(mId->Get());
			stream->WriteStr(">\n");
		}
		else if ( mChildren != mmlNULL )
		{
			stream->WriteStr(">\n");
			if ( depth != -1 ) depth ++;
			mmlSharedPtr < mmlXMLNode > node;
			node.dynamicCast(mChildren);
			while ( node != mmlNULL )
			{
				if ( node->Save(stream, depth) != mmlTrue )
				{
					return mmlFalse;
				}
				node.dynamicCast(node->mSibling);
			}
			if ( depth != -1 ) depth --;
			mml_xml_node_write_depth(stream , depth);
			stream->WriteStr("</");
			stream->WriteStr(mId->Get());
			stream->WriteStr(">\n");
		}

	}
	else
	{
		stream->WriteStr("/>\n");
	}
	return mmlTrue;
}


typedef struct  
{
	const mmlChar unescaped;
	const mmlChar * escaped;
	const mmlInt32 escaped_len;
}XML_ESCAPE_T;

static const XML_ESCAPE_T xml_escape_map [] =
{
	{ '\"',   "&quot;" , 6 },
	{ '\'',   "&apos;" , 6 },
	{ '<' ,   "&lt;"   , 4 },
	{ '>' ,   "&gt;"   , 4 },
	{ '&' ,   "&amp;"  , 5 },
	{ '\n',   "&#x0D;" , 6 },
	{ '\r',   "&#x0A;" , 6 },
	{ '\t',   "&#x09;" , 6 }
};

static void xml_unescape (mmlICString * src , mmlICString ** dst)
{
	if ( src == mmlNULL || src->Length() == 0 )
	{
		*dst = src;
		MML_ADDREF(*dst);
		return;
	}
	mmlBool escaped = mmlFalse;
	const mmlChar * ptr = src->Get();
	for ( mmlInt32 index = 0; index < (mmlInt32)src->Length(); index ++)
	{
		if ( ptr[index] == '&')
		{
			escaped = mmlTrue;
			break;
		}
	}
	if ( escaped == mmlFalse )
	{
		*dst = src;
		MML_ADDREF(*dst);
	}
	else
	{
		mmlChar * dst_str = (mmlChar *)mmlAlloc(src->Length() + 1);
		mmlChar * dst_ptr = dst_str;

		const mmlChar * ptr = src->Get();
		for ( mmlInt32 index = 0, left = (mmlInt32)src->Length(); index < (mmlInt32)src->Length(); )
		{
			if ( *ptr == '&')
			{
				const XML_ESCAPE_T * escape = mmlNULL;
				for ( mmlUInt32 escape_index = 0 ; escape_index < sizeof(xml_escape_map) / sizeof(XML_ESCAPE_T); escape_index ++ )
				{
					if ( xml_escape_map[escape_index].escaped_len <= left &&
						mmlStrNCompare(xml_escape_map[escape_index].escaped , ptr , xml_escape_map[escape_index].escaped_len) == 0 )
					{
						escape = xml_escape_map + escape_index;
						break;
					}
				}
				if ( escape != mmlNULL )
				{
					*dst_ptr = escape->unescaped;
					dst_ptr ++;
					index += escape->escaped_len;
					ptr += escape->escaped_len;
					left -= escape->escaped_len;
				}
				else
				{
					*dst_ptr =  *ptr; index ++; left --; ptr ++; dst_ptr ++;
				}
			}
			else
			{
				*dst_ptr =  *ptr; index ++; left --; ptr ++; dst_ptr ++;
			}
		}
		*dst_ptr = 0;
		mmlSharedPtr < mmlICString > str_inst = mmlNewObject(MML_OBJECT_UUID(mmlICString));
		str_inst->Adopt(dst_str);
		*dst = str_inst;
		MML_ADDREF(*dst);
	}
}


mmlBool mmlXMLNode::Load ( mmlICStringInputStream * stream )
{
	mmlInt32 count;
	stream->Skip("\t\n\r " , &count);
	if ( stream->Skip("<" , &count) == mmlFalse || count == 0 ) 
	{
		return mmlFalse;
	}
	if ( stream->ReadToken(" >/\t\r\n" , mmlTrue , mId.getterAddRef()) == mmlFalse )
	{
		return mmlFalse;
	}
	stream->Skip("\t\n\r " , &count);
	if ( stream->Skip("/" , &count) == mmlTrue && count == 1 &&
		stream->Skip(">" , &count) == mmlTrue && count == 1 ) 
	{return mmlTrue; }


	for (;;)
	{
		mmlAutoPtr < mmlICString > key;
		stream->Skip("\t\n\r " , &count);
		if ( stream->Skip("/" , &count) == mmlTrue && count == 1 &&
			 stream->Skip(">" , &count) == mmlTrue && count == 1 )
		{return mmlTrue; }

		if ( stream->Skip(">" , &count) == mmlTrue && count == 1 ) break;
		if ( stream->ReadToken("= \t\n\r" , mmlTrue , key.getterAddRef()) == mmlFalse )
		{
			return mmlFalse;
		}
		stream->Skip("\t\n\r " , &count);
		if ( stream->Skip("=" , &count) == mmlTrue && count == 1 )
		{
			mmlAutoPtr < mmlICString > value;
			if ( stream->Skip("\"", &count) != mmlTrue || count == 0 )
			{
				return mmlFalse;
			}
			if ( count == 1 )
			{
				if ( stream->ReadUntil("\"" , " " , " " , mmlNULL , value.getterAddRef()) == mmlFalse )
				{
					return mmlFalse;
				}
				if ( stream->Skip("\"", &count) != mmlTrue || count == 0 ) 
				{
					return mmlFalse;
				}
				mmlAutoPtr < mmlICString > unescaped;
				xml_unescape(value , unescaped.getterAddRef());
				this->SetAttribute(key , unescaped);
			}
			else if ( count != 2 )
			{
				return mmlFalse;
			}
			else
			{
				this->SetAttribute(key , mmlNewStaticCString(""));
			}
		}
		else
		{
			this->SetAttribute(key , mmlNULL);
		}
	}



	stream->Skip("\t\n\r " , &count);

	for (;;)
	{
		mmlInt32 restore_point;
		stream->GetRestorePoint(&restore_point);
		stream->Skip("\t\n\r " , &count);
		if ( stream->Skip("<", &count) == mmlFalse )
		{
			return mmlFalse;
		}
		if ( count == 1 )
		{
			if ( stream->Skip("/" , &count ) == mmlFalse )
			{
				return mmlFalse;
			}
			if ( count == 1 )
			{
				stream->Skip("\t\n\r " , &count);
				mmlAutoPtr < mmlICString > id_terminator;
				if ( stream->ReadToken(" >\t" , mmlTrue , id_terminator.getterAddRef()) == mmlFalse )
				{
					return mmlFalse;
				}
				mmlBool equal;
				if ( mId->CompareStr(MML_EQUAL , id_terminator->Get() , equal) == mmlFalse || equal == mmlFalse )
				{
					return mmlFalse;
				}
				stream->Skip("\t\n\r " , &count);
				stream->Skip(">" , &count);
				if ( count == 1 )
				{
					return mmlTrue;
				}
				return mmlFalse;
			}
			stream->Restore(restore_point);
			mmlAutoPtr < mmlXMLNode > child = new mmlXMLNode();
			if ( child->Load(stream) != mmlTrue )
			{
				return mmlFalse;
			}
			this->AddChild(mmlNULL , child);
		}
		else
		{
			stream->Restore(restore_point);
			mmlAutoPtr < mmlICString > text;
			if ( stream->ReadUntil("<" , " \r\n\t" , " \r\n\t" , mmlNULL , text.getterAddRef()) == mmlFalse ) 
			{
				return mmlFalse;
			}
			mmlAutoPtr < mmlICString > unescaped;
			xml_unescape(text , unescaped.getterAddRef());
			this->SetText(unescaped);
			if ( stream->Skip("<", &count) == mmlFalse )
			{
				return mmlFalse;
			}
			if ( count == 1 )
			{
				if ( stream->Skip("/" , &count ) == mmlFalse )
				{
					return mmlFalse;
				}
				if ( count == 1 )
				{
					stream->Skip("\t\n\r " , &count);
					mmlAutoPtr < mmlICString > id_terminator;
					if ( stream->ReadToken(" >\t" , mmlTrue , id_terminator.getterAddRef()) == mmlFalse )
					{
						return mmlFalse;
					}
					mmlBool equal;
					if ( mId->CompareStr(MML_EQUAL , id_terminator->Get() , equal) == mmlFalse || equal == mmlFalse )
					{
						return mmlFalse;
					}
					stream->Skip("\t\n\r " , &count);
					stream->Skip(">" , &count);
					if ( count == 1 )
					{
						return mmlTrue;
					}
				}
			}
			break;
		}
	}
	return mmlFalse;
}

mmlBool mmlXMLNode::NewNode ( const mmlChar * name, mmlIXMLNode **node)
{
	mmlSharedPtr < mmlICString > id = mmlNewObject(MML_OBJECT_UUID(mmlICString));
	id->Assign(name);
	*node = new mmlXMLNode (id);
	MML_ADDREF(*node);
	return AddChild(mmlNULL , *node);
}

mmlBool mmlXMLNode::Construct ( mmlICStringInputStream * stream )
{
	return Load (stream);
}

mmlBool mmlXMLNode::Match ( const mmlChar * name)
{
	mmlBool result = mmlFalse;
	mId->CompareStr(MML_EQUAL, name, result, MML_CASE_INSENSITIVE);
	return result;
}