//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_printers.hpp
 * @brief		iris unit test print 出力ヘルパー ファイル
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
#ifndef INCG_IRIS_IUTEST_PRINTERS_HPP_A6A321C9_9279_4336_8167_058C59EC0FD0_
#define INCG_IRIS_IUTEST_PRINTERS_HPP_A6A321C9_9279_4336_8167_058C59EC0FD0_

//======================================================================
// include
#include "iutest_defs.hpp"
#include "internal/iutest_string.hpp"

namespace iutest
{

//======================================================================
// declare
template<typename T>
std::string PrintToString(const T& v);

namespace detail
{

inline void PrintBytesInObjectTo(const unsigned char* buf, size_t size, iu_ostream* os)
{
IUTEST_PRAGMA_CONSTEXPR_CALLED_AT_RUNTIME_WARN_DISABLE_BEGIN()
	const size_t kMaxCount = detail::kValues::MaxPrintContainerCount;
	*os << size << "-Byte object < ";
	for( size_t i=0; i < size; ++i )
	{
		*os << detail::ToHex(buf[i]>>4) << ToHex(buf[i]&0xF) << " ";
		if( i == kMaxCount )
		{
			*os << "... ";
			break;
		}
	}
	*os << ">";
IUTEST_PRAGMA_CONSTEXPR_CALLED_AT_RUNTIME_WARN_DISABLE_END()
}

namespace printer_internal
{

namespace formatter
{

/** @private */
template<bool convertible>
struct Printer
{
	template<typename T>
	static void Print(const T& value, iu_ostream* os)
	{
		const unsigned char* ptr = reinterpret_cast<const unsigned char*>(&value);
		const size_t size = sizeof(T);
		PrintBytesInObjectTo(ptr, size, os);
	}
};

template<>
struct Printer<true>
{
	template<typename T>
	static void Print(const T& value, iu_ostream* os)
	{
#if defined(_STLPORT_VERSION) && !defined(_STLP_LONG_LONG)
		const Int32 v = value;
#else
		const BiggestInt v = value;
#endif
		*os << v;
	}
};

}

/** @private */
template<typename T>
class TypeWithoutFormatter
{
public:
	static void PrintValue(const T& value, iu_ostream* os)
	{
		formatter::Printer<iutest_type_traits::is_convertible<const T&, BiggestInt>::value>::Print(value, os);
	}
};

#if IUTEST_HAS_STRINGSTREAM || IUTEST_HAS_STRSTREAM
template<typename Elem, typename Traits, typename T>
::std::basic_ostream<Elem, Traits>& operator << (::std::basic_ostream<Elem, Traits>& os, const T& value)
{
	TypeWithoutFormatter<T>::PrintValue(value, &os);
	return os;
}
#else
template<typename T>
iu_ostream& operator << (iu_ostream& os, const T& value)
{
	TypeWithoutFormatter<T>::PrintValue(value, &os);
	return os;
}
#endif

}	// end of printer_internal

namespace printer_internal2
{

// 解決順序
// foo::operator <<
// ::operator <<
// ::iutest::detail::printer_internal::operator <<
template<typename T>
void DefaultPrintNonContainerTo(const T& value, iu_ostream* os)
{
	using namespace ::iutest::detail::printer_internal;
	*os << value;
}

}	// end of printer_internal2

//======================================================================
// declare
template<typename T>
inline void UniversalPrintTo(T value, iu_ostream* os);

//======================================================================
// class
/** @private */
template<typename T>
class iuUniversalPrinter
{
public:
	static void Print(const T& value, iu_ostream* os)
	{
		UniversalPrintTo(value, os);
	}
};

//======================================================================
// function
/** @private */
template<typename T>
inline void	UniversalPrint(const T& value, iu_ostream* os)
{
	iuUniversalPrinter<T>::Print(value, os);
}

/**
 * @brief	デフォルト文字列変換関数
*/
template<typename T>
inline void DefaultPrintTo(IsContainerHelper::yes_t
						   , iutest_type_traits::false_type
						   , const T& container, iu_ostream* os)
{
	const size_t kMaxCount = kValues::MaxPrintContainerCount;
	size_t count = 0;
	*os << "{";
	for( typename T::const_iterator it=container.begin(), end=container.end(); it != end; ++it, ++count)
	{
		if( count > 0 )
		{
			*os << ",";
			if( count == kMaxCount )
			{
				*os << "...";
				break;
			}
		}
		*os << " ";
		UniversalPrint(*it, os);
	}
	if( count > 0 )
	{
		*os << " ";
	}
	*os << "}";
}
template<typename T>
inline void DefaultPrintTo(IsContainerHelper::no_t
						   , iutest_type_traits::false_type
						   , const T& value, iu_ostream* os)
{
	printer_internal2::DefaultPrintNonContainerTo(value, os);
}

#if !defined(IUTEST_NO_SFINAE) && !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template<typename T>
inline void DefaultPtrPrintTo(T* ptr, iu_ostream* os, typename enable_if_t< is_convertible<T*, const void*> >::type*& = enabler::value)
{
	*os << ptr;
}
template<typename T>
inline void DefaultPtrPrintTo(T* ptr, iu_ostream* os, typename disable_if_t< is_convertible<T*, const void*> >::type*& = enabler::value)
{
	*os << reinterpret_cast<const void*>(reinterpret_cast<type_least_t<8>::UInt>(ptr));
}

#else

template<typename T>
inline void DefaultPtrPrintTo(T* ptr, iu_ostream* os)
{
	*os << reinterpret_cast<const void*>(reinterpret_cast<type_least_t<8>::UInt>(ptr));
}

#endif

template<typename T>
inline void DefaultPrintTo(IsContainerHelper::no_t
						   , iutest_type_traits::true_type
						   , T* ptr, iu_ostream* os)
{
	if( ptr == NULL )
	{
		*os << kStrings::Null;
	}
	else
	{
		DefaultPtrPrintTo<T>(ptr, os);
	}
}

/**
 * @brief	文字列変換関数
*/
template<typename T>
inline void PrintTo(const T& value, iu_ostream* os)	{
	DefaultPrintTo(
#if !defined(IUTEST_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS)
		IsContainerHelper::IsContainer<T>(0)
#else
		IsContainerHelper::IsContainer(0, &detail::type<T>())
#endif
		, iutest_type_traits::is_pointer<T>(), value, os);
}
inline void PrintTo(bool b, iu_ostream* os)			{ *os << (b ? "true" : "false"); }
inline void PrintTo(const char* c, iu_ostream* os)	{ *os << c; }
inline void PrintTo(char* c, iu_ostream* os)		{ *os << c; }
inline void PrintTo(const ::std::string& str, iu_ostream* os)	{ *os << str.c_str(); }
template<typename T>
inline void PrintTo(const floating_point<T>& f, iu_ostream* os)	{ *os << f.raw() << "(0x" << ToHexString(f.bit()) << ")"; }
template<typename T1, typename T2>
inline void PrintTo(const ::std::pair<T1, T2>& value, iu_ostream* os)
{
	*os << "(";
	iuUniversalPrinter<T1>::Print(value.first, os);
	*os << ", ";
	iuUniversalPrinter<T2>::Print(value.second, os);
	*os << ")";
}
// char or unsigned char の時に、 0 が NULL 文字にならないように修正
inline void PrintTo(const char value, iu_ostream* os)
{
	if( value == 0 )
	{
		*os << "\\0";
	}
	else
	{
		*os << "\'" << value << "\'";
	}
}
inline void PrintTo(const wchar_t value, iu_ostream* os)
{
	if( value == 0 )
	{
		*os << "\\0";
	}
	else
	{
		*os << "\'" << value << "\'";
	}
}
inline void PrintTo(const unsigned char value, iu_ostream* os)
{
	*os << static_cast<unsigned int>(value);
}

#if IUTEST_HAS_NULLPTR
inline void PrintTo(const ::std::nullptr_t&, iu_ostream* os) { *os << "nullptr"; }
#endif

#if IUTEST_HAS_TUPLE

template<typename T, int I, int SIZE>
inline void PrintTupleElemTo(const T& t, iu_ostream* os, typename detail::enable_if<I == 0, void>::type*& = detail::enabler::value)
{
	IUTEST_UNUSED_VAR(t);
	IUTEST_UNUSED_VAR(os);
}
template<typename T, int I, int SIZE>
inline void PrintTupleElemTo(const T& t, iu_ostream* os, typename detail::enable_if<I == 1, void>::type*& = detail::enabler::value)
{
	PrintTo(tuples::get<SIZE-I>(t), os);
}
template<typename T, int I, int SIZE>
inline void PrintTupleElemTo(const T& t, iu_ostream* os, typename detail::enable_if<(I&(~1)) != 0, void>::type*& = detail::enabler::value)
{
	PrintTo(tuples::get<SIZE-I>(t), os);
	*os << ", ";
	PrintTupleElemTo<T, I-1, SIZE>(t, os);
}

template<typename T>
inline void PrintTupleTo(const T& t, iu_ostream* os)
{
	*os << "(";
	PrintTupleElemTo<T, tuples::tuple_size<T>::value, tuples::tuple_size<T>::value>(t, os);
	*os << ")";
}

#if IUTEST_HAS_VARIADIC_TEMPLATES && IUTEST_HAS_TUPLE

template<typename ...Args>
inline void PrintTo(const tuples::tuple<Args...>& t, iu_ostream* os)
{
	PrintTupleTo(t, os);
}

#else

inline void PrintTo(const tuples::tuple<>& t, iu_ostream* os) { PrintTupleTo(t, os); }
template<typename A1>
inline void PrintTo(const tuples::tuple<A1>& t, iu_ostream* os) { PrintTupleTo(t, os); }
template<typename A1, typename A2>
inline void PrintTo(const tuples::tuple<A1, A2>& t, iu_ostream* os) { PrintTupleTo(t, os); }
template<typename A1, typename A2, typename A3>
inline void PrintTo(const tuples::tuple<A1, A2, A3>& t, iu_ostream* os) { PrintTupleTo(t, os); }
template<typename A1, typename A2, typename A3, typename A4>
inline void PrintTo(const tuples::tuple<A1, A2, A3, A4>& t, iu_ostream* os) { PrintTupleTo(t, os); }
template<typename A1, typename A2, typename A3, typename A4, typename A5>
inline void PrintTo(const tuples::tuple<A1, A2, A3, A4, A5>& t, iu_ostream* os) { PrintTupleTo(t, os); }
template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
inline void PrintTo(const tuples::tuple<A1, A2, A3, A4, A5, A6>& t, iu_ostream* os) { PrintTupleTo(t, os); }
template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
inline void PrintTo(const tuples::tuple<A1, A2, A3, A4, A5, A6, A7>& t, iu_ostream* os) { PrintTupleTo(t, os); }
template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
inline void PrintTo(const tuples::tuple<A1, A2, A3, A4, A5, A6, A7, A8>& t, iu_ostream* os) { PrintTupleTo(t, os); }
template<typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
inline void PrintTo(const tuples::tuple<A1, A2, A3, A4, A5, A6, A7, A8, A9>& t, iu_ostream* os) { PrintTupleTo(t, os); }

#endif

#endif

//======================================================================
// function

/** @private */
template<typename T>
inline void	IUTEST_ATTRIBUTE_UNUSED_ UniversalTersePrint(const T& value, iu_ostream* os)
{
	UniversalPrint(value, os);
}

inline void IUTEST_ATTRIBUTE_UNUSED_ UniversalTersePrint(const char* str, iu_ostream* os)
{
	if( str == NULL )
	{
		*os << kStrings::Null;
	}
	else
	{
		UniversalPrint(::std::string(str), os);
	}
}
inline void IUTEST_ATTRIBUTE_UNUSED_ UniversalTersePrint(char* str, iu_ostream* os)
{
	UniversalTersePrint(static_cast<const char*>(str), os);
}
inline void IUTEST_ATTRIBUTE_UNUSED_ UniversalTersePrint(const wchar_t* str, iu_ostream* os)
{
	if( str == NULL )
	{
		*os << kStrings::Null;
	}
	else
	{
		UniversalPrint(detail::ShowWideCString(str), os);
	}
}
inline void IUTEST_ATTRIBUTE_UNUSED_ UniversalTersePrint(wchar_t* str, iu_ostream* os)
{
	UniversalTersePrint(static_cast<const wchar_t*>(str), os);
}
#if IUTEST_HAS_CHAR16_T
inline void IUTEST_ATTRIBUTE_UNUSED_ UniversalTersePrint(const char16_t* str, iu_ostream* os)
{
	if( str == NULL )
	{
		*os << kStrings::Null;
	}
	else
	{
		UniversalPrint(detail::ShowWideCString(str), os);
	}
}
inline void IUTEST_ATTRIBUTE_UNUSED_ UniversalTersePrint(char16_t* str, iu_ostream* os)
{
	UniversalTersePrint(static_cast<const char16_t*>(str), os);
}
#endif

/**
 * @brief	配列の出力
*/
template<typename T>
inline void PrintRawArrayTo(const T* a, size_t cnt, iu_ostream* os)
{
	UniversalPrint<T>(a[0], os);
	for( size_t i=1; i < cnt; ++i )
	{
		*os << ", ";
		UniversalPrint<T>(a[i], os);
	}
}

/**
 * @brief	配列の出力
*/
template<typename T>
inline void IUTEST_ATTRIBUTE_UNUSED_ UniversalPrintArray(const T* begin, size_t N, iu_ostream* os)
{
	if( N == 0 )
	{
		*os << "{}";
	}
	else
	{
		*os << "{";
		const size_t kThreshold = kValues::PrintArrayThreshold;
		const size_t kChunksize = kValues::PrintArrayChunksize;
		if( N <= kThreshold )
		{
			PrintRawArrayTo(begin, N, os);
		}
		else
		{
			PrintRawArrayTo(begin, kChunksize, os);
			*os << ", ..., ";
			PrintRawArrayTo(begin + N - kChunksize, kChunksize, os);
		}
		*os << "}";
	}
}
/**
 * @brief	配列の出力
*/
inline void IUTEST_ATTRIBUTE_UNUSED_ UniversalPrintArray(const char* begin, size_t N, iu_ostream* os)
{
	IUTEST_UNUSED_VAR(N);
	UniversalTersePrint(begin, os);
}
inline void IUTEST_ATTRIBUTE_UNUSED_ UniversalPrintArray(const wchar_t* begin, size_t N, iu_ostream* os)
{
	IUTEST_UNUSED_VAR(N);
	UniversalTersePrint(begin, os);
}

//======================================================================
// class

/** @private */
template<typename T>
inline void IUTEST_ATTRIBUTE_UNUSED_ UniversalPrintTo(T value, iu_ostream* os) { PrintTo(value, os); }

#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

/** @private */
template<typename T, size_t SIZE>
class iuUniversalPrinter<T[SIZE]>
{
public:
	static void Print(const T (&a)[SIZE], iu_ostream* os)
	{
		UniversalPrintArray(a, SIZE, os);
	}
};

#endif

/**
 * @}
*/

}	// end of namespace detail

//======================================================================
// function

/**
 * @brief	文字列化
*/
template<typename T>
inline ::std::string PrintToString(const T& v)
{
	detail::iuStringStream::type strm;
#if !defined(IUTEST_NO_FUNCTION_TEMPLATE_ORDERING)
	detail::UniversalTersePrint(v, &strm);
#else
	strm << v;
#endif
	return strm.str();
}

#if IUTEST_HAS_VARIADIC_TEMPLATES
/**
 * @brief	文字列化
*/
template<typename T>
inline ::std::string PrintToStrings(const char* separate, const T& v)
{
	IUTEST_UNUSED_VAR(separate);
	return PrintToString(v);
}
/**
 * @brief	文字列化
*/
template<typename T, typename ...Args>
inline ::std::string PrintToStrings(const char* separate, const T& v, Args... args)
{
	::std::string str = PrintToString(v);
	str += separate;
	str += PrintToStrings(separate, args...);
	return str;
}
#endif

}	// end of namespace iutest

#endif	// INCG_IRIS_IUTEST_PRINTERS_HPP_A6A321C9_9279_4336_8167_058C59EC0FD0_
