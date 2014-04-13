//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_spi.hpp
 * @brief		iris unit test spi 対応用 ファイル
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
 * @par			copyright
 * Copyright (C) 2012-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_SPI_HPP_
#define INCG_IRIS_IUTEST_SPI_HPP_

//======================================================================
// include
#include "iutest.hpp"

#if !defined(IUTEST_USE_GTEST)

#if defined(_MSC_VER)
// /ZI オプションだと __LINE__ が __LINE__Var+N(Nは番号) になりコンパイルエラーになるための対応
#  ifndef __LINE__Var
#    define __LINE__Var	0
#  endif
#endif

//======================================================================
// define
/**
 * @ingroup	IUTEST_ASSERT_
 * @brief	Fatal Failure 発生確認テスト
*/
#define IUTEST_ASSERT_FATAL_FAILURE(statement, substr)		IUTEST_TEST_FATAL_FAILURE_(statement, #statement, substr, IUTEST_ASSERT_FAILURE)

/**
 * @ingroup	IUTEST_ASSERT_
 * @brief	Non Fatal Failure 発生確認テスト
*/
#define IUTEST_ASSERT_NONFATAL_FAILURE(statement, substr)	IUTEST_TEST_NONFATAL_FAILURE_(statement, #statement, substr, IUTEST_ASSERT_FAILURE)

/**
 * @ingroup	IUTEST_EXPECT_
 * @brief	Fatal Failure 発生確認テスト
*/
#define IUTEST_EXPECT_FATAL_FAILURE(statement, substr)		IUTEST_TEST_FATAL_FAILURE_(statement, #statement, substr, IUTEST_EXPECT_FAILURE)

/**
 * @ingroup	IUTEST_EXPECT_
 * @brief	Non Fatal Failure 発生確認テスト
*/
#define IUTEST_EXPECT_NONFATAL_FAILURE(statement, substr)	IUTEST_TEST_NONFATAL_FAILURE_(statement, #statement, substr, IUTEST_EXPECT_FAILURE)


/**
 * @private
 * @{
*/

#if IUTEST_HAS_SPI_LAMBDA_SUPPORT

#if IUTEST_HAS_EXCEPTIONS && IUTEST_USE_THROW_ON_ASSERTION_FAILURE
#  define IIUT_STATEMENT_EXECUTER(statement)	[&](){ try {	\
	::iutest::detail::ScopedSPITestFlag guard;					\
	statement;													\
	} catch(...) {}												\
	}()
#else
#  define IIUT_STATEMENT_EXECUTER(statement)	[&](){ ::iutest::detail::ScopedSPITestFlag guard; statement; }()
#endif

#define IUTEST_TEST_FATAL_FAILURE_(statement, text, substr, on_failure)				\
	if( ::iutest::AssertionResult iutest_ar = [&]() -> ::iutest::AssertionResult {	\
		::iutest::detail::SPIFailureChecker<										\
			::iutest::TestPartResult::kFatalFailure> iutest_failure_checker;		\
		IIUT_STATEMENT_EXECUTER(statement);											\
		return iutest_failure_checker.GetResult(substr);							\
	}() )																			\
		;																			\
	else																			\
		on_failure(iutest_ar.message())

#define IUTEST_TEST_NONFATAL_FAILURE_(statement, text, substr, on_failure)			\
	if( ::iutest::AssertionResult iutest_ar = [&]() -> ::iutest::AssertionResult {	\
		::iutest::detail::SPIFailureChecker<										\
			::iutest::TestPartResult::kNonFatalFailure> iutest_failure_checker;		\
		IIUT_STATEMENT_EXECUTER(statement);											\
		return iutest_failure_checker.GetResult(substr);							\
	}() )																			\
		;																			\
	else																			\
		on_failure(iutest_ar.message())

#else

#if IUTEST_HAS_EXCEPTIONS && IUTEST_USE_THROW_ON_ASSERTION_FAILURE
#  define IIUT_STATEMENT_EXECUTER(statement)	struct IUTestFatalFailureStatement {	\
	static void Execute() { ::iutest::detail::ScopedSPITestFlag guard;					\
	try { statement; } catch(...) {} }													\
	};																					\
	IUTestFatalFailureStatement::Execute()
#else
#  define IIUT_STATEMENT_EXECUTER(statement)	struct IUTestFatalFailureStatement {	\
	static void Execute() { ::iutest::detail::ScopedSPITestFlag guard; statement; }		\
	};																					\
	IUTestFatalFailureStatement::Execute()
#endif

#define IUTEST_TEST_FATAL_FAILURE_(statement, text, substr, on_failure)			\
	IUTEST_AMBIGUOUS_ELSE_BLOCKER_												\
	if( ::iutest::AssertionResult iutest_ar = ::iutest::AssertionSuccess() ) {	\
		::iutest::detail::SPIFailureChecker<									\
			::iutest::TestPartResult::kFatalFailure> iutest_failure_checker;	\
		IIUT_STATEMENT_EXECUTER(statement);										\
		::iutest::AssertionResult ar = iutest_failure_checker.GetResult(substr);\
		if( !ar ) {																\
			iutest_ar << ar.message();											\
			goto IUTEST_PP_CAT(iutest_label_test_fatalfailure_, __LINE__);		\
		}																		\
	} else																		\
		IUTEST_PP_CAT(iutest_label_test_fatalfailure_, __LINE__):				\
		on_failure(iutest_ar.message())

#define IUTEST_TEST_NONFATAL_FAILURE_(statement, text, substr, on_failure)		\
	IUTEST_AMBIGUOUS_ELSE_BLOCKER_												\
	if( ::iutest::AssertionResult iutest_ar = ::iutest::AssertionSuccess() ) {	\
		::iutest::detail::SPIFailureChecker<									\
			::iutest::TestPartResult::kNonFatalFailure> iutest_failure_checker;	\
		IIUT_STATEMENT_EXECUTER(statement);										\
		::iutest::AssertionResult ar = iutest_failure_checker.GetResult(substr);\
		if( !ar ) {																\
			iutest_ar << ar.message();											\
			goto IUTEST_PP_CAT(iutest_label_test_fatalfailure_, __LINE__);		\
		}																		\
	} else																		\
		IUTEST_PP_CAT(iutest_label_test_fatalfailure_, __LINE__):				\
		on_failure(iutest_ar.message())

//		IUTEST_SUPPRESS_UNREACHABLE_CODE_WARNING(statement);

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
 * @brief	SPI 用リポーター
*/
class FakeTestPartResultReporter : public TestPartResultReporterInterface
{
public:
	virtual ~FakeTestPartResultReporter(void) {}
	virtual void ReportTestPartResult(const TestPartResult& result) IUTEST_CXX_OVERRIDE
	{
		IUTEST_UNUSED_VAR(result);
	}
};

/**
 * @brief	SPI チェッカー
*/
template<TestPartResult::Type Type>
class SPIFailureChecker
	: public NewTestPartResultCheckHelper::Collector<FakeTestPartResultReporter>
{
public:
	AssertionResult GetResult(const ::std::string& substr)
	{
		const char* expected = Type == TestPartResult::kFatalFailure ? "1 fatal failure" : "1 non-fatal failure";
		const size_t num = count();
		if( num != 1 )
		{
			AssertionResult ar = AssertionFailure() << "error: Expected: " << expected
				<< "\n  Actual: " << num << " failures\n";
			for( size_t i=0; i < num; ++i )
			{
				ar << GetTestPartResult(i);
			}
			return ar;
		}

		const TestPartResult& tr = GetTestPartResult(0);
		if( tr.type() != Type )
		{
			return AssertionFailure() << "error: Expected: " << expected
				<< "\"\n  Actual:\n" << tr;
		}

		if( strstr(tr.message(), substr.c_str()) == NULL )
		{
			return AssertionFailure() << "error: Expected: " << expected
				<< "containing \"" << substr
				<< "\"\n  Actual:\n" << tr;
		}
		return AssertionSuccess();
	}
};

/**
 * @biref	フラグ変更スコープ
*/
class ScopedSPITestFlag : public TestFlag::ScopedGuard
{
public:
	ScopedSPITestFlag(void)
	{
		IUTEST_FLAG(throw_on_failure) = false;
		IUTEST_FLAG(break_on_failure) = false;
	}
};

}	// end of namespace detail
}	// end of namespace iutest

#else

#include "gtest/iutest_spi_switch.hpp"

#endif

#endif // INCG_IRIS_IUTEST_SPI_HPP_
