//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_switch_assume.hpp
 * @brief		ASSUME フレーバー切り替え ファイル
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
 * @par			copyright
 * Copyright (C) 2013-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_SWITCH_ASSUME_HPP_E22A3162_D581_4F4F_A344_F5D701B21F53_
#define INCG_IRIS_IUTEST_SWITCH_ASSUME_HPP_E22A3162_D581_4F4F_A344_F5D701B21F53_

#if !defined(IUTEST_USE_GTEST)

//======================================================================
// define
#define ASSUME_TRUE		IUTEST_ASSUME_TRUE
#define ASSUME_FALSE	IUTEST_ASSUME_FALSE
#define ASSUME_EQ		IUTEST_ASSUME_EQ
#define ASSUME_NE		IUTEST_ASSUME_NE
#define ASSUME_LT		IUTEST_ASSUME_LT
#define ASSUME_LE		IUTEST_ASSUME_LE
#define ASSUME_GT		IUTEST_ASSUME_GT
#define ASSUME_GE		IUTEST_ASSUME_GE
#define ASSUME_NEAR		IUTEST_ASSUME_NEAR
#define ASSUME_FLOAT_EQ		IUTEST_ASSUME_FLOAT_EQ
#define ASSUME_DOUBLE_EQ	IUTEST_ASSUME_DOUBLE_EQ
#define ASSUME_STREQ		IUTEST_ASSUME_STREQ
#define ASSUME_STRNE		IUTEST_ASSUME_STRNE
#define ASSUME_STRCASEEQ	IUTEST_ASSUME_STRCASEEQ
#define ASSUME_STRCASENE	IUTEST_ASSUME_STRCASENE
#define ASSUME_THROW		IUTEST_ASSUME_THROW
#define ASSUME_NO_THROW		IUTEST_ASSUME_NO_THROW
#define ASSUME_ANY_THROW	IUTEST_ASSUME_ANY_THROW
#define ASSUME_NO_FATAL_FAILURE	IUTEST_ASSUME_NO_FATAL_FAILURE

#ifdef IUTEST_OS_WINDOWS
#  define ASSUME_HRESULT_SUCCEEDED	IUTEST_ASSUME_HRESULT_SUCCEEDED
#  define ASSUME_HRESULT_FAILED		IUTEST_ASSUME_HRESULT_FAILED
#endif

#define ASSUME_PRED1	IUTEST_ASSUME_PRED1
#define ASSUME_PRED2	IUTEST_ASSUME_PRED2
#define ASSUME_PRED3	IUTEST_ASSUME_PRED3
#define ASSUME_PRED4	IUTEST_ASSUME_PRED4
#define ASSUME_PRED5	IUTEST_ASSUME_PRED5

#define ASSUME_PRED_FORMAT1	IUTEST_ASSUME_PRED_FORMAT1
#define ASSUME_PRED_FORMAT2	IUTEST_ASSUME_PRED_FORMAT2
#define ASSUME_PRED_FORMAT3	IUTEST_ASSUME_PRED_FORMAT3
#define ASSUME_PRED_FORMAT4	IUTEST_ASSUME_PRED_FORMAT4
#define ASSUME_PRED_FORMAT5	IUTEST_ASSUME_PRED_FORMAT5


#else	// !defined(IUTEST_USE_GTEST)

//======================================================================
// undef
#ifdef INCG_IRIS_IUTEST_HPP_

#undef IUTEST_ASSUME_TRUE
#undef IUTEST_ASSUME_FALSE
#undef IUTEST_ASSUME_EQ
#undef IUTEST_ASSUME_NE
#undef IUTEST_ASSUME_LT
#undef IUTEST_ASSUME_LE
#undef IUTEST_ASSUME_GT
#undef IUTEST_ASSUME_GE
#undef IUTEST_ASSUME_NEAR
#undef IUTEST_ASSUME_NULL
#undef IUTEST_ASSUME_NOTNULL
#undef IUTEST_ASSUME_SAME
#undef IUTEST_ASSUME_FLOAT_EQ
#undef IUTEST_ASSUME_DOUBLE_EQ
#undef IUTEST_ASSUME_STREQ
#undef IUTEST_ASSUME_STRNE
#undef IUTEST_ASSUME_STRCASEEQ
#undef IUTEST_ASSUME_STRCASENE
#undef IUTEST_ASSUME_THROW
#undef IUTEST_ASSUME_NO_THROW
#undef IUTEST_ASSUME_ANY_THROW
#undef IUTEST_ASSUME_NO_FATAL_FAILURE
#undef IUTEST_ASSUME_PRED_FORMAT1
#undef IUTEST_ASSUME_PRED_FORMAT2
#undef IUTEST_ASSUME_PRED_FORMAT3
#undef IUTEST_ASSUME_PRED_FORMAT4
#undef IUTEST_ASSUME_PRED_FORMAT5
#undef IUTEST_ASSUME_PRED1
#undef IUTEST_ASSUME_PRED2
#undef IUTEST_ASSUME_PRED3
#undef IUTEST_ASSUME_PRED4
#undef IUTEST_ASSUME_PRED5

#ifdef IUTEST_OS_WINDOWS
#undef IUTEST_ASSUME_HRESULT_SUCCEEDED
#undef IUTEST_ASSUME_HRESULT_FAILED
#endif

#undef IUTEST_ASSUME_FAILURE

#endif

//======================================================================
// define
#define GTEST_ASSUME_FAILURE_(message) \
	return GTEST_MESSAGE_(message, ::testing::TestPartResult::kSuccess)

#define ASSUME_PRED_FORMAT1(pred_format, v1) \
	GTEST_PRED_FORMAT1_(pred_format, v1, GTEST_ASSUME_FAILURE_)
#define ASSUME_PRED1(pred, v1) \
	GTEST_PRED1_(pred, v1, GTEST_ASSUME_FAILURE_)

#define ASSUME_PRED_FORMAT2(pred_format, v1, v2) \
	GTEST_PRED_FORMAT2_(pred_format, v1, v2, GTEST_ASSUME_FAILURE_)
#define ASSUME_PRED2(pred, v1, v2) \
	GTEST_PRED2_(pred, v1, v2, GTEST_ASSUME_FAILURE_)

#define ASSUME_PRED_FORMAT3(pred_format, v1, v2, v3) \
	GTEST_PRED_FORMAT3_(pred_format, v1, v2, v3, GTEST_ASSUME_FAILURE_)
#define ASSUME_PRED3(pred, v1, v2, v3) \
	GTEST_PRED3_(pred, v1, v2, v3, GTEST_ASSUME_FAILURE_)

#define ASSUME_PRED_FORMAT4(pred_format, v1, v2, v3, v4) \
	GTEST_PRED_FORMAT4_(pred_format, v1, v2, v3, v4, GTEST_ASSUME_FAILURE_)
#define ASSUME_PRED4(pred, v1, v2, v3, v4) \
	GTEST_PRED4_(pred, v1, v2, v3, v4, GTEST_ASSUME_FAILURE_)

#define ASSUME_PRED_FORMAT5(pred_format, v1, v2, v3, v4, v5) \
	GTEST_PRED_FORMAT5_(pred_format, v1, v2, v3, v4, v5, GTEST_ASSUME_FAILURE_)
#define ASSUME_PRED5(pred, v1, v2, v3, v4, v5) \
	GTEST_PRED5_(pred, v1, v2, v3, v4, v5, GTEST_ASSUME_FAILURE_)


#define ASSUME_TRUE(condition)	GTEST_TEST_BOOLEAN_(condition, #condition, false, true, GTEST_ASSUME_FAILURE_)
#define ASSUME_FALSE(condition)	GTEST_TEST_BOOLEAN_(!(condition), #condition, true, false, GTEST_ASSUME_FAILURE_)

#define ASSUME_EQ(expected, actual) \
	ASSUME_PRED_FORMAT2(::testing::internal:: \
	EqHelper<GTEST_IS_NULL_LITERAL_(expected)>::Compare, \
	expected, actual)
#define ASSUME_NE(expected, actual) \
	ASSUME_PRED_FORMAT2(::testing::internal::CmpHelperNE, expected, actual)
#define ASSUME_LE(val1, val2) \
	ASSUME_PRED_FORMAT2(::testing::internal::CmpHelperLE, val1, val2)
#define ASSUME_LT(val1, val2) \
	ASSUME_PRED_FORMAT2(::testing::internal::CmpHelperLT, val1, val2)
#define ASSUME_GE(val1, val2) \
	ASSUME_PRED_FORMAT2(::testing::internal::CmpHelperGE, val1, val2)
#define ASSUME_GT(val1, val2) \
	ASSUME_PRED_FORMAT2(::testing::internal::CmpHelperGT, val1, val2)

#define ASSUME_STREQ(expected, actual) \
	ASSUME_PRED_FORMAT2(::testing::internal::CmpHelperSTREQ, expected, actual)
#define ASSUME_STRNE(s1, s2) \
	ASSUME_PRED_FORMAT2(::testing::internal::CmpHelperSTRNE, s1, s2)
#define ASSUME_STRCASEEQ(expected, actual) \
	ASSUME_PRED_FORMAT2(::testing::internal::CmpHelperSTRCASEEQ, expected, actual)
#define ASSUME_STRCASENE(s1, s2)\
	ASSUME_PRED_FORMAT2(::testing::internal::CmpHelperSTRCASENE, s1, s2)

#define ASSUME_FLOAT_EQ(expected, actual)\
	ASSUME_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<float>, \
	expected, actual)
#define ASSUME_DOUBLE_EQ(expected, actual)\
	ASSUME_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<double>, \
	expected, actual)
#define ASSUME_NEAR(val1, val2, abs_error)\
	ASSUME_PRED_FORMAT3(::testing::internal::DoubleNearPredFormat, \
	val1, val2, abs_error)

#define ASSUME_THROW(statement, expected_exception) \
	GTEST_TEST_THROW_(statement, expected_exception, GTEST_ASSUME_FAILURE_)
#define ASSUME_NO_THROW(statement) \
	GTEST_TEST_NO_THROW_(statement, GTEST_ASSUME_FAILURE_)
#define ASSUME_ANY_THROW(statement) \
	GTEST_TEST_ANY_THROW_(statement, GTEST_ASSUME_FAILURE_)

#define ASSUME_NO_FATAL_FAILURE(statement) \
	GTEST_TEST_NO_FATAL_FAILURE_(statement, GTEST_ASSUME_FAILURE_)

#define IUTEST_ASSUME_TRUE(...)				ASSUME_TRUE(!!(__VA_ARGS__))
#define IUTEST_ASSUME_FALSE(...)			ASSUME_FALSE((__VA_ARGS__))
#define IUTEST_ASSUME_EQ					ASSUME_EQ
#define IUTEST_ASSUME_NE(expected, actual)	ASSUME_PRED_FORMAT2(::testing::internal::NeHelper<GTEST_IS_NULL_LITERAL_(expected)>::Compare, expected, actual)
#define IUTEST_ASSUME_LT					ASSUME_LT
#define IUTEST_ASSUME_LE					ASSUME_LE
#define IUTEST_ASSUME_GT					ASSUME_GT
#define IUTEST_ASSUME_GE					ASSUME_GE
#define IUTEST_ASSUME_NULL(v)				ASSUME_EQ(NULL, v)
#define IUTEST_ASSUME_NOTNULL(v)			ASSUME_TRUE(NULL != (v))
#define IUTEST_ASSUME_SAME(v1, v2)			ASSUME_PRED_FORMAT2(::testing::internal::CmpHelperSame, v1, v2)
#define IUTEST_ASSUME_NEAR					ASSUME_NEAR
#define IUTEST_ASSUME_FLOAT_EQ				ASSUME_FLOAT_EQ
#define IUTEST_ASSUME_DOUBLE_EQ				ASSUME_DOUBLE_EQ
#define IUTEST_ASSUME_STREQ					ASSUME_STREQ
#define IUTEST_ASSUME_STRNE					ASSUME_STRNE
#define IUTEST_ASSUME_STRCASEEQ				ASSUME_STRCASEEQ
#define IUTEST_ASSUME_STRCASENE				ASSUME_STRCASENE
#define IUTEST_ASSUME_THROW(statement, expected_exception)	ASSUME_THROW((void)(statement), expected_exception)
#define IUTEST_ASSUME_NO_THROW(statement)	ASSUME_NO_THROW((void)(statement))
#define IUTEST_ASSUME_ANY_THROW(statement)	ASSUME_ANY_THROW((void)(statement))
#define IUTEST_ASSUME_NO_FATAL_FAILURE		ASSUME_NO_FATAL_FAILURE

#ifdef GTEST_OS_WINDOWS
# define ASSUME_HRESULT_SUCCEEDED(expr) \
	ASSUME_PRED_FORMAT1(::testing::internal::IsHRESULTSuccess, (expr))
# define ASSUME_HRESULT_FAILED(expr) \
	ASSUME_PRED_FORMAT1(::testing::internal::IsHRESULTFailure, (expr))

#  define IUTEST_ASSUME_HRESULT_SUCCEEDED	ASSUME_HRESULT_SUCCEEDED
#  define IUTEST_ASSUME_HRESULT_FAILED		ASSUME_HRESULT_FAILED
#endif

#define IUTEST_ASSUME_PRED1	ASSUME_PRED1
#define IUTEST_ASSUME_PRED2	ASSUME_PRED2
#define IUTEST_ASSUME_PRED3	ASSUME_PRED3
#define IUTEST_ASSUME_PRED4	ASSUME_PRED4
#define IUTEST_ASSUME_PRED5	ASSUME_PRED5

#define IUTEST_ASSUME_PRED_FORMAT1	ASSUME_PRED_FORMAT1
#define IUTEST_ASSUME_PRED_FORMAT2	ASSUME_PRED_FORMAT2
#define IUTEST_ASSUME_PRED_FORMAT3	ASSUME_PRED_FORMAT3
#define IUTEST_ASSUME_PRED_FORMAT4	ASSUME_PRED_FORMAT4
#define IUTEST_ASSUME_PRED_FORMAT5	ASSUME_PRED_FORMAT5

#define IUTEST_ASSUME_FAILURE	GTEST_ASSUME_FAILURE_

#endif	// !defined(IUTEST_USE_GTEST)

#define ASSUME_NULL		IUTEST_ASSUME_NULL
#define ASSUME_NOTNULL	IUTEST_ASSUME_NOTNULL
#define ASSUME_SAME		IUTEST_ASSUME_SAME

#endif	// INCG_IRIS_IUTEST_SWITCH_ASSUME_HPP_E22A3162_D581_4F4F_A344_F5D701B21F53_
