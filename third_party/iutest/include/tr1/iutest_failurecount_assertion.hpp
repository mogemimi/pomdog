//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_failurecount_assertion.hpp
 * @brief		iris unit test 失敗数アサーション 定義 ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_FAILURECOUNT_ASSERTION_HPP_DB1A509F_DD9A_4F70_A926_131B0B0F5438_
#define INCG_IRIS_IUTEST_FAILURECOUNT_ASSERTION_HPP_DB1A509F_DD9A_4F70_A926_131B0B0F5438_

//======================================================================
// define
#if defined(_MSC_VER)
// /ZI オプションだと __LINE__ が __LINE__Var+N(Nは番号) になりコンパイルエラーになるための対応
#  ifndef __LINE__Var
#    define __LINE__Var	0
#  endif
#endif

/**
 * @ingroup	IUTEST_EXPECT_
 * @brief	失敗数アサーション
 * @param	statements	= 複合ステートメント
 * @param	n = 失敗数上限値 (failed count < n)
*/
#define IUTEST_EXPECT_FAILURECOUNT_LT(statements, n) IUTEST_TEST_FAILURECOUNT_LT(statements, n, #statements, IUTEST_EXPECT_FAILURE)

/**
 * @ingroup	IUTEST_INFORM_
 * @brief	失敗数アサーション
 * @param	statements	= 複合ステートメント
 * @param	n = 失敗数上限値 (failed count < n)
*/
#define IUTEST_INFORM_FAILURECOUNT_LT(statements, n) IUTEST_TEST_FAILURECOUNT_LT(statements, n, #statements, IUTEST_INFORM_FAILURE)

/**
 * @ingroup	IUTEST_ASSUME_
 * @brief	失敗数アサーション
 * @param	statements	= 複合ステートメント
 * @param	n = 失敗数上限値 (failed count < n)
*/
#define IUTEST_ASSUME_FAILURECOUNT_LT(statements, n) IUTEST_TEST_FAILURECOUNT_LT(statements, n, #statements, IUTEST_ASSUME_FAILURE)

/**
 * @ingroup	IUTEST_ASSERT_
 * @brief	失敗数アサーション
 * @param	statements	= 複合ステートメント
 * @param	n = 失敗数上限値 (failed count < n)
*/
#define IUTEST_ASSERT_FAILURECOUNT_LT(statements, n) IUTEST_TEST_FAILURECOUNT_LT(statements, n, #statements, IUTEST_ASSERT_FAILURE)

/**
 * @private
 * @{
*/

#if IUTEST_HAS_LAMBDA_STATEMENTS

#if IUTEST_HAS_EXCEPTIONS && IUTEST_USE_THROW_ON_ASSERTION_FAILURE
#  define IIUT_STATEMENTS_EXECUTER(statements)	[&](){ try {	\
	::iutest::detail::ScopedDisableTestFailureBreak statements_failure_count_testflag_guard;	\
	statements;													\
	} catch(...) {}												\
	}()
#else
#  define IIUT_STATEMENTS_EXECUTER(statements)					\
	[&](){ ::iutest::detail::ScopedDisableTestFailureBreak statements_failure_count_testflag_guard; statements; }()
#endif

#define IUTEST_TEST_FAILURECOUNT_LT(statements, count, statements_str, on_failure)	\
	IUTEST_AMBIGUOUS_ELSE_BLOCKER_													\
	if( const ::iutest::AssertionResult statemnets_ar = [&]() -> ::iutest::AssertionResult {	\
		::iutest::detail::StatementsFailureCount statement_failure_checker;			\
		IIUT_STATEMENTS_EXECUTER(statements);										\
		return statement_failure_checker.GetResult(count, statements_str);			\
	}() )																			\
		;																			\
	else																			\
		on_failure(statemnets_ar.message())

#else

#if IUTEST_HAS_EXCEPTIONS && IUTEST_USE_THROW_ON_ASSERTION_FAILURE
#  define IIUT_STATEMENTS_EXECUTER(statements) struct IUTestStatementsExecuter {	\
	static void Execute() { ::iutest::detail::ScopedDisableTestFailureBreak guard;	\
	try { statements; } catch(...) {} }												\
	};																				\
	IUTestStatementsExecuter::Execute()
#else
#  define IIUT_STATEMENTS_EXECUTER(statements) struct IUTestStatementsExecuter {	\
	static void Execute() { ::iutest::detail::ScopedDisableTestFailureBreak guard;	\
		statements; }																\
	};																				\
	IUTestStatementsExecuter::Execute()
#endif

#define IUTEST_TEST_FAILURECOUNT_LT(statements, count, statements_str, on_failure)	\
	IUTEST_AMBIGUOUS_ELSE_BLOCKER_													\
	if( ::iutest::AssertionResult statemnets_ar = ::iutest::AssertionSuccess() ) {	\
		::iutest::detail::StatementsFailureCount statement_failure_checker;			\
		IIUT_STATEMENTS_EXECUTER(statements);										\
		::iutest::AssertionResult ar = statement_failure_checker.GetResult(count, statements_str);	\
		if( !ar ) {																	\
			statemnets_ar << ar.message();											\
			goto IUTEST_PP_CAT(iutest_label_test_failurecount_lt_, __LINE__);		\
		}																			\
	} else																			\
		IUTEST_PP_CAT(iutest_label_test_failurecount_lt_, __LINE__):				\
		on_failure(statemnets_ar.message())

#endif

/**
 * @}
*/

namespace iutest {
namespace detail
{

//======================================================================
// class

/**
 * @brief	statements check
*/
class StatementsFailureCount : public NewTestPartResultCheckHelper::Collector< NoTestPartResultReporter >
{
public:
	AssertionResult GetResult(size_t cnt, const char* stetements_str)
	{
		const size_t num = count();
		size_t n=0;
		for( size_t i=0; i < num; ++i )
		{
			if( GetTestPartResult(i).failed() )
			{
				++n;
			}
		}
		if( n < cnt ) return AssertionSuccess();
		AssertionResult ar = AssertionFailure() << stetements_str << "\nExpected: failure count less than " << cnt << "\n  Actual: " << n;
		ar << "\nReported results:";
		for( size_t i=0; i < num; ++i )
		{
			ar << "\n" << GetTestPartResult(i).message();
		}
		return ar;
	}
};

/**
 * @biref	フラグ変更スコープ
*/
class ScopedDisableTestFailureBreak : public TestFlag::ScopedGuard
{
public:
	ScopedDisableTestFailureBreak(void)
	{
		IUTEST_FLAG(throw_on_failure) = false;
		IUTEST_FLAG(break_on_failure) = false;
	}
};

}	// end of namespace detail
}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_FAILURECOUNT_ASSERTION_HPP_DB1A509F_DD9A_4F70_A926_131B0B0F5438_
