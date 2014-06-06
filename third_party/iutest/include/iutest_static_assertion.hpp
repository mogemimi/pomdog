//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_static_assertion.hpp
 * @brief		iris unit test static assertion 定義 ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2012-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_STATIC_ASSERTION_HPP_53842FD6_9E5D_40A6_905D_EC6DEBE5ED6A_
#define INCG_IRIS_IUTEST_STATIC_ASSERTION_HPP_53842FD6_9E5D_40A6_905D_EC6DEBE5ED6A_

//======================================================================
// define
#if IUTEST_HAS_STATIC_ASSERT_TYPEEQ

#if 0
/**
 * @ingroup	IUTEST_UTIL
 * @brief	型アサーション
*/
template<typename T1, typename T2>
static bool	StaticAssertTypeEq(void)
{
	(void)detail::StaticAssertTypeEqHelper<T1, T2>();
	return true;
}
#else

/**
 * @ingroup	IUTEST_UTIL
 * @brief	型アサーション
 * @note	マクロバージョン
 *			エラーが呼び出し位置に出る
*/
#define StaticAssertTypeEq	detail::StaticAssertTypeEqHelper

#endif

#endif

/**
 * @brief	static_assert
*/
#if IUTEST_HAS_STATIC_ASSERT
#  define IUTEST_STATIC_ASSERT_MSG(B, Msg)	static_assert(B, Msg)
#else
#  define IUTEST_STATIC_ASSERT_MSG(B, Msg)	\
	typedef ::iutest::detail::StaticAssertionTest< sizeof(::iutest::detail::StaticAssertionFailure< (bool)B >) > IUTEST_PP_CAT(iutest_static_assert_typedef_, IUTEST_PP_COUNTER)
#endif

/**
 * @brief	static_assert
*/
#ifdef IUTEST_STATIC_ASSERT_MSG
#  ifdef IUTEST_NO_VARIADIC_MACROS
#    define IUTEST_STATIC_ASSERT(B)		IUTEST_STATIC_ASSERT_MSG(B, "")
#  else
#    define IUTEST_STATIC_ASSERT(...)	IUTEST_STATIC_ASSERT_MSG((__VA_ARGS__), "")
#  endif
#endif

/**
 * @brief	static assert if exists
*/
#ifndef IUTEST_STATIC_ASSERT_IF_EXISTS
#  if IUTEST_HAS_IF_EXISTS
#    define IUTEST_STATIC_ASSERT_IF_EXISTS(identifier, msg)	\
		IUTEST_STATIC_ASSERT_MSG( IUTEST_IF_EXISTS(identifier, true) IUTEST_IF_NOT_EXISTS(identifier,false), msg)
#  else
#    define IUTEST_STATIC_ASSERT_IF_EXISTS(identifier, msg)
#  endif
#endif

/**
* @brief	static assert if not exists
*/
#ifndef IUTEST_STATIC_ASSERT_IF_NOT_EXISTS
#  if IUTEST_HAS_IF_EXISTS
#    define IUTEST_STATIC_ASSERT_IF_NOT_EXISTS(identifier, msg)	\
		IUTEST_STATIC_ASSERT_MSG(IUTEST_IF_NOT_EXISTS(identifier, true) IUTEST_IF_EXISTS(identifier, false), msg)
#  else
#    define IUTEST_STATIC_ASSERT_IF_NOT_EXISTS(identifier, msg)
#  endif
#endif

//======================================================================
// function
namespace iutest
{

namespace detail
{

/** @private */
template<typename T1, typename T2>
struct StaticAssertTypeEqHelper;

#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
namespace helper
{

/** @private */
template<bool b>struct static_assert_typeeq;
/** @overload */
template<>struct static_assert_typeeq<true> { operator bool (void) const { return true; } };

}

/** @private */
template<typename T1, typename T2>
struct StaticAssertTypeEqHelper
	: public helper::static_assert_typeeq< iutest_type_traits::is_same<T1, T2>::value > {};

#endif

/** @private */
template<bool b>struct StaticAssertionFailure;
/** @overload */
template<> struct StaticAssertionFailure<true> { enum { value = 1 }; };

/** @private */
template<int x>struct StaticAssertionTest {};

}	// end of namespace detail
}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_STATIC_ASSERTION_HPP_53842FD6_9E5D_40A6_905D_EC6DEBE5ED6A_
