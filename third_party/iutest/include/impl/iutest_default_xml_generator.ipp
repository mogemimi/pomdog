//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_default_xml_generator.ipp
 * @brief		output xml event listener implementation
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
#ifndef INCG_IRIS_IUTEST_DEFAULT_XML_GENERATOR_IPP_791DCB98_05CC_49BA_8518_0EC9CA2B5450_
#define INCG_IRIS_IUTEST_DEFAULT_XML_GENERATOR_IPP_791DCB98_05CC_49BA_8518_0EC9CA2B5450_

//======================================================================
// include
#include "../listener/iutest_default_xml_generator.hpp"

namespace iutest
{

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_BEGIN()

IUTEST_IPP_INLINE void DefaultXmlGeneratorListener::OnTestProgramStart(const UnitTest& test)
{
	IUTEST_UNUSED_VAR(test);
	FileOpen(m_output_path.c_str());
}

IUTEST_IPP_INLINE void DefaultXmlGeneratorListener::OnTestProgramEnd(const UnitTest& test)
{
	if( m_fp == NULL ) 
	{
		FileOpen(m_output_path.c_str());
		if( m_fp == NULL )
		{
			return;
		}
	}

	m_fp->Printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	m_fp->Printf("<testsuites tests=\"%d\" failures=\"%d\" disabled=\"%d\" "
		, test.reportable_test_count()
		, test.failed_test_count()
		, test.reportable_disabled_test_count()
		);
#if IUTEST_REPORT_SKIPPED
	m_fp->Printf("skip=\"%d\" ", test.reportable_skip_test_count() );
#endif
	m_fp->Printf("errors=\"0\" time=\"%s\" timestamp=\"%s\" "
		, detail::FormatTimeInMillisecAsSecond(test.elapsed_time()).c_str()
		, detail::FormatTimeInMillisecAsIso8601(test.start_timestamp()).c_str()
		);
	if( TestFlag::IsEnableFlag(TestFlag::SHUFFLE_TESTS) )
	{
		m_fp->Printf("random_seed=\"%d\" ", test.random_seed());
	}
	m_fp->Printf("name=\"AllTests\"");

	// propertys
	OnReportTestProperty(m_fp, *test.ad_hoc_testresult(), UnitTest::ValidateTestPropertyName);

	m_fp->Printf(">\n");

	for( int i=0, count=test.total_test_case_count(); i < count; ++i )
	{
		OnReportTestCase(m_fp, *test.GetTestCase(i));
	}
	m_fp->Printf("</testsuites>\n");

	FileClose();
}

IUTEST_IPP_INLINE void DefaultXmlGeneratorListener::OnReportTestCase(IFile* file, const TestCase& test_case)
{
	if( test_case.reportable_test_count() == 0 )
	{
		return;
	}

	file->Printf("  <testsuite ");
	OutputXmlAttribute(file, "name"
		, EscapeXmlAttribute(test_case.name()).c_str() );
	file->Printf("tests=\"%d\" failures=\"%d\" disabled=\"%d\" "
		, test_case.reportable_test_count()
		, test_case.failed_test_count()
		, test_case.reportable_disabled_test_count()
		);
#if IUTEST_REPORT_SKIPPED
	file->Printf("skip=\"%d\" ", test_case.reportable_skip_test_count() );
#endif
	file->Printf("errors=\"0\" time=\"%s\""
		, detail::FormatTimeInMillisecAsSecond(test_case.elapsed_time()).c_str()
	);

	// propertys
	OnReportTestProperty(file, *test_case.ad_hoc_testresult(), TestCase::ValidateTestPropertyName);

	file->Printf(">\n");

	for( int i=0, count=test_case.total_test_count(); i < count; ++i )
	{
		OnReportTestInfo(file, *test_case.GetTestInfo(i));
	}
	file->Printf("  </testsuite>\n");
}

IUTEST_IPP_INLINE void DefaultXmlGeneratorListener::OnReportTestInfo(IFile* file, const TestInfo& test_info)
{
	if( !test_info.is_reportable() )
	{
		return;
	}

	file->Printf("    <testcase ");
	OutputXmlAttribute(file, "name", EscapeXmlAttribute(test_info.name()).c_str() );

	{
		const char* type_param = test_info.type_param();
		if( type_param != NULL )
		{
			OutputXmlAttribute(file, "type_param"
				, EscapeXmlAttribute(type_param).c_str() );
		}
	}
	{
		const char* value_param = test_info.value_param();
		if( value_param != NULL )
		{
			OutputXmlAttribute(file, "value_param"
				, EscapeXmlAttribute(value_param).c_str() );
		}
	}

	if( test_info.is_ran() )
	{
		file->Printf("status=\"run\" ");
	}
	else
	{
		file->Printf("status=\"notrun\" ");
	}
	file->Printf("time=\"%s\" "
		, detail::FormatTimeInMillisecAsSecond(test_info.elapsed_time()).c_str()
		);
	OutputXmlAttribute(file, "classname"
		, EscapeXmlAttribute(test_info.test_case_name()).c_str() );

	// propertys
	OnReportTestProperty(file, *test_info.result(), TestInfo::ValidateTestPropertyName);

	bool notrun = test_info.should_run() && !test_info.is_ran();
	if( test_info.HasFailure() || notrun )
	{
		file->Printf(">\n");
		for( int i=0, count=test_info.result()->total_part_count(); i < count; ++i )
		{
			const TestPartResult& part = test_info.result()->GetTestPartResult(i);
			if( part.passed() )
			{
				continue;
			}

			file->Printf("      <failure ");
			OutputXmlAttribute(file, "message"
				, EscapeXmlAttribute(part.summary()).c_str());
			file->Printf("type=\"\">");
			::std::string message = detail::FormatCompilerIndependentFileLocation(part.file_name(), part.line_number());
			message += "\n";
			message += detail::MultiByteStringToUTF8(part.summary());
			OutputXmlCDataSection(file, message.c_str());
			file->Printf("\n      </failure>\n");
		}
		if( notrun )
		{
			file->Printf("      <failure message=\"Not Run\" type=\"\">");
			OutputXmlCDataSection(file, "Not Run");
			file->Printf("\n      </failure>\n");
		}
		file->Printf("    </testcase>\n");
	}
	else
	{
#if IUTEST_REPORT_SKIPPED
		bool skipped = test_info.is_skipped() || !test_info.should_run();
		if( skipped )
		{
			file->Printf(">\n");
			OnReportTestSkipped(file, test_info);
			file->Printf("    </testcase>\n");
		}
		else
#endif
		{
			file->Printf(" />\n");
		}
	}
}

IUTEST_IPP_INLINE void DefaultXmlGeneratorListener::OnReportTestSkipped(IFile* file, const TestInfo& test_info)
{
	file->Printf("      <skipped type=\"iutest.skip\" ");
	const TestResult* tr = test_info.result();
	const int count = tr->total_part_count();
	for( int i=0; i < count; ++i )
	{
		const TestPartResult& part = tr->GetTestPartResult(i);
		if( part.skipped() )
		{
#if 0
			OutputXmlAttribute(file, "message"
				, EscapeXmlAttribute(part.summary()).c_str());
			::std::string message = detail::FormatCompilerIndependentFileLocation(part.file_name(), part.line_number());
			file->Printf(">\n");
			message += "\n";
			message += detail::MultiByteStringToUTF8(part.summary());
			OutputXmlCDataSection(file, message.c_str());
			file->Printf("\n      </skipped>\n");
#else
			::std::string message = detail::FormatCompilerIndependentFileLocation(part.file_name(), part.line_number());
			message += ": ";
			message += detail::MultiByteStringToUTF8(part.summary());
			OutputXmlAttribute(file, "message"
				, EscapeXmlAttribute(message.c_str()).c_str());
			file->Printf(" />\n");
#endif
			return;
		}
	}

	if( test_info.is_disabled_test() )
	{
		OutputXmlAttribute(file, "message", "disabled test.");
	}

	file->Printf("/>\n");
}

IUTEST_IPP_INLINE void DefaultXmlGeneratorListener::OnReportTestProperty(IFile* file, const TestResult& test_result
																		 , bool (*pfnValidate)(const ::std::string&))
{
	for( int i=0, count=test_result.test_property_count(); i < count; ++i )
	{
		const TestProperty& prop = test_result.GetTestProperty(i);
		if( (*pfnValidate)(prop.key()) )
		{
			file->Printf(" %s=\"%s\""
				, EscapeXmlAttribute(prop.key()).c_str()
				, EscapeXmlAttribute(prop.value()).c_str()
				);
		}
	}
}

IUTEST_IPP_INLINE bool DefaultXmlGeneratorListener::FileOpen(const char* path)
{
	m_fp = detail::IFileSystem::New();
	if( m_fp == NULL )
	{
		return false;
	}

	if( !m_fp->Open(path, IFile::OpenWrite) )
	{
		fprintf(stderr, "Unable to open file \"%s\".\n", m_output_path.c_str());
		fflush(stderr);
		return false;
	}
	return true;
}

IUTEST_IPP_INLINE void DefaultXmlGeneratorListener::FileClose(void)
{
	if( m_fp == NULL )
	{
		return;
	}
	m_fp->Close();
	detail::IFileSystem::Free(m_fp);
	m_fp = NULL;
}

IUTEST_IPP_INLINE void	DefaultXmlGeneratorListener::OutputXmlCDataSection(IFile* file, const char* data)
{
	file->Printf("<![CDATA[");
	file->Write(data, strlen(data), 1);
	file->Printf("]]>");
}
IUTEST_IPP_INLINE void DefaultXmlGeneratorListener::OutputXmlAttribute(IFile* file, const char* name, const char* value)
{
	file->Printf("%s=\"", name);
	file->Write(value, strlen(value), 1 );
	file->Printf("\" ");
}

IUTEST_IPP_INLINE ::std::string DefaultXmlGeneratorListener::EscapeXml(const char* str, bool is_attribute)
{
IUTEST_PRAGMA_CONSTEXPR_CALLED_AT_RUNTIME_WARN_DISABLE_BEGIN()
	::std::string msg;
	if( str != NULL )
	{
		for( const char* src = str; *src; ++src )
		{
			switch(*src)
			{
			case '<':
				msg += "&lt;";
				break;
			case '>':
				msg += "&gt;";
				break;
			case '&':
				msg += "&amp;";
				break;
			case '\'':
				msg += is_attribute ? "&apos;" : "\'";
				break;
			case '\"':
				msg += is_attribute ? "&quot;" : "\"";
				break;
			default:
				{
#if !defined(IUTEST_OS_WINDOWS_MOBILE)
					wchar_t wc = 0;
					const int len = detail::iu_mbtowc(&wc, src, MB_CUR_MAX);
					if( len > 1 )
					{
						msg += detail::WideStringToUTF8(&wc, 1);
						src += len-1;
					}
					else if( IsValidXmlCharacter(*src) )
#else
					if( IsValidXmlCharacter(*src) )
#endif
					{
						if( is_attribute
							&& IsWhitespace(*src) )
						{
							char tmp[8];
							sprintf(tmp, "&#x%02X;", *src);
							msg += tmp;
						}
						else
						{
							msg += *src;
						}
					}
				}
				break;
			}
		}
	}
	return msg;
IUTEST_PRAGMA_CONSTEXPR_CALLED_AT_RUNTIME_WARN_DISABLE_END()
}

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_END()

}	// end of namespace iutest

#endif	// INCG_IRIS_IUTEST_DEFAULT_XML_GENERATOR_IPP_791DCB98_05CC_49BA_8518_0EC9CA2B5450_
