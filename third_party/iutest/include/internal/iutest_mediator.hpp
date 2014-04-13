//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_mediator.hpp
 * @brief		iris unit test 仲介者 ファイル
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
#ifndef INCG_IRIS_IUTEST_MEDIATOR_HPP_BFBE202B_0E60_4E44_8BDA_CB801D264F3E_
#define INCG_IRIS_IUTEST_MEDIATOR_HPP_BFBE202B_0E60_4E44_8BDA_CB801D264F3E_

namespace iutest {

//======================================================================
// declare
class TestInfo;
class TestCase;
class TestPartResult;

namespace detail
{

//======================================================================
// class
/**
 * @brief	TestInfo 仲介者インターフェイス
*/
class iuITestInfoMediator
{
protected:
	TestInfo* m_test_info;
public:
	iuITestInfoMediator(TestInfo* p=NULL) IUTEST_CXX_NOEXCEPT_SPEC : m_test_info(p) {}
public:
	virtual bool HasFatalFailure(void) const = 0;
	virtual bool HasNonfatalFailure(void) const = 0;
	virtual bool HasFailure(void) const = 0;
	virtual bool IsSkipped(void) const = 0;
	TestInfo* ptr(void) const IUTEST_CXX_NOEXCEPT_SPEC { return m_test_info; }
};

/**
 * @brief	TestCase 仲介者インターフェイス
*/
class iuITestCaseMediator
{
protected:
	TestCase* m_test_case;
public:
	iuITestCaseMediator(TestCase* p=NULL) IUTEST_CXX_NOEXCEPT_SPEC : m_test_case(p) {}
public:
	virtual const char* test_case_name(void) const = 0;
	virtual const char* type_param(void) const = 0;
	TestCase* ptr(void) const IUTEST_CXX_NOEXCEPT_SPEC { return m_test_case; }
};

}	// end of namespace detail
}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_MEDIATOR_HPP_BFBE202B_0E60_4E44_8BDA_CB801D264F3E_
