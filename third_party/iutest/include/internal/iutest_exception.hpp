//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_exception.hpp
 * @brief		iris unit test exception
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
#ifndef INCG_IRIS_IUTEST_EXCEPTION_HPP_B73B2E25_97C8_4DFC_BC32_5E53039A1A64_
#define INCG_IRIS_IUTEST_EXCEPTION_HPP_B73B2E25_97C8_4DFC_BC32_5E53039A1A64_

//======================================================================
// include
#include "../iutest_defs.hpp"

#if IUTEST_HAS_EXCEPTIONS
#include "iutest_string.hpp"
#include <exception>
//#include <stdexcept>	// std::invalid_argment

namespace iutest {
namespace detail
{

/**
 * @brief	C++例外メッセージのフォーマット
 * @param [in]	description	= 説明
 * @return	メッセージ
*/
inline ::std::string FormatCxxException(const char* description)
{
	iuStringStream::type strm;
	if( description != NULL )
	{
		strm << "C++ exception with description \"" << description << "\"";
	}
	else
	{
		strm << "Unknown C++ exception";
	}
	return strm.str();
}

}	// end of namespace detail
}	// end of namespace iutest

#if IUTEST_HAS_EXCEPTIONS && IUTEST_HAS_SEH
#include <iomanip>

namespace iutest {
namespace detail
{

/**
 * @brief	SEH 例外
*/
class seh_exception : public ::std::exception
{
public:
	seh_exception(const char *const& _What) : ::std::exception(_What) {}
	seh_exception(void) : ::std::exception() {}
public:
	static void	translator(DWORD code, _EXCEPTION_POINTERS* ep)
	{
		IUTEST_UNUSED_VAR(ep);
		iuStringStream::type strm;
#if IUTEST_HAS_STRINGSTREAM || IUTEST_HAS_STRSTREAM
		strm << "SEH exception with code 0x" << ::std::setbase(16) << code << ::std::setbase(10);
#else
		strm << "SEH exception with code " << code;
#endif
		throw seh_exception(strm.str().c_str());
	}
	static int should_process_through_break_and_cppexceptions(DWORD code)
	{
		bool should_handle = true;
		// break point と C++ 例外はハンドリングしない
		if( code == EXCEPTION_BREAKPOINT )
			should_handle = false;
		if( code == kCxxExceptionCode )
			should_handle = false;
		return should_handle ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH;
	}
public:
	static const DWORD kCxxExceptionCode = 0xe06d7363;	//!< c++ exception コード
};

}	// end of namespace detail
}	// end of namespace iutest

#endif

#endif

#endif	// INCG_IRIS_IUTEST_EXCEPTION_HPP_B73B2E25_97C8_4DFC_BC32_5E53039A1A64_
