//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_default_printer.ipp
 * @brief		iris unit test デフォルト出力イベントリスナー ファイル
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
#ifndef INCG_IRIS_IUTEST_DEFAULT_PRINTER_IPP_77055C2B_AAE1_4944_A61C_26C58B04B37B_
#define INCG_IRIS_IUTEST_DEFAULT_PRINTER_IPP_77055C2B_AAE1_4944_A61C_26C58B04B37B_

//======================================================================
// include
#include "../listener/iutest_default_printer.hpp"

namespace iutest
{

IUTEST_IPP_INLINE void DefalutResultPrintListener::OnTestProgramStart(const UnitTest& test)
{
	IUTEST_UNUSED_VAR(test);
	// フィルタリング
	if( TestFlag::IsEnableFlag(TestFlag::FILTERING_TESTS) )
	{
		detail::iuConsole::color_output(detail::iuConsole::yellow, "Note: iutest filter = %s\n", TestEnv::test_filter());
	}
}
IUTEST_IPP_INLINE void DefalutResultPrintListener::OnTestIterationStart(const UnitTest& test
								, int iteration)
{
	if( IUTEST_FLAG(repeat) != 1 )
	{
		detail::iuConsole::output("\nRepeating all tests (iteration %d) . . .\n\n", iteration+1);
	}
	// シャッフルテスト
	if( TestFlag::IsEnableFlag(TestFlag::SHUFFLE_TESTS) )
	{
		detail::iuConsole::color_output(detail::iuConsole::yellow, "Note: Randomizing tests' orders with a seed of %u\n", test.random_seed());
	}
	detail::iuConsole::color_output(detail::iuConsole::green, "[==========] ");
	detail::iuConsole::output("Running %d tests from %d test cases.\n", test.test_to_run_count(), test.test_case_to_run_count() );
}
IUTEST_IPP_INLINE void DefalutResultPrintListener::OnEnvironmentsSetUpStart(const UnitTest& test)
{
	IUTEST_UNUSED_VAR(test);

	detail::iuConsole::color_output(detail::iuConsole::green, "[----------] ");
	detail::iuConsole::output("Global test environment set-up.\n" );
}
IUTEST_IPP_INLINE void DefalutResultPrintListener::OnEnvironmentsSetUpEnd(const UnitTest& test)
{
	IUTEST_UNUSED_VAR(test);
}
IUTEST_IPP_INLINE void DefalutResultPrintListener::OnTestCaseStart(const TestCase& test_case)
{
	detail::iuConsole::color_output(detail::iuConsole::green, "[----------] ");
	detail::iuConsole::output("%d tests from %s\n", test_case.test_to_run_count(), test_case.testcase_name_with_where().c_str() );
}
IUTEST_IPP_INLINE void DefalutResultPrintListener::OnTestStart(const TestInfo& test_info)
{
	detail::iuConsole::color_output(detail::iuConsole::green, "[ RUN      ] ");
	detail::iuConsole::output("%s.%s\n", test_info.test_case_name(), test_info.name());
}
IUTEST_IPP_INLINE void DefalutResultPrintListener::OnTestPartResult(const TestPartResult& test_part_result)
{
	//if( test_part_result.type() == TestPartResult::kSuccess ) return;
	detail::iuConsole::output(test_part_result.make_newline_message().c_str());
#ifdef _MSC_VER
#ifdef IUTEST_OS_WINDOWS_MOBILE
#else
	OutputDebugStringA(test_part_result.make_newline_message().c_str());
#endif
#endif
}
IUTEST_IPP_INLINE void DefalutResultPrintListener::OnTestRecordProperty(const TestProperty& test_property)
{
	detail::iuConsole::output("iutest record property:\n  %s=%s\n", test_property.key(), test_property.value());
}
IUTEST_IPP_INLINE void DefalutResultPrintListener::OnTestEnd(const TestInfo& test_info)
{
	if( test_info.HasFailure() )
	{
		detail::iuConsole::color_output(detail::iuConsole::red  , "[  FAILED  ] ");
		detail::iuConsole::output("%s.%s", test_info.test_case_name(), test_info.test_name_with_where().c_str());
	}
	else if( test_info.is_skipped() )
	{
		detail::iuConsole::color_output(detail::iuConsole::yellow,"[  SKIPPED ] ");
		detail::iuConsole::output("%s.%s", test_info.test_case_name(), test_info.name());
	}
	else
	{
		detail::iuConsole::color_output(detail::iuConsole::green, "[       OK ] ");
		detail::iuConsole::output("%s.%s", test_info.test_case_name(), test_info.name());
	}
	if( TestFlag::IsEnableFlag(TestFlag::PRINT_TIME) )
	{
#if defined(IUTEST_NOT_SUPPORT_STOPWATCH)
		detail::iuConsole::output(" (--ms)\n" );
#else
		detail::iuConsole::output(" (%dms)\n", test_info.elapsed_time());
#endif
	}
	else
	{
		detail::iuConsole::output("\n");
	}
}
IUTEST_IPP_INLINE void DefalutResultPrintListener::OnTestCaseEnd(const TestCase& test_case)
{
	detail::iuConsole::color_output(detail::iuConsole::green, "[----------] ");
#if !defined(IUTEST_NOT_SUPPORT_STOPWATCH)
	if( TestFlag::IsEnableFlag(TestFlag::PRINT_TIME) )
	{
		detail::iuConsole::output("%d tests from %s (%dms total)\n\n", test_case.test_to_run_count(), test_case.name(), test_case.elapsed_time() );
	}
	else
#endif
	{
		detail::iuConsole::output("%d tests from %s\n\n", test_case.test_to_run_count(), test_case.name() );
	}
}
IUTEST_IPP_INLINE void DefalutResultPrintListener::OnEnvironmentsTearDownStart(const UnitTest& test)
{
	IUTEST_UNUSED_VAR(test);

	detail::iuConsole::color_output(detail::iuConsole::green, "[----------] ");
	detail::iuConsole::output("Global test environment tear-down.\n" );
}
IUTEST_IPP_INLINE void DefalutResultPrintListener::OnEnvironmentsTearDownEnd(const UnitTest& test)
{
	IUTEST_UNUSED_VAR(test);
}
IUTEST_IPP_INLINE void DefalutResultPrintListener::OnTestIterationEnd(const UnitTest& test
								, int iteration)
{
	IUTEST_UNUSED_VAR(iteration);

	detail::iuConsole::color_output(detail::iuConsole::green, "[==========] ");
#if !defined(IUTEST_NOT_SUPPORT_STOPWATCH)
	if( TestFlag::IsEnableFlag(TestFlag::PRINT_TIME) )
	{
		detail::iuConsole::output("%d tests from %d testcase ran. (%dms total)\n", test.test_to_run_count(), test.test_case_to_run_count(), test.elapsed_time() );
	}
	else
#endif
	{
		detail::iuConsole::output("%d tests from %d testcase ran.\n", test.test_to_run_count(), test.test_case_to_run_count() );
	}

	{
		{
			detail::iuConsole::color_output(detail::iuConsole::green, "[  PASSED  ] ");
			detail::iuConsole::output("%d tests.\n", test.successful_test_count() );
		}
		{
			const int count = test.reportable_disabled_test_count();
			if( !TestFlag::IsEnableFlag(TestFlag::RUN_DISABLED_TESTS) && count > 0 )
			{
				detail::iuConsole::color_output(detail::iuConsole::yellow, "[ DISABLED ] ");
				detail::iuConsole::output("%d tests.\n", count );
			}
		}
		{
			const int count = test.reportable_test_run_skipped_count();
			if( count > 0 )
			{
				detail::iuConsole::color_output(detail::iuConsole::yellow, "[  SKIPPED ] ");
				detail::iuConsole::output("%d tests.\n", count );
			}
		}
		
		if( !test.Passed() )
		{
			const int failed_num = test.failed_test_count();
			detail::iuConsole::color_output(detail::iuConsole::red, "[  FAILED  ] ");
			detail::iuConsole::output("%d %s, listed below:\n", failed_num, failed_num == 1 ? "test" : "tests" );

			for( int i=0, count=test.total_test_case_count(); i < count; ++i )
			{
				const TestCase* testcase = test.GetTestCase(i);
				for( int j=0, info_count=testcase->total_test_count(); j < info_count; ++j )
				{
					const TestInfo* testinfo = testcase->GetTestInfo(j);
					if( testinfo->HasFailure() )
					{
						detail::iuConsole::color_output(detail::iuConsole::red, "[  FAILED  ] ");
						detail::iuConsole::output("%s.%s\n", testinfo->test_case_name(), testinfo->name());
					}
				}
				if( testcase->ad_hoc_testresult()->Failed() )
				{
					detail::iuConsole::color_output(detail::iuConsole::red, "[  FAILED  ] ");
					detail::iuConsole::output("%s at SetUpTestCase/TearDownTestCase\n", testcase->name());
				}
			}

			if( test.ad_hoc_testresult()->Failed() )
			{
				detail::iuConsole::color_output(detail::iuConsole::red, "[  FAILED  ] ");
				detail::iuConsole::output("other than\n");
			}
			detail::iuConsole::output("\n%d FAILED %s.\n", failed_num, failed_num == 1 ? "TEST" : "TESTS" );
		}
	}
}
IUTEST_IPP_INLINE void DefalutResultPrintListener::OnTestProgramEnd(const UnitTest& test)
{
	IUTEST_UNUSED_VAR(test);
}

}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_DEFAULT_PRINTER_IPP_77055C2B_AAE1_4944_A61C_26C58B04B37B_
