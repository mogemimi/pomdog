//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_junit_xml_generator.hpp
 * @brief		output junit xml event listener
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_JUNIT_XML_GENERATOR_HPP_C6AD11BD_BDA4_45F3_A736_53B09DD6A7BD_
#define INCG_IRIS_IUTEST_JUNIT_XML_GENERATOR_HPP_C6AD11BD_BDA4_45F3_A736_53B09DD6A7BD_

//======================================================================
// include
#include "iutest_default_xml_generator.hpp"

namespace iutest
{

//======================================================================
// class
/**
 * @brief	xml出力イベントリスナー
*/
class JunitXmlGeneratorListener : public DefaultXmlGeneratorListener
{
public:
	/**
	 * @brief	コンストラクタ
	 * @param [in] path = 出力パス
	*/
	JunitXmlGeneratorListener(const ::std::string& path)
		: DefaultXmlGeneratorListener(path)
	{
	}
	virtual ~JunitXmlGeneratorListener(void)
	{
	}
public:
	virtual void OnReportTest(IFile* file, const UnitTest& test) IUTEST_CXX_OVERRIDE;

private:
	static void OnReportTestCase(IFile* file, const TestCase& test_case);
	static void OnReportTestInfo(IFile* file, const TestInfo& test_info);
	static void OnReportTestProperty(IFile* file, const char* name, const char* value);
	static void OnReportTestProperty(IFile* file, const TestResult& test_result);
	static void OnReportTestSkipped(IFile* file, const TestInfo& test_info);
public:
	/** @private */
	static void SetUp(void)
	{
		::std::string xmlpath = TestEnv::get_report_junit_xml_filepath();
		if( !xmlpath.empty() )
		{
			TestEnv::event_listeners().set_default_xml_generator(new JunitXmlGeneratorListener(xmlpath));
		}
	}
};

}	// end of namespace iutest

#if !IUTEST_HAS_LIB
#  include "../impl/iutest_junit_xml_generator.ipp"
#endif

#endif // INCG_IRIS_IUTEST_JUNIT_XML_GENERATOR_HPP_C6AD11BD_BDA4_45F3_A736_53B09DD6A7BD_
