//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_charcode.ipp
 * @brief		iris unit test 文字コード対応 ファイル
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
 * @par			copyright
 * Copyright (C) 2011-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_CHARCODE_IPP_D444FB3E_3AFA_46D0_AD69_33FAAF5615E3_
#define INCG_IRIS_IUTEST_CHARCODE_IPP_D444FB3E_3AFA_46D0_AD69_33FAAF5615E3_

//======================================================================
// include
#include "../internal/iutest_charcode.hpp"

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_BEGIN()

namespace iutest {
namespace detail
{

//======================================================================
// variable
const UInt32 kMaxCodePoint1 = (static_cast<UInt32>(1) << 7) - 1;
const UInt32 kMaxCodePoint2 = (static_cast<UInt32>(1) << (5+6)) - 1;
const UInt32 kMaxCodePoint3 = (static_cast<UInt32>(1) << (4+2*6)) - 1;
const UInt32 kMaxCodePoint4 = (static_cast<UInt32>(1) << (3+3*6)) - 1;

//======================================================================
// function
/**
 * @brief	サロゲートペアかどうか
*/
IUTEST_IPP_INLINE IUTEST_CXX_CONSTEXPR bool	IsUtf16SurrogatePair(wchar_t first, wchar_t second)
{
	return (sizeof(wchar_t) == 2)
		&& ((first & 0xFC00) == 0xD800) && ((second & 0xFC00) == 0xDC00);
}
/**
 * @brief	サロゲートペアからコードポイントへ変換
*/
IUTEST_IPP_INLINE IUTEST_CXX_CONSTEXPR UInt32 CreateCodePointFromUtf16SurrogatePair(wchar_t first, wchar_t second)
{
	//const UInt32 mask = (1<<10) -1;	// 0x3FF
	return (sizeof(wchar_t)==2) ?
		(((first & 0x3FF) << 10) | (second & 0x3FF)) + 0x10000 :
		static_cast<UInt32>(first);	// こっちは未対応
}
/**
 * @brief	下位から指定ビット数のビットを取得してシフトする
*/
IUTEST_IPP_INLINE UInt32 ChopLowBits(UInt32* bits, int n)
{
	const UInt32 lowbits = *bits & ((static_cast<UInt32>(1) << n) - 1);
	*bits >>= n;
	return lowbits;
}

/**
* @brief	コードポイントからUTF8へ変換
* @param [in]	code_point	= コードポイント
* @param [out]	buf			= 出力バッファ(32バイト以上を要求)
* @return	出力バッファ
*/
IUTEST_IPP_INLINE char* CodePointToUtf8(UInt32 code_point, char* buf)
{
	if( code_point <= kMaxCodePoint1 )
	{
		buf[1] = '\0';
		buf[0] = static_cast<char>(code_point);
	}
	else if( code_point <= kMaxCodePoint2 )
	{
		buf[2] = '\0';
		buf[1] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));
		buf[0] = static_cast<char>(0xC0 | code_point);
	}
	else if( code_point <= kMaxCodePoint3 )
	{
		buf[3] = '\0';
		buf[2] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));
		buf[1] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));
		buf[0] = static_cast<char>(0xE0 | code_point);
	}
	else if( code_point <= kMaxCodePoint4 )
	{
		buf[4] = '\0';
		buf[3] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));
		buf[2] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));
		buf[1] = static_cast<char>(0x80 | ChopLowBits(&code_point, 6));
		buf[0] = static_cast<char>(0xF0 | code_point);
	}
	else
	{
		sprintf(buf, "(Invalid UTF16 0x%X)", code_point);
	}
	return buf;
}

IUTEST_IPP_INLINE ::std::string IUTEST_ATTRIBUTE_UNUSED_ WideStringToUTF8(const wchar_t* str, int num)
{
IUTEST_PRAGMA_CONSTEXPR_CALLED_AT_RUNTIME_WARN_DISABLE_BEGIN()
	if(num == -1)
	{
		num = static_cast<int>(wcslen(str));
	}
#if IUTEST_HAS_CXX_HDR_CODECVT && 0
	typedef ::std::codecvt_utf8<wchar_t> convert;
	::std::locale loc("japanese");
	const convert& conv = ::std::use_facet<convert>(loc);
	::std::mbstate_t state=0;
	const size_t utf8_length = num * 2 + 1;
	char* utf8 = new char[utf8_length];
	const wchar_t* src_next = NULL;
	char* utf8_next = NULL;
	convert::result res = conv.out(state, str, str + num, src_next, utf8, utf8 + utf8_length, utf8_next);
	::std::string ret;
	if( res == convert::ok )
	{
		ret = utf8;
	}
	delete[] utf8;
	return ret;
#else
	iuStringStream::type ss;
	for(int i=0; i < num; ++i )
	{
		UInt32 code_point;
		if( str[i] == L'\0' ) 
		{
			break;
		}
		else if( i + 1 < num && IsUtf16SurrogatePair(str[i], str[i+1]) )
		{
			code_point = CreateCodePointFromUtf16SurrogatePair(str[i], str[i+1]);
			++i;
		}
		else
		{
			code_point = static_cast<UInt32>(str[i]);
		}
		char buf[32];
		ss << CodePointToUtf8(code_point, buf);
	}
	return ss.str();
#endif
IUTEST_PRAGMA_CONSTEXPR_CALLED_AT_RUNTIME_WARN_DISABLE_END()
}

IUTEST_IPP_INLINE::std::string IUTEST_ATTRIBUTE_UNUSED_ WideStringToMultiByteString(const wchar_t* str, int num)
{
	IUTEST_UNUSED_VAR(num);
#if defined(IUTEST_OS_WINDOWS) && IUTEST_MBS_CODE == IUTEST_MBS_CODE_WINDOWS31J
	return win::WideStringToMultiByteString(str);
#else
	const size_t length = wcslen(str) * MB_CUR_MAX + 1;
	char* mbs = new char [length];
IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_BEGIN()
	if( wcstombs(mbs, str, length) == (size_t)-1 )
	{
		delete [] mbs;
		return "(convert error)";
	}
IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_END()
	::std::string ret = mbs;
	delete [] mbs;
	return ret;
#endif
}

IUTEST_IPP_INLINE ::std::string IUTEST_ATTRIBUTE_UNUSED_ MultiByteStringToUTF8(const char* src, int num)
{
#if (defined(__STDC_ISO_10646__) || defined(_MSC_VER)) && !defined(IUTEST_OS_WINDOWS_MOBILE)
	if( num == -1 )
	{
		num = static_cast<int>(strlen(src));
	}
	std::string str;
	const char* p = src;
	//char* locale = setlocale(LC_CTYPE, "JPN");
	for(const char* end = src + num; p < end; )
	{
		wchar_t wc=0;
		const int len = iu_mbtowc(&wc, p, MB_CUR_MAX);
		if( len > 1 )
		{
			str += WideStringToUTF8(&wc, 1);
			p += len;
		}
		else
		{
			str += *p;
			++p;
		}
	}
	//setlocale(LC_CTYPE, locale);
	return str;
#else
	IUTEST_UNUSED_VAR(num);
	return src;
#endif
}

#if IUTEST_HAS_CXX_HDR_CODECVT

#if IUTEST_HAS_CHAR16_T
IUTEST_IPP_INLINE ::std::string UTF16ToUTF8(const char16_t* str, int num)
#else
IUTEST_IPP_INLINE ::std::string UTF16ToUTF8(const wchar_t* str, int num)
#endif
{
	if(num == -1)
	{
		num = static_cast<int>(wcslen(str));
	}
#if IUTEST_HAS_CHAR16_T
	typedef ::std::codecvt_utf8<char16_t> convert;
#else
	typedef ::std::codecvt_utf8<wchar_t> convert;
#endif
	::std::locale loc("japanese");
	const convert& conv = ::std::use_facet<convert>(loc);
	::std::mbstate_t state=0;
	const size_t utf8_length = num * 2 + 1;
	char* utf8 = new char[utf8_length];
	const convert::_Elem* src_next = NULL;
	char* utf8_next = NULL;
	convert::result res = conv.out(state, str, str + num, src_next, utf8, utf8 + utf8_length, utf8_next);
	::std::string ret;
	if(res == convert::ok)
	{
		ret = utf8;
	}
	delete[] utf8;
	return ret;
}

#endif

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_END()

}	// end of namespace detail
}	// end of namespace iutest

#endif	// INCG_IRIS_IUTEST_CHARCODE_IPP_D444FB3E_3AFA_46D0_AD69_33FAAF5615E3_
