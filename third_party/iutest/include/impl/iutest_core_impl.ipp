//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_core_impl.ipp
 * @brief		iris unit test UnitTest 実装 ファイル
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
#ifndef INCG_IRIS_IUTEST_CORE_IMPL_IPP_D5ABC7DE_C751_4AC0_922F_547880163891_
#define INCG_IRIS_IUTEST_CORE_IMPL_IPP_D5ABC7DE_C751_4AC0_922F_547880163891_

//======================================================================
// include
#include "../internal/iutest_core_impl.hpp"

namespace iutest
{

IUTEST_IPP_INLINE TestResult* UnitTestImpl::current_test_result(void)
{
	UnitTestImpl* p = ptr();
	if( Test::GetCurrentTestInfo() )
	{
		return &(Test::GetCurrentTest()->m_test_info->ptr()->m_test_result);
	}
	if( p->m_current_testcase != NULL )
	{
		return &p->m_current_testcase->m_ad_hoc_testresult;
	}
	return &p->m_ad_hoc_testresult;
}

IUTEST_IPP_INLINE void	UnitTestImpl::AddTestInfo(TestCase* pCase, TestInfo* pInfo)
{
	++m_total_test_num;
	pCase->push_back(pInfo);
}

IUTEST_IPP_INLINE void	UnitTestImpl::SkipTest(void)
{
	Test* test = Test::GetCurrentTest();
	if( test != NULL && test->m_test_info->ptr() != NULL )
	{
		test->m_test_info->ptr()->m_skip = true;
	}
}

IUTEST_IPP_INLINE int	UnitTestImpl::Listup(void) const
{
	detail::iuConsole::output("%d tests from %d testcase\n", m_total_test_num, m_testcases.size() );
	for( iuTestCases::const_iterator it = m_testcases.begin(), end=m_testcases.end(); it != end; ++it )
	{
		detail::iuConsole::output((it)->name());
		detail::iuConsole::output("\n");

		for( TestCase::iuTestInfos::const_iterator it2 = (it)->begin(), end2=(it)->end(); it2 != end2; ++it2 )
		{
			detail::iuConsole::output("  ");
			detail::iuConsole::output((it2)->name());
			detail::iuConsole::output("\n");
		}
	}
	return 0;
}

IUTEST_IPP_INLINE int	UnitTestImpl::ListupWithWhere(void) const
{
	detail::iuConsole::output("%d tests from %d testcase\n", m_total_test_num, m_testcases.size() );
	for( iuTestCases::const_iterator it = m_testcases.begin(), end=m_testcases.end(); it != end; ++it )
	{
		detail::iuConsole::output((it)->testcase_name_with_where().c_str());
		detail::iuConsole::output("\n");

		for( TestCase::iuTestInfos::const_iterator it2 = (it)->begin(), end2=(it)->end(); it2 != end2; ++it2 )
		{
			detail::iuConsole::output("  ");
			detail::iuConsole::output((it2)->name());
			detail::iuConsole::output("\n");
		}
	}
	return 0;
}

IUTEST_IPP_INLINE bool	UnitTestImpl::PreRunner(void)
{
	InitializeImpl();

	if( TestFlag::IsEnableFlag(TestFlag::SHOW_HELP) )
	{
		detail::iuOptionMessage::ShowHelp();
	}
	else if( TestFlag::IsEnableFlag(TestFlag::SHOW_VERSION) )
	{
		detail::iuOptionMessage::ShowVersion();
	}
	else if( TestFlag::IsEnableFlag(TestFlag::SHOW_FEATURE) )
	{
		detail::iuOptionMessage::ShowFeature();
	}
	else if( TestFlag::IsEnableFlag(TestFlag::SHOW_TESTS_LIST_WITH_WHERE) )
	{
		ListupWithWhere();
	}
	else if( TestFlag::IsEnableFlag(TestFlag::SHOW_TESTS_LIST) )
	{
		Listup();
	}
	else
	{
		return true;
	}
	return false;
}

IUTEST_IPP_INLINE void UnitTestImpl::ClearNonAdHocTestResult(void)
{
	for( iuTestCases::iterator it=m_testcases.begin(), end=m_testcases.end(); it != end; ++it )
	{
		(it)->clear();
	}
}

IUTEST_IPP_INLINE void UnitTestImpl::RecordProperty(const TestProperty& prop)
{
	UnitTestImpl* p = ptr();
	TestResult* tr = NULL;
	if( Test::GetCurrentTestInfo() )
	{
		tr = &(Test::GetCurrentTest()->m_test_info->ptr()->m_test_result);
		// 不正なキーのチェック
		if( !TestInfo::ValidateTestPropertyName(prop.key()) )
		{
			IIUT_ADD_FAILURE() << "Reserved key used in RecordProperty(): " << prop.key();
			return;
		}
	}
	else if( p->m_current_testcase != NULL )
	{
		tr = &p->m_current_testcase->m_ad_hoc_testresult;
		// 不正なキーのチェック
		if( !TestCase::ValidateTestPropertyName(prop.key()) )
		{
			IIUT_ADD_FAILURE() << "Reserved key used in RecordProperty(): " << prop.key();
			return;
		}
	}
	else
	{
		tr =&p->m_ad_hoc_testresult;
		// 不正なキーのチェック
		if( !ValidateTestPropertyName(prop.key()) )
		{
			IIUT_ADD_FAILURE() << "Reserved key used in RecordProperty(): " << prop.key();
			return;
		}
	}
	tr->RecordProperty(prop);
	TestEnv::event_listeners().OnTestRecordProperty(prop);
}

IUTEST_IPP_INLINE void	UnitTestImpl::TerminateImpl(void)
{
	for( iuTestCases::iterator it = m_testcases.begin(); it != m_testcases.end(); it = m_testcases.begin())
	{
		TestCase* p = (it);
		m_testcases.erase(it);
		delete p;
	}
}

#if IUTEST_HAS_EXCEPTIONS && (defined(_MSC_VER) && (_MSC_VER >= 1400)) && !defined(IUTEST_OS_WINDOWS_MOBILE)

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_BEGIN()

	// _invalid_parameter_handler
IUTEST_IPP_INLINE void UnitTestImpl::OnInvalidParameter(const wchar_t * expression, const wchar_t * function
	, const wchar_t * file, unsigned int line, uintptr_t pReserved)
{
	IUTEST_UNUSED_VAR(file);
	IUTEST_UNUSED_VAR(line);
	IUTEST_UNUSED_VAR(pReserved);
	char func[260];
	wcstombs(func, function, 260);
	char expr[260];
	wcstombs(expr, expression, 260);
	std::string msg = func;
	msg += expr;
	throw std::invalid_argument(msg);
}

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_END()

#endif

namespace detail
{

IUTEST_IPP_INLINE ::std::string MakeIndexTestName(const char* basename, int index)
{
	::std::string name = basename;
	iuStringStream::type strm;
	strm << index;
	name += "/";
	name += strm.str();
	return name;
}

IUTEST_IPP_INLINE ::std::string MakePrefixedIndexTestName(const char* prefix, const char* basename, int index)
{
	::std::string name = prefix;
	if( !name.empty() )
	{
		name += "/";
	}
	name += MakeIndexTestName(basename, index);
	return name;
}

}	// end of namespace detail

IUTEST_IPP_INLINE void Test::TestRecordPropertyHelper::RecordProperty(const TestProperty& prop)
{
	iutest::UnitTestImpl::RecordProperty(prop);
}

}	// end of namespace iutest

#endif	// INCG_IRIS_IUTEST_CORE_IMPL_IPP_D5ABC7DE_C751_4AC0_922F_547880163891_
