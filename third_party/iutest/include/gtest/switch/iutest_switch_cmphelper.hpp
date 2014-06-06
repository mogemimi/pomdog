//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_switch_cmphelper.hpp
 * @brief		gtest 用 比較ヘルパー関数 ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2012-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_SWITCH_CMPHELPER_HPP_0378DF29_2DE7_4C60_8E12_32FEA8BEAA7C_
#define INCG_IRIS_IUTEST_SWITCH_CMPHELPER_HPP_0378DF29_2DE7_4C60_8E12_32FEA8BEAA7C_

#if !defined(IUTEST_USE_GTEST)

//======================================================================
// define

#else // !defined(IUTEST_USE_GTEST)

namespace testing
{

namespace internal
{

#if GTEST_MINORVER < 0x06
template<bool> struct EnableIf;
template<> struct EnableIf<true> { typedef void type; };
#endif

//======================================================================
// function
template<typename T1, typename T2>
inline AssertionResult	CmpHelperSame(const char* expected_str, const char* actual_str
	, const T1& expected, const T2& actual)
{
	return EqHelper<false>::Compare(expected_str, actual_str, &expected, &actual);
}

template<typename Elem, typename Traits, typename Ax>
inline AssertionResult IUTEST_ATTRIBUTE_UNUSED_ CmpHelperSTREQ(const char* expr1, const char* expr2
	, const ::std::basic_string<Elem, Traits, Ax>& val1
	, const ::std::basic_string<Elem, Traits, Ax>& val2)
{
	return CmpHelperSTREQ(expr1, expr2, val1.c_str(), val2.c_str());
}
template<typename Elem, typename Traits, typename Ax>
inline AssertionResult IUTEST_ATTRIBUTE_UNUSED_ CmpHelperSTREQ(const char* expr1, const char* expr2
	, const Elem* val1
	, const ::std::basic_string<Elem, Traits, Ax>& val2)
{
	return CmpHelperSTREQ(expr1, expr2, val1, val2.c_str());
}
template<typename Elem, typename Traits, typename Ax>
inline AssertionResult IUTEST_ATTRIBUTE_UNUSED_ CmpHelperSTREQ(const char* expr1, const char* expr2
	, const ::std::basic_string<Elem, Traits, Ax>& val1
	, const Elem* val2)
{
	return CmpHelperSTREQ(expr1, expr2, val1.c_str(), val2);
}
template<typename Elem, typename Traits, typename Ax>
inline AssertionResult IUTEST_ATTRIBUTE_UNUSED_ CmpHelperSTRNE(const char* expr1, const char* expr2
	, const ::std::basic_string<Elem, Traits, Ax>& val1
	, const ::std::basic_string<Elem, Traits, Ax>& val2)
{
	return CmpHelperSTRNE(expr1, expr2, val1.c_str(), val2.c_str());
}
template<typename Elem, typename Traits, typename Ax>
inline AssertionResult IUTEST_ATTRIBUTE_UNUSED_ CmpHelperSTRNE(const char* expr1, const char* expr2
	, const Elem* val1
	, const ::std::basic_string<Elem, Traits, Ax>& val2)
{
	return CmpHelperSTRNE(expr1, expr2, val1, val2.c_str());
}
template<typename Elem, typename Traits, typename Ax>
inline AssertionResult IUTEST_ATTRIBUTE_UNUSED_ CmpHelperSTRNE(const char* expr1, const char* expr2
	, const ::std::basic_string<Elem, Traits, Ax>& val1
	, const Elem* val2)
{
	return CmpHelperSTRNE(expr1, expr2, val1.c_str(), val2);
}

inline AssertionResult IUTEST_ATTRIBUTE_UNUSED_ CmpHelperSTRCASEEQ(const char* expr1, const char* expr2
	, const wchar_t* val1, const wchar_t* val2)
{
	if( val1 == NULL || val2 == NULL )
	{
		if( val1 == val2 ) return AssertionSuccess();
	}
	else
	{
		if( iu_wcsicmp(val1, val2) == 0 ) return AssertionSuccess();
	}
	return EqFailure(expr1, expr2
		, FormatForComparisonFailureMessage(val1, val2).c_str()
		, FormatForComparisonFailureMessage(val2, val1).c_str()
		, true);
}
template<typename Elem, typename Traits, typename Ax>
inline AssertionResult IUTEST_ATTRIBUTE_UNUSED_ CmpHelperSTRCASEEQ(const char* expr1, const char* expr2
	, const ::std::basic_string<Elem, Traits, Ax>& val1
	, const ::std::basic_string<Elem, Traits, Ax>& val2)
{
	return CmpHelperSTRCASEEQ(expr1, expr2, val1.c_str(), val2.c_str());
}
template<typename Elem, typename Traits, typename Ax>
inline AssertionResult IUTEST_ATTRIBUTE_UNUSED_ CmpHelperSTRCASEEQ(const char* expr1, const char* expr2
	, const Elem* val1
	, const ::std::basic_string<Elem, Traits, Ax>& val2)
{
	return CmpHelperSTRCASEEQ(expr1, expr2, val1, val2.c_str());
}
template<typename Elem, typename Traits, typename Ax>
inline AssertionResult IUTEST_ATTRIBUTE_UNUSED_ CmpHelperSTRCASEEQ(const char* expr1, const char* expr2
	, const ::std::basic_string<Elem, Traits, Ax>& val1
	, const Elem* val2)
{
	return CmpHelperSTRCASEEQ(expr1, expr2, val1.c_str(), val2);
}

inline AssertionResult IUTEST_ATTRIBUTE_UNUSED_ CmpHelperSTRCASENE(const char* expr1, const char* expr2
	, const wchar_t* val1, const wchar_t* val2)
{
	if( val1 == NULL || val2 == NULL )
	{
		if( val1 != val2 ) return AssertionSuccess();
	}
	else
	{
		if( iu_wcsicmp(val1, val2) != 0 ) return AssertionSuccess();
	}
	return AssertionFailure() << "error: Value of: " << expr1 << " != " << expr2
		<< "\n  Actual: " << FormatForComparisonFailureMessage(val2, val1) << "\nExpected: " << FormatForComparisonFailureMessage(val1, val2) ;
}

template<typename Elem, typename Traits, typename Ax>
inline AssertionResult IUTEST_ATTRIBUTE_UNUSED_ CmpHelperSTRCASENE(const char* expr1, const char* expr2
	, const ::std::basic_string<Elem, Traits, Ax>& val1
	, const ::std::basic_string<Elem, Traits, Ax>& val2)
{
	return CmpHelperSTRCASENE(expr1, expr2, val1.c_str(), val2.c_str());
}
template<typename Elem, typename Traits, typename Ax>
inline AssertionResult IUTEST_ATTRIBUTE_UNUSED_ CmpHelperSTRCASENE(const char* expr1, const char* expr2
	, const Elem* val1
	, const ::std::basic_string<Elem, Traits, Ax>& val2)
{
	return CmpHelperSTRCASENE(expr1, expr2, val1, val2.c_str());
}
template<typename Elem, typename Traits, typename Ax>
inline AssertionResult IUTEST_ATTRIBUTE_UNUSED_ CmpHelperSTRCASENE(const char* expr1, const char* expr2
	, const ::std::basic_string<Elem, Traits, Ax>& val1
	, const Elem* val2)
{
	return CmpHelperSTRCASENE(expr1, expr2, val1.c_str(), val2);
}

template <bool lhs_is_null_literal>
class NeHelper {
public:
	template <typename T1, typename T2>
	static AssertionResult Compare(const char* expected_expression,
		const char* actual_expression,
		const T1& expected,
		const T2& actual) {
			return CmpHelperNE(expected_expression, actual_expression, expected,
				actual);
	}

	static AssertionResult Compare(const char* expected_expression,
		const char* actual_expression,
		BiggestInt expected,
		BiggestInt actual) {
			return CmpHelperNE(expected_expression, actual_expression, expected,
				actual);
	}
};

template <>
class NeHelper<true> {
public:
	template <typename T1, typename T2>
	static AssertionResult Compare(
		const char* expected_expression,
		const char* actual_expression,
		const T1& expected,
		const T2& actual,
		typename EnableIf<!is_pointer<T2>::value>::type* = 0) {
			return CmpHelperNE(expected_expression, actual_expression, expected,
				actual);
	}

	template <typename T>
	static AssertionResult Compare(
		const char* expected_expression,
		const char* actual_expression,
		Secret* /* expected (NULL) */,
		T* actual) {
			return CmpHelperNE(expected_expression, actual_expression,
				static_cast<T*>(NULL), actual);
	}
};

}	// end of namespace internal
}	// end of namespace testing

#endif // !defined(IUTEST_USE_GTEST)

#endif // INCG_IRIS_IUTEST_SWITCH_CMPHELPER_HPP_0378DF29_2DE7_4C60_8E12_32FEA8BEAA7C_
