//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_gtest_ver.hpp
 * @brief		gtest version 識別 ファイル
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
#ifndef INCG_IRIS_IUTEST_GTEST_VER_HPP_7396AEF4_D1AB_44B5_9FE3_23CEC1240158_
#define INCG_IRIS_IUTEST_GTEST_VER_HPP_7396AEF4_D1AB_44B5_9FE3_23CEC1240158_

#if defined(IUTEST_USE_GTEST)

//======================================================================
// include
#include <gtest/gtest.h>

//======================================================================
// define

// gtest version

#ifndef GTEST_MAJORVER
#  define GTEST_MAJORVER	0x01		//!< Major Version
#endif

//!< Minor Version
#ifndef GTEST_MINORVER
#  if	!defined(RUN_ALL_TESTS) && !defined(GTEST_CREF_WORKAROUND_)
#    define GTEST_MINORVER	0x07
#  elif	defined(ADD_FAILURE_AT)
#    define GTEST_MINORVER	0x06
#  elif	defined(GTEST_API_)
#    define GTEST_MINORVER	0x05
#  elif	defined(EXPECT_DEATH_IF_SUPPORTED)
#    define GTEST_MINORVER	0x04
#  elf	defined(GTEST_HIDE_UNREACHABLE_CODE_)
#    define GTEST_MINORVER	0x03
#  elif	defined(GTEST_INCLUDE_GTEST_GTEST_PARAM_TEST_H_)
#    define GTEST_MINORVER	0x02
#  elif	defined(GTEST_INCLUDE_GTEST_GTEST_TYPED_TEST_H_)
#    define GTEST_MINORVER	0x01
#  else
#    define GTEST_MINORVER	0x00
#  endif
#endif

#ifndef GTEST_BUILD
#  if GTEST_MINORVER == 0x02 && defined(GTEST_HAS_CLONE)
#    define GTEST_BUILD		0x01		//!< Build
#  elif GTEST_MINORVER == 0x00
// 判断難しい
#    define GTEST_BUILD		0x01		//!< Build
#  else
#    define GTEST_BUILD		0x00		//!< Build
#  endif
#endif

#ifndef GTEST_REVISION
#  define GTEST_REVISION	0x00		//!< Revision
#endif

#ifndef GTEST_VER
#  define	GTEST_VER		((GTEST_MAJORVER<<24)	\
							|(GTEST_MINORVER<<16)	\
							|(GTEST_BUILD<<8)		\
							|(GTEST_REVISION<<0)	\
							)	//!< google test version
#endif

#endif

#endif	// INCG_IRIS_IUTEST_GTEST_VER_HPP_7396AEF4_D1AB_44B5_9FE3_23CEC1240158_
