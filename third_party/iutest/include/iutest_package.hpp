//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_package.hpp
 * @brief		iris unit test testcase package ファイル
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
#ifndef INCG_IRIS_IUTEST_PACKAGE_HPP_F57C9B7E_7CAA_4429_BE75_FCAAEED1B220_
#define INCG_IRIS_IUTEST_PACKAGE_HPP_F57C9B7E_7CAA_4429_BE75_FCAAEED1B220_

//======================================================================
// include
#include "internal/iutest_internal_defs.hpp"

//======================================================================
// define
/**
 * @ingroup	TESTDEF
 * @brief	パッケージ名前空間の定義
 * @param	name	= パッケージ名
*/
#define IUTEST_PACKAGE(name)	IIUT_PACKAGE_(name)

/**
 * @private
 * @{
*/

#if IUTEST_HAS_PACKAGE

#define IUTEST_CONCAT_PACKAGE_(testcase_name)	IIUT_CONCAT_PACKAGE_I(testcase_name).c_str()
#define IIUT_CONCAT_PACKAGE_I(testcase_name)					\
	iuTest_ConcatTestCaseName( iuTest_GetTestCasePackageName(	\
	static_cast<iuTest_TestCasePackage*>(NULL))					\
	, #testcase_name)

#define IUTEST_GET_PACKAGENAME_()	\
	iuTest_GetTestCasePackageName( static_cast<iuTest_TestCasePackage*>(NULL))

#define IIUT_PACKAGE_DECL_NAME_FUNC(name)				\
	static ::std::string IUTEST_ATTRIBUTE_UNUSED_		\
	iuTest_GetTestCasePackageName(const iuTest_TestCasePackage*) {			\
		return iuTest_GetTestCaseParentPackageName(		\
			static_cast<iuTest_TestCaseParentPackage*>(NULL)) + #name ".";	\
	}													\

#define IIUT_PACKAGE_DECL_PARENT_NAME_FUNC(name)		\
	static ::std::string IUTEST_ATTRIBUTE_UNUSED_		\
	iuTest_GetTestCaseParentPackageName(const iuTest_TestCaseParentPackage*) {				\
		return iuTest_GetTestCasePackageName(static_cast<iuTest_TestCasePackage*>(NULL));	\
	}													\


#if IUTEST_HAS_IF_EXISTS

#define IIUT_PACKAGE_(name)									\
	namespace name {										\
	class iuTest_TestCasePackage;							\
	__if_not_exists(name::iuTest_GetTestCasePackageName) {	\
		IIUT_PACKAGE_DECL_NAME_FUNC(name)					\
	}														\
	class iuTest_TestCaseParentPackage;						\
	__if_not_exists(name::iuTest_GetTestCaseParentPackageName) {	\
		IIUT_PACKAGE_DECL_PARENT_NAME_FUNC(name)			\
	}														\
	}														\
	namespace name

#else

#define IIUT_PACKAGE_(name)								\
	namespace name {									\
	class iuTest_TestCasePackage;						\
	IIUT_PACKAGE_DECL_NAME_FUNC(name)					\
	class iuTest_TestCaseParentPackage;					\
	IIUT_PACKAGE_DECL_PARENT_NAME_FUNC(name)			\
	}													\
	namespace name

#endif

#else

#define IUTEST_CONCAT_PACKAGE_(testcase_name)	IIUT_CONCAT_PACKAGE_I(testcase_name)
#define IIUT_CONCAT_PACKAGE_I(testcase_name)	#testcase_name
#define IIUT_PACKAGE_(name)						namespace name
#define IUTEST_GET_PACKAGENAME_()				""

#endif

/**
 * @}
*/

#if IUTEST_HAS_PACKAGE

//======================================================================
// declare
class iuTest_TestCasePackage;			//!< パッケージ名参照用定義
class iuTest_TestCaseParentPackage;		//!< 親パッケージ名参照用定義

//======================================================================
// function
/**
 * @brief	グローバルパッケージ名の取得
 * @return	パッケージ名
*/
inline ::std::string IUTEST_ATTRIBUTE_UNUSED_	iuTest_GetTestCasePackageName(const iuTest_TestCasePackage*) { return ""; }

/**
 * @brief	親空間のパッケージ名の取得
 * @return	パッケージ名
*/
inline ::std::string IUTEST_ATTRIBUTE_UNUSED_	iuTest_GetTestCaseParentPackageName(const iuTest_TestCaseParentPackage*) { return ""; }

/**
 * @brief	テストケース名との結合
 * @return	テストケース名
*/
inline ::std::string IUTEST_ATTRIBUTE_UNUSED_	iuTest_ConcatTestCaseName(const ::std::string& package, const char* testcase_name) { return package + testcase_name; }

#endif

#endif	// INCG_IRIS_IUTEST_PACKAGE_HPP_F57C9B7E_7CAA_4429_BE75_FCAAEED1B220_
