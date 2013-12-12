//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_util.hpp
 * @brief		iris unit test utility ファイル
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
#ifndef INCG_IRIS_IUTEST_UTIL_HPP_EBAC3846_4D1E_4B32_A85F_6D6A7CCFAF75_
#define INCG_IRIS_IUTEST_UTIL_HPP_EBAC3846_4D1E_4B32_A85F_6D6A7CCFAF75_

//======================================================================
// include
#include "util/iutest_util_assertion.hpp"
#include "util/iutest_util_output.hpp"
#include "util/iutest_util_quiet_result_printer.hpp"
#include "util/iutest_util_tests.hpp"

/**
 * @brief iutest utility namespace
*/
namespace iuutil {}

/**
 * @ingroup		IUTEST_UTIL
 * @brief		ファイルシステムの設定
 * @note		ユーザー定義のファイルクラスを利用するようにするためのマクロです。
*/
#define IUTEST_FILESYSTEM_INSTANTIATE(file_class_name)	IIUT_FILESYSTEM_INSTANTIATE_(file_class_name)

/**
 * @private
 * @{
*/
#define IIUT_FILESYSTEM_INSTANTIATE_(file_class_name)	static ::iutest::FileSystem< file_class_name > IUTEST_ATTRIBUTE_UNUSED_ s_iutest_filesystem_

/**
 * @}
*/

#endif
