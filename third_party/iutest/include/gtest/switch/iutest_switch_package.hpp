//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_switch_package.hpp
 * @brief		IUTEST_PACKAGE 切り替え定義 ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2012-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_SWITCH_PACKAGE_HPP_5D6B75EE_F445_4730_8FEA_FB6249BCBD9C_
#define INCG_IRIS_IUTEST_SWITCH_PACKAGE_HPP_5D6B75EE_F445_4730_8FEA_FB6249BCBD9C_

#if !defined(IUTEST_USE_GTEST)

//======================================================================
// define

#else // !defined(IUTEST_USE_GTEST)

//======================================================================
// undef
#ifdef INCG_IRIS_IUTEST_HPP_

#undef IUTEST_PACKAGE
#undef IUTEST_HAS_PACKAGE

#endif

//======================================================================
// define
#define IUTEST_PACKAGE(name)		namespace name
#define IUTEST_HAS_PACKAGE			0

#endif // !defined(IUTEST_USE_GTEST)

#endif // INCG_IRIS_IUTEST_SWITCH_PACKAGE_HPP_5D6B75EE_F445_4730_8FEA_FB6249BCBD9C_
