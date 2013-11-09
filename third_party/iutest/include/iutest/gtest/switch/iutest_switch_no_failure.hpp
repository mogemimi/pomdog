//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_switch_no_failure.hpp
 * @brief		IUTEST_*_no_failure アサーション切り替え ファイル
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
#ifndef INCG_IRIS_iutest_switch_no_failure_HPP_5D6B75EE_F445_4730_8FEA_FB6249BCBD9C_
#define INCG_IRIS_iutest_switch_no_failure_HPP_5D6B75EE_F445_4730_8FEA_FB6249BCBD9C_

#if !defined(IUTEST_USE_GTEST)

//======================================================================
// define
#define ASSERT_NO_FAILURE	IUTEST_ASSERT_NO_FAILURE
#define EXPECT_NO_FAILURE	IUTEST_EXPECT_NO_FAILURE
#define INFORM_NO_FAILURE	IUTEST_INFORM_NO_FAILURE

#else

//======================================================================
// undef
#ifdef INCG_IRIS_iutest_HPP_

#undef IUTEST_ASSERT_NO_FAILURE
#undef IUTEST_EXPECT_NO_FAILURE
#undef IUTEST_INFORM_NO_FAILURE

#endif

//======================================================================
// define
#define IUTEST_ASSERT_NO_FAILURE	IUTEST_ASSERT_NO_FATAL_FAILURE
#define IUTEST_EXPECT_NO_FAILURE	IUTEST_EXPECT_NO_FATAL_FAILURE
#define IUTEST_INFORM_NO_FAILURE	IUTEST_INFORM_NO_FATAL_FAILURE

#endif

#endif
