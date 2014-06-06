//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_message.ipp
 * @brief		iris unit test メッセージ ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2011-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_MESSAGE_IPP_0A05C876_F204_41F5_895F_F8454AB283B1_
#define INCG_IRIS_IUTEST_MESSAGE_IPP_0A05C876_F204_41F5_895F_F8454AB283B1_

//======================================================================
// include
#include "../internal/iutest_message.hpp"

namespace iutest {
namespace detail
{

IUTEST_IPP_INLINE void iuStreamMessage::append(const char* str)
{
	if( str == NULL )
	{
		m_stream << kStrings::Null;
	}
	else
	{
		m_stream << str;
	}
}

IUTEST_IPP_INLINE ::std::string iuCodeMessage::make_message(void) const
{
	::std::string str = FormatFileLocation(m_file, m_line);
	str += ": ";
	str += message();
	return str;
}

IUTEST_IPP_INLINE ::std::string FormatFileLocation(const char* file, int line)
{
	const char* const file_name = file == NULL ? kStrings::UnkownFile : file;
	if( line < 0 )
	{
		return file_name;
	}
	iuStringStream::type strm;
	if( TestFlag::IsEnableFlag(TestFlag::FILELOCATION_STYLE_MSVC) )
	{
		strm << file_name << "(" << line << ")";
	}
	else
	{
		strm << file_name << ":" << line;
	}
	return strm.str();
}

IUTEST_IPP_INLINE ::std::string FormatCompilerIndependentFileLocation(const char* file, int line)
{
	const char* const file_name = file == NULL ? kStrings::UnkownFile : file;
	if( line < 0 )
	{
		return file_name;
	}
	iuStringStream::type strm;
	strm << file_name << ":" << line;
	return strm.str();
}

IUTEST_IPP_INLINE void iuCodeMessage::add_message(const ::std::string& str)
{
	m_message += str;
}

}	// end of namespace detail
}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_MESSAGE_IPP_0A05C876_F204_41F5_895F_F8454AB283B1_
