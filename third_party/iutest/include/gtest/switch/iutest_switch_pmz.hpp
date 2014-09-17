//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_switch_pmz.hpp
 * @brief		IUTEST_PMZ 切り替え定義 ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2012-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_SWITCH_PMZ_HPP_3913D1E7_E6FD_49B9_BB4C_795939FEB5C5_
#define INCG_IRIS_IUTEST_SWITCH_PMZ_HPP_3913D1E7_E6FD_49B9_BB4C_795939FEB5C5_

#if !defined(IUTEST_USE_GTEST)

//======================================================================
// define

#else // !defined(IUTEST_USE_GTEST)

//======================================================================
// undef
#if defined(INCG_IRIS_IUTEST_HPP_)

#undef IUTEST_HAS_PARAM_METHOD_TEST

#endif

//======================================================================
// define
#ifndef IUTEST_HAS_PARAM_METHOD_TEST
#  define IUTEST_HAS_PARAM_METHOD_TEST	0
#endif

#endif // !defined(IUTEST_USE_GTEST)

#endif // INCG_IRIS_IUTEST_SWITCH_PMZ_HPP_3913D1E7_E6FD_49B9_BB4C_795939FEB5C5_
