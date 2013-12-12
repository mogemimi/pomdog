//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_info.ipp
 * @brief		iris unit test 情報 ファイル
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
 * @par			copyright
 * Copyright (C) 2011-2013, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_INFO_IPP_764A79A8_E822_4C0F_8CB7_82C635BA28BA_
#define INCG_IRIS_IUTEST_INFO_IPP_764A79A8_E822_4C0F_8CB7_82C635BA28BA_

//======================================================================
// include
#include "../iutest_info.hpp"

namespace iutest
{

/**
 * @brief	実行
*/
IUTEST_IPP_INLINE bool	TestInfo::Run(void)
{
	if( !m_should_run )
	{
		return true;
	}

	// テスト開始
	TestEnv::event_listeners().OnTestStart(*this);

	RunImpl();

	// テスト終了
	TestEnv::event_listeners().OnTestEnd(*this);
	return !HasFailure();
}

IUTEST_IPP_INLINE void	TestInfo::RunImpl(void)
{
	detail::iuStopWatch sw;
	TimeInMillisec elapsedmsec = 0;

	m_ran = true;

#if IUTEST_HAS_EXCEPTIONS
	if( TestFlag::IsEnableFlag(TestFlag::CATCH_EXCEPTION_EACH) )
	{
		detail::auto_ptr<Test> p = m_factory->Create();
		try
		{
			sw.start();
#if IUTEST_HAS_SEH
			RunOnMSC(p);
#else
			p->Run(&m_mediator);
#endif
			elapsedmsec = sw.stop();
		}
		catch (const ::std::exception& e)
		{
			elapsedmsec = sw.stop();
			iutest::AssertionHelper(NULL, -1, detail::FormatCxxException(e.what()), TestPartResult::kFatalFailure).OnFixed(AssertionHelper::Fixed());
			if( TestFlag::IsEnableFlag(TestFlag::THROW_ON_FAILURE) )
			{
				throw;
			}
		}
		catch (TestPartResult::Type& eType)
		{
			IUTEST_UNUSED_VAR(eType);
			elapsedmsec = sw.stop();
			if( TestFlag::IsEnableFlag(TestFlag::THROW_ON_FAILURE) )
			{
				throw;
			}
		}
		catch (...)
		{
			elapsedmsec = sw.stop();
			iutest::AssertionHelper(NULL, -1, detail::FormatCxxException(NULL), TestPartResult::kFatalFailure).OnFixed(AssertionHelper::Fixed());
			if( TestFlag::IsEnableFlag(TestFlag::THROW_ON_FAILURE) )
			{
				throw;
			}
		}
	}
	else
#endif
	{
		detail::auto_ptr<Test> p = m_factory->Create();
		sw.start();
		p->Run(&m_mediator);
		elapsedmsec = sw.stop();
	}

	m_test_result.set_elapsed_time(elapsedmsec);

	if( HasFailure() && TestFlag::IsEnableFlag(TestFlag::THROW_ON_FAILURE) )
	{
#if IUTEST_HAS_EXCEPTIONS
		throw HasFatalFailure() ? TestPartResult::kFatalFailure : TestPartResult::kNonFatalFailure;
#else
		exit(1);
#endif
	}
}

#if IUTEST_HAS_EXCEPTIONS && IUTEST_HAS_SEH
#if IUTEST_HAS_MINIDUMP

IUTEST_IPP_INLINE void	TestInfo::MiniDump(_EXCEPTION_POINTERS* ep)
{
#if defined(_MSC_VER)

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_BEGIN()
		char path[MAX_PATH];
		strcpy(path, test_case_name());
		strcat(path, "_");
		strcat(path, name());
		strcat(path, ".dump");
IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_END()

		detail::MiniDump::Create(path, ep);
#endif
}

#endif

IUTEST_IPP_INLINE void	TestInfo::RunOnMSC(detail::auto_ptr<Test>& test)
{
	_EXCEPTION_POINTERS* ep = NULL;
	__try
	{
		test->Run(&m_mediator);
	}
	__except (ep = GetExceptionInformation()
#if IUTEST_HAS_MINIDUMP
		, MiniDump(ep)
#endif
		, detail::seh_exception::should_process_through_break_and_cppexceptions(GetExceptionCode()))
	{
		detail::seh_exception::translator(GetExceptionCode(), ep);
	}
}
#endif

IUTEST_IPP_INLINE void	TestInfo::clear(void)
{
	m_ran = false;
	m_skip = false;
	m_test_result.Clear();
}

IUTEST_IPP_INLINE bool	TestInfo::filter(void)
{
	bool run = true;
	// 無効テストなら実行しない
	if( !TestFlag::IsEnableFlag(TestFlag::RUN_DISABLED_TESTS)
		&& is_disabled_test() )
	{
		run = false;
	}
	bool match = true;
	if( TestFlag::IsEnableFlag(TestFlag::FILTERING_TESTS) )
	{
		if( !detail::iuRegex::match(TestEnv::test_filter(), test_full_name().c_str()) ) 
		{
			match = false;
			run = false;
		}
	}
	m_matches_filter = match;
	m_should_run = run;
	return m_should_run;
}

}	// end of namespace iutest

#endif
