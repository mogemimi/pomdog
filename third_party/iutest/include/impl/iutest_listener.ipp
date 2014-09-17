//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_listener.ipp
 * @brief		iris unit test イベントリスナー 定義 ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2011-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_LISTENER_IPP_C94B8142_F8B3_413F_B479_8AA2B78604CE_
#define INCG_IRIS_IUTEST_LISTENER_IPP_C94B8142_F8B3_413F_B479_8AA2B78604CE_

//======================================================================
// include
#include "../iutest_listener.hpp"

namespace iutest
{

IUTEST_IPP_INLINE TestEventListener* TestEventRepeater::Release(TestEventListener* listener)
{
	ListenerContainer::iterator it = ::std::find(m_listeners.begin(), m_listeners.end(), listener);
	if( it == m_listeners.end() ) return NULL;
	m_listeners.erase(it);
	return listener;
}

IUTEST_IPP_INLINE void TestEventRepeater::OnTestProgramStart(const UnitTest& test)
{
	for( ListenerContainer::iterator it=m_listeners.begin(), end=m_listeners.end(); it != end; ++it )
	{
		(*it)->OnTestProgramStart(test);
	}
}
IUTEST_IPP_INLINE void TestEventRepeater::OnTestIterationStart(const UnitTest& test, int iteration)
{
	for( ListenerContainer::iterator it=m_listeners.begin(), end=m_listeners.end(); it != end; ++it )
	{
		(*it)->OnTestIterationStart(test, iteration);
	}
}
IUTEST_IPP_INLINE void TestEventRepeater::OnEnvironmentsSetUpStart(const UnitTest& test)
{
	for( ListenerContainer::iterator it=m_listeners.begin(), end=m_listeners.end(); it != end; ++it )
	{
		(*it)->OnEnvironmentsSetUpStart(test);
	}
}
IUTEST_IPP_INLINE void TestEventRepeater::OnEnvironmentsSetUpEnd(const UnitTest& test)
{
	for( ListenerContainer::reverse_iterator it=m_listeners.rbegin(), end=m_listeners.rend(); it != end; ++it )
	{
		(*it)->OnEnvironmentsSetUpEnd(test);
	}
}

IUTEST_IPP_INLINE void TestEventRepeater::OnTestCaseStart(const TestCase& test_case)
{
	for( ListenerContainer::iterator it=m_listeners.begin(), end=m_listeners.end(); it != end; ++it )
	{
		(*it)->OnTestCaseStart(test_case);
	}
}
IUTEST_IPP_INLINE void TestEventRepeater::OnTestStart(const TestInfo& test_info)
{
	for( ListenerContainer::iterator it=m_listeners.begin(), end=m_listeners.end(); it != end; ++it )
	{
		(*it)->OnTestStart(test_info);
	}
}
IUTEST_IPP_INLINE void TestEventRepeater::OnTestPartResult(const TestPartResult& test_part_result)
{
	for( ListenerContainer::iterator it=m_listeners.begin(), end=m_listeners.end(); it != end; ++it )
	{
		(*it)->OnTestPartResult(test_part_result);
	}
}
IUTEST_IPP_INLINE void TestEventRepeater::OnTestRecordProperty(const TestProperty& test_property)
{
	for( ListenerContainer::iterator it=m_listeners.begin(), end=m_listeners.end(); it != end; ++it )
	{
		(*it)->OnTestRecordProperty(test_property);
	}
}
IUTEST_IPP_INLINE void TestEventRepeater::OnTestEnd(const TestInfo& test_info)
{
	for( ListenerContainer::reverse_iterator it=m_listeners.rbegin(), end=m_listeners.rend(); it != end; ++it )
	{
		(*it)->OnTestEnd(test_info);
	}
}
IUTEST_IPP_INLINE void TestEventRepeater::OnTestCaseEnd(const TestCase& test_case)
{
	for( ListenerContainer::reverse_iterator it=m_listeners.rbegin(), end=m_listeners.rend(); it != end; ++it )
	{
		(*it)->OnTestCaseEnd(test_case);
	}
}

IUTEST_IPP_INLINE void TestEventRepeater::OnEnvironmentsTearDownStart(const UnitTest& test)
{
	for( ListenerContainer::iterator it=m_listeners.begin(), end=m_listeners.end(); it != end; ++it )
	{
		(*it)->OnEnvironmentsTearDownStart(test);
	}
}
IUTEST_IPP_INLINE void TestEventRepeater::OnEnvironmentsTearDownEnd(const UnitTest& test)
{
	for( ListenerContainer::reverse_iterator it=m_listeners.rbegin(), end=m_listeners.rend(); it != end; ++it )
	{
		(*it)->OnEnvironmentsTearDownEnd(test);
	}
}
IUTEST_IPP_INLINE void TestEventRepeater::OnTestIterationEnd(const UnitTest& test, int iteration)
{
	for( ListenerContainer::reverse_iterator it=m_listeners.rbegin(), end=m_listeners.rend(); it != end; ++it )
	{
		(*it)->OnTestIterationEnd(test, iteration);
	}
}
IUTEST_IPP_INLINE void TestEventRepeater::OnTestProgramEnd(const UnitTest& test)
{
	for( ListenerContainer::reverse_iterator it=m_listeners.rbegin(), end=m_listeners.rend(); it != end; ++it )
	{
		(*it)->OnTestProgramEnd(test);
	}
}

IUTEST_IPP_INLINE void TestEventListeners::set_default_result_printer(TestEventListener* listener)
{
	delete Release(m_default_result_printer);
	if( listener != NULL )
	{
		Append(listener);
	}
	m_default_result_printer = listener;
}
IUTEST_IPP_INLINE void TestEventListeners::set_default_xml_generator(TestEventListener* listener)
{
	delete Release(m_default_xml_generator);
	if( listener != NULL )
	{
		Append(listener);
	}
	m_default_xml_generator = listener;
}

}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_LISTENER_IPP_C94B8142_F8B3_413F_B479_8AA2B78604CE_
