//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_streaming_listener.ipp
 * @brief		iris unit test stream result ƒtƒ@ƒCƒ‹
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
 * @par			copyright
 * Copyright (C) 2013, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_IUTEST_STREAMING_LISTENER_IPP_1187A63F_E99B_4289_A562_3C87B9739B7D_
#define INCG_IRIS_IUTEST_IUTEST_STREAMING_LISTENER_IPP_1187A63F_E99B_4289_A562_3C87B9739B7D_

//======================================================================
// include
#include "../listener/iutest_streaming_listener.hpp"

namespace iutest
{

IUTEST_IPP_INLINE StreamResultListener::StreamResultListener(const char* host, const char* port)
{
	if( !m_socket.Open(host, port) )
	{
		IUTEST_LOG_(WARNING) << "StreamResultListener: failed connect to " << host << ":" << port;
	}
}

IUTEST_IPP_INLINE void StreamResultListener::OnTestProgramStart(const UnitTest& test)
{
	IUTEST_UNUSED_VAR(test);
	Start();
	SendLn("event=TestProgramStart");
}
IUTEST_IPP_INLINE void StreamResultListener::OnTestIterationStart(const UnitTest& test
								  , int iteration)
{
	IUTEST_UNUSED_VAR(test);
	SendLn("event=TestIterationStart&iteration=" + detail::StreamableToString(iteration));
}
IUTEST_IPP_INLINE void StreamResultListener::OnTestCaseStart(const TestCase& test_case)
{
	SendLn("event=TestCaseStart&name=" + detail::StreamableToString(test_case.name()));
}
IUTEST_IPP_INLINE void StreamResultListener::OnTestStart(const TestInfo& test_info)
{
	SendLn("event=TestStart&name=" + detail::StreamableToString(test_info.name()));
}
IUTEST_IPP_INLINE void StreamResultListener::OnTestPartResult(const TestPartResult& test_part_result)
{
	const char* filename = test_part_result.file_name();
	if( filename == NULL )
	{
		filename = "";
	}
	SendLn("event=TestPartResult&file=" + UrlEncode(filename)
		+ "&line=" + detail::StreamableToString(test_part_result.line_number())
		+ "&message=" + UrlEncode(test_part_result.message()));
}
IUTEST_IPP_INLINE void StreamResultListener::OnTestRecordProperty(const TestProperty& test_property)
{
	SendLn("event=TestRecordProperty&name=" + UrlEncode(test_property.key())
		+ "&value=" + UrlEncode(test_property.value()));
}
IUTEST_IPP_INLINE void StreamResultListener::OnTestEnd(const TestInfo& test_info)
{
	SendLn("event=TestEnd&passed=" 
		+ FormatBool(test_info.Passed())
		+ "&elapsed_time=" + detail::StreamableToString(test_info.elapsed_time()) + "ms"
		);
}
IUTEST_IPP_INLINE void StreamResultListener::OnTestCaseEnd(const TestCase& test_case)
{
	SendLn("event=TestCaseEnd&passed=" 
		+ FormatBool(test_case.Passed())
		+ "&elapsed_time=" + detail::StreamableToString(test_case.elapsed_time()) + "ms"
		);
}
IUTEST_IPP_INLINE void StreamResultListener::OnTestIterationEnd(const UnitTest& test
								, int iteration)
{
	IUTEST_UNUSED_VAR(iteration);
	SendLn("event=TestIterationEnd&passed=" 
		+ FormatBool(test.Passed())
		+ "&elapsed_time=" + detail::StreamableToString(test.elapsed_time()) + "ms"
		);
}
IUTEST_IPP_INLINE void StreamResultListener::OnTestProgramEnd(const UnitTest& test)
{
	SendLn("event=TestProgramEnd&passd=" + FormatBool(test.Passed()));
	m_socket.Close();
}

IUTEST_IPP_INLINE ::std::string StreamResultListener::UrlEncode(const char* str)
{
	::std::string result;
	if( str != NULL )
	{
		result.reserve(strlen(str)+1);
		for( const char* p=str; *p != '\0'; ++p )
		{
			const char ch = *p;
			switch(ch)
			{
			case '%':
			case '=':
			case '&':
			case '\n':
				result.append(detail::ToHexString(ch));
				break;
			default:
				result.push_back(ch);
			}
		}
	}
	return result;
}

IUTEST_IPP_INLINE ::std::string StreamResultListener::FormatBool(bool b)
{
	return b ? "1" : "0";
}

IUTEST_IPP_INLINE void StreamResultListener::Start()
{
	SendLn("iutest_streaming_protocol_version=1.0");
}

IUTEST_IPP_INLINE void StreamResultListener::SendLn(const ::std::string& message)
{
	m_socket.SendLn(message);
}

}	// end of namespace iutest

#endif
