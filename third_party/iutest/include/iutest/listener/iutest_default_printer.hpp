//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_default_printer.hpp
 * @brief		iris unit test デフォルト出力イベントリスナー ファイル
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
#ifndef INCG_IRIS_iutest_default_printer_HPP_77055C2B_AAE1_4944_A61C_26C58B04B37B_
#define INCG_IRIS_iutest_default_printer_HPP_77055C2B_AAE1_4944_A61C_26C58B04B37B_

//======================================================================
// include
#include "../iutest_core.hpp"

namespace iutest
{

//======================================================================
// class
/**
 * @brief	デフォルト出力イベントリスナー
*/
class DefalutResultPrintListener : public TestEventListener
{
public:
	DefalutResultPrintListener(void) {}

	virtual ~DefalutResultPrintListener(void)
	{
		TestEnv::event_listeners().set_default_result_printer(NULL);
	}
public:
	virtual void OnTestProgramStart(const UnitTest& test) IUTEST_CXX_OVERRIDE;
	virtual void OnTestIterationStart(const UnitTest& test
									, int iteration) IUTEST_CXX_OVERRIDE;
	virtual void OnEnvironmentsSetUpStart(const UnitTest& test) IUTEST_CXX_OVERRIDE;
	virtual void OnEnvironmentsSetUpEnd(const UnitTest& test) IUTEST_CXX_OVERRIDE;
	virtual void OnTestCaseStart(const TestCase& test_case) IUTEST_CXX_OVERRIDE;
	virtual void OnTestStart(const TestInfo& test_info) IUTEST_CXX_OVERRIDE;
	virtual void OnTestPartResult(const TestPartResult& test_part_result) IUTEST_CXX_OVERRIDE;
	virtual void OnTestRecordProperty(const TestProperty& test_property) IUTEST_CXX_OVERRIDE;
	virtual void OnTestEnd(const TestInfo& test_info) IUTEST_CXX_OVERRIDE;
	virtual void OnTestCaseEnd(const TestCase& test_case) IUTEST_CXX_OVERRIDE;
	virtual void OnEnvironmentsTearDownStart(const UnitTest& test) IUTEST_CXX_OVERRIDE;
	virtual void OnEnvironmentsTearDownEnd(const UnitTest& test) IUTEST_CXX_OVERRIDE;
	virtual void OnTestIterationEnd(const UnitTest& test
									, int iteration) IUTEST_CXX_OVERRIDE;
	virtual void OnTestProgramEnd(const UnitTest& test) IUTEST_CXX_OVERRIDE;
};

}	// end of namespace iutest

#if !IUTEST_HAS_LIB
#  include "../impl/iutest_default_printer.ipp"
#endif

#endif
