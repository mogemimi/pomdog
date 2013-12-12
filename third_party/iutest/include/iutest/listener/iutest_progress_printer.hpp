//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_progress_printer.hpp
 * @brief		iris unit test プログレス出力イベントリスナー ファイル
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
#ifndef INCG_IRIS_IUTEST_PROGRESS_PRINTER_HPP_D31CE626_EA82_4265_B2B6_50EC5C63CFB6_
#define INCG_IRIS_IUTEST_PROGRESS_PRINTER_HPP_D31CE626_EA82_4265_B2B6_50EC5C63CFB6_

//======================================================================
// include
#include "../iutest_core.hpp"

namespace iutest
{

//======================================================================
// class
/**
 * @brief	プログレス出力イベントリスナー
*/
class ProgressPrintListener : public EmptyTestEventListener
{
public:
	ProgressPrintListener(void) : m_should_run_num(0), m_ran_num(0) {}
public:
	virtual void OnTestProgramStart(const UnitTest& test) IUTEST_CXX_OVERRIDE;
	virtual void OnTestIterationStart(const UnitTest& test
									, int iteration) IUTEST_CXX_OVERRIDE;
	virtual void OnTestEnd(const TestInfo& test_info) IUTEST_CXX_OVERRIDE;

private:
	int m_should_run_num;
	int m_ran_num;

public:
	/**
	 * @brief	ProgressPrintListener に切り替え
	*/
	static void SetUp(void)
	{
		TestEventListeners& listeners = UnitTest::GetInstance()->listeners();
		delete listeners.Release(listeners.default_result_printer());
		listeners.Append(new ProgressPrintListener);
	}
};

inline void ProgressPrintListener::OnTestProgramStart(const UnitTest& test)
{
	IUTEST_UNUSED_VAR(test);

	detail::iuConsole::output("0%%   10   20   30   40   50   60   70   80   90   100%%\n");
	detail::iuConsole::output("|----|----|----|----|----|----|----|----|----|----|   \n");
}
inline void ProgressPrintListener::OnTestIterationStart(const UnitTest& test
								  , int iteration)
{
	IUTEST_UNUSED_VAR(iteration);

	m_should_run_num = test.test_to_run_count();
	m_ran_num = 0;
}
inline void ProgressPrintListener::OnTestEnd(const TestInfo& test_info)
{
	IUTEST_UNUSED_VAR(test_info);

	++m_ran_num;
	const int star_count = 51;
	int n = (m_ran_num * star_count) / m_should_run_num;
	char progress[52] = {0};
	for( int i=0; i < n; ++i ) progress[i] = '*';
	progress[n++] = m_ran_num == m_should_run_num ? '\n' : '\r';
	progress[n++] = '\0';
	detail::iuConsole::output(progress);
}

}	// end of namespace iutest

#endif
