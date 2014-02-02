//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_regex.hpp
 * @brief		iris unit test 用 regex ファイル
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
#ifndef INCG_IRIS_IUTEST_REGEX_HPP_1575CB44_189A_4248_B305_DB4882E3BFC2_
#define INCG_IRIS_IUTEST_REGEX_HPP_1575CB44_189A_4248_B305_DB4882E3BFC2_

//======================================================================
// include

namespace iutest {
namespace detail
{

/**
 * @brief	regex クラス
*/
class iuRegex
{
	static bool	match_impl(const char* begin, const char* end, const char* src);
public:
	static bool	match(const char* regex, const char* src);
};

}	// end of namespace detail
}	// end of namespace iutest

#if !IUTEST_HAS_LIB
#  include "../impl/iutest_regex.ipp"
#endif

#endif	// INCG_IRIS_IUTEST_REGEX_HPP_1575CB44_189A_4248_B305_DB4882E3BFC2_
