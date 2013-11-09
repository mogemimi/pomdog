//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_spi_switch.hpp
 * @brief		gtest から iutest への切り替え用 ファイル
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
 * @par			copyright
 * Copyright (C) 2012-2013, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_iutest_spi_switch_HPP_673AE7AC_6F80_4be0_9209_581E3A568525_
#define INCG_IRIS_iutest_spi_switch_HPP_673AE7AC_6F80_4be0_9209_581E3A568525_

#include "iutest_switch.hpp"

#if !defined(IUTEST_USE_GTEST)

#include "../iutest_spi.hpp"

#ifdef GTEST_INCLUDE_GTEST_GTEST_SPI_H_
#undef EXPECT_FATAL_FAILURE
#undef EXPECT_FATAL_FAILURE_ON_ALL_THREADS
#undef EXPECT_NONFATAL_FAILURE
#undef EXPECT_NONFATAL_FAILURE_ON_ALL_THREADS
#endif

#define EXPECT_FATAL_FAILURE					IUTEST_EXPECT_FATAL_FAILURE
#define EXPECT_FATAL_FAILURE_ON_ALL_THREADS		IUTEST_EXPECT_FATAL_FAILURE
#define EXPECT_NONFATAL_FAILURE					IUTEST_EXPECT_NONFATAL_FAILURE
#define EXPECT_NONFATAL_FAILURE_ON_ALL_THREADS	IUTEST_EXPECT_NONFATAL_FAILURE

#else

#include <gtest/gtest-spi.h>

#if defined(_MSC_VER)
// /ZI オプションだと __LINE__ が __LINE__Var+N(Nは番号) になりコンパイルエラーになるための対応
#  ifndef __LINE__Var
#    define __LINE__Var	0
#  endif
#endif

#ifdef INCG_IRIS_iutest_spi_HPP_
#undef IUTEST_ASSERT_FATAL_FAILURE
#undef IUTEST_EXPECT_FATAL_FAILURE
#undef IUTEST_ASSERT_NONFATAL_FAILURE
#undef IUTEST_EXPECT_NONFATAL_FAILURE
#endif

#define IUTEST_ASSERT_FATAL_FAILURE				EXPECT_FATAL_FAILURE
#define IUTEST_EXPECT_FATAL_FAILURE				EXPECT_FATAL_FAILURE
#define IUTEST_ASSERT_NONFATAL_FAILURE			EXPECT_NONFATAL_FAILURE
#define IUTEST_EXPECT_NONFATAL_FAILURE			EXPECT_NONFATAL_FAILURE

#endif

#endif
