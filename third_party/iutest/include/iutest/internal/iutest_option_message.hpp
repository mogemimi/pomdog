//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_option_message.hpp
 * @brief		iris unit test コマンドラインメッセージ ファイル
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
#ifndef INCG_IRIS_iutest_option_message_HPP_00EB9B17_0615_4678_9AD0_1F5B295B404F_
#define INCG_IRIS_iutest_option_message_HPP_00EB9B17_0615_4678_9AD0_1F5B295B404F_

//======================================================================
// include
#include "iutest_console.hpp"

namespace iutest {
namespace detail
{

//======================================================================
// class
class iuOptionMessage
{
public:
	/**
	 * @brief	ヘルプの出力
	*/
	static void	ShowHelp(void);
	/**
	 * @brief	バージョン出力
	*/
	static void	ShowVersion(void);
	/**
	 * @brief	機能出力
	*/
	static void ShowFeature(void);
};

inline void	iuOptionMessage::ShowHelp(void)
{
	const char* readme = 
		"--------------------------------------------------\n"
		"Name\n"
		"    iutest - iris unit test framework\n"
		"--------------------------------------------------\n"
		"Command Line Options\n"
		"\n"
		"    --help, -h                       : generate help message.\n"
		"    --iutest_output=xml[:path]       : path of xml report.\n"
		"    --iutest_list_tests              : list up tests.\n"
		"    --iutest_list_tests_with_where   : list up tests with where.\n"
		"    --iutest_color=<yes|no|auto|ansi>: console color enable.\n"
		"    --iutest_filter=<filter>         : Select the test run.\n"
		"    --iutest_shuffle                 : do shuffle test.\n"
		"    --iutest_random_seed=<seed>      : set random seed.\n"
		"    --iutest_also_run_disabled_tests : run disabled tests.\n"
		"    --iutest_break_on_failure[=0|1]  : When that failed to break.\n"
		"    --iutest_throw_on_failure[=0|1]  : When that failed to throw.\n"
		"    --iutest_catch_exceptions=<0|1>  : catch exceptions enable.\n"
		"    --iutest_print_time=<0|1>        : Setting the display of elapsed time.\n"
		"    --iutest_repeat=<count>          : Set the number of repetitions\n"
		"                                       of the test.\n"
#if IUTEST_HAS_STREAM_RESULT
		"    --iutest_stream_result_to=<host:port>\n"
		"                                     : Set stream test results server.\n"
#endif
		"    --iutest_file_location=<auto|vs|gcc>\n"
		"                                     : Format file location messages.\n"
		"    --feature                        : show iutest feature.\n"
		"    --version, -v                    : show iutest version.\n"
		"\n"
		"--------------------------------------------------\n"
		"License\n"
		"\n"
		"    Copyright (c) 2011-2013, Takazumi-Shirayanagi\n"
		"\n"
		"    The new(3-clause) BSD License is applied to this software, see LICENSE\n"
		"\n"
		;
	detail::iuConsole::color_output(detail::iuConsole::cyan, readme);
}

inline void	iuOptionMessage::ShowVersion(void)
{
	detail::iuConsole::output("iutest version %x.%x.%x.%x\n"
		, IUTEST_MAJORVER, IUTEST_MINORVER, IUTEST_BUILD, IUTEST_REVISION);
}

inline void iuOptionMessage::ShowFeature(void)
{
#define SHOW_MACRO(macro)	detail::iuConsole::output("#define %s  %s\n", #macro, IUTEST_PP_TOSTRING(macro))
	SHOW_MACRO(IUTEST_HAS_ANY_PARAM_TEST);
	SHOW_MACRO(IUTEST_HAS_ASSERTION_RETURN);
	SHOW_MACRO(IUTEST_HAS_CHAR16_T);
	SHOW_MACRO(IUTEST_HAS_CHAR32_T);
	SHOW_MACRO(IUTEST_HAS_CLOCK);
	SHOW_MACRO(IUTEST_HAS_COMBINE);
	SHOW_MACRO(IUTEST_HAS_CONCAT);
	SHOW_MACRO(IUTEST_HAS_CONSTEXPR);
	SHOW_MACRO(IUTEST_HAS_CTIME);
	SHOW_MACRO(IUTEST_HAS_CXX11);
	SHOW_MACRO(IUTEST_HAS_DELETED_FUNCTIONS);
	SHOW_MACRO(IUTEST_HAS_EXCEPTIONS);
	SHOW_MACRO(IUTEST_HAS_GENRAND);
	SHOW_MACRO(IUTEST_HAS_GETTIMEOFDAY);
	SHOW_MACRO(IUTEST_HAS_IF_EXISTS);
	SHOW_MACRO(IUTEST_HAS_IGNORE_TEST);
	SHOW_MACRO(IUTEST_HAS_LIB);
	SHOW_MACRO(IUTEST_HAS_MINIDUMP);
	SHOW_MACRO(IUTEST_HAS_NULLPTR);
	SHOW_MACRO(IUTEST_HAS_PACKAGE);
	SHOW_MACRO(IUTEST_HAS_PAIRWISE);
	SHOW_MACRO(IUTEST_HAS_PARAM_METHOD_TEST);
	SHOW_MACRO(IUTEST_HAS_PARAM_TEST);
	SHOW_MACRO(IUTEST_HAS_PEEP);
	SHOW_MACRO(IUTEST_HAS_PEEP_FUNC);
	SHOW_MACRO(IUTEST_HAS_PEEP_STATIC_FUNC);
	SHOW_MACRO(IUTEST_HAS_PRINT_TO);
	SHOW_MACRO(IUTEST_HAS_RANDOMVALUES);
	SHOW_MACRO(IUTEST_HAS_RTTI);
	SHOW_MACRO(IUTEST_HAS_RVALUE_REFS);
	SHOW_MACRO(IUTEST_HAS_SEH);
	SHOW_MACRO(IUTEST_HAS_SOCKET);
	SHOW_MACRO(IUTEST_HAS_SPI_LAMBDA_SUPPORT);
	SHOW_MACRO(IUTEST_HAS_STATIC_ASSERT);
	SHOW_MACRO(IUTEST_HAS_STATIC_ASSERT_TYPEEQ);
	SHOW_MACRO(IUTEST_HAS_STREAM_RESULT);
	SHOW_MACRO(IUTEST_HAS_TESTNAME_ALIAS);
	SHOW_MACRO(IUTEST_HAS_TESTNAME_ALIAS_JP);
	SHOW_MACRO(IUTEST_HAS_TUPLE);
	SHOW_MACRO(IUTEST_HAS_TYPED_TEST);
	SHOW_MACRO(IUTEST_HAS_TYPED_TEST_P);
	SHOW_MACRO(IUTEST_HAS_VALUESGEN);
	SHOW_MACRO(IUTEST_HAS_VARIADIC_COMBINE);
	SHOW_MACRO(IUTEST_HAS_VARIADIC_PAIRWISE);
	SHOW_MACRO(IUTEST_HAS_VARIADIC_TEMPLATES);
	SHOW_MACRO(IUTEST_HAS_VARIADIC_TEMPLATE_TEMPLATES);
	SHOW_MACRO(IUTEST_HAS_VARIADIC_VALUES);
	SHOW_MACRO(IUTEST_PLATFORM);
	SHOW_MACRO(IUTEST_REPORT_SKIPPED);
	SHOW_MACRO(IUTEST_USE_THROW_ON_ASSERT_FAILURE);

#undef SHOW_MACRO
}

}	// end of namespace detail
}	// end of namespace iutest

#endif
