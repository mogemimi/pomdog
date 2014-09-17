//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_switch_core.hpp
 * @brief		コア機能定義切り替え ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2012-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_SWITCH_CORE_HPP_0385C393_90D3_4B06_A027_4B07A17A33CB_
#define INCG_IRIS_IUTEST_SWITCH_CORE_HPP_0385C393_90D3_4B06_A027_4B07A17A33CB_

#if !defined(IUTEST_USE_GTEST)

//======================================================================
// undef
#ifdef GTEST_INCLUDE_GTEST_GTEST_H_

#undef RUN_ALL_TESTS

#undef TEST
#undef GTEST_TEST
#undef TEST_F
#undef TEST_P
#undef INSTANTIATE_TEST_CASE_P

#undef FRIEND_TEST
#undef SCOPED_TRACE
#undef GTEST_FLAG

#undef TYPED_TEST_CASE
#undef TYPED_TEST
#undef TYPED_TEST_CASE_P
#undef TYPED_TEST_P
#undef REGISTER_TYPED_TEST_CASE_P
#undef INSTANTIATE_TYPED_TEST_CASE_P

#undef GTEST_ATTRIBUTE_UNUSED_
#undef GTEST_AMBIGUOUS_ELSE_BLOCKER_

#undef GTEST_TEST_CLASS_NAME_
#undef GTEST_TEST_

#endif

//======================================================================
// define
#define RUN_ALL_TESTS	IUTEST_RUN_ALL_TESTS

#define TEST		IUTEST
#define GTEST_TEST	IUTEST_TEST
#define TEST_F		IUTEST_F
#define TEST_P		IUTEST_P
#define INSTANTIATE_TEST_CASE_P	IUTEST_INSTANTIATE_TEST_CASE_P

#define FRIEND_TEST			IUTEST_FRIEND_TEST
#define SCOPED_TRACE		IUTEST_SCOPED_TRACE
#define GTEST_FLAG			IUTEST_FLAG

#define TYPED_TEST_CASE		IUTEST_TYPED_TEST_CASE
#define TYPED_TEST			IUTEST_TYPED_TEST
#define TYPED_TEST_CASE_P	IUTEST_TYPED_TEST_CASE_P
#define TYPED_TEST_P		IUTEST_TYPED_TEST_P
#define REGISTER_TYPED_TEST_CASE_P		IUTEST_REGISTER_TYPED_TEST_CASE_P
#define INSTANTIATE_TYPED_TEST_CASE_P	IUTEST_INSTANTIATE_TYPED_TEST_CASE_P

#define GTEST_ATTRIBUTE_UNUSED_			IUTEST_ATTRIBUTE_UNUSED_
#define GTEST_AMBIGUOUS_ELSE_BLOCKER_	IUTEST_AMBIGUOUS_ELSE_BLOCKER_

#define GTEST_TEST_CLASS_NAME_	IUTEST_TEST_CLASS_NAME_
#define GTEST_TEST_				IUTEST_TEST_


#else // !defined(IUTEST_USE_GTEST)

//======================================================================
// undef
#if defined(INCG_IRIS_IUTEST_HPP_)

#undef IUTEST_RUN_ALL_TESTS
#undef IUTEST_INIT

#undef IUTEST
#undef IUTEST_TEST
#undef IUTEST_F
#undef IUTEST_P
#undef IUTEST_INSTANTIATE_TEST_CASE_P

#undef IUTEST_FRIEND_TEST
#undef IUTEST_FRIEND_TYPED_TEST
#undef IUTEST_SCOPED_TRACE
#undef IUTEST_FLAG

#undef IUTEST_TYPED_TEST_CASE
#undef IUTEST_TYPED_TEST
#undef IUTEST_T
#undef IUTEST_TYPED_TEST_CASE_P
#undef IUTEST_TYPED_TEST_P
#undef IUTEST_TP
#undef IUTEST_REGISTER_TYPED_TEST_CASE_P
#undef IUTEST_INSTANTIATE_TYPED_TEST_CASE_P

#ifdef IUTEST_AP
#  undef IUTEST_AP
#endif
#ifdef INSTANTIATE_TYPED_TEST_CASE_AP
#  undef INSTANTIATE_TYPED_TEST_CASE_AP
#endif


#undef IUTEST_ATTRIBUTE_UNUSED_
#undef IUTEST_AMBIGUOUS_ELSE_BLOCKER_

#undef IUTEST_TEST_CLASS_NAME_
#undef IUTEST_TEST_

#undef IUTEST_SUPPRESS_UNREACHABLE_CODE_WARNING
#undef IUTEST_PP_CAT
#undef IUTEST_IS_NULLLITERAL

#undef IUTEST_THROUGH_ANALYSIS_ASSUME

#endif

//======================================================================
// define
#define IUTEST_RUN_ALL_TESTS	RUN_ALL_TESTS
#define IUTEST_INIT				::testing::InitIrisUnitTest

#define IUTEST		GTEST_TEST
#define IUTEST_TEST	GTEST_TEST
#define IUTEST_F	TEST_F
#define IUTEST_P	TEST_P
#define IUTEST_INSTANTIATE_TEST_CASE_P	INSTANTIATE_TEST_CASE_P

#define IUTEST_FRIEND_TEST			FRIEND_TEST
#define IUTEST_FRIEND_TYPED_TEST	template<typename T>FRIEND_TEST
#define IUTEST_SCOPED_TRACE			SCOPED_TRACE
#define IUTEST_FLAG					GTEST_FLAG

#define IUTEST_TYPED_TEST_CASE		TYPED_TEST_CASE
#define IUTEST_TYPED_TEST			TYPED_TEST
#define IUTEST_T					TYPED_TEST
#define IUTEST_TYPED_TEST_CASE_P	TYPED_TEST_CASE_P
#define IUTEST_TYPED_TEST_P			TYPED_TEST_P
#define IUTEST_TP					TYPED_TEST_P
#define IUTEST_REGISTER_TYPED_TEST_CASE_P		REGISTER_TYPED_TEST_CASE_P
#define IUTEST_INSTANTIATE_TYPED_TEST_CASE_P	INSTANTIATE_TYPED_TEST_CASE_P

#define IUTEST_ATTRIBUTE_UNUSED_		GTEST_ATTRIBUTE_UNUSED_
#define IUTEST_AMBIGUOUS_ELSE_BLOCKER_	GTEST_AMBIGUOUS_ELSE_BLOCKER_

#define IUTEST_TEST_CLASS_NAME_	GTEST_TEST_CLASS_NAME_
#define IUTEST_TEST_			GTEST_TEST_

#define IUTEST_SUPPRESS_UNREACHABLE_CODE_WARNING	GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_

#define IUTEST_PP_CAT	GTEST_CONCAT_TOKEN_
#define IUTEST_IS_NULLLITERAL	GTEST_IS_NULL_LITERAL_


// __analysis_assume
#if defined(_MSC_VER) && (_MSC_VER >= 1500) && !defined(__CUDACC__)

#define IUTEST_THROUGH_ANALYSIS_ASSUME(expr, todo)					\
	IUTEST_AMBIGUOUS_ELSE_BLOCKER_									\
	if( bool b = true ) {											\
		__analysis_assume(expr);									\
		goto IUTEST_PP_CAT(iutest_label_analysis_assume, __LINE__);	\
	} else															\
		IUTEST_PP_CAT(iutest_label_analysis_assume, __LINE__):		\
		todo

#else

#define IUTEST_THROUGH_ANALYSIS_ASSUME(expr, todo)	todo

#endif

#endif // !defined(IUTEST_USE_GTEST)

#endif // INCG_IRIS_IUTEST_SWITCH_CORE_HPP_0385C393_90D3_4B06_A027_4B07A17A33CB_
