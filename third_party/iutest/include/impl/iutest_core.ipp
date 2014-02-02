//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_core.ipp
 * @brief		iris unit test core ファイル
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
#ifndef INCG_IRIS_IUTEST_CORE_IPP_33560431_783B_4AE7_8FC8_6E78355CAF87_
#define INCG_IRIS_IUTEST_CORE_IPP_33560431_783B_4AE7_8FC8_6E78355CAF87_

//======================================================================
// include
#include "../iutest_core.hpp"

namespace iutest
{

IUTEST_IPP_INLINE int UnitTest::reportable_test_count(void) const
{
	return detail::SumOverList(m_testcases, &TestCase::reportable_test_count);
}

IUTEST_IPP_INLINE int UnitTest::failed_test_count(void) const
{
	return detail::SumOverList(m_testcases, &TestCase::failed_test_count);
}

IUTEST_IPP_INLINE int UnitTest::reportable_disabled_test_count(void) const
{
	return detail::SumOverList(m_testcases, &TestCase::reportable_disabled_test_count);
}

IUTEST_IPP_INLINE int UnitTest::successful_test_count(void) const
{
	return detail::SumOverList(m_testcases, &TestCase::successful_test_count);
}

IUTEST_IPP_INLINE int UnitTest::skip_test_count(void) const
{
	return detail::SumOverList(m_testcases, &TestCase::skip_test_count);
}

IUTEST_IPP_INLINE int UnitTest::reportable_skip_test_count(void) const
{
	return detail::SumOverList(m_testcases, &TestCase::reportable_skip_test_count);
}

IUTEST_IPP_INLINE int UnitTest::test_run_skipped_count(void) const
{
	return detail::SumOverList(m_testcases, &TestCase::test_run_skipped_count);
}

IUTEST_IPP_INLINE int UnitTest::reportable_test_run_skipped_count(void) const
{
	return detail::SumOverList(m_testcases, &TestCase::reportable_test_run_skipped_count);
}

IUTEST_IPP_INLINE int UnitTest::test_case_to_run_count(void) const
{
	return detail::CountIfOverList(m_testcases, &TestCase::should_run);
}

IUTEST_IPP_INLINE int UnitTest::successful_test_case_count(void) const
{
	return detail::CountIfOverList(m_testcases, &TestCase::Passed);
}

IUTEST_IPP_INLINE int UnitTest::failed_test_case_count(void) const
{
	return detail::CountIfOverList(m_testcases, &TestCase::Failed);
}

IUTEST_IPP_INLINE bool UnitTest::Passed(void) const
{
	if( m_ad_hoc_testresult.Failed() )
	{
		return false;
	}
	for( iuTestCases::const_iterator it=m_testcases.begin(), end=m_testcases.end(); it != end; ++it )
	{
		if( (it)->Failed() )
		{
			return false;
		}
	}
	return true;
}

IUTEST_IPP_INLINE int UnitTest::Run(void)
{
	if( m_init_iutest_count == 0 )
	{
//#if IUTEST_HAS_PARAM_TEST
//		if( m_param_testcase_holder.count() )
//#endif
		{
			detail::iuConsole::output("This test program did NOT call ::iutest::InitIrisUnitTest before calling IUTEST_RUN_ALL_TESTS().  Please fix it.\n");
			return 1;
		}
	}

	if( !PreRunner() )
	{
		return 0;
	}

	int ret = 1;
#if IUTEST_HAS_EXCEPTIONS
	if( TestFlag::IsEnableFlag(TestFlag::CATCH_EXCEPTION_GLOBAL) )
	{
		try
		{
#if IUTEST_HAS_SEH
			ret = RunOnMSC();
#else
			ret = RunImpl();
#endif
		}
		//catch( ::std::exception& e )
		//{
		//	iutest::AssertionHelper(NULL, -1, detail::FormatCxxException(e.what()), TestPartResult::kFatalFailure) = AssertionHelper::Fixed();
		//}
		catch( ... )
		{
		}
	}
	else
#endif
	{
		ret = RunImpl();
	}
	return ret;
}

#if IUTEST_HAS_EXCEPTIONS && IUTEST_HAS_SEH
IUTEST_IPP_INLINE int	UnitTest::RunOnMSC(void)
{
	_EXCEPTION_POINTERS* ep = NULL;
	int ret = 1;
	__try
	{
		ret = RunImpl();
	}
	__except (ep = GetExceptionInformation()
#if IUTEST_HAS_MINIDUMP
		, detail::MiniDump::Create("minidump.dump", ep)
#endif
		, detail::seh_exception::should_process_through_break_and_cppexceptions(GetExceptionCode()))
	{
		detail::seh_exception::translator(GetExceptionCode(), ep);
	}
	return ret;
}
#endif

IUTEST_IPP_INLINE int UnitTest::RunImpl(void)
{
	m_repeat_counter = 0;
	int repeat = TestEnv::get_repeat_count();
	if( repeat == 0 )
	{
		return 0;
	}

	m_start_timestamp = detail::GetTimeInMillis();
	TestProgramStart();

	bool result = true;
	while(repeat)
	{
		SetUpTestIteration();

		listeners().OnTestIterationStart(*this, m_repeat_counter);
		if( !RunOnce() )
		{
			result = false;
		}
		listeners().OnTestIterationEnd(*this, m_repeat_counter);

		++m_repeat_counter;
		if( repeat > 0 )
		{
			--repeat;
		}
	}

	TestProgramEnd();

	return result ? 0 : 1;
}

IUTEST_IPP_INLINE bool	UnitTest::RunOnce(void)
{
	m_elapsedmsec = 0;

	// テスト結果のクリア
	ClearNonAdHocTestResult();

	// 実行対象のテストがない場合は何もしない
	if( test_to_run_count() == 0 )
	{
		return Passed();
	}

	// シャッフル
	if( TestFlag::IsEnableFlag(TestFlag::SHUFFLE_TESTS) )
	{
		detail::RandomShuffle(m_testcases, TestEnv::genrand());
	}

	// グローバル環境セット
	EnvironmentSetUp();

	if( m_ad_hoc_testresult.HasFatalFailure() )
	{
		return false;
	}

	{
		detail::iuStopWatch sw;
		sw.start();
		for( iuTestCases::iterator it=m_testcases.begin(), end=m_testcases.end(); it != end; ++it )
		{
			if( !(it)->should_run() )
			{
				continue;
			}

			m_current_testcase = it;
			(it)->Run();
			m_current_testcase = NULL;
		}
		m_elapsedmsec = sw.stop();
	}

	// グローバル環境セット
	EnvironmentTearDown();

	return Passed();
}

IUTEST_IPP_INLINE void	UnitTest::TestProgramStart(void)
{
#if 0
#if IUTEST_HAS_EXCEPTIONS && defined(_MSC_VER) && IUTEST_HAS_SEH
	// /EHa オプションが必要
	if( TestFlag::IsEnableFlag(TestFlag::CATCH_EXCEPTION) )
	{
		// SEH 例外に対応する
		_set_se_translator(detail::seh_exception::translator);
	}
#endif
#endif

	// フィルタリング
	m_should_run_num = 0;
	m_disable_num = 0;
	for( iuTestCases::iterator it = m_testcases.begin(), end=m_testcases.end(); it != end; ++it )
	{
		(it)->filter();
		m_should_run_num += (it)->test_to_run_count();
		m_disable_num += (it)->disabled_test_count();
	}

	atexit(OnExit);

	m_test_started = true;

	listeners().OnTestProgramStart(*this);
}

IUTEST_IPP_INLINE void	UnitTest::SetUpTestIteration(void)
{
	TestEnv::SetUp();
}

IUTEST_IPP_INLINE void	UnitTest::EnvironmentSetUp(void)
{
	listeners().OnEnvironmentsSetUpStart(*this);
	for( iuEnvironmentList::iterator it = TestEnv::environments().begin(), end=TestEnv::environments().end(); it != end; ++it )
	{
		(*it)->SetUp();
	}
	listeners().OnEnvironmentsSetUpEnd(*this);
}

IUTEST_IPP_INLINE void	UnitTest::EnvironmentTearDown(void)
{
	listeners().OnEnvironmentsTearDownStart(*this);
	for( iuEnvironmentList::reverse_iterator it = TestEnv::environments().rbegin(), end=TestEnv::environments().rend(); it != end; ++it )
	{
		(*it)->TearDown();
	}
	listeners().OnEnvironmentsTearDownEnd(*this);
}

IUTEST_IPP_INLINE void	UnitTest::TestProgramEnd(void)
{
	if( !m_test_started )
	{
		return;
	}
	listeners().OnTestProgramEnd(*this);
	m_test_started = false;
}

IUTEST_IPP_INLINE void	UnitTest::Initialize(void)
{
	m_init_iutest_count++;

	ClearAdHocTestResult();

	// ファイルシステムの初期化
	if( detail::IFileSystem::GetInstance() == NULL )
	{
#if defined(IUTEST_FILE)
		static FileSystem<IUTEST_FILE> filesystem;
		filesystem.Initialize();
#elif IUTEST_HAS_FOPEN
		static FileSystem<StdioFile> filesystem;
		filesystem.Initialize();
#endif
	}

	// 以降初回のみ
	if( m_init_iutest_count != 1 )
	{
		return;
	}

#if IUTEST_HAS_PARAM_TEST
	m_param_testcase_holder.RegisterTests();
#endif
}

}	// end of namespace iutest

#endif	// INCG_IRIS_IUTEST_CORE_IPP_33560431_783B_4AE7_8FC8_6E78355CAF87_
