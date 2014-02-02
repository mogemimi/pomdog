//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_stdlib.hpp
 * @brief		iris unit test stdlib
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
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
#if	defined(IUTEST_USE_EXTERNAL_TR1_TUPLE) && IUTEST_USE_EXTERNAL_TR1_TUPLE
# define IUTEST_HAS_STD_TUPLE	0
#endif

#ifndef IUTEST_USE_EXTERNAL_TR1_TUPLE
#  define IUTEST_USE_EXTERNAL_TR1_TUPLE	0
#endif

// ::std::tuple
#ifndef IUTEST_HAS_STD_TUPLE
#  if defined(IUTEST_USE_EXTERNAL_STD_TUPLE) && IUTEST_USE_EXTERNAL_STD_TUPLE
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
#  elif	defined(__GNUC__)
#    if IUTEST_HAS_VARIADIC_TEMPLATES
#      define IUTEST_HAS_STD_TUPLE	1
#    endif
#  endif
#endif

#ifndef IUTEST_HAS_STD_TUPLE
#  define IUTEST_HAS_STD_TUPLE		0
#endif

#ifndef IUTEST_USE_EXTERNAL_STD_TUPLE
#  define IUTEST_USE_EXTERNAL_STD_TUPLE	0
#endif

#if !IUTEST_HAS_STD_TUPLE

// ::std::tr1::tuple
#ifndef IUTEST_HAS_TR1_TUPLE
#  if defined(IUTEST_USE_EXTERNAL_TR1_TUPLE) && IUTEST_USE_EXTERNAL_TR1_TUPLE
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
#  elif	defined(__GNUC__)
#    if (!defined(__CUDACC__) && !defined(__ARMCC_VERSION) && (__GNUC__ >= 4))
#      define IUTEST_HAS_TR1_TUPLE	1
#    endif
#  endif
#endif

#endif

#ifndef IUTEST_HAS_TR1_TUPLE
#  define IUTEST_HAS_TR1_TUPLE		0
#endif

#ifndef IUTEST_USE_OWN_TR1_TUPLE
#  define IUTEST_USE_OWN_TR1_TUPLE	0
#endif

#if IUTEST_HAS_STD_TUPLE || IUTEST_HAS_TR1_TUPLE
#  ifndef IUTEST_HAS_TUPLE
#    define IUTEST_HAS_TUPLE	1
#  endif
#else
#  ifdef IUTEST_HAS_TUPLE
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
		using ::std::tuple;
		using ::std::tuple_size;
		using ::std::tuple_element;
		using ::std::make_tuple;
		using ::std::get;
#elif IUTEST_HAS_TR1_TUPLE
		using ::std::tr1::tuple;
		using ::std::tr1::tuple_size;
		using ::std::tr1::tuple_element;
		using ::std::tr1::make_tuple;
		using ::std::tr1::get;
#endif
	}
	using tuples::tuple;
	using tuples::tuple_size;
	using tuples::tuple_element;
	using tuples::make_tuple;
	using tuples::get;
}

#endif

// chrono
#ifndef IUTEST_HAS_CXX11_HDR_CHRONO

#if		defined(__has_include)
#  if __has_include( <chrono> )
#    if IUTEST_HAS_CXX11
#      define IUTEST_HAS_CXX11_HDR_CHRONO	1
#    endif
#  endif
#elif	defined(__GLIBCXX__)
#  if defined(__GNUC__)
#    if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#      define IUTEST_HAS_CXX11_HDR_CHRONO	1
#    endif
#  else
#  endif
#elif	defined(_MSC_VER)
#  if _MSC_VER > 1600
#    define IUTEST_HAS_CXX11_HDR_CHRONO		1
#  endif
#endif

#endif

#ifndef IUTEST_HAS_CXX11_HDR_CHRONO
#  define IUTEST_HAS_CXX11_HDR_CHRONO	0
#endif

// random
#ifndef IUTEST_HAS_CXX_HDR_RANDOM

#if		defined(__has_include)
#  if __has_include( <random> )
#    if IUTEST_HAS_CXX11
#      define IUTEST_HAS_CXX_HDR_RANDOM		1
#    endif
#  endif
#elif	defined(__GLIBCXX__)
#  if defined(__GNUC__)
#    if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#      define IUTEST_HAS_CXX_HDR_RANDOM		1
#    endif
#  else
#  endif
#elif	defined(_LIBCPP_VERSION)
#  if IUTEST_HAS_CXX11
#    define IUTEST_HAS_CXX_HDR_RANDOM		1
#  endif
#elif	defined(_MSC_VER)
#  if _MSC_VER > 1600
#    define IUTEST_HAS_CXX_HDR_RANDOM		1
#  endif
#endif

#endif

#ifndef IUTEST_HAS_CXX_HDR_RANDOM
#  define IUTEST_HAS_CXX_HDR_RANDOM		0
#endif

// codecvt
/*
#ifndef IUTEST_HAS_CXX_HDR_CODECVT

#if		defined(__has_include)
#  if __has_include( <codecvt> )
#    if IUTEST_HAS_CXX11
#      define IUTEST_HAS_CXX_HDR_CODECVT	1
#    endif
#  endif
#elif	defined(__GLIBCXX__)
#elif	defined(_MSC_VER)
#  if _MSC_VER > 1600
#    define IUTEST_HAS_CXX_HDR_CODECVT		1
#  endif
#endif

#endif
*/

#ifndef IUTEST_HAS_CXX_HDR_CODECVT
#  define IUTEST_HAS_CXX_HDR_CODECVT		0
#endif


// abi
#ifndef IUTEST_HAS_HDR_CXXABI

#if		defined(__has_include)
#  if __has_include( <cxxabi.h> )
#    define IUTEST_HAS_HDR_CXXABI		1
#  endif
#elif	defined(__GLIBCXX__) || defined(_LIBCPP_VERSION)
#  define IUTEST_HAS_HDR_CXXABI			1
#else
#  define IUTEST_HAS_HDR_CXXABI			0
#endif

#endif

#ifndef IUTEST_HAS_HDR_CXXABI
#  define IUTEST_HAS_HDR_CXXABI			0
#endif

// sys/time.h
#ifndef IUTEST_HAS_HDR_SYSTIME
#  if defined(IUTEST_OS_WINDOWS) && !defined(IUTEST_OS_WINDOWS_MINGW)
#    define IUTEST_HAS_HDR_SYSTIME		0
#  endif
#endif

#ifndef IUTEST_HAS_HDR_SYSTIME
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

#endif	// INCG_IRIS_IUTEST_STDLIB_HPP_54D4BEEE_7B6B_4AF4_B1F6_138560480D55_
