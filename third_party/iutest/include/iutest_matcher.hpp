//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_matcher.hpp
 * @brief		iris unit test matcher 定義 ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_MATCHER_HPP_23746E00_1A4B_4778_91AD_45C6DEFEEFA7_
#define INCG_IRIS_IUTEST_MATCHER_HPP_23746E00_1A4B_4778_91AD_45C6DEFEEFA7_

//======================================================================
// include
#include "iutest_assertion.hpp"

//======================================================================
// define
/**
 * @private
 * @{
*/
#define IUTEST_TEST_THAT(actual, matcher, on_failure)					\
	IUTEST_AMBIGUOUS_ELSE_BLOCKER_										\
	if( ::iutest::AssertionResult iutest_ar = matcher(actual) ) {		\
	} else																\
		on_failure(::iutest::detail::MatcherAssertionFailureMessage(	\
			::iutest::PrintToString(actual).c_str(), #matcher, iutest_ar))

/**
 * @}
*/

namespace iutest {
namespace detail
{

//======================================================================
// function
/**
 * @brief	Matcher Assertion Failure Message
*/
inline ::std::string MatcherAssertionFailureMessage(const char* actual, const char* matcher_str, const AssertionResult& ar)
{
	detail::iuStringStream::type strm;
	strm << "error: Expected: " << matcher_str
		<< "\n  Actual: " << actual
		<< "\nWhich is: " << ar.message();
	return strm.str();
}
	
//======================================================================
// class
/**
 * @brief	StartsWith matcher
*/
template<typename T>
class StartsWithMatcher
{
public:
	StartsWithMatcher(T str) : m_str(str) {}

public:
	template<typename U>
	AssertionResult operator ()(const U& actual) const
	{
		if( StartsWith(actual, m_str) ) return AssertionSuccess();
		return AssertionFailure() << WitchIs();
	}

public:
	::std::string WitchIs(void) const
	{
		detail::iuStringStream::type strm;
		strm << "StartsWith: " << m_str;
		return strm.str();
	}
private:
	static bool StartsWith(const char* actual, const char* start)
	{
		return strstr(actual, start) == actual;
	}
	static bool StartsWith(const ::std::string& actual, const char* start)
	{
		const char* p = actual.c_str();
		return StartsWith(p, start);
	}
	static bool StartsWith(const char* actual, const ::std::string& start)
	{
		const char* p = start.c_str();
		return StartsWith(actual, p);
	}
	static bool StartsWith(const ::std::string& actual, const ::std::string& start)
	{
		const char* p = start.c_str();
		return StartsWith(actual, p);
	}
private:
	IUTEST_PP_DISALLOW_ASSIGN(StartsWithMatcher);

	T m_str;
};

/**
 * @brief	Contains matcher
*/
template<typename T>
class ContainsMatcher
{
public:
	ContainsMatcher(T expected) : m_expected(expected) {}

public:
	template<typename U>
	AssertionResult operator ()(const U& actual) const
	{
		if( Contains(actual, m_expected) ) return AssertionSuccess();
		return AssertionFailure() << WitchIs();
	}

public:
	::std::string WitchIs(void) const
	{
		detail::iuStringStream::type strm;
		strm << "Contains: " << m_expected;
		return strm.str();
	}
private:
	template<typename TT, typename Container>
	static bool Contains(const Container& actual, TT expected)
	{
		return Contains<typename Container::value_type>(actual.begin(), actual.end(), expected);
	}
#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
	template<typename TT, typename U, size_t SIZE>
	static bool Contains(const U(&actual)[SIZE], TT expected)
	{
		return Contains<U>(actual, actual+SIZE, expected);
	}
#endif

	template<typename TT, typename Ite>
	static bool Contains(Ite begin, Ite end, TT expected)
	{
		return ::std::find(begin, end, expected) != end;
	}

	static bool Contains(const char* actual, const char* expected)
	{
		return strstr(actual, expected) != NULL;
	}
	static bool Contains(const ::std::string& actual, const char* expected)
	{
		const char* p = actual.c_str();
		return Contains(p, expected);
	}
	static bool Contains(const char* actual, const ::std::string& expected)
	{
		const char* p = expected.c_str();
		return Contains(actual, p);
	}
	static bool Contains(const ::std::string& actual, const ::std::string& expected)
	{
		const char* p = expected.c_str();
		return Contains(actual, p);
	}

private:
	IUTEST_PP_DISALLOW_ASSIGN(ContainsMatcher);

	T m_expected;
};

/**
 * @brief	EndsWith matcher
*/
template<typename T>
class EndsWithMatcher
{
public:
	EndsWithMatcher(T str) : m_str(str) {}

public:
	template<typename U>
	AssertionResult operator ()(const U& actual) const
	{
		if( EndsWith(actual, m_str) ) return AssertionSuccess();
		return AssertionFailure() << WitchIs();
	}

public:
	::std::string WitchIs(void) const
	{
		detail::iuStringStream::type strm;
		strm << "EndsWith: " << m_str;
		return strm.str();
	}
private:
	static bool EndsWith(const char* actual, const char* end)
	{
		const size_t len = strlen(end);
		const size_t actual_len = strlen(actual);
		if( len > actual_len ) return false;
		const char* p = actual + actual_len - 1;
		const char* q = end + len - 1;
		for( size_t i=0; i < len; ++i, --p, --q )
		{
			if( *p != *q ) return false;
		}	
		return true;
	}
	static bool EndsWith(const ::std::string& actual, const char* end)
	{
		const char* p = actual.c_str();
		return EndsWith(p, end);
	}
	static bool EndsWith(const char* actual, const ::std::string& end)
	{
		const char* p = end.c_str();
		return EndsWith(actual, p);
	}
	static bool EndsWith(const ::std::string& actual, const ::std::string& end)
	{
		const char* p = end.c_str();
		return EndsWith(actual, p);
	}
private:
	IUTEST_PP_DISALLOW_ASSIGN(EndsWithMatcher);

	T m_str;
};

/**
 * @brief	Equals matcher
*/
template<typename T>
class EqualsMatcher
{
public:
	EqualsMatcher(T expected) : m_expected(expected) {}

public:
	template<typename U>
	AssertionResult operator ()(const U& actual) const
	{
		if( Equals(actual, m_expected) ) return AssertionSuccess();
		return AssertionFailure() << WitchIs();
	}

public:
	::std::string WitchIs(void) const
	{
		detail::iuStringStream::type strm;
		strm << "Equals: " << m_expected;
		return strm.str();
	}
private:
	template<typename A, typename B>
	static bool Equals(const A& actual, const B& expected)
	{
		return actual == expected;
	}
	static bool Equals(const char* actual, const char* expected)
	{
		return strcmp(actual, expected) == 0;
	}
	static bool Equals(const ::std::string& actual, const char* expected)
	{
		const char* p = actual.c_str();
		return Equals(p, expected);
	}
	static bool Equals(const ::std::string& actual, const ::std::string& expected)
	{
		const char* p = expected.c_str();
		return Equals(actual, p);
	}
private:
	IUTEST_PP_DISALLOW_ASSIGN(EqualsMatcher);

	T m_expected;
};

#if IUTEST_HAS_MATCHER_ALLOF_AND_ANYOF

/**
 * @brief	AllOf matcher base class
*/
class AllOfMatcherBase
{
protected:
	template<typename T, typename U>
	static AssertionResult Check(const T& matchers, const U& actual)
	{
		return Check<T, U, 0, tuples::tuple_size<T>::value>(matchers, actual);
	}
private:
	template<typename T, typename U, int N, int END>
	static AssertionResult Check(const T& matchers, const U& actual, typename detail::enable_if<N == END - 1, void>::type*& = detail::enabler::value)
	{
		AssertionResult ar = tuples::get<N>(matchers)(actual);
		if( ar )
		{
			return ar;
		}
		return AssertionFailure() << WitchIs<T, 0, N>(matchers);
	}
	template<typename T, typename U, int N, int END>
	static AssertionResult Check(const T& matchers, const U& actual, typename detail::disable_if<N == END - 1, void>::type*& = detail::enabler::value)
	{
		AssertionResult ar = tuples::get<N>(matchers)(actual);
		if( ar )
		{
			return Check<T, U, N+1, END>(matchers, actual);
		}
		return AssertionFailure() << WitchIs<T, 0, N>(matchers);
	}

	template<typename T, int N, int END>
	static ::std::string  WitchIs(const T& matchers, typename detail::enable_if<N == END, void>::type*& = detail::enabler::value)
	{
		return tuples::get<N>(matchers).WitchIs();
	}
	template<typename T, int N, int END>
	static ::std::string  WitchIs(const T& matchers, typename detail::disable_if<N == END, void>::type*& = detail::enabler::value)
	{
		return tuples::get<N>(matchers).WitchIs() + " and " + WitchIs<T, N+1, END>(matchers);
	}
};

#if IUTEST_HAS_VARIADIC_TEMPLATES

/**
 * @brief	AllOf matcher
*/
template<typename ...T>
class AllOfMatcher : public AllOfMatcherBase
{
public:
	AllOfMatcher(T... t) : m_matchers(t...) {}

public:
	template<typename U>
	AssertionResult operator ()(const U& actual) const
	{
		return Check(m_matchers, actual);
	}

private:
	tuples::tuple<T...> m_matchers;
};

#else

/*
template<typename T0, typename T1>
class AllOfMatcher : public AllOfMatcherBase
{
public:
	AllOfMatcher(T0 m0, T1 m1) : m_matchers(m0, m1) {}

public:
	template<typename U>
	AssertionResult operator ()(const U& actual) const
	{
		return Check(m_matchers, actual);
	}
private:
	IUTEST_PP_DISALLOW_ASSIGN(AllOfMatcher);

	tuples::tuple<T0, T1> m_matchers;
};
*/

#define IIUT_DECL_ALLOF_MATCHER(n)													\
	template< IUTEST_PP_ENUM_PARAMS(n, typename T) >								\
	class IUTEST_PP_CAT(AllOfMatcher, n) : public AllOfMatcherBase {				\
	public: IUTEST_PP_CAT(AllOfMatcher, n)(IUTEST_PP_ENUM_BINARY_PARAMS(n, T, m))	\
		: m_matchers(IUTEST_PP_ENUM_PARAMS(n, m)) {}								\
	template<typename U>AssertionResult operator ()(const U& actual) const {		\
		return Check(m_matchers, actual); }											\
	private: IUTEST_PP_DISALLOW_ASSIGN(IUTEST_PP_CAT(AllOfMatcher, n));				\
	tuples::tuple< IUTEST_PP_ENUM_PARAMS(n, T) > m_matchers;						\
	}

IIUT_DECL_ALLOF_MATCHER(2);
IIUT_DECL_ALLOF_MATCHER(3);
IIUT_DECL_ALLOF_MATCHER(4);
IIUT_DECL_ALLOF_MATCHER(5);
IIUT_DECL_ALLOF_MATCHER(6);
IIUT_DECL_ALLOF_MATCHER(7);
IIUT_DECL_ALLOF_MATCHER(8);
IIUT_DECL_ALLOF_MATCHER(9);

#undef IIUT_DECL_ALLOF_MATCHER

#endif

/**
 * @brief	AnyOf matcher base class
*/
class AnyOfMatcherBase
{
protected:
	template<typename T, typename U>
	static AssertionResult Check(const T& matchers, const U& actual)
	{
		return Check<T, U, 0, tuples::tuple_size<T>::value>(matchers, actual);
	}
private:
	template<typename T, typename U, int N, int END>
	static AssertionResult Check(const T& matchers, const U& actual, typename detail::enable_if<N == END - 1, void>::type*& = detail::enabler::value)
	{
		AssertionResult ar = tuples::get<N>(matchers)(actual);
		if( ar )
		{
			return ar;
		}
		return AssertionFailure() << WitchIs<T, 0, N>(matchers);
	}
	template<typename T, typename U, int N, int END>
	static AssertionResult Check(const T& matchers, const U& actual, typename detail::disable_if<N == END - 1, void>::type*& = detail::enabler::value)
	{
		AssertionResult ar = tuples::get<N>(matchers)(actual);
		if( ar )
		{
			return ar;
		}
		return Check<T, U, N+1, END>(matchers, actual);
	}

	template<typename T, int N, int END>
	static ::std::string  WitchIs(const T& matchers, typename detail::enable_if<N == END, void>::type*& = detail::enabler::value)
	{
		return tuples::get<N>(matchers).WitchIs();
	}
	template<typename T, int N, int END>
	static ::std::string  WitchIs(const T& matchers, typename detail::disable_if<N == END, void>::type*& = detail::enabler::value)
	{
		return tuples::get<N>(matchers).WitchIs() + " or " + WitchIs<T, N+1, END>(matchers);
	}
};

#if IUTEST_HAS_VARIADIC_TEMPLATES

/**
 * @brief	AnyOf matcher
*/
template<typename ...T>
class AnyOfMatcher : public AnyOfMatcherBase
{
public:
	AnyOfMatcher(T... t) : m_matchers(t...) {}

public:
	template<typename U>
	AssertionResult operator ()(const U& actual) const
	{
		return Check(m_matchers, actual);
	}

private:
	tuples::tuple<T...> m_matchers;
};

#else

/*
template<typename T0, typename T1>
class AnyOfMatcher : public AnyOfMatcherBase
{
public:
	AnyOfMatcher(T0 m0, T1 m1) : m_matchers(m0, m1) {}

public:
	template<typename U>
	AssertionResult operator ()(const U& actual) const
	{
		return Check(m_matchers, actual);
	}
private:
	IUTEST_PP_DISALLOW_ASSIGN(AnyOfMatcher);

	tuples::tuple<T0, T1> m_matchers;
};
*/

#define IIUT_DECL_ANYOF_MATCHER(n)													\
	template< IUTEST_PP_ENUM_PARAMS(n, typename T) >								\
	class IUTEST_PP_CAT(AnyOfMatcher, n) : public AnyOfMatcherBase {				\
	public: IUTEST_PP_CAT(AnyOfMatcher, n)(IUTEST_PP_ENUM_BINARY_PARAMS(n, T, m))	\
		: m_matchers(IUTEST_PP_ENUM_PARAMS(n, m)) {}								\
	template<typename U>AssertionResult operator ()(const U& actual) const {		\
		return Check(m_matchers, actual); }											\
	private: IUTEST_PP_DISALLOW_ASSIGN(IUTEST_PP_CAT(AnyOfMatcher, n));				\
	tuples::tuple< IUTEST_PP_ENUM_PARAMS(n, T) > m_matchers;						\
	}

IIUT_DECL_ANYOF_MATCHER(2);
IIUT_DECL_ANYOF_MATCHER(3);
IIUT_DECL_ANYOF_MATCHER(4);
IIUT_DECL_ANYOF_MATCHER(5);
IIUT_DECL_ANYOF_MATCHER(6);
IIUT_DECL_ANYOF_MATCHER(7);
IIUT_DECL_ANYOF_MATCHER(8);
IIUT_DECL_ANYOF_MATCHER(9);

#undef IIUT_DECL_ANYOF_MATCHER

#endif

#endif

}	// end of namespace detail

/**
 * @brief	Make StartsWith matcher
*/
template<typename T>
detail::StartsWithMatcher<const T&> StartsWith(const T& str) { return detail::StartsWithMatcher<const T&>(str); }

/**
 * @brief	Make Contains matcher
*/
template<typename T>
detail::ContainsMatcher<const T&> Contains(const T& expected) { return detail::ContainsMatcher<const T&>(expected); }

/**
 * @brief	Make EndsWith matcher
*/
template<typename T>
detail::EndsWithMatcher<const T&> EndsWith(const T& str) { return detail::EndsWithMatcher<const T&>(str); }

/**
 * @brief	Make Equals matcher
*/
template<typename T>
detail::EqualsMatcher<const T&> Equals(const T& expected) { return detail::EqualsMatcher<const T&>(expected); }

#if IUTEST_HAS_MATCHER_ALLOF_AND_ANYOF

#if IUTEST_HAS_VARIADIC_TEMPLATES

/**
 * @brief	Make AllOf matcher
*/
template<typename ...T>
detail::AllOfMatcher<T...> AllOf(const T&... m) { return detail::AllOfMatcher<T...>(m...); }

/**
 * @brief	Make AnyOf matcher
*/
template<typename ...T>
detail::AnyOfMatcher<T...> AnyOf(const T&... m) { return detail::AnyOfMatcher<T...>(m...); }

#else

#define IIUT_ANYOF_AND_ALLOF_MATCHER_NAME(name, n)	IUTEST_PP_CAT( IUTEST_PP_CAT(name, Matcher), n)
#define IIUT_DECL_ANYOF_AND_ALLOF(name, n)												\
	template< IUTEST_PP_ENUM_PARAMS(n, typename T) >									\
	detail:: IIUT_ANYOF_AND_ALLOF_MATCHER_NAME(name, n)< IUTEST_PP_ENUM_PARAMS(n, T) >	\
	name( IUTEST_PP_ENUM_BINARY_PARAMS(n, const T, &m) ) { return						\
	detail:: IIUT_ANYOF_AND_ALLOF_MATCHER_NAME(name, n)< IUTEST_PP_ENUM_PARAMS(n, T) >	\
		( IUTEST_PP_ENUM_PARAMS(n, m) ); }


IIUT_DECL_ANYOF_AND_ALLOF(AllOf, 2)
IIUT_DECL_ANYOF_AND_ALLOF(AllOf, 3)
IIUT_DECL_ANYOF_AND_ALLOF(AllOf, 4)
IIUT_DECL_ANYOF_AND_ALLOF(AllOf, 5)
IIUT_DECL_ANYOF_AND_ALLOF(AllOf, 6)
IIUT_DECL_ANYOF_AND_ALLOF(AllOf, 7)
IIUT_DECL_ANYOF_AND_ALLOF(AllOf, 8)
IIUT_DECL_ANYOF_AND_ALLOF(AllOf, 9)

IIUT_DECL_ANYOF_AND_ALLOF(AnyOf, 2)
IIUT_DECL_ANYOF_AND_ALLOF(AnyOf, 3)
IIUT_DECL_ANYOF_AND_ALLOF(AnyOf, 4)
IIUT_DECL_ANYOF_AND_ALLOF(AnyOf, 5)
IIUT_DECL_ANYOF_AND_ALLOF(AnyOf, 6)
IIUT_DECL_ANYOF_AND_ALLOF(AnyOf, 7)
IIUT_DECL_ANYOF_AND_ALLOF(AnyOf, 8)
IIUT_DECL_ANYOF_AND_ALLOF(AnyOf, 9)

#undef IIUT_ANYOF_AND_ALLOF_MATCHER_NAME
#undef IIUT_DECL_ANYOF_AND_ALLOF

#endif

#endif

}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_MATCHER_HPP_23746E00_1A4B_4778_91AD_45C6DEFEEFA7_
