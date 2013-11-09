//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_ignore.hpp
 * @brief		iris unit test ignore test ファイル
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
 * @par			copyright
 * Copyright (C) 2013, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_iutest_ignore_HPP_D87CADCE_110D_43CE_9E68_3B6BB0B91E66_
#define INCG_IRIS_iutest_ignore_HPP_D87CADCE_110D_43CE_9E68_3B6BB0B91E66_

//======================================================================
// define

#if IUTEST_HAS_IGNORE_TEST

/**
 * @ingroup	TESTDEF
 * @def		IUTEST_TEST_IGNORE(testcase_, testname_)
 * @brief	テスト関数定義マクロ
 * @param	testcase_	= テストケース名
 * @param	testname_	= テスト名
 * @note	ignore 版ではテストコードのコンパイルが通らなくても問題ない
*/
#define IUTEST_TEST_IGNORE(testcase_, testname_)	IUTEST_TEST_IGNORE_(testcase_, testname_, ::iutest::Test	\
														, ::iutest::internal::GetTestTypeId())

/**
 * @ingroup	TESTDEF
 * @def		IUTEST_TEST_IGNORE(testcase_, testname_)
 * @brief	テスト関数定義マクロ
 * @param	testcase_	= テストケース名
 * @param	testname_	= テスト名
 * @note	ignore 版ではテストコードのコンパイルが通らなくても問題ない
*/
#define IUTEST_IGNORE(testcase_, testname_)			IUTEST_TEST_IGNORE(testcase_, testname_)


/**
 * @ingroup	TESTDEF
 * @def		IUTEST_F_IGNORE(testfixture_, testname_)
 * @brief	ユーザー指定テスト関数定義マクロ
 * @param	testfixture_	= テストフィクスチャ名
 * @param	testname_		= テスト名
 * @note	ignore 版ではテストコードのコンパイルが通らなくても問題ない
*/
#define IUTEST_F_IGNORE(testfixture_, testname_)	IUTEST_TEST_F_IGNORE_(testfixture_, testname_)

#if IUTEST_HAS_PARAM_TEST

/**
 * @ingroup	TESTDEF
 * @def		IUTEST_P_IGNORE(testcase_, testname_)
 * @brief	パラメータユーザー指定テスト関数定義マクロ
 * @param	testcase_	= テストケース名
 * @param	testname_	= テスト名
 * @note	ignore 版ではテストコードのコンパイルが通らなくても問題ない
*/
#define IUTEST_P_IGNORE(testcase_, testname_)				IIUT_TEST_P_IGNORE_(testcase_, testname_)

#endif

#if IUTEST_HAS_TYPED_TEST

/**
 * @ingroup	TESTDEF
 * @def		IUTEST_TYPED_TEST(testcase_, testname_)
 * @brief	型付けテスト関数定義マクロ
 * @param	testcase_	= テストケース名
 * @param	testname_	= テスト名
 * @note	ignore 版ではテストコードのコンパイルが通らなくても問題ない
*/
#define IUTEST_TYPED_TEST_IGNORE(testcase_, testname_)		IIUT_TYPED_TEST_IGNORE_(testcase_, testname_)

#endif

#if IUTEST_HAS_TYPED_TEST_P

/**
 * @ingroup	TESTDEF
 * @def		IUTEST_TYPED_TEST_P(testcase_, testname_)
 * @brief	型パラメータテスト関数定義マクロ
 * @param	testcase_	= テストケース名
 * @param	testname_	= テスト名
 * @note	ignore 版ではテストコードのコンパイルが通らなくても問題ない
*/
#define IUTEST_TYPED_TEST_P_IGNORE(testcase_, testname_)	IIUT_TYPED_TEST_P_IGNORE_(testcase_, testname_)

#endif

#endif

#endif
