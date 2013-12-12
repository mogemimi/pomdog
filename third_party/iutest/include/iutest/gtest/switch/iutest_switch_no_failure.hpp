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
#ifndef INCG_IRIS_IUTEST_SWITCH_NO_FAILURE_HPP_5D6B75EE_F445_4730_8FEA_FB6249BCBD9C_
#define INCG_IRIS_IUTEST_SWITCH_NO_FAILURE_HPP_5D6B75EE_F445_4730_8FEA_FB6249BCBD9C_

#if !defined(IUTEST_USE_GTEST)

//======================================================================
// define
#define ASSERT_NO_FAILURE	IUTEST_ASSERT_NO_FAILURE
#define EXPECT_NO_FAILURE	IUTEST_EXPECT_NO_FAILURE
#define INFORM_NO_FAILURE	IUTEST_INFORM_NO_FAILURE
#define ASSUME_NO_FAILURE	IUTEST_ASSUME_NO_FAILURE

#else

//======================================================================
// undef
#ifdef INCG_IRIS_IUTEST_HPP_

#undef IUTEST_ASSERT_NO_FAILURE
#undef IUTEST_EXPECT_NO_FAILURE
#undef IUTEST_INFORM_NO_FAILURE
#undef IUTEST_ASSUME_NO_FAILURE

#endif

//======================================================================
// define
#define IUTEST_ASSERT_NO_FAILURE	ASSERT_NO_FAILURE
#define IUTEST_EXPECT_NO_FAILURE	EXPECT_NO_FAILURE
#define IUTEST_INFORM_NO_FAILURE	INFORM_NO_FAILURE
#define IUTEST_ASSUME_NO_FAILURE	ASSUME_NO_FAILURE

#define ASSERT_NO_FAILURE(statement)	GTEST_TEST_NO_FAILURE_(statement, GTEST_FATAL_FAILURE_)
#define EXPECT_NO_FAILURE(statement)	GTEST_TEST_NO_FAILURE_(statement, GTEST_NONFATAL_FAILURE_)
#define INFORM_NO_FAILURE(statement)	GTEST_TEST_NO_FAILURE_(statement, GTEST_INFORM_FAILURE_)
#define ASSUME_NO_FAILURE(statement)	GTEST_TEST_NO_FAILURE_(statement, GTEST_ASSUME_FAILURE_)

#define GTEST_TEST_NO_FAILURE_(statement, fail)							\
	GTEST_AMBIGUOUS_ELSE_BLOCKER_										\
	if(::testing::internal::AlwaysTrue()) {								\
		::testing::internal::HasNewFailureHelper gtest_failure_checker;	\
		GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement);		\
		if(gtest_failure_checker.has_new_failure()) {					\
			goto GTEST_CONCAT_TOKEN_(gtest_label_testnofail_, __LINE__);\
		}																\
	} else																\
		GTEST_CONCAT_TOKEN_(gtest_label_testnofail_, __LINE__):			\
			fail("Expected: " #statement " doesn't generate new "		\
					"failures in the current thread.\n"					\
					"  Actual: it does.")

//======================================================================
// class
namespace testing
{

namespace internal
{

class HasNewFailureHelper : public HasNewFatalFailureHelper
{
public:
	HasNewFailureHelper(void) : m_has_new_failure(false) 
	{
	}
	virtual ~HasNewFailureHelper(void)
	{
	}
public:
	virtual void ReportTestPartResult(const TestPartResult& result)
	{
		if( result.failed() )
		{
			m_has_new_failure = true;
		}
	}
	bool has_new_failure() const { return m_has_new_failure; }

private:
	bool m_has_new_failure;
};

}	// end of namespace internal
}	// end of namespace testing

#endif

#endif
