//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_string.hpp
 * @brief		iris unit test 文字列操作 ファイル
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
#ifndef INCG_IRIS_IUTEST_STRING_HPP_E22B02D7_E9E7_412C_B609_DC3D9C66895D_
#define INCG_IRIS_IUTEST_STRING_HPP_E22B02D7_E9E7_412C_B609_DC3D9C66895D_

//======================================================================
// include
#if defined(__MWERKS__)
#  define _MBSTATE_T
#endif

#include <string>
#include <wchar.h>
#include <wctype.h>
#include <stdarg.h>
#include <string.h>
#if defined(IUTEST_OS_CYGWIN)
#include <strings.h>
#endif
#if		IUTEST_HAS_STRINGSTREAM
#  include <sstream>
#elif	IUTEST_HAS_STRSTREAM
#  include <strstream>
#endif

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_BEGIN()

namespace iutest {
namespace detail
{

//======================================================================
// function
/*
 * 今のところ使っていない

static char*			xcsstr(char* src, const char* substr)				{ return strstr(src, substr); }
static const char*		xcsstr(const char* src, const char* substr)			{ return strstr(src, substr); }
static char*			xcschr(char* src, char ch)							{ return strchr(src, ch); }
static const char*		xcschr(const char* src, char ch)					{ return strchr(src, ch); }
static int				xcscmp(const char* str1, const char* str2)			{ return strcmp(str1, str2); }
static long				xcstol(const char* src, char** end, int radix)		{ return strtol(src, end, radix); }

static wchar_t*			xcsstr(wchar_t* src, const wchar_t* substr)			{ return wcsstr(src, substr); }
static const wchar_t*	xcsstr(const wchar_t* src, const wchar_t* substr)	{ return wcsstr(src, substr); }
static wchar_t*			xcschr(wchar_t* src, wchar_t ch)					{ return wcschr(src, ch); }
static const wchar_t*	xcschr(const wchar_t* src, wchar_t ch)				{ return wcschr(src, ch); }
static int				xcscmp(const wchar_t* str1, const wchar_t* str2)	{ return wcscmp(str1, str2); }
static long				xcstol(const wchar_t* src, wchar_t** end, int radix){ return wcstol(src, end, radix); }

*/

/**
 * @internal
 * @brief	stricmp
*/
inline int iu_stricmp(const char* str1, const char* str2)
{
#if	defined(IUTEST_OS_WINDOWS)

#if	defined(__BORLANDC__)
	return stricmp(str1, str2);
#else
	return _stricmp(str1, str2);
#endif

#elif !defined(__MWERKS__)
	return strcasecmp(str1, str2);

#else
	const char* l = str1;
	const char* r = str2;
	while(*l)
	{
		int ul = toupper(*l);
		int ur = toupper(*r);
		if( ul < ur ) return -1;
		if( ul > ur ) return 1;
		++l;
		++r;
	}
	if( *l < *r ) return -1;
	if( *l > *r ) return 1;
	return 0;
#endif
}

/**
 * @internal
 * @brief	wcsicmp
*/
inline int iu_wcsicmp(const wchar_t * str1, const wchar_t * str2)
{
#if		defined(_MSC_VER)
	return _wcsicmp(str1, str2);
#elif	defined(IUTEST_OS_LINUX) && !defined(IUTEST_OS_LINUX_ANDROID)
	return wcscasecmp(str1, str2);
#else
	const wchar_t* l = str1;
	const wchar_t* r = str2;
	while(*l)
	{
		wchar_t ul = towupper(*l);
		wchar_t ur = towupper(*r);
		if( ul < ur ) return -1;
		if( ul > ur ) return 1;
		++l;
		++r;
	}
	if( *l < *r ) return -1;
	if( *l > *r ) return 1;
	return 0;
#endif
}

inline bool IsEmpty(const char* p) { return p == NULL || *p == '\0'; }
inline IUTEST_CXX_CONSTEXPR bool IsSpace(char ch)	{ return ch == ' ' || ch =='\t'; }
inline IUTEST_CXX_CONSTEXPR const char*	SkipSpace(const char* p)
{
IUTEST_PRAGMA_CONSTEXPR_CALLED_AT_RUNTIME_WARN_DISABLE_BEGIN()
	return p == NULL ? NULL : (IsSpace(*p) ? SkipSpace(++p) : p);
IUTEST_PRAGMA_CONSTEXPR_CALLED_AT_RUNTIME_WARN_DISABLE_END()
}
inline IUTEST_CXX_CONSTEXPR const char*	FindComma(const char* p)
{
	return (p == NULL || *p == '\0') ? NULL : ((*p == ',') ? p : FindComma(++p));
}
inline bool IsStringEqual(const char* str1, const char* str2) { return strcmp(str1, str2) == 0; }
inline bool IsStringCaseEqual(const char* str1, const char* str2) { return iu_stricmp(str1, str2) == 0; }
inline bool IsStringForwardMatching(const char* str1, const char* str2) { return strstr(str1, str2) == str1; }

inline void StringReplace(::std::string& str, char a, const char* to)
{
	::std::string::size_type pos = 0;
	while( pos = str.find(a, pos), pos != ::std::string::npos )
	{
		str.replace(pos, 1, to);
		++pos;
	}
}

inline IUTEST_CXX_CONSTEXPR char ToHex(unsigned int n) { return (n&0xF) >= 0xA ? 'A'+((n&0xF)-0xA) : '0'+(n&0xF); }

template<typename T>
inline ::std::string ToHexString(T value)
{
IUTEST_PRAGMA_CONSTEXPR_CALLED_AT_RUNTIME_WARN_DISABLE_BEGIN()
	const size_t len = sizeof(T)*2;
	char buf[len+1];
	for( size_t i=0; i < len; ++i )
	{
		buf[i] = ToHex(static_cast<unsigned int>((value>>((len-i-1)*4))));
	}
	buf[len] = '\0';
	return buf;
IUTEST_PRAGMA_CONSTEXPR_CALLED_AT_RUNTIME_WARN_DISABLE_END()
}

inline ::std::string FormatIntWidth2(int value)
{
	char buf[3] = "00";
	buf[0] = (value/10)%10 + '0';
	buf[1] = (value   )%10 + '0';
	return buf;
}

inline ::std::string ShowStringQuoted(const char* str) { ::std::string s = "\""; s += str; s += "\""; return s; }
inline ::std::string ShowStringQuoted(const ::std::string& str) { ::std::string s = "\""; s += str; s += "\""; return s; }

//======================================================================
// declare
#if !IUTEST_HAS_STRINGSTREAM && !IUTEST_HAS_STRSTREAM
template<class _Elem, class _Traits>class iu_basic_stream;
#endif

//======================================================================
// class
class iuStringStream
{
public:
#if		IUTEST_HAS_STRINGSTREAM
	typedef ::std::stringstream	stlstream;
#elif	IUTEST_HAS_STRSTREAM
IUTEST_PRAGMA_MSC_WARN_PUSH()
IUTEST_PRAGMA_MSC_WARN_DISABLE(4250)
	class stlstream : public ::std::strstream
	{
		char buf[512];
	public:
		stlstream(void)
			: ::std::strstream(buf, sizeof(buf)-2, ::std::ios::out)
		{}
		stlstream(const char* str)
			: ::std::strstream(buf, sizeof(buf)-2, ::std::ios::out)
		{
			*this << str;
		}
		stlstream(const ::std::string& str)
			: ::std::strstream(buf, sizeof(buf)-2, ::std::ios::out)
		{
			*this << str;
		}
	public:
		::std::string str(void) const
		{
			return const_cast<stlstream*>(this)->str();
		}
		virtual ::std::string str(void)
		{
			*this << ::std::ends;
			::std::string str = ::std::strstream::str();
			return str;
		}
	};
IUTEST_PRAGMA_MSC_WARN_POP()
#else
	typedef iu_basic_stream<char, ::std::char_traits<char> >		iu_stream;
	typedef iu_basic_stream<wchar_t, ::std::char_traits<wchar_t> >	iu_wstream;
#endif

public:

#if IUTEST_HAS_STRINGSTREAM || IUTEST_HAS_STRSTREAM
	typedef stlstream	type;
#else
	typedef iu_stream	type;
#endif
};

#if !IUTEST_HAS_STRINGSTREAM && !IUTEST_HAS_STRSTREAM

template<class _Elem, class _Traits>
class iu_basic_stream
{
	typedef iu_basic_stream<_Elem, _Traits>			_Myt;
	//typedef ::std::basic_streambuf<_Elem, _Traits>	streambuf;
	//typedef ::std::basic_ostream<_Elem, _Traits>	ostream;
	typedef ::std::basic_string<_Elem, _Traits>		string;
	string s;

	template<typename T>
	struct xcs
	{
	private:
		template<typename TMP, typename TN>
		struct impl_select
		{
			template<typename TA, typename TB>
			static const TA	constant(const TA a, const TB b)
			{
				(void)b;
				return a;
			}
		};
		template<typename TMP>
		struct impl_select<TMP, wchar_t>
		{
			template<typename TA, typename TB>
			static const TB	constant(const TA a, const TB b)
			{
				(void)a;
				return b;
			}
		};

	public:
		typedef impl_select<void, T>	select;
	};
#define IUTEST_PP_XCS(txt_)	xcs<_Elem>::select::constant(txt_, L##txt_)

	struct impl
	{
		template<typename E>
		static int vastring(E* dst, const E* fmt, va_list va);
		static int vastring(char* dst, size_t len, const char* fmt, va_list va)
		{
			(void)len;
			return vsprintf(dst, fmt, va);
		}
		static int vastring(wchar_t* dst, size_t len, const wchar_t* fmt, va_list va)
		{
#ifdef IUTEST_OS_WINDOWS_MINGW
			return _vsnwprintf(dst, len, fmt, va);
#else
			return vswprintf(dst, len, fmt, va);
#endif
		}

		template<typename E>
		static int tostring(E* dst, size_t len, const E* fmt, ...)
		{
			va_list va;
			va_start(va, fmt);
			int ret = vastring(dst, len, fmt, va);
			va_end(va);
			return ret;
		}
	};
public:
	iu_basic_stream(void) {}
	explicit iu_basic_stream(const char* str) : s(str) {}
	explicit iu_basic_stream(const ::std::string& str) : s(str) {}
public:

	inline _Myt& operator<< (char v)
	{
		s += v;
		return *this;
	}
	inline _Myt& operator<< (signed char v)
	{
		s += (char)v;
		return *this;
	}
	inline _Myt& operator<< (unsigned char v)
	{
		s += (char)v;
		return *this;
	}
	inline _Myt& operator<< (const _Elem* v)
	{
		s += v;
		return *this;
	}
	//inline _Myt& operator<< (const signed _Elem* v)
	//{
	//	s += v;
	//	return *this;
	//}
	//inline _Myt& operator<< (const unsigned _Elem* v)
	//{
	//	s += v;
	//	return *this;
	//}
	inline _Myt& operator<< (bool v)
	{
#if 0
		_Elem a[16];
		impl::tostring(a, 16, IUTEST_PP_XCS("%i"), v);
		s += a;
#else
		s += (v ? IUTEST_PP_XCS("true") : IUTEST_PP_XCS("false"));
#endif
		return *this;
	}
	inline _Myt& operator<< (short v)
	{
		_Elem a[64];
		impl::tostring(a, 64, IUTEST_PP_XCS("%i"), v);
		s += a;
		return *this;
	}
	inline _Myt& operator<< (unsigned short v)
	{
		_Elem a[64];
		impl::tostring(a, 64, IUTEST_PP_XCS("%u"), v);
		s += a;
		return *this;
	}
	inline _Myt& operator<< (int v)
	{
		_Elem a[64];
		impl::tostring(a, 64, IUTEST_PP_XCS("%i"), v);
		s += a;
		return *this;
	}
	inline _Myt& operator<< (unsigned int v)
	{
		_Elem a[64];
		impl::tostring(a, 64, IUTEST_PP_XCS("%u"), v);
		s += a;
		return *this;
	}
	inline _Myt& operator<< (long v)
	{
		_Elem a[64];
		impl::tostring(a, 64, IUTEST_PP_XCS("%i"), v);
		s += a;
		return *this;
	}
	inline _Myt& operator<< (unsigned long v)
	{
		_Elem a[64];
		impl::tostring(a, 64, IUTEST_PP_XCS("%u"), v);
		s += a;
		return *this;
	}
	inline _Myt& operator<< (long long int v)
	{
		_Elem a[64];
		impl::tostring(a, 64, IUTEST_PP_XCS("%ll"), v);
		s += a;
		return *this;
	}
	inline _Myt& operator<< (unsigned long long int v)
	{
		_Elem a[64];
		impl::tostring(a, 64, IUTEST_PP_XCS("%llu"), v);
		s += a;
		return *this;
	}
	inline _Myt& operator<< (float v)
	{
		_Elem a[64];
		impl::tostring(a, 64, IUTEST_PP_XCS("%f"), v);
		s += a;
		return *this;
	}
	inline _Myt& operator<< (double v)
	{
		_Elem a[64];
		impl::tostring(a, 64, IUTEST_PP_XCS("%l"), v);
		s += a;
		return *this;
	}
	inline _Myt& operator<< (long double v)
	{
		_Elem a[64];
		impl::tostring(a, 64, IUTEST_PP_XCS("%L"), v);
		s += a;
		return *this;
	}
	inline _Myt& operator<< (const void* v)
	{
		_Elem a[64];
		impl::tostring(a, 64, IUTEST_PP_XCS("%t"), v);
		s += a;
		return *this;
	}
	inline _Myt& operator<< (const ::std::string& v)
	{
		s += v;
		return *this;
	}
public:
	const string& str(void) const { return s; }
};

#undef IUTEST_PP_XCS

#endif

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_END()

}	// end of namespace detail

#if IUTEST_HAS_STRINGSTREAM || IUTEST_HAS_STRSTREAM
typedef ::std::ostream					iu_ostream;
typedef std::ostream& (*iu_basic_iomanip)(std::ostream&);
#else
typedef detail::iuStringStream::type	iu_ostream;
#endif

}	// end of namespace iutest

#endif	// INCG_IRIS_IUTEST_STRING_HPP_E22B02D7_E9E7_412C_B609_DC3D9C66895D_
