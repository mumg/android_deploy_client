#include "mmlHTTPRequest.h"
#include "mmlIList.h"
#include "mml_components.h"
#include "mmlICStringUtils.h"
#include <list>
#include <stdlib.h>
#include <string.h>


// 0 - not allowed
// 1 - regular character
// 2 - special character

static mmlChar http_characters_map [] =
{
//  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,  // 0
	0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,  // 1
	2 , 1 , 1 , 2 , 2 , 2 , 2 , 1 , 1 , 1 , 1 , 3 , 1 , 1 , 1 , 2 ,  // 2
	1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 ,  // 3
	1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,  // 4
	1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 1 ,  // 5
	2 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,  // 6
	1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 1 ,  // 7
	2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,  // 8
	2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,  // 9
	2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,  // A
	2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,  // B
	2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,  // C
	2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,  // D
	2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 ,  // E
	2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2    // F
};

mmlHTTPRequestUri::mmlHTTPRequestUri()
	:mVersion(1.0)
{
}

mmlFloat64 mmlHTTPRequestUri::GetVersion ()
{
	return mVersion;
}

void mmlHTTPRequestUri::SetVersion (const mmlFloat64 version)
{
	mVersion = version;
}

const mmlChar * mmlHTTPRequestUri::Get ()
{
	return mURI->Get();
}

const mmlChar * mmlHTTPRequestUri::GetArgument ( const mmlChar * key )
{
	for ( std::list <  std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > > >::iterator arg = mArguments.begin(); arg != mArguments.end(); arg ++ )
	{
		mmlBool equal;
		if ( arg->first->CompareStr(MML_EQUAL , key, equal) == mmlTrue && equal == mmlTrue )
		{
			return arg->second->Get();
		}
	}
	return NULL;
}

void mmlHTTPRequestUri::SetArgument ( const mmlChar * key , const mmlChar * value )
{
	mArguments.push_back(std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > > (mmlNewCString(key) , value == mmlNULL ? mmlNULL : mmlNewCString(value)));
}

void mmlHTTPRequestUri::Set ( const mmlChar * uri )
{
	mURI = mmlNewCString(uri);
}

void mml_http_request_uri_escape (mmlICStringOutputStream * stream , const mmlChar * str )
{
	static const mmlChar * hexmap = "0123456789ABCDEF";
	while ( *str != 0 )
	{
		int code = http_characters_map[(mmlUInt8)*str];
		if ( code == 1 )
		{
			stream->WriteChar(*str);
		}
		else if ( code == 2 )
		{
			stream->WriteChar('%');
			stream->WriteChar(hexmap[0x0F & (*str >> 4)]);
			stream->WriteChar(hexmap[0x0F & (*str)]);
		}
		else if ( code == 3 )
		{
			stream->WriteChar('+');
		}
		str ++;
	}
}

void mml_http_request_uri_unescape (const mmlChar * str , mmlICString ** result )
{
	static mmlInt32 str2hex [] = 
	{
	//  0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
		-1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 ,  // 0
		-1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 ,  // 1
		-1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 ,  // 2
		0  ,  1 ,  2 ,  3 ,  4 ,  5 ,  6 ,  7 ,  8 ,  9 , -1 , -1 , -1 , -1 , -1 , -1 ,  // 3
		-1 , 10 , 11 , 12 , 13 , 14 , 15 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 ,  // 4
		-1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 ,  // 5
		-1 , 10 , 11 , 12 , 13 , 14 , 15 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 ,  // 6
		-1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 ,  // 7
		-1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 ,  // 8
		-1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 ,  // 9
		-1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 ,  // A
		-1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 ,  // B
		-1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 ,  // C
		-1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 ,  // D
		-1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 ,  // E
		-1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1 , -1    // F
	};
	int len = strlen(str);
	if ( len > 0 )
	{
		mmlChar * unescaped = (mmlChar*)malloc(len + 1);
		mmlMemorySet(unescaped, 0 , len + 1);
		mmlChar * _dst = unescaped;
		int _len = 0;
		while ( *str != 0 )
		{
			if (*str == '%')
			{
				mmlChar character = 0;
				str ++;
				if (*str == 0 ) break;
				if (str2hex[(unsigned int)*str] == -1) continue;
				character = str2hex[(unsigned int)*str] << 4;
				str ++;
				if (*str == 0 ) break;
				if (str2hex[(unsigned int)*str] == -1) continue;
				character = character | str2hex[(unsigned int)*str];
				*_dst = character;
				_dst ++;
				_len ++;
			}
			else if (*str == '+')
			{
				*_dst = ' ';
				_dst ++;
				_len ++;
			}
			else if ( http_characters_map[(unsigned int)*str] == 1 )
			{
				*_dst = *str;
				_dst ++;
				_len ++;
			}
			str ++;
		}
		mmlSharedPtr < mmlICStringUtils > utils = mmlNewObject(MML_OBJECT_UUID(mmlICStringUtils));
		*result = mmlNewCString(unescaped , _len, mmlTrue);
		MML_ADDREF(*result);
	}
}
void mmlHTTPRequestUri::Serialize ( mmlICStringOutputStream * stream )
{
	stream->WriteStr(mURI->Get());
	if ( mArguments.size() > 0 )
	{
		stream->WriteChar('?');
		for ( std::list <  std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > > >::iterator arg = mArguments.begin(); arg != mArguments.end();  )
		{
			mml_http_request_uri_escape(stream , arg->first->Get());
			if ( arg->second != mmlNULL )
			{
				stream->WriteChar('=');
				mml_http_request_uri_escape(stream , arg->second->Get());
			}
			arg ++;
			if (arg == mArguments.end() )
			{
				break;
			}
			stream->WriteChar('&');
		}
	}
}

mmlBool mmlHTTPRequestUri::Load ( mmlICStringInputStream * stream )
{
	mmlAutoPtr < mmlICString > uri;
	if ( stream->ReadUntil("? " , " " , " " , NULL , uri.getterAddRef()) == mmlFalse ) return mmlFalse;
	int skipped = 0;
	stream->Skip("?" , &skipped);
	mURI = uri;
	if ( skipped == 1 )
	{
		for(;;)
		{
			mmlAutoPtr < mmlICString > key;
			if ( stream->ReadToken("=& ", mmlTrue , key.getterAddRef()) == mmlFalse ) break;
			mmlAutoPtr < mmlICString > un_key;
			mml_http_request_uri_unescape(key->Get() , un_key.getterAddRef());
			if ( stream->Skip(" " , &skipped) == mmlFalse || skipped >= 1 ) break;
			if ( stream->Skip("&" , &skipped) == mmlTrue && skipped == 1 )
			{
				mArguments.push_back(std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > >(un_key , (mmlICString*)NULL));
			}
			else if ( stream->Skip("=" , &skipped) == mmlTrue && skipped == 1 )
			{
				mmlAutoPtr < mmlICString > value;
				if ( stream->ReadToken("& ", mmlTrue , value.getterAddRef()) == mmlFalse ) break;
				mmlAutoPtr < mmlICString > un_value;
				mml_http_request_uri_unescape(value->Get() , un_value.getterAddRef());
				mArguments.push_back(std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > >(un_key, un_value));
				stream->Skip("&" , NULL);
			}
		}
	}
	return mmlTrue;
}


int mmlHTTPRequestUri::Size ()
{
	return mArguments.size();
}

mmlBool mmlHTTPRequestUri::GetArgument (const mmlInt32 index , mmlICString ** key , mmlICString ** value )
{
	int count = 0;
	for ( std::list <  std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > > >::iterator arg = mArguments.begin(); arg != mArguments.end(); arg ++, count ++  )
	{
		if ( count == index )
		{
			if (key != mmlNULL)
			{
				*key = arg->first;
				MML_ADDREF(*key);
			}
			if ( value != mmlNULL )
			{
				*value = arg->second;
				MML_ADDREF(*value);
			}
			return mmlTrue;
		}
	}
	return mmlFalse;
}


MML_OBJECT_IMPL2(mmlHTTPRequestUri, mmlHTTPRequestUri , mmlIHTTPRequestUri)

int http_characters_safe_map [] = 
{
	0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
	1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
	1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
	1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
	1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
	1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 ,
	0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
	0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
};

mmlHTTPRequest::mmlHTTPRequest ()
:mVersion(1.0)
{
}

mmlBool mmlHTTPRequest::Load ( void * data , const int data_len, int * data_decoded, mmlIHTTPRequest ** request)
{
	std::list < mmlSharedPtr < mmlICStringInputStream > > lines;
	mmlChar * buffer = (mmlChar*)data;
	int begin = 0;
	int index = 0;

	mmlBool delimiter_found = mmlFalse;
	for ( index = 0 ; index < data_len; index ++ )
	{
		if ( index + 1 < data_len &&
			(( buffer[index] == '\n' && buffer[index + 1] == '\r' ) ||
			( buffer[index] == '\r' && buffer[index + 1] == '\n' )) )
		{
			if ( index - begin == 0 )
			{
				break;
			}

			mmlSharedPtr < mmlICStringInputStream > input_stream = mmlNewObject(MML_OBJECT_UUID(mmlICStringInputStream), 
				                                                                mmlNewObject(MML_OBJECT_UUID(mmlIMemoryInputStream) , mmlNewStaticBuffer(buffer + begin , index + 1 - begin)));
			lines.push_back(input_stream);
			begin = index + 2;
			index ++;

			if ( index + 2 <= data_len)
			{
				if ( buffer[index + 1] == '\r' &&
					buffer[index + 2] == '\n')
				{
					delimiter_found = mmlTrue;
					index += 3;
					break;
				}
			}
		}
		else if ( http_characters_safe_map[(mmlUInt8)buffer[index]] == 0 )
		{
			return mmlFalse;
		}
	}
	if (delimiter_found == mmlFalse )
	{
		return mmlFalse;
	}
	if ( lines.size() > 0 )
	{
		*data_decoded = index;
		mmlInt32 skipped;
		mmlSharedPtr < mmlICStringUtils > utils = mmlNewObject(MML_OBJECT_UUID(mmlICStringUtils));
		mmlSharedPtr < mmlICStringInputStream > request_line = lines.front(); lines.pop_front();
	    mmlAutoPtr < mmlHTTPRequest > req = new mmlHTTPRequest;

		if ( request_line->ReadToken(" " , mmlTrue , req->mMethod.getterAddRef()) == mmlFalse || req->mMethod == mmlNULL ) return mmlFalse;
		request_line->Skip(" " , NULL);
		req->mReqURI = new mmlHTTPRequestUri();
		if ( req->mReqURI->Load(request_line) == mmlFalse ) return mmlFalse;
		request_line->Skip(" " , NULL);
		mmlAutoPtr < mmlICString > protocol;
		mmlBool equal;
		if ( request_line->ReadToken("/ " , mmlTrue , protocol.getterAddRef()) == mmlFalse || protocol->CompareStr(MML_EQUAL, "HTTP", equal ) == mmlFalse || equal == mmlFalse ) return mmlFalse;
		if ( request_line->Skip("/" , &skipped) == mmlFalse || skipped != 1 ) return mmlFalse;
		if ( request_line->ReadFloat(&req->mVersion) == mmlFalse ) return mmlFalse;
		for ( std::list < mmlSharedPtr < mmlICStringInputStream > > ::iterator line = lines.begin(); line != lines.end(); line ++ )
		{
			mmlAutoPtr < mmlICString > key;
			mmlAutoPtr < mmlICString > value;
			if ( (*line)->ReadToken(":" , mmlTrue , key.getterAddRef()) == mmlFalse || (*line)->Skip(":" , &skipped) == mmlFalse || skipped != 1) return mmlFalse;
			if ( (*line)->ReadUntil("\r\n" , " " , " " , NULL , value.getterAddRef()) == mmlFalse ) return mmlFalse;
			req->mHeaders.push_back(std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > >(key, value));
		}
		*request = req;
		MML_ADDREF(*request);
		return mmlTrue;
	}
	return mmlFalse;
}

mmlBool mmlHTTPRequest::Serialize ( mmlICStringOutputStream * stream )
{
	if ( mMethod == mmlNULL )
	{
		return mmlFalse;
	}
	stream->WriteStr(mMethod->Get());
	stream->WriteChar(' ');
	mmlSharedPtr < mmlHTTPRequestUri > request = (mmlIHTTPRequestUri*)mReqURI;
	if ( request == mmlNULL )
	{
		return mmlFalse;
	}
	request->Serialize(stream);
	stream->WriteFormatted(" HTTP/%1.1f\r\n", request->GetVersion());


	if ( mHeaders.size() > 0 )
	{
		for ( std::list <  std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > > >::iterator hdr = mHeaders.begin(); hdr != mHeaders.end(); hdr ++ )
		{
			stream->WriteStr((*hdr).first->Get());
			stream->WriteStr(": ");
			stream->WriteStr((*hdr).second->Get());
			stream->WriteStr("\r\n");
		}
	}
	return mmlTrue;
}

const mmlChar * mmlHTTPRequest::GetMethod ()
{
	return mMethod != mmlNULL ? mMethod->Get() : mmlNULL;
}

void mmlHTTPRequest::SetMethod ( const mmlChar * method )
{
	mMethod = mmlNewCString(method);
}

const mmlChar * mmlHTTPRequest::GetHeader ( const mmlChar * name )
{
	for ( std::list <  std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > > >::iterator hdr = mHeaders.begin(); hdr != mHeaders.end(); hdr ++ )
	{
		mmlBool equal;
		if ( hdr->first->CompareStr(MML_EQUAL , name , equal, MML_CASE_INSENSITIVE) == mmlTrue && equal == mmlTrue )
		{
			if ( hdr->second != mmlNULL )
			{
				return hdr->second->Get();
			}
			return mmlNULL;
		}
	}
	return mmlNULL;
}

void mmlHTTPRequest::DeleteHeader ( const mmlChar * name )
{
	for ( std::list <  std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > > >::iterator hdr = mHeaders.begin(); hdr != mHeaders.end(); hdr ++ )
	{
		mmlBool equal;
		if ( hdr->first->CompareStr(MML_EQUAL , name , equal) == mmlTrue && equal == mmlTrue )
		{
			mHeaders.erase(hdr);
			break;
		}
	}
}

void mmlHTTPRequest::SetHeader ( const mmlChar * name, const mmlChar * value )
{
	mHeaders.push_back(std::pair < mmlAutoPtr < mmlICString > , mmlAutoPtr < mmlICString > >(mmlNewCString(name), mmlNewCString(value)));
}

mmlIHTTPRequestUri * mmlHTTPRequest::GetReqURI ()
{
	return mReqURI;
}

void mmlHTTPRequest::SetContent ( mmlIInputStream * content )
{
	mContent = content;
}

void mmlHTTPRequest::SetReqURI(mmlIHTTPRequestUri * uri)
{
	mReqURI = uri;
}

mmlBool mmlHTTPRequest::GetContent ( mmlIInputStream ** content )
{
	if ( mContent != NULL )
	{
		*content = mContent;
		MML_ADDREF(*content);
		return mmlTrue;
	}
	return mmlFalse;
}


MML_OBJECT_IMPL2(mmlHTTPRequest, mmlHTTPRequest , mmlIHTTPRequest)

