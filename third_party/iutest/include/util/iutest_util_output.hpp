//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_util_output.hpp
 * @brief		iris unit test print 出力 utility ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2012-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_UTIL_OUTPUT_HPP_44C683CF_3452_4F68_B34E_10606EB9E190_
#define INCG_IRIS_IUTEST_UTIL_OUTPUT_HPP_44C683CF_3452_4F68_B34E_10606EB9E190_

//======================================================================
// include
#ifdef INCG_IRIS_IUTEST_SWITCH_HPP_
#  ifdef IUTEST_USE_GTEST
#    ifndef iutest
#      define iutest	testing
#      include "../internal/iutest_console.hpp"
#      undef iutest
#    else
#      undef INCG_IRIS_IUTEST_CONSOLE_HPP_DCAC5025_B7BB_424E_A849_9E6FE0A3B460_
#      include "../internal/iutest_console.hpp"
#    endif
#  endif
#endif

namespace iuutil
{

//! コンソール出力クラスの再利用
typedef ::iutest::detail::iuConsole Console;

}	// end of namespace iuutil

#endif // INCG_IRIS_IUTEST_UTIL_OUTPUT_HPP_44C683CF_3452_4F68_B34E_10606EB9E190_
