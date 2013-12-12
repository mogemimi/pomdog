//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_switch_pmz.hpp
 * @brief		IUTEST_PMZ 切り替え定義 ファイル
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
#ifndef INCG_IRIS_IUTEST_SWITCH_PMZ_HPP_3913D1E7_E6FD_49B9_BB4C_795939FEB5C5_
#define INCG_IRIS_IUTEST_SWITCH_PMZ_HPP_3913D1E7_E6FD_49B9_BB4C_795939FEB5C5_

#if !defined(IUTEST_USE_GTEST)

//======================================================================
// define

#else

//======================================================================
// undef
#ifdef INCG_IRIS_IUTEST_HPP_

#undef IUTEST_HAS_PARAM_METHOD_TEST

#endif

//======================================================================
// define
#ifndef IUTEST_HAS_PARAM_METHOD_TEST
#  define IUTEST_HAS_PARAM_METHOD_TEST	0
#endif

#endif

#endif
