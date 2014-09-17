//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_result_reporter.hpp
 * @brief		iris unit test TestPartResult リポーター ファイル
 *
 * @author		t.shirayanagi
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
	virtual ~DefaultGlobalTestPartResultReporter(void) IUTEST_CXX_OVERRIDE {}
	virtual void ReportTestPartResult(const TestPartResult& test_part_result) IUTEST_CXX_OVERRIDE
	{
		DefaultReportTestPartResult(test_part_result);
	}
	static void DefaultReportTestPartResult(const TestPartResult& test_part_result)
	{
		TestResult* result = UnitTestImpl::current_test_result();
		if( result )
		{
			result->AddTestPartResult(test_part_result);
		}
		else
		{
			iuConsole::output(test_part_result.make_newline_message().c_str());
		}
		TestEnv::event_listeners().OnTestPartResult(test_part_result);
	}
};

/**
 * @brief	Not report
*/
class NoTestPartResultReporter : public TestPartResultReporterInterface
{
public:
	virtual ~NoTestPartResultReporter(void) IUTEST_CXX_OVERRIDE {}
	virtual void ReportTestPartResult(const TestPartResult& result) IUTEST_CXX_OVERRIDE
	{
		IUTEST_UNUSED_VAR(result);
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
	class ReporterHolder
	{
	public:
		ReporterHolder() : m_origin(NULL) {}
		virtual ~ReporterHolder(void)
		{
			Detach();
		}
		void Attach(TestPartResultReporterInterface* p)
		{
			m_origin = TestEnv::GetGlobalTestPartResultReporter();
			TestEnv::SetGlobalTestPartResultReporter(p);
		}
		void Detach(void)
		{
			TestEnv::SetGlobalTestPartResultReporter(m_origin);
		}
	public:
		void ReportTestPartResultOrigin(const TestPartResult& result)
		{
			if( m_origin )
			{
				m_origin->ReportTestPartResult(result);
			}
		}
	private:
		TestPartResultReporterInterface* m_origin;
	};

public:
	template<typename COND, typename REPORTER=DefaultGlobalTestPartResultReporter>
	class Counter : public REPORTER
	{
		typedef REPORTER _Mybase;
	public:
		Counter(void) : m_count(0)
		{
			m_holder.Attach(this);
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
		int count(void) const IUTEST_CXX_NOEXCEPT_SPEC { return m_count; }
	private:
		ReporterHolder m_holder;
		COND m_cond;
		int m_count;

		IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(Counter);
	};
	template<typename REPORTER=DefaultGlobalTestPartResultReporter>
	class Collector : public REPORTER
	{
		typedef REPORTER _Mybase;
		typedef ::std::vector<TestPartResult> TestPartResults;
	public:
		Collector(void)
		{
			m_holder.Attach(this);
		}
	public:
		virtual void ReportTestPartResult(const TestPartResult& result) IUTEST_CXX_OVERRIDE
		{
			m_results.push_back(result);
			_Mybase::ReportTestPartResult(result);
		}
	public:
		size_t count(void) const IUTEST_CXX_NOEXCEPT_SPEC { return m_results.size(); }
		const TestPartResult& GetTestPartResult(int index) const { return m_results[index]; }

		void ReportTestPartResult(void)
		{
			for( TestPartResults::iterator it=m_results.begin(); it != m_results.end(); ++it )
			{
				m_holder.ReportTestPartResultOrigin(*it);
			}
		}
	private:
		ReporterHolder m_holder;
		TestPartResults m_results;
	};
};

//======================================================================
// function
/**
 * @brief	TestPartResult リポーター
*/
inline void DefaultReportTestPartResult(const TestPartResult& test_part_result)
{
	DefaultGlobalTestPartResultReporter::DefaultReportTestPartResult(test_part_result);
}

}	// end of namespace detail
}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_RESULT_REPORTER_HPP_803FD1F7_1FD2_4D1E_9AFC_A5851284316F_
