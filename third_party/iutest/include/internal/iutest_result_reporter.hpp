//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_result_reporter.hpp
 * @brief		iris unit test TestPartResult リポーター ファイル
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
#ifndef INCG_IRIS_IUTEST_RESULT_REPORTER_HPP_803FD1F7_1FD2_4D1E_9AFC_A5851284316F_
#define INCG_IRIS_IUTEST_RESULT_REPORTER_HPP_803FD1F7_1FD2_4D1E_9AFC_A5851284316F_

//======================================================================
// include
#include "iutest_core_impl.hpp"

namespace iutest {
namespace detail
{

//======================================================================
// class
/**
 * @brief	デフォルト TestPartResult リポーター
*/
class DefaultGlobalTestPartResultReporter : public TestPartResultReporterInterface
{
public:
	virtual ~DefaultGlobalTestPartResultReporter(void) {}
	virtual void ReportTestPartResult(const TestPartResult& test_part_result)
	{
		UnitTestImpl::current_test_result()->AddTestPartResult(test_part_result);
		TestEnv::event_listeners().OnTestPartResult(test_part_result);
	}
};

/**
 * @brief	TestPartResult の情報収集ヘルパー
*/
class NewTestPartResultCheckHelper
{
public:
	template<TestPartResult::Type Type>
	struct CondEq
	{
		bool operator ()(const TestPartResult& result)
		{
			return result.type() == Type;
		}
	};

	template<TestPartResult::Type Type>
	struct CondNe
	{
		bool operator ()(const TestPartResult& result)
		{
			return result.type() != Type;
		}
	};

	template<TestPartResult::Type Type>
	struct CondGt
	{
		bool operator ()(const TestPartResult& result)
		{
			return result.type() > Type;
		}
	};
public:
	template<typename REPORTER>
	class Reporter : public REPORTER
	{
	public:
		Reporter(void)
			: m_origin(TestEnv::GetGlobalTestPartResultReporter())
		{
			TestEnv::SetGlobalTestPartResultReporter(this);
		}
		virtual ~Reporter(void) 
		{
			TestEnv::SetGlobalTestPartResultReporter(m_origin);
		}
	private:
		TestPartResultReporterInterface*	m_origin;
	};

public:
	template<typename COND, typename REPORTER=DefaultGlobalTestPartResultReporter>
	class Counter : public Reporter<REPORTER>
	{
		typedef REPORTER	_Mybase;
	public:
		Counter(void) : m_count(0)
		{
		}
		virtual void ReportTestPartResult(const TestPartResult& result) IUTEST_CXX_OVERRIDE
		{
			if( m_cond(result) )
			{
				++m_count;
			}
			_Mybase::ReportTestPartResult(result);
		}
	public:
		int	count(void) const IUTEST_CXX_NOEXCEPT_SPEC { return m_count; }
	private:
		COND m_cond;
		int m_count;

		IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(Counter);
	};
	template<typename REPORTER=DefaultGlobalTestPartResultReporter>
	class Collector : public Reporter<REPORTER>
	{
		typedef REPORTER	_Mybase;
		typedef ::std::vector<TestPartResult>	TestPartResults;
	public:
		virtual void ReportTestPartResult(const TestPartResult& result) IUTEST_CXX_OVERRIDE
		{
			m_results.push_back(result);
			_Mybase::ReportTestPartResult(result);
		}
	public:
		size_t	count(void) const IUTEST_CXX_NOEXCEPT_SPEC { return m_results.size(); }
		const TestPartResult&	GetTestPartResult(int index) const	{ return m_results[index]; }

	private:
		TestPartResults m_results;
	};
};

}	// end of namespace detail
}	// end of namespace iutest

namespace iutest_report_result
{

/**
	* @brief	TestPartResult リポーター
*/
inline void ReportTestPartResult(const ::iutest::TestPartResult& test_part_result)
{
	::iutest::detail::iuConsole::output(test_part_result.make_newline_message().c_str());
}

}

#endif	// INCG_IRIS_IUTEST_RESULT_REPORTER_HPP_803FD1F7_1FD2_4D1E_9AFC_A5851284316F_
