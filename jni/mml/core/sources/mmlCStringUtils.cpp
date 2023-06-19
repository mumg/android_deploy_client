#include "mmlCStringUtils.h"
#include "mmlCString.h"
#include "mml_strutils.h"
#include "mmlList.h"
#include <string.h>



MML_OBJECT_IMPL1(mmlCStringUtils , mmlICStringUtils)

mmlBool mmlCStringUtils::Find ( mmlICString * str , const mmlChar * sstr , mmlInt32 * pos , mml_char_conversion_t rule)
{
	*pos = 0;
	const mmlUInt8 * _str = (const mmlUInt8 *)str->Get();
	mmlInt32 _str_len = str->Length();
	const mmlUInt8 * _sstr = (const mmlUInt8 *)sstr;
	mmlInt32 _sstr_len = strlen(sstr);
	mmlInt32 next = 1;
	while ( *pos < _str_len)
	{
		mmlInt32 index;
		mmlBool found = mmlFalse;
		for ( index = 0 ; (found = (index < _sstr_len) ? mmlFalse : mmlTrue) == mmlFalse ;  )
		{
			if ( index >= _str_len )
			{
				break;
			}
			mmlUChar _c_str;
			mmlInt32 offset = mml_utf8_mbtowc(&_c_str , _str + (*pos) + index , _str_len - index);
			if ( offset == 0 )
			{
				return mmlFalse;
			}
			if ( index == 0)
			{
				next = offset;
			}
			_c_str = rule != mmlNULL ? rule (_c_str) : _c_str;
			mmlUChar _c_sstr;
			if ( offset != mml_utf8_mbtowc(&_c_sstr , _sstr + index , _sstr_len - index))
			{
				break;
			}

			index += offset;
			_c_sstr = rule != mmlNULL ? rule (_c_sstr) : _c_sstr;
			if ( _c_sstr != _c_str )
			{
				break;
			}
		}
		if ( found == mmlTrue )
		{
			return mmlTrue;
		}
		else
		{
			(*pos) += next;
		}
	}
	return mmlFalse;
}

mmlBool mmlCStringUtils::FindFirstOf ( mmlICString * str , const mmlChar * sstr , mmlInt32 * pos, mml_char_conversion_t rule)
{
	*pos = 0;
	const mmlUInt8 * _str = (const mmlUInt8 *)str->Get();
	mmlInt32 _str_len = str->Length();
	const mmlUInt8 * _sstr = (const mmlUInt8 *)sstr;
	mmlInt32 _sstr_len = strlen(sstr);

	while ( *pos < _str_len)
	{
		mmlUChar _c_str;
		mmlInt32 offset = mml_utf8_mbtowc(&_c_str , _str + (*pos)  , _str_len);
		if ( offset == 0 )
		{
			return mmlFalse;
		}
        _c_str = rule != mmlNULL ? rule(_c_str) : _c_str;

		for ( mmlInt32 index = 0 ; index < _sstr_len; )
		{

			mmlUChar _c_sstr;
            mmlInt32 f_offset = mml_utf8_mbtowc(&_c_sstr , _sstr + index , _sstr_len - index);
			if ( offset != f_offset)
			{
                index += f_offset;
			}
            _c_sstr = rule != mmlNULL ? rule(_c_sstr) : _c_sstr;
			if ( _c_sstr == _c_str )
			{
				return mmlTrue;
			}
            else
            {
                index += f_offset;
            }

		}
		(*pos) += offset;
	}
	return mmlFalse;
}

mmlBool mmlCStringUtils::FindFirstNotOf ( mmlICString * str , const mmlChar * sstr , mmlInt32 * pos, mml_char_conversion_t rule)
{
    *pos = 0;
    const mmlUInt8 * _str = (const mmlUInt8 *)str->Get();
    mmlInt32 _str_len = str->Length();
    const mmlUInt8 * _sstr = (const mmlUInt8 *)sstr;
    mmlInt32 _sstr_len = strlen(sstr);

    while ( *pos < _str_len)
    {
        mmlUChar _c_str;
        mmlInt32 offset = mml_utf8_mbtowc(&_c_str , _str + (*pos)  , _str_len);
        if ( offset == 0 )
        {
            return mmlFalse;
        }
        _c_str = rule != mmlNULL ? rule(_c_str) : _c_str;
        mmlBool found = mmlFalse;
        for ( mmlInt32 index = 0 ; index < _sstr_len; )
        {

            mmlUChar _c_sstr;
            mmlInt32 f_offset = mml_utf8_mbtowc(&_c_sstr , _sstr + index , _sstr_len - index);
            if ( offset != f_offset)
            {
                index += f_offset;
            }
            _c_sstr = rule != mmlNULL ? rule(_c_sstr) : _c_sstr;
            if ( _c_sstr == _c_str )
            {
                found = mmlTrue;
                break;
            }
            else
            {
                index += f_offset;
            }
        }
        if ( found == mmlFalse )
        {
            return mmlTrue;
        }
        (*pos) += offset;
    }
    return mmlFalse;
}

mmlBool mmlCStringUtils::FindLastOf ( mmlICString * str , const mmlChar * sstr , mmlInt32 * pos, mml_char_conversion_t rule)
{
    *pos = -1;
    const mmlUInt8 * _str = (const mmlUInt8 *)str->Get();
    mmlInt32 _str_len = str->Length();
    const mmlUInt8 * _sstr = (const mmlUInt8 *)sstr;
    mmlInt32 _sstr_len = strlen(sstr);
    mmlInt32 cursor = 0;
    while ( cursor < _str_len)
    {

        mmlUChar _c_str;
        mmlInt32 offset = mml_utf8_mbtowc(&_c_str , _str + cursor  , _str_len);
        if ( offset == 0 )
        {
            return mmlFalse;
        }
        _c_str = rule != mmlNULL ? rule(_c_str) : _c_str;

        for ( mmlInt32 index = 0 ; index < _sstr_len; )
        {

            mmlUChar _c_sstr;
            mmlInt32 f_offset = mml_utf8_mbtowc(&_c_sstr , _sstr + index , _sstr_len - index);
            if ( offset != f_offset)
            {
                index += f_offset;
            }
            _c_sstr = rule != mmlNULL ? rule(_c_sstr) : _c_sstr;
            if ( _c_sstr == _c_str )
            {
                (*pos) = cursor;
                break;
            }
            else
            {
                index += f_offset;
            }

        }
        cursor += offset;
    }
    if ( *pos != -1)
    {
        return mmlTrue;
    }
    return mmlFalse;
}

mmlBool mmlCStringUtils::FindLastNotOf ( mmlICString * str , const mmlChar * sstr , mmlInt32 * pos, mml_char_conversion_t rule)
{
    *pos = -1;
    const mmlUInt8 * _str = (const mmlUInt8 *)str->Get();
    mmlInt32 _str_len = str->Length();
    const mmlUInt8 * _sstr = (const mmlUInt8 *)sstr;
    mmlInt32 _sstr_len = strlen(sstr);
    mmlInt32 cursor = 0;
    while ( cursor < _str_len)
    {

	mmlBool found = mmlFalse;
        mmlUChar _c_str;
        mmlInt32 offset = mml_utf8_mbtowc(&_c_str , _str + cursor  , _str_len);
        if ( offset == 0 )
        {
            return mmlFalse;
        }
        _c_str = rule != mmlNULL ? rule(_c_str) : _c_str;

        
        for ( mmlInt32 index = 0 ; index < _sstr_len; )
        {
            mmlUChar _c_sstr;
            mmlInt32 f_offset = mml_utf8_mbtowc(&_c_sstr , _sstr + index , _sstr_len - index);
            if ( offset != f_offset)
            {
                index += f_offset;
            }
            _c_sstr = rule != mmlNULL ? rule(_c_sstr) : _c_sstr;
            if ( _c_sstr == _c_str )
            {
                found = mmlTrue;
                break;
            }
            else
            {
                index += f_offset;
            }
        }
        if ( found == mmlFalse )
        {
            (*pos) = cursor;
        }
        cursor += offset;
    }
    if ( *pos != -1)
    {
        return mmlTrue;
    }
    return mmlFalse;
}

mmlBool mmlCStringUtils::Get ( mmlICString * str , const  mmlInt32 pos , const mmlInt32 len , mmlICString ** sub_str )
{
	if ( str->Length() == 0 ||
		 (mmlUInt32)(pos + len) > str->Length() )
	{
		return mmlFalse;
	}
	const mmlChar * _str = str->Get();
	*sub_str  = new mmlCString ( _str + pos , len );
	MML_ADDREF(*sub_str);
	return mmlTrue;
}

mmlBool mmlCStringUtils::ToInteger16 ( mmlICString * str , mmlInt16 * value )
{
	if ( str->Length() == 0 )
	{
		return mmlFalse;
	}
	mmlBool res = mmlFalse;
	*value = (mmlInt16)mmlStoD(str->Get() , &res);
	return res;
}
mmlBool mmlCStringUtils::ToInteger32 ( mmlICString * str , mmlInt32 * value )
{
	if ( str->Length() == 0 )
	{
		return mmlFalse;
	}
	mmlBool res = mmlFalse;
	*value = (mmlInt32)mmlStoD(str->Get() , &res);
	return res;
}
mmlBool mmlCStringUtils::ToInteger64 ( mmlICString * str , mmlInt64 * value )
{
	if ( str->Length() == 0 )
	{
		return mmlFalse;
	}
	mmlBool res = mmlFalse;
	*value = mmlStoD(str->Get() , &res);
	return res;
}
mmlBool mmlCStringUtils::ToUInteger16 ( mmlICString * str, mmlUInt16 * value )
{
	if ( str->Length() == 0 )
	{
		return mmlFalse;
	}
	mmlBool res = mmlFalse;
	*value = (mmlUInt16)mmlStoUD(str->Get() , &res);
	return res;
}
mmlBool mmlCStringUtils::ToUInteger32 ( mmlICString * str, mmlUInt32 * value )
{
	if ( str->Length() == 0 )
	{
		return mmlFalse;
	}
	mmlBool res = mmlFalse;
	*value = (mmlUInt32)mmlStoUD(str->Get() , &res);
	return res;
}
mmlBool mmlCStringUtils::ToUInteger64 ( mmlICString * str, mmlUInt64 * value ) 
{
	if ( str->Length() == 0 )
	{
		return mmlFalse;
	}
	mmlBool res = mmlFalse;
	*value = mmlStoUD(str->Get() , &res);
	return res;
}

mmlBool mmlCStringUtils::ToFloat ( mmlICString * str , const  mmlInt32 pos , const mmlInt32 len , mmlFloat64 * value )
{
	if ( str->Length() == 0 ||
		(mmlUInt32)(pos + len) > str->Length() )
	{
		return mmlFalse;
	}
	mmlChar * integer_str = (mmlChar*)mmlAlloc(len+1);
	mmlStrCopy(integer_str , str->Get() + pos , len +1);
	for ( mmlInt32 i = 0; i < len; i++ )
	{
		if ( integer_str[i] == ',' )
		{
			integer_str[i] = '.';
			break;
		}
	}
	mmlBool result = mmlFalse;
	*value = mmlStoF(integer_str , &result);
	mmlFree(integer_str);
	return result;
}

mmlBool mmlCStringUtils::Explode ( mmlICString * str , const mmlChar * separators , mmlIList ** lst )
{
	if ( str->Length() == 0 )
	{
		return mmlFalse;
	}
	const mmlChar * _str = str->Get();
	*lst = new mmlList;
	MML_ADDREF(*lst);
	mmlInt32 _begin = 0;
	mmlInt32 _index = 0;
	for ( _index = 0; _str[_index] != 0 ; _index ++ )
	{
		for ( mmlInt32 _sep_index = 0 ; separators[_sep_index] != 0 ; _sep_index ++ )
		{
			if ( _str[_index] == separators[_sep_index] )
			{
				(*lst)->PushBack(new mmlCString(_str + _begin , _index - _begin ));
				_begin = _index + 1;
				break;
			}
		}
	}
	if ( _begin != _index )	
	{
		(*lst)->PushBack(new mmlCString(_str + _begin , _index - _begin ));
	}
	return mmlTrue;
}

mmlBool mmlCStringUtils::Implode ( mmlIList * lst , mmlICString ** str )
{
	mmlInt32 len = 0;
	mmlBool has_more = mmlTrue;
	mmlAutoPtr < mmlIListEnumerator > en;
	lst->GetEnumerator(en.getterAddRef());
	while (en->HasMore(&has_more) == mmlTrue &&
		   has_more == mmlTrue )
	{
		mmlAutoPtr < mmlICString > _str;
		en->Next(_str.getterAddRef());
		if ( _str != mmlNULL )
		{
			len += _str->Length();
		}
	}
	mmlChar * _str_buffer = (mmlChar*) mmlAlloc(len + 1);
	lst->GetEnumerator(en.getterAddRef());
	mmlInt32 _offset = 0;
	while (en->HasMore(&has_more) == mmlTrue &&
		   has_more == mmlTrue )
	{
		mmlAutoPtr < mmlICString > _str;
		en->Next(_str.getterAddRef());
		if ( _str != mmlNULL )
		{
			mmlStrCopy(_str_buffer + _offset , _str->Get() , _str->Length() + 1);
			_offset += _str->Length();
		}
	}
	*str = new mmlCString;
	MML_ADDREF(*str);
	(*str)->Adopt(_str_buffer);
	return mmlTrue;
}

mmlBool mmlCStringUtils::Split ( mmlICString * src , const mmlInt32 pos, mmlICString ** left , mmlICString ** right )
{
	if ( (mmlUInt32)pos >= src->Length() - 1)
	{
		return mmlFalse;
	}
	*left = new mmlCString(src->Get() , pos );
	MML_ADDREF(*left);
	*right = new mmlCString(src->Get() + pos + 1 , src->Length() - pos - 1 );
	MML_ADDREF(*right);
	return mmlTrue;
};


mmlBool mmlCStringUtils::Concat ( mmlICString * left , mmlICString * right , mmlICString ** result )
{
	mmlInt32 len_left = left->Length();
	mmlInt32 len_right = right->Length();
	mmlChar * _str_buffer = (mmlChar*) mmlAlloc(len_left + len_right + 1);
	mmlStrCopy(_str_buffer , left->Get() , len_left + 1);
	mmlStrCopy(_str_buffer + len_left , right->Get() , len_right + 1);
	*result = new mmlCString;
	MML_ADDREF(*result);
	(*result)->Adopt(_str_buffer);
	return mmlTrue;
}

mmlBool mmlCStringUtils::Integer16( const mmlInt16 integer , mmlICString ** value )
{
	mmlChar integer_str[64] = {0};
	mmlSprintf(integer_str , sizeof(integer_str) , "%d" , integer);
	*value = new mmlCString(integer_str, mmlStrLength(integer_str));
	MML_ADDREF(*value);
	return mmlTrue;
}

mmlBool mmlCStringUtils::Integer32( const mmlInt32 integer , mmlICString ** value )
{
	mmlChar integer_str[64] = {0};
	mmlSprintf(integer_str , sizeof(integer_str) , "%d" , integer);
	*value = new mmlCString(integer_str, mmlStrLength(integer_str));
	MML_ADDREF(*value);
	return mmlTrue;
}

mmlBool mmlCStringUtils::Integer64( const mmlInt64 integer , mmlICString ** value )
{
	mmlChar integer_str[64] = {0};
	mmlSprintf(integer_str , sizeof(integer_str) , "%lld" , integer);
	*value = new mmlCString(integer_str, mmlStrLength(integer_str));
	MML_ADDREF(*value);
	return mmlTrue;
}

mmlBool mmlCStringUtils::UInteger16( const mmlUInt16 integer , mmlICString ** value )
{
	mmlChar integer_str[64] = {0};
	mmlSprintf(integer_str , sizeof(integer_str) , "%u" , integer);
	*value = new mmlCString(integer_str, mmlStrLength(integer_str));
	MML_ADDREF(*value);
	return mmlTrue;
}

mmlBool mmlCStringUtils::UInteger32( const mmlUInt32 integer , mmlICString ** value )
{
	mmlChar integer_str[64] = {0};
	mmlSprintf(integer_str , sizeof(integer_str) , "%u" , integer);
	*value = new mmlCString(integer_str, mmlStrLength(integer_str));
	MML_ADDREF(*value);
	return mmlTrue;
}

mmlBool mmlCStringUtils::UInteger64( const mmlUInt64 integer , mmlICString ** value )
{
	mmlChar integer_str[64] = {0};
	mmlSprintf(integer_str , sizeof(integer_str) , "%llu" , integer);
	*value = new mmlCString(integer_str, mmlStrLength(integer_str));
	MML_ADDREF(*value);
	return mmlTrue;
}

mmlBool mmlCStringUtils::Float( const mmlFloat64 integer , mmlICString ** value )
{
	mmlChar integer_str[64] = {0};
	mmlSprintf(integer_str , sizeof(integer_str) , "%f" , integer);
	*value = new mmlCString(integer_str, mmlStrLength(integer_str));
	MML_ADDREF(*value);
	return mmlTrue;
}

mmlBool mmlCStringUtils::Escape ( mmlICString * src , const mmlChar * search, const mmlChar ** replace , mmlICString ** dst )
{
	mmlUInt8 charmap [256] = {0};
	mmlUInt8 lenmap [256] = {0};
	mmlInt32 slen = (mmlInt32)strlen(search);
	for ( mmlInt32 index = 0 ; index < slen; index ++ )
	{
		charmap[(mmlUInt8)search[index]] = index + 1;
		lenmap[(mmlUInt8)search[index]] = (mmlInt32)strlen(replace[index]);
	}

	const mmlChar * ptr = src->Get();
	mmlInt32 len = 0;
	mmlInt32 replaces = 0;
	for ( mmlInt32 index = 0 ; index < (mmlInt32)src->Length(); index ++ )
	{
		if ( charmap[(mmlUInt8)ptr[index]] > 0 )
		{
			len += lenmap[(mmlUInt8)ptr[index]];
			replaces ++;
		}
		else
		{
			len ++;
		}
	}
	if ( replaces > 0 )
	{
		mmlChar * str = (mmlChar*)malloc (len + 1);
		const mmlChar * ptr = src->Get();
		mmlChar * dst_ptr = str;
		for ( mmlInt32 index = 0 ; index < (mmlInt32)src->Length(); index ++ )
		{
			if ( charmap[(mmlUInt8)ptr[index]] > 0 )
			{
				memcpy(dst_ptr , replace[charmap[(mmlUInt8)ptr[index]] - 1] , lenmap[(mmlUInt8)ptr[index]]);
				dst_ptr += lenmap[(mmlUInt8)ptr[index]];
			}
			else
			{
				*dst_ptr = ptr[index];
				dst_ptr ++;
			}
		}
		str[len] = 0;
		*dst = new mmlCString();
		(*dst)->Adopt(str);
		MML_ADDREF(*dst);
	}
	else
	{
		*dst = src;
		MML_ADDREF(*dst);
	}
	return mmlTrue;
}

mmlBool mmlCStringUtils::LowerCase ( mmlICString * src , mmlICString ** dst )
{
    if ( src->Length() == 0 )
    {
        return mmlFalse;
    }
    const mmlUInt8 * _src = (const mmlUInt8*)src->Get();
    mmlInt32 _src_len = src->Length();

    mmlUInt8 * _dst = (mmlUInt8*)mmlAlloc(_src_len + 1);
    mmlInt32 _dst_len = _src_len;

    mmlAutoPtr < mmlICString > res = new mmlCString((mmlChar*)_dst, _dst_len, mmlTrue);
//    res->Adopt((mmlChar*)_dst);
   
    while ( _src_len > 0 && _dst_len > 0 )
    {
        mmlUChar c;
        mmlInt32 dec_size = mml_utf8_mbtowc(&c , _src , _src_len);
        if ( dec_size == 0 )
        {
            return mmlFalse;
        }

        c = mmlToLower(c);

        mmlInt32 enc_size = mml_utf8_wctomb(_dst, c , _dst_len);

        if ( enc_size == 0 )
        {
            return mmlFalse;
        }

        _src += dec_size; _src_len -= dec_size;
        _dst += enc_size; _dst_len -= enc_size;
    }

    if ( _src_len != 0 || _dst_len != 0 )
    {
        return mmlFalse;
    }
    *dst = res;
    MML_ADDREF(*dst);
    return mmlTrue;
}

mmlBool mmlCStringUtils::UpperCase ( mmlICString * src , mmlICString ** dst )
{
    if ( src->Length() == 0 )
    {
        return mmlFalse;
    }
    const mmlUInt8 * _src = (const mmlUInt8*)src->Get();
    mmlInt32 _src_len = src->Length();

    mmlUInt8 * _dst = (mmlUInt8*)mmlAlloc(_src_len + 1);

    mmlAutoPtr < mmlICString > res = new mmlCString();
    res->Adopt((mmlChar*)_dst);

    mmlInt32 _dst_len = _src_len;

    while ( _src_len > 0 && _dst_len > 0 )
    {
        mmlUChar c;
        mmlInt32 dec_size = mml_utf8_mbtowc(&c , _src , _src_len);
        if ( dec_size == 0 )
        {
            return mmlFalse;
        }

        c = mmlToUpper(c);

        mmlInt32 enc_size = mml_utf8_wctomb(_dst, c , _dst_len);

        if ( enc_size == 0 )
        {
            return mmlFalse;
        }

        _src += dec_size; _src_len -= dec_size;
        _dst += enc_size; _dst_len -= enc_size;
    }

    if ( _src_len != 0 || _dst_len != 0 )
    {
        return mmlFalse;
    }
    *dst = res;
    MML_ADDREF(*dst);
    return mmlTrue;
}


mmlBool mmlCStringUtils::Trim ( mmlICString * src , mmlICString ** dst )
{
	if ( src->Length() == 0 )
	{
		return mmlFalse;
	}
	const mmlChar * begin = (const mmlChar *)src->Get();
	const mmlChar * end = (const mmlChar *)src->Get() + src->Length() - 1;

	for (;;)
	{
		if ( end == begin )
		{
			break;
		}
		if ( *end == ' ' || *end == '\t' || *end == '\n' || *end == '\r' )
		{
			end --;
		}
		else
		{
			break;
		}
	}


	for (;;)
	{
		if ( *begin == 0 )
		{
			break;
		}
		if ( *begin == ' ' || *begin == '\t' || *begin == '\n' || *begin == '\r' )
		{
			begin ++;
		}
		else
		{
			break;
		}
	}

	if ( end <= begin )
	{
		return mmlFalse;
	}
	*dst = mmlNewCString(begin , end - begin + 1);
	MML_ADDREF(*dst);
	return mmlTrue;
}

mmlBool mmlCStringUtils::Replace ( mmlICString * src, const mmlChar * sstr, const mmlChar * replacer, mmlICString ** dst, mml_char_conversion_t rule )
{
	if(src->Length() == 0 || src->Length() < mmlStrLength(sstr)) 
	{
		*dst = src;
		MML_ADDREF(*dst);
		return mmlTrue;
	}

	const mmlUInt8 * str1_mb = (const mmlUInt8*)src->Get();
	const mmlUInt8 * str2_mb = (const mmlUInt8*)sstr;

	mmlUInt32 dst_len = src->Length();
	mmlUInt32 sstr_len = mmlStrLength(sstr);

	mmlUInt32 rep_len = mmlStrLength(replacer);
	mmlInt32 len_diff = rep_len - sstr_len;
		
	mmlInt32 offset_1 = 0;
	mmlInt32 offset_2 = 0;
	mmlBool found = mmlFalse;

	if ( len_diff != 0 )
	{
		mmlUInt32 i = 0;
		mmlUInt32 j = 0;
		while( i < src->Length() )
		{
			j = 0;
			while ( j < sstr_len )
			{
				mmlUChar c1;
				offset_1 = mml_utf8_mbtowc(&c1 , str1_mb + i + j , src->Length() - i - j);
				if ( offset_1 == 0 )
				{
					return mmlFalse;
				}
				c1 = rule != mmlNULL ? rule (c1) : c1;

				mmlUChar c2;
				offset_2 = mml_utf8_mbtowc(&c2 , str2_mb + j , sstr_len - j);
				if ( offset_2 == 0 )
				{
					return mmlFalse;
				}
				c2 = rule != mmlNULL ? rule (c2) : c2;

				if ( c1 != c2 )
				{
					j = -1;
					break;
				}
				else if(found != mmlTrue)
				{
					found = mmlTrue;
				}

				j+=offset_2;
			}
	
			if ( j != -1 )
			{
				dst_len += len_diff;
			}
			
			i+=offset_1;
		}
	}

	if(found == mmlFalse)
	{
		*dst = src;
		MML_ADDREF(*dst);
		return mmlTrue;
	}
	
	mmlChar * dst_str = (mmlChar *)mmlAlloc(dst_len+1);

	*dst = mmlNewCString(dst_str, dst_len, mmlTrue);
	MML_ADDREF(*dst);
	
	mmlUInt32 i = 0;
	mmlUInt32 j = 0;
	mmlUInt32 index = 0;
	offset_1 = 0;
	offset_2 = 0;

	while ( i < src->Length() )
	{
		j = 0;
		while ( j < sstr_len )
		{
			mmlUChar c1;
			offset_1 = mml_utf8_mbtowc(&c1 , str1_mb + i + j , src->Length() - i - j);
			if ( offset_1 == 0 )
			{
				return mmlFalse;
			}
			c1 = rule != mmlNULL ? rule (c1) : c1;

			mmlUChar c2;
			offset_2 = mml_utf8_mbtowc(&c2 , str2_mb + j , sstr_len - j);
			if ( offset_2 == 0 )
			{
				return mmlFalse;
			}
			c2 = rule != mmlNULL ? rule (c2) : c2;

			if ( c1 != c2 )
			{
				j = -1;
				break;
			}

			j+= offset_2;
		}
		if ( j != -1 )
		{
			mmlMemoryCopy(&(dst_str[index]), replacer, rep_len);
			index += rep_len;
		}
		else
		{
			mmlMemoryCopy(&(dst_str[index]), &(src->Get()[i]), offset_1);
			index += offset_1;
		}

		i+=offset_1;
	}

	dst_str[dst_len] = 0;

	return mmlTrue;
}

// Substrings and replacers are separated by "\0"
// Strings ends off as "\0\0"
mmlBool mmlCStringUtils::MultiReplace ( mmlICString * src, const mmlChar * sstr, const mmlChar * replacer, mmlICString ** dst, mml_char_conversion_t rule )
{
	if ( sstr[0] == 0 || replacer[0] == 0 )
	{
		return mmlFalse;
	}
	
	mmlUInt32 sstr_count = 1;
	mmlUInt32 rep_count = 1;

	// Computation of count of elements
	for( mmlUInt32 i = 0; sstr[i] != 0 || sstr[i+1] != 0; i++ )
	{
		if ( sstr[i] == 0 )
		{
			sstr_count++;
		}
	}
	for( mmlUInt32 i = 0; replacer[i] != 0 || replacer[i+1] != 0; i++ )
	{
		if ( replacer[i] == 0 )
		{
			rep_count++;
		}
	}
	if ( rep_count != sstr_count )
	{
		return mmlFalse;
	}

	const mmlChar ** sstr_arr = new const mmlChar * [sstr_count+1];
	const mmlChar ** rep_arr = new  const mmlChar * [rep_count+1];
	mmlUInt32 * diff_len_arr = new mmlUInt32 [rep_count];	

	sstr_arr[0] = sstr;
	rep_arr[0] = replacer;

	// Filling of array of elements and differences of their lengths
	mmlUInt32 sstr_index = 1;
	mmlUInt32 i;
	for( i = 0; sstr[i] != 0 || sstr[i+1] != 0; i++ )
	{
		if ( sstr[i] == 0 )
		{
			sstr_arr[sstr_index] = &(sstr[i+1]);
			sstr_index++;
		}
	}
	sstr_arr[sstr_index] = &(sstr[i+1]);

	mmlUInt32 rep_index = 1;
	for( i = 0; replacer[i] != 0 || replacer[i+1] != 0; i++ )
	{
		if ( replacer[i] == 0 )
		{
			rep_arr[rep_index] = &(replacer[i+1]);
			rep_index++;
		}
	}
	rep_arr[rep_index] = &(replacer[i+1]);

	for( mmlUInt32 i = 0; i < rep_count; i++ )
	{
		diff_len_arr[i] = ( (mmlInt32)(rep_arr[i+1] - rep_arr[i]) - (mmlInt32)(sstr_arr[i+1] - sstr_arr[i]) ) / sizeof(mmlChar);
	}

	// Computation of destination string length
	mmlUInt32 dst_len = mmlStrLength(src->Get());
	for ( mmlUInt32 index = 0; index < rep_count; index++ )
	{
		if ( diff_len_arr[index] != 0 )
		{
			mmlUInt32 i = 0;
			mmlUInt32 j;
			while( i < src->Length() )
			{
				mmlUInt32 len = sstr_arr[index+1] - sstr_arr[index] - 1;
				for ( j = 0; j < len; j++ )
				{
					mmlUChar c1;
					mmlInt32 offset_1 = mml_utf8_mbtowc(&c1 , (const mmlUInt8*)(src->Get()) + i + j , src->Length() - i - j);
					if ( offset_1 == 0 )
					{
						return mmlFalse;
					}
					c1 = rule != mmlNULL ? rule (c1) : c1;

					mmlUChar c2;
					mmlInt32 offset_2 = mml_utf8_mbtowc(&c2 , ((const mmlUInt8*)sstr_arr[index]) + j , mmlStrLength(sstr_arr[index]) - j);
					if ( offset_2 == 0 )
					{
						return mmlFalse;
					}
					c2 = rule != mmlNULL ? rule (c2) : c2;

					if ( c1 != c2 )
					{
						j = -1;
						break;
					}
				}
				if ( j != -1 )
				{
					dst_len += diff_len_arr[index];
					i += len;
				}
				else
				{
					i++;
				}
			}
		}
	}
	
	mmlChar * dst_str = (mmlChar *)mmlAlloc(dst_len);
	
	// Replacement	
	mmlUInt32 src_index = 0;
	mmlUInt32 dst_index = 0;
	mmlUInt32 index;
	mmlUInt32 sstr_len;
	mmlBool is_found = mmlFalse;
	while ( src_index < src->Length() )
	{
		for ( index = 0; index < rep_count; index++ )
		{
			sstr_len = sstr_arr[index+1] - sstr_arr[index] - 1;
			is_found = mmlTrue;
			for ( mmlUInt32 j = 0; j < sstr_len; j++ )
			{
				mmlUChar c1;
				mmlInt32 offset_1 = mml_utf8_mbtowc(&c1 , (const mmlUInt8*)(src->Get()) + src_index + j , src->Length() - i - j);
				if ( offset_1 == 0 )
				{
					return mmlFalse;
				}
				c1 = rule != mmlNULL ? rule (c1) : c1;

				mmlUChar c2;
				mmlInt32 offset_2 = mml_utf8_mbtowc(&c2 , ((const mmlUInt8*)sstr_arr[index]) + j , mmlStrLength(sstr_arr[index]) - j);
				if ( offset_2 == 0 )
				{
					return mmlFalse;
				}
				c2 = rule != mmlNULL ? rule (c2) : c2;

				if ( c1 != c2 )
				{
					is_found = mmlFalse;
					break;
				}
			}
			if ( is_found == mmlTrue )
			{
				break;
			}			
		}
		if ( is_found == mmlTrue )
		{
			mmlUInt32 rep_len = rep_arr[index+1] - rep_arr[index] - 1;
			mmlMemoryCopy(&(dst_str[dst_index]), rep_arr[index], rep_len);
			src_index += sstr_len;
			dst_index += rep_len;
		}
		else
		{
			dst_str[dst_index] = src->Get()[src_index];
			src_index++;
			dst_index++;
		}
	}
	
	*dst = mmlNewCString(dst_str, dst_len);
	MML_ADDREF(*dst);
	return mmlTrue;	
}

mmlBool mmlCStringUtils::Read ( mmlIInputStream * stream, mmlICString ** dst , const mmlInt32 len )
{
	mmlInt32 r_size = stream->Size();
	if ( len > 0 && r_size > len )
	{
		r_size = len;
	}
	mmlChar * data = (mmlChar*)mmlAlloc(r_size + 1);
	mmlInt32 offset = 0;
	mmlMemorySet(data, 0 , r_size + 1);
	while ( r_size > 0 )
	{
		mmlInt32 rd = stream->Read(r_size, data + offset);
		if ( rd <= 0 )
		{
			mmlFree(data);
			return mmlFalse;
		}
		r_size -= rd;
		offset += rd;
	}
	*dst = new mmlCString(data, mmlStrLength(data), mmlTrue);
	MML_ADDREF(*dst);
	return mmlTrue;
}
