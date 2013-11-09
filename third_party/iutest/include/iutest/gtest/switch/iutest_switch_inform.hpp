//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_switch_inform.hpp
 * @brief		INFORM フレーバー切り替え ファイル
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
#ifndef INCG_IRIS_iutest_switch_inform_HPP_F4AD6E1B_4B1F_40ee_A0CC_D454587B2C39_
#define INCG_IRIS_iutest_switch_inform_HPP_F4AD6E1B_4B1F_40ee_A0CC_D454587B2C39_

#if !defined(IUTEST_USE_GTEST)

//======================================================================
// define
// INFORM
#define INFORM_TRUE		IUTEST_INFORM_TRUE
#define INFORM_FALSE	IUTEST_INFORM_FALSE
#define INFORM_EQ		IUTEST_INFORM_EQ
#define INFORM_NE		IUTEST_INFORM_NE
#define INFORM_LT		IUTEST_INFORM_LT
#define INFORM_LE		IUTEST_INFORM_LE
#define INFORM_GT		IUTEST_INFORM_GT
#define INFORM_GE		IUTEST_INFORM_GE
#define INFORM_NEAR		IUTEST_INFORM_NEAR
#define INFORM_FLOAT_EQ		IUTEST_INFORM_FLOAT_EQ
#define INFORM_DOUBLE_EQ	IUTEST_INFORM_DOUBLE_EQ
#define INFORM_STREQ		IUTEST_INFORM_STREQ
#define INFORM_STRNE		IUTEST_INFORM_STRNE
#define INFORM_STRCASEEQ	IUTEST_INFORM_STRCASEEQ
#define INFORM_STRCASENE	IUTEST_INFORM_STRCASENE
#define INFORM_THROW		IUTEST_INFORM_THROW
#define INFORM_NO_THROW		IUTEST_INFORM_NO_THROW
#define INFORM_ANY_THROW	IUTEST_INFORM_ANY_THROW
#define INFORM_NO_FATAL_FAILURE	IUTEST_INFORM_NO_FATAL_FAILURE

#ifdef IUTEST_OS_WINDOWS
#  define INFORM_HRESULT_SUCCEEDED	IUTEST_INFORM_HRESULT_SUCCEEDED
#  define INFORM_HRESULT_FAILED		IUTEST_INFORM_HRESULT_FAILED
#endif

#define INFORM_PRED1	IUTEST_INFORM_PRED1
#define INFORM_PRED2	IUTEST_INFORM_PRED2
#define INFORM_PRED3	IUTEST_INFORM_PRED3
#define INFORM_PRED4	IUTEST_INFORM_PRED4
#define INFORM_PRED5	IUTEST_INFORM_PRED5

#define INFORM_PRED_FORMAT1	IUTEST_INFORM_PRED_FORMAT1
#define INFORM_PRED_FORMAT2	IUTEST_INFORM_PRED_FORMAT2
#define INFORM_PRED_FORMAT3	IUTEST_INFORM_PRED_FORMAT3
#define INFORM_PRED_FORMAT4	IUTEST_INFORM_PRED_FORMAT4
#define INFORM_PRED_FORMAT5	IUTEST_INFORM_PRED_FORMAT5

#else

//======================================================================
// undef
#ifdef INCG_IRIS_iutest_HPP_

#undef IUTEST_INFORM_TRUE
#undef IUTEST_INFORM_FALSE
#undef IUTEST_INFORM_EQ
#undef IUTEST_INFORM_NE
#undef IUTEST_INFORM_LT
#undef IUTEST_INFORM_LE
#undef IUTEST_INFORM_GT
#undef IUTEST_INFORM_GE
#undef IUTEST_INFORM_NEAR
#undef IUTEST_INFORM_NULL
#undef IUTEST_INFORM_NOTNULL
#undef IUTEST_INFORM_SAME
#undef IUTEST_INFORM_FLOAT_EQ
#undef IUTEST_INFORM_DOUBLE_EQ
#undef IUTEST_INFORM_STREQ
#undef IUTEST_INFORM_STRNE
#undef IUTEST_INFORM_STRLNEQ
#undef IUTEST_INFORM_STRCASEEQ
#undef IUTEST_INFORM_STRCASENE
#undef IUTEST_INFORM_THROW
#undef IUTEST_INFORM_NO_THROW
#undef IUTEST_INFORM_ANY_THROW
#undef IUTEST_INFORM_NO_FATAL_FAILURE
#undef IUTEST_INFORM_PRED_FORMAT1
#undef IUTEST_INFORM_PRED_FORMAT2
#undef IUTEST_INFORM_PRED_FORMAT3
#undef IUTEST_INFORM_PRED_FORMAT4
#undef IUTEST_INFORM_PRED_FORMAT5
#undef IUTEST_INFORM_PRED1
#undef IUTEST_INFORM_PRED2
#undef IUTEST_INFORM_PRED3
#undef IUTEST_INFORM_PRED4
#undef IUTEST_INFORM_PRED5

#ifdef IUTEST_OS_WINDOWS
#undef IUTEST_INFORM_HRESULT_SUCCEEDED
#undef IUTEST_INFORM_HRESULT_FAILED
#endif

#undef IUTEST_INFORM_FAILURE

#endif

//======================================================================
// define
// INFORM 対応のために、初期化関数を呼ぶ
#undef RUN_ALL_TESTS
#define RUN_ALL_TESTS()			(::testing::inform_support::AppendInformSupportPrinter(), (::testing::UnitTest::GetInstance()->Run()))

//	INFORM 対応
#define GTEST_INFORM_FAILURE_(message) \
	GTEST_MESSAGE_(message, ::testing::TestPartResult::kSuccess)

#define INFORM_PRED_FORMAT1(pred_format, v1) \
	GTEST_PRED_FORMAT1_(pred_format, v1, GTEST_INFORM_FAILURE_)
#define INFORM_PRED1(pred, v1) \
	GTEST_PRED1_(pred, v1, GTEST_INFORM_FAILURE_)

#define INFORM_PRED_FORMAT2(pred_format, v1, v2) \
	GTEST_PRED_FORMAT2_(pred_format, v1, v2, GTEST_INFORM_FAILURE_)
#define INFORM_PRED2(pred, v1, v2) \
	GTEST_PRED2_(pred, v1, v2, GTEST_INFORM_FAILURE_)

#define INFORM_PRED_FORMAT3(pred_format, v1, v2, v3) \
	GTEST_PRED_FORMAT3_(pred_format, v1, v2, v3, GTEST_INFORM_FAILURE_)
#define INFORM_PRED3(pred, v1, v2, v3) \
	GTEST_PRED3_(pred, v1, v2, v3, GTEST_INFORM_FAILURE_)

#define INFORM_PRED_FORMAT4(pred_format, v1, v2, v3, v4) \
	GTEST_PRED_FORMAT4_(pred_format, v1, v2, v3, v4, GTEST_INFORM_FAILURE_)
#define INFORM_PRED4(pred, v1, v2, v3, v4) \
	GTEST_PRED4_(pred, v1, v2, v3, v4, GTEST_INFORM_FAILURE_)

#define INFORM_PRED_FORMAT5(pred_format, v1, v2, v3, v4, v5) \
	GTEST_PRED_FORMAT5_(pred_format, v1, v2, v3, v4, v5, GTEST_INFORM_FAILURE_)
#define INFORM_PRED5(pred, v1, v2, v3, v4, v5) \
	GTEST_PRED5_(pred, v1, v2, v3, v4, v5, GTEST_INFORM_FAILURE_)


#define INFORM_TRUE(condition)	GTEST_TEST_BOOLEAN_(condition, #condition, false, true, GTEST_INFORM_FAILURE_)
#define INFORM_FALSE(condition)	GTEST_TEST_BOOLEAN_(!(condition), #condition, true, false, GTEST_INFORM_FAILURE_)

#define INFORM_EQ(expected, actual) \
	INFORM_PRED_FORMAT2(::testing::internal:: \
	EqHelper<GTEST_IS_NULL_LITERAL_(expected)>::Compare, \
	expected, actual)
#define INFORM_NE(expected, actual) \
	INFORM_PRED_FORMAT2(::testing::internal::CmpHelperNE, expected, actual)
#define INFORM_LE(val1, val2) \
	INFORM_PRED_FORMAT2(::testing::internal::CmpHelperLE, val1, val2)
#define INFORM_LT(val1, val2) \
	INFORM_PRED_FORMAT2(::testing::internal::CmpHelperLT, val1, val2)
#define INFORM_GE(val1, val2) \
	INFORM_PRED_FORMAT2(::testing::internal::CmpHelperGE, val1, val2)
#define INFORM_GT(val1, val2) \
	INFORM_PRED_FORMAT2(::testing::internal::CmpHelperGT, val1, val2)

#define INFORM_STREQ(expected, actual) \
	INFORM_PRED_FORMAT2(::testing::internal::CmpHelperSTREQ, expected, actual)
#define INFORM_STRNE(s1, s2) \
	INFORM_PRED_FORMAT2(::testing::internal::CmpHelperSTRNE, s1, s2)
#define INFORM_STRCASEEQ(expected, actual) \
	INFORM_PRED_FORMAT2(::testing::internal::CmpHelperSTRCASEEQ, expected, actual)
#define INFORM_STRCASENE(s1, s2)\
	INFORM_PRED_FORMAT2(::testing::internal::CmpHelperSTRCASENE, s1, s2)

#define INFORM_FLOAT_EQ(expected, actual)\
	INFORM_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<float>, \
	expected, actual)
#define INFORM_DOUBLE_EQ(expected, actual)\
	INFORM_PRED_FORMAT2(::testing::internal::CmpHelperFloatingPointEQ<double>, \
	expected, actual)
#define INFORM_NEAR(val1, val2, abs_error)\
	INFORM_PRED_FORMAT3(::testing::internal::DoubleNearPredFormat, \
	val1, val2, abs_error)

#define INFORM_THROW(statement, expected_exception) \
	GTEST_TEST_THROW_(statement, expected_exception, GTEST_INFORM_FAILURE_)
#define INFORM_NO_THROW(statement) \
	GTEST_TEST_NO_THROW_(statement, GTEST_INFORM_FAILURE_)
#define INFORM_ANY_THROW(statement) \
	GTEST_TEST_ANY_THROW_(statement, GTEST_INFORM_FAILURE_)

#define INFORM_NO_FATAL_FAILURE(statement) \
	GTEST_TEST_NO_FATAL_FAILURE_(statement, GTEST_INFORM_FAILURE_)

#define IUTEST_INFORM_TRUE					INFORM_TRUE
#define IUTEST_INFORM_FALSE					INFORM_FALSE
#define IUTEST_INFORM_EQ					INFORM_EQ
#define IUTEST_INFORM_NE(expected, actual)	INFORM_PRED_FORMAT2(::testing::internal::NeHelper<GTEST_IS_NULL_LITERAL_(expected)>::Compare, expected, actual)
#define IUTEST_INFORM_LT					INFORM_LT
#define IUTEST_INFORM_LE					INFORM_LE
#define IUTEST_INFORM_GT					INFORM_GT
#define IUTEST_INFORM_GE					INFORM_GE
#define IUTEST_INFORM_NULL(v)				INFORM_EQ(NULL, v)
#define IUTEST_INFORM_NOTNULL(v)			INFORM_TRUE(NULL != (v))
#define IUTEST_INFORM_SAME(v1, v2)			INFORM_PRED_FORMAT2(::testing::internal::CmpHelperSame, v1, v2)
#define IUTEST_INFORM_NEAR					INFORM_NEAR
#define IUTEST_INFORM_FLOAT_EQ				INFORM_FLOAT_EQ
#define IUTEST_INFORM_DOUBLE_EQ				INFORM_DOUBLE_EQ
#define IUTEST_INFORM_STREQ					INFORM_STREQ
#define IUTEST_INFORM_STRNE					INFORM_STRNE
#define IUTEST_INFORM_STRLNEQ(len, v2)		INFORM_PRED_FORMAT2(iuutil::CmpHelperSTRLNEQ, len, v2)
#define IUTEST_INFORM_STRCASEEQ				INFORM_STRCASEEQ
#define IUTEST_INFORM_STRCASENE				INFORM_STRCASENE
#define IUTEST_INFORM_NO_THROW(statement)	INFORM_NO_THROW((void)(statement))
#define IUTEST_INFORM_ANY_THROW(statement)	INFORM_ANY_THROW((void)(statement))
#define IUTEST_INFORM_NO_FATAL_FAILURE		INFORM_NO_FATAL_FAILURE

#ifdef GTEST_OS_WINDOWS
# define INFORM_HRESULT_SUCCEEDED(expr) \
	INFORM_PRED_FORMAT1(::testing::internal::IsHRESULTSuccess, (expr))
# define INFORM_HRESULT_FAILED(expr) \
	INFORM_PRED_FORMAT1(::testing::internal::IsHRESULTFailure, (expr))

#  define IUTEST_INFORM_HRESULT_SUCCEEDED	INFORM_HRESULT_SUCCEEDED
#  define IUTEST_INFORM_HRESULT_FAILED		INFORM_HRESULT_FAILED
#endif

#define IUTEST_INFORM_PRED1	INFORM_PRED1
#define IUTEST_INFORM_PRED2	INFORM_PRED2
#define IUTEST_INFORM_PRED3	INFORM_PRED3
#define IUTEST_INFORM_PRED4	INFORM_PRED4
#define IUTEST_INFORM_PRED5	INFORM_PRED5

#define IUTEST_INFORM_PRED_FORMAT1	INFORM_PRED_FORMAT1
#define IUTEST_INFORM_PRED_FORMAT2	INFORM_PRED_FORMAT2
#define IUTEST_INFORM_PRED_FORMAT3	INFORM_PRED_FORMAT3
#define IUTEST_INFORM_PRED_FORMAT4	INFORM_PRED_FORMAT4
#define IUTEST_INFORM_PRED_FORMAT5	INFORM_PRED_FORMAT5

#define IUTEST_INFORM_FAILURE	GTEST_INFORM_FAILURE_

//======================================================================
// class
namespace testing
{

namespace inform_support
{
	class DefaultPrinter : public ::testing::EmptyTestEventListener
	{
	private:
		virtual void OnTestPartResult(const TestPartResult& test_part_result)
		{
			if( ::testing::UnitTest::GetInstance()->listeners().default_result_printer() == NULL ) return;

			if( test_part_result.type() == TestPartResult::kSuccess )
			{
				// 成功のときに出力
				TestPartResult tmp(TestPartResult::kNonFatalFailure
					, test_part_result.file_name()
					, test_part_result.line_number()
					, test_part_result.message());
				::testing::UnitTest::GetInstance()->listeners().default_result_printer()->OnTestPartResult(tmp);
			}
		}
	};
	inline void	AppendInformSupportPrinter(void)
	{
		// INFORM 用に printer を追加
		::testing::UnitTest::GetInstance()->listeners().Append( new DefaultPrinter() );
	}
}

}

#endif

#endif
