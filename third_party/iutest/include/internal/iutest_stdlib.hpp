//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_stdlib.hpp
 * @brief		iris unit test stdlib
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2012-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_STDLIB_HPP_54D4BEEE_7B6B_4AF4_B1F6_138560480D55_
#define INCG_IRIS_IUTEST_STDLIB_HPP_54D4BEEE_7B6B_4AF4_B1F6_138560480D55_

//======================================================================
// include
#if defined(IUTEST_USE_GTEST) && defined(__STRICT_ANSI__)
#  undef __STRICT_ANSI__
#  include <cstdio>
#  include <string.h>
#  include <stdlib.h>
#  define __STRICT_ANSI__
#endif
#include <stdlib.h>

//======================================================================
// define

// tuple
#if defined(IUTEST_USE_EXTERNAL_TR1_TUPLE) && IUTEST_USE_EXTERNAL_TR1_TUPLE
# define IUTEST_HAS_STD_TUPLE	0
#endif

#if !defined(IUTEST_USE_EXTERNAL_TR1_TUPLE)
#  define IUTEST_USE_EXTERNAL_TR1_TUPLE	0
#endif

//! has ::std::tuple
#if !defined(IUTEST_HAS_STD_TUPLE)
#  if   defined(IUTEST_USE_EXTERNAL_STD_TUPLE) && IUTEST_USE_EXTERNAL_STD_TUPLE
#    define IUTEST_HAS_STD_TUPLE	1
#  elif defined(_STLPORT_VERSION)
#    define IUTEST_HAS_STD_TUPLE	0
#  elif defined(_MSC_VER)
#    if (_MSC_VER > 1700) || (_MSC_VER == 1700 && _VARIADIC_MAX >= 9)
#      define IUTEST_HAS_STD_TUPLE	1
#    endif
#  elif defined(__clang__)
#    if __has_include( <tuple> ) && IUTEST_HAS_VARIADIC_TEMPLATES
#      define IUTEST_HAS_STD_TUPLE	1
#    endif
#  elif defined(__GNUC__)
#    if IUTEST_HAS_VARIADIC_TEMPLATES
#      define IUTEST_HAS_STD_TUPLE	1
#    endif
#  endif
#endif

#if !defined(IUTEST_HAS_STD_TUPLE)
#  define IUTEST_HAS_STD_TUPLE		0
#endif

#ifndef IUTEST_USE_EXTERNAL_STD_TUPLE
#  define IUTEST_USE_EXTERNAL_STD_TUPLE	0
#endif

#if !IUTEST_HAS_STD_TUPLE

// ::std::tr1::tuple
#if !defined(IUTEST_HAS_TR1_TUPLE)
#  if   defined(IUTEST_USE_EXTERNAL_TR1_TUPLE) && IUTEST_USE_EXTERNAL_TR1_TUPLE
#    define IUTEST_HAS_TR1_TUPLE	1
#  elif defined(_STLPORT_VERSION)
#    define IUTEST_HAS_TR1_TUPLE	0
#  elif defined(IUTEST_OS_LINUX_ANDROID) && defined(_STLPORT_MAJOR)
#    define IUTEST_HAS_TR1_TUPLE	0
#  elif defined(_MSC_VER) && defined(_MSC_FULL_VER)
#    if (_MSC_VER >= 1500) && (_MSC_VER < 1700) && (_MSC_FULL_VER > 150021022)
#      define IUTEST_HAS_TR1_TUPLE	1
#    endif
#  elif defined(__clang__)
#    if __has_include( <tr1/tuple> )
#      define IUTEST_HAS_TR1_TUPLE	1
#    endif
#  elif defined(__GNUC__)
#    if (!defined(__CUDACC__) && !defined(__ARMCC_VERSION) && (__GNUC__ >= 4))
#      define IUTEST_HAS_TR1_TUPLE	1
#    endif
#  endif
#endif

#endif

#if !defined(IUTEST_HAS_TR1_TUPLE)
#  define IUTEST_HAS_TR1_TUPLE		0
#endif

#if !defined(IUTEST_USE_OWN_TR1_TUPLE)
#  define IUTEST_USE_OWN_TR1_TUPLE	0
#endif

#if IUTEST_HAS_STD_TUPLE || IUTEST_HAS_TR1_TUPLE
#  if !defined(IUTEST_HAS_TUPLE)
#    define IUTEST_HAS_TUPLE	1
#  endif
#else
#  if defined(IUTEST_HAS_TUPLE)
#    undef IUTEST_HAS_TUPLE
#  endif
#  define IUTEST_HAS_TUPLE		0
#endif

#if IUTEST_HAS_TUPLE

#if IUTEST_HAS_STD_TUPLE
#  if !IUTEST_USE_EXTERNAL_STD_TUPLE
#    include <tuple>
#  endif
#else
#  if IUTEST_HAS_TR1_TUPLE && !IUTEST_USE_EXTERNAL_TR1_TUPLE
#    if IUTEST_USE_OWN_TR1_TUPLE
#      include "iutest_tuple.hpp"
#    else
#      if (defined(__GNUC__) && (__GNUC__ >= 4))
#        include <tr1/tuple>
#      else
#        include <tuple>
#      endif
#    endif
#  endif
#endif

namespace iutest {
namespace tuples
{
#if IUTEST_HAS_STD_TUPLE
	namespace alias = ::std;
#elif IUTEST_HAS_TR1_TUPLE
	namespace alias = ::std::tr1;
#endif

	using alias::tuple;
	using alias::tuple_element;
	using alias::make_tuple;
	using alias::get;

	template<typename T>struct tuple_size : public alias::tuple_size<T> {};
	template<typename T>struct tuple_size<const T> : public alias::tuple_size<T> {};
	template<typename T>struct tuple_size<volatile T> : public alias::tuple_size<T>{};
	template<typename T>struct tuple_size<const volatile T> : public alias::tuple_size<T>{};

	namespace detail
	{
		template<typename T, typename F, int Begin>
		struct tuple_foreach_impl
		{
			template<int N, int I>
			struct impl
			{
				static void do_something(T& t, F fn)
				{
					fn(I, get<I>(t));
					impl<N, I + 1>::do_something(t, fn);
				}
			};
			template<int N>
			struct impl<N, N>
			{
				static void do_something(T&, F) {}
			};

			static void do_something(T& t, F fn)
			{
				impl<tuple_size<T>::value, Begin>::do_something(t, fn);
			}
		};

		template<typename T, typename U>
		struct tuple_cast_copy_impl
		{
			template<int N, int I>
			struct impl
			{
				static void copy(T& dst, const U& src)
				{
					get<I>(dst) = static_cast<typename tuple_element<I, T>::type>(get<I>(src));
					impl<N, I + 1>::copy(dst, src);
				}
			};
			template<int N>
			struct impl<N, N>
			{
				static void copy(T&, const U&) {}
			};

			static void copy(T& dst, const U& src)
			{
				impl<tuple_size<T>::value, 0>::copy(dst, src);
			}
		};

	}
	template<int I, typename tuple_t, typename F>
	void tuple_foreach(tuple_t& t, F& fn)
	{
		detail::tuple_foreach_impl<tuple_t, F&, I>::do_something(t, fn);
	}
	template<typename tuple_t, typename F>
	void tuple_foreach(tuple_t& t, F& fn)
	{
		tuple_foreach<0>(t, fn);
	}
	template<int I, typename tuple_t, typename F>
	void tuple_foreach(tuple_t& t, const F& fn)
	{
		detail::tuple_foreach_impl<tuple_t, const F&, I>::do_something(t, fn);
	}
	template<typename tuple_t, typename F>
	void tuple_foreach(tuple_t& t, const F& fn)
	{
		tuple_foreach<0>(t, fn);
	}
	template<typename T, typename U>
	void tuple_cast_copy(T& dst, const U& src)
	{
		detail::tuple_cast_copy_impl<T, U>::copy(dst, src);
	}
}

using tuples::tuple;
using tuples::tuple_size;
using tuples::tuple_element;
using tuples::tuple_foreach;
using tuples::make_tuple;
using tuples::get;

}

#endif

// chrono
#if !defined(IUTEST_HAS_CXX_HDR_CHRONO)

#if   defined(__has_include)
#  if __has_include( <chrono> )
#    if IUTEST_HAS_CXX11
#      define IUTEST_HAS_CXX_HDR_CHRONO		1
#    endif
#  endif
#elif defined(__GLIBCXX__)
#  if defined(__GNUC__)
#    if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#      define IUTEST_HAS_CXX_HDR_CHRONO		1
#    endif
#  else
#  endif
#elif defined(_MSC_VER)
#  if _MSC_VER > 1600
#    define IUTEST_HAS_CXX_HDR_CHRONO		1
#  endif
#endif

#endif

#if !defined(IUTEST_HAS_CXX_HDR_CHRONO)
#  define IUTEST_HAS_CXX_HDR_CHRONO			0
#endif

// random
#if !defined(IUTEST_HAS_CXX_HDR_RANDOM)

#if   defined(__has_include)
#  if __has_include( <random> )
#    if IUTEST_HAS_CXX11
#      define IUTEST_HAS_CXX_HDR_RANDOM		1
#    endif
#  endif
#elif defined(__GLIBCXX__)
#  if defined(__GNUC__)
#    if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#      define IUTEST_HAS_CXX_HDR_RANDOM		1
#    endif
#  else
#  endif
#elif defined(_LIBCPP_VERSION)
#  if IUTEST_HAS_CXX11
#    define IUTEST_HAS_CXX_HDR_RANDOM		1
#  endif
#elif defined(_MSC_VER)
#  if _MSC_VER > 1600
#    define IUTEST_HAS_CXX_HDR_RANDOM		1
#  endif
#endif

#endif

#if !defined(IUTEST_HAS_CXX_HDR_RANDOM)
#  define IUTEST_HAS_CXX_HDR_RANDOM		0
#endif

// codecvt
/*
#if !defined(IUTEST_HAS_CXX_HDR_CODECVT)

#if   defined(__has_include)
#  if __has_include( <codecvt> )
#    if IUTEST_HAS_CXX11
#      define IUTEST_HAS_CXX_HDR_CODECVT	1
#    endif
#  endif
#elif defined(__GLIBCXX__)
#elif defined(_MSC_VER)
#  if _MSC_VER > 1600
#    define IUTEST_HAS_CXX_HDR_CODECVT		1
#  endif
#endif

#endif
*/

#if !defined(IUTEST_HAS_CXX_HDR_CODECVT)
#  define IUTEST_HAS_CXX_HDR_CODECVT		0
#endif

// emplace
#if !defined(IUTEST_HAS_STD_EMPLACE)
#  if defined(__GNUC__)
#    if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#      define IUTEST_HAS_STD_EMPLACE		1
#    endif
#  elif defined(__clang__)
#    define IUTEST_HAS_STD_EMPLACE			1
#  elif defined(_MSC_VER)
#    if (_MSC_VER > 1700)
#      define IUTEST_HAS_STD_EMPLACE		1
#    endif
#  endif
#endif

#if !defined(IUTEST_HAS_STD_EMPLACE)
#  define IUTEST_HAS_STD_EMPLACE			0
#endif

// abi
#if !defined(IUTEST_HAS_HDR_CXXABI)

#if   defined(__has_include)
#  if __has_include( <cxxabi.h> )
#    define IUTEST_HAS_HDR_CXXABI		1
#  endif
#elif defined(__GLIBCXX__) || defined(_LIBCPP_VERSION)
#  define IUTEST_HAS_HDR_CXXABI			1
#else
#  define IUTEST_HAS_HDR_CXXABI			0
#endif

#endif

#if !defined(IUTEST_HAS_HDR_CXXABI)
#  define IUTEST_HAS_HDR_CXXABI			0
#endif

// sys/time.h
#if !defined(IUTEST_HAS_HDR_SYSTIME)
#  if defined(IUTEST_OS_WINDOWS) && !defined(IUTEST_OS_WINDOWS_MINGW)
#    define IUTEST_HAS_HDR_SYSTIME		0
#  endif
#endif

#if !defined(IUTEST_HAS_HDR_SYSTIME)
#  define IUTEST_HAS_HDR_SYSTIME		1
#endif


namespace iutest {
namespace detail
{

//======================================================================
// function
/**
 * @internal
 * @brief	mbtowc
*/
inline int iu_mbtowc(wchar_t* dst, const char* src, size_t size)
{
#if defined(IUTEST_OS_LINUX_ANDROID) || defined(IUTEST_OS_WINDOWS_MOBILE)
	// unimplimented
	IUTEST_UNUSED_VAR(dst);
	IUTEST_UNUSED_VAR(src);
	IUTEST_UNUSED_VAR(size);
	return 0;
#else
	return mbtowc(dst, src, size);
#endif
}

}	// end of namespace detail
}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_STDLIB_HPP_54D4BEEE_7B6B_4AF4_B1F6_138560480D55_
