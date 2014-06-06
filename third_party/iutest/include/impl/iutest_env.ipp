//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_env.ipp
 * @brief		iris unit test 環境 ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2011-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_ENV_IPP_F4017EAB_6CA3_4E6E_8983_059393DADD04_
#define INCG_IRIS_IUTEST_ENV_IPP_F4017EAB_6CA3_4E6E_8983_059393DADD04_

//======================================================================
// include
#include "../iutest_env.hpp"

namespace iutest
{

IUTEST_IPP_INLINE ::std::string TestEnv::get_report_xml_filepath(void)
{
	const ::std::string& option = get_vars().m_output_option;
	if(option.find("xml") != ::std::string::npos)
	{
		const ::std::string::size_type pos = option.find("xml:");
		if(pos != ::std::string::npos)
		{
			return option.substr(pos + 4);
		}
		return detail::kStrings::DefaultXmlReportFileName;
	}
	return "";
}


IUTEST_IPP_INLINE bool TestEnv::ParseCommandLineElemA(const char* str)
{
	bool find = false;
	if( *str == '-' )
	{
		find = true;
		++str;
		if( *str == '-' )
		{
			++str;
			bool iuoption = false;
			{
				const char* const base_str = str;
				if( *str == 'g' )
				{
					++str;
					iuoption = true;
				}
				if(*str == 'i' && *(str+1) == 'u' )
				{
					str += 2;
					iuoption = true;
				}
				const char option_prefix[] = "test_";
				for( int i=0, size=sizeof(option_prefix)/sizeof(option_prefix[0])-1; i < size; ++i, ++str )
				{
					if( *str != option_prefix[i] )
					{
						iuoption = false;
						str = base_str;
						break;
					}
				}
			}
			if( iuoption )
			{
				// --iutest_*
				if( detail::IsStringForwardMatching(str, "output") )
				{
					find = ParseOutputOption(ParseOptionSettingStr(str));
				}
				else if( detail::IsStringEqual(str, "list_tests") )
				{
					TestFlag::SetFlag(TestFlag::SHOW_TESTS_LIST);
				}
				else if( detail::IsStringEqual(str, "list_tests_with_where") )
				{
					TestFlag::SetFlag(TestFlag::SHOW_TESTS_LIST_WITH_WHERE);
				}
				else if( detail::IsStringForwardMatching(str, "color") )
				{
					find = ParseColorOption(ParseOptionSettingStr(str));
				}
				else if( detail::IsStringEqual(str, "shuffle") )
				{
					TestFlag::SetFlag(TestFlag::SHUFFLE_TESTS);
				}
				else if( detail::IsStringForwardMatching(str, "random_seed") )
				{
					const char* opt = ParseOptionSettingStr(str);
					if( opt != NULL )
					{
						char* end = NULL;
						long seed = strtol(opt, &end, 0);
						init_random(static_cast<unsigned int>(seed));
					}
					else
					{
						find = false;
					}
				}
				else if( detail::IsStringEqual(str, "also_run_disabled_tests") )
				{
					TestFlag::SetFlag(TestFlag::RUN_DISABLED_TESTS);
				}
				else if( detail::IsStringForwardMatching(str, "break_on_failure") )
				{
					find = ParseYesNoFlagCommandLine(str, TestFlag::BREAK_ON_FAILURE, 1);
				}
				else if( detail::IsStringForwardMatching(str, "catch_exceptions") )
				{
					find = ParseYesNoFlagCommandLine(str, TestFlag::CATCH_EXCEPTION, -1);
				}
				else if( detail::IsStringForwardMatching(str, "throw_on_failure") )
				{
					find = ParseYesNoFlagCommandLine(str, TestFlag::THROW_ON_FAILURE, 1);
				}
				else if( detail::IsStringForwardMatching(str, "print_time") )
				{
					find = ParseYesNoFlagCommandLine(str, TestFlag::PRINT_TIME, -1);
				}
				else if( detail::IsStringForwardMatching(str, "repeat") )
				{
					const char* opt = ParseOptionSettingStr(str);
					if( opt != NULL )
					{
						char* end = NULL;
						long count = strtol(opt, &end, 0);
						set_repeat_count(static_cast<int>(count));
					}
					else
					{
						find = false;
					}
				}
				else if( detail::IsStringForwardMatching(str, "filter") )
				{
					set_test_filter(ParseOptionSettingStr(str));
				}
#if IUTEST_HAS_STREAM_RESULT
				else if( detail::IsStringForwardMatching(str, "stream_result_to") )
				{
					const char* opt = ParseOptionSettingStr(str);
					if( opt != NULL )
					{
						set_stream_result_to(opt);
					}
					else
					{
						find = false;
					}
				}
#endif
				else if( detail::IsStringForwardMatching(str, "file_location") )
				{
					find = ParseFileLocationOption(ParseOptionSettingStr(str));
				}
				else
				{
					find = false;
				}
				if( !find )
				{
					// 該当するオプションがない場合はヘルプ表示
					TestFlag::SetFlag(TestFlag::SHOW_HELP);
				}
			}
			else if( detail::IsStringEqual(str, "help") )
			{
				TestFlag::SetFlag(TestFlag::SHOW_HELP);
			}
			else if( detail::IsStringEqual(str, "version") )
			{
				TestFlag::SetFlag(TestFlag::SHOW_VERSION);
			}
			else if( detail::IsStringEqual(str, "feature") )
			{
				TestFlag::SetFlag(TestFlag::SHOW_FEATURE);
			}
			else
			{
				find = false;
			}
		}
		else
		{
			if( detail::IsStringEqual(str, "v") )
			{
				TestFlag::SetFlag(TestFlag::SHOW_VERSION);
			}
			else if( detail::IsStringEqual(str, "h")
				|| detail::IsStringEqual(str, "?") )
			{
				TestFlag::SetFlag(TestFlag::SHOW_HELP);
			}
			else
			{
				find = false;
			}
		}
	}
	return find;
}

IUTEST_IPP_INLINE void TestEnv::LoadEnviromentVariable(void)
{
	{
		int var = 0;
		if( detail::GetEnvironmentInt("IUTEST_ALSO_RUN_DISABLED_TESTS", var)
		||  detail::GetEnvironmentInt("GTEST_ALSO_RUN_DISABLED_TESTS", var) )
		{
			TestFlag::SetFlag(TestFlag::RUN_DISABLED_TESTS
				, var ? TestFlag::MASK : ~(TestFlag::RUN_DISABLED_TESTS) );
		}
		if( detail::GetEnvironmentInt("IUTEST_SHUFFLE", var)
		||  detail::GetEnvironmentInt("GTEST_SHUFFLE", var) )
		{
			TestFlag::SetFlag(TestFlag::SHUFFLE_TESTS
				, var ? TestFlag::MASK : ~(TestFlag::SHUFFLE_TESTS) );
		}
		if( detail::GetEnvironmentInt("IUTEST_RANDOM_SEED", var)
		||  detail::GetEnvironmentInt("GTEST_RANDOM_SEED", var) )
		{
			init_random((unsigned int)var);
		}
		if( detail::GetEnvironmentInt("IUTEST_CATCH_EXCEPTIONS", var)
		||  detail::GetEnvironmentInt("GTEST_CATCH_EXCEPTIONS", var) )
		{
			TestFlag::SetFlag(TestFlag::CATCH_EXCEPTION
				, var ? TestFlag::MASK : ~(TestFlag::CATCH_EXCEPTION) );
		}
		if( detail::GetEnvironmentInt("IUTEST_BREAK_ON_FAILURE", var)
		||  detail::GetEnvironmentInt("GTEST_BREAK_ON_FAILURE", var) )
		{
			TestFlag::SetFlag(TestFlag::BREAK_ON_FAILURE
				, var ? TestFlag::MASK : ~(TestFlag::BREAK_ON_FAILURE) );
		}
		if( detail::GetEnvironmentInt("IUTEST_THROW_ON_FAILURE", var)
		||  detail::GetEnvironmentInt("GTEST_THROW_ON_FAILURE", var) )
		{
			TestFlag::SetFlag(TestFlag::THROW_ON_FAILURE
				, var ? TestFlag::MASK : ~(TestFlag::THROW_ON_FAILURE) );
		}
		if( detail::GetEnvironmentInt("IUTEST_PRINT_TIME", var)
		||  detail::GetEnvironmentInt("GTEST_PRINT_TIME", var) )
		{
			TestFlag::SetFlag(TestFlag::PRINT_TIME
				, var ? TestFlag::MASK : ~(TestFlag::PRINT_TIME) );
		}
		if( detail::GetEnvironmentInt("IUTEST_REPEAT", var)
		||  detail::GetEnvironmentInt("GTEST_REPEAT", var) )
		{
			set_repeat_count(var);
		}
	}
	{
		char var[128] = {0};
		if( detail::GetEnvironmentVariable("IUTEST_COLOR", var, sizeof(var))
		||  detail::GetEnvironmentVariable("GTEST_COLOR", var, sizeof(var)) )
		{
			ParseColorOption(var);
		}
		if( detail::GetEnvironmentVariable("IUTEST_FILE_LOCATION", var, sizeof(var)) )
		{
			ParseFileLocationOption(var);
		}
	}
	{
		char str[260+32] = {0};
		if( detail::GetEnvironmentVariable("IUTEST_OUTPUT", str, sizeof(str))
		||  detail::GetEnvironmentVariable("GTEST_OUTPUT", str, sizeof(str)) )
		{
			ParseOutputOption(str);
		}
		if( detail::GetEnvironmentVariable("IUTEST_FILTER", str, sizeof(str))
		||  detail::GetEnvironmentVariable("GTEST_FILTER", str, sizeof(str)) )
		{
			set_test_filter(str);
		}
#if IUTEST_HAS_STREAM_RESULT
		if( detail::GetEnvironmentVariable("IUTEST_STREAM_RESULT_TO", str, sizeof(str))
		||  detail::GetEnvironmentVariable("GTEST_STREAM_RESULT_TO", str, sizeof(str)) )
		{
			set_stream_result_to(str);
		}
#endif
	}
}

IUTEST_IPP_INLINE void TestEnv::SetUp(void)
{
	unsigned int seed = get_random_seed();
	if( (seed == 0)
		|| (get_vars().m_current_random_seed != 0 && TestFlag::IsEnableFlag(TestFlag::SHUFFLE_TESTS)) )
	{
		const unsigned int gen_seeed = detail::GetIndefiniteValue();
		if( get_vars().m_current_random_seed == gen_seeed
			|| get_vars().m_before_origin_random_seed == gen_seeed )
		{
			seed = get_vars().m_current_random_seed;
		}
		seed += gen_seeed;
		get_vars().m_before_origin_random_seed = gen_seeed;
	}
	get_vars().m_current_random_seed = seed;
	genrand().init(seed);
}

IUTEST_IPP_INLINE bool TestEnv::ParseColorOption(const char* option)
{
	if( option == NULL )
	{
		return false;
	}

	if( IsYes(option) )
	{
		TestFlag::SetFlag(TestFlag::CONSOLE_COLOR_ON, ~(TestFlag::CONSOLE_COLOR_OFF|TestFlag::CONSOLE_COLOR_ANSI));
	}
	else if( IsNo(option) )
	{
		TestFlag::SetFlag(TestFlag::CONSOLE_COLOR_OFF, ~(TestFlag::CONSOLE_COLOR_ON|TestFlag::CONSOLE_COLOR_ANSI));
	}
	else if( detail::IsStringCaseEqual(option, "auto") )
	{
		// auto
		TestFlag::SetFlag(0, ~(TestFlag::CONSOLE_COLOR_ON|TestFlag::CONSOLE_COLOR_OFF|TestFlag::CONSOLE_COLOR_ANSI));
	}
	else if( detail::IsStringCaseEqual(option, "ansi") )
	{
		// ansi
		TestFlag::SetFlag(TestFlag::CONSOLE_COLOR_ON|TestFlag::CONSOLE_COLOR_ANSI, ~TestFlag::CONSOLE_COLOR_OFF);
	}
	else
	{
		return false;
	}
	return true;
}

IUTEST_IPP_INLINE bool TestEnv::ParseOutputOption(const char* option)
{
	if(option == NULL)
	{
		get_vars().m_output_option = "";
		return false;
	}
	get_vars().m_output_option = option;
	return true;
}

IUTEST_IPP_INLINE bool TestEnv::ParseFileLocationOption(const char* option)
{
	if( option == NULL )
	{
		return false;
	}

	if( detail::IsStringCaseEqual(option, "auto") )
	{
#ifdef _MSC_VER
		TestFlag::SetFlag(TestFlag::FILELOCATION_STYLE_MSVC);
#else
		TestFlag::SetFlag(0, ~TestFlag::FILELOCATION_STYLE_MSVC);
#endif
	}
	else if( detail::IsStringCaseEqual(option, "vs") )
	{
		TestFlag::SetFlag(TestFlag::FILELOCATION_STYLE_MSVC);
	}
	else if( detail::IsStringCaseEqual(option, "gcc") )
	{
		TestFlag::SetFlag(0, ~TestFlag::FILELOCATION_STYLE_MSVC);
	}
	else
	{
		return false;
	}
	return true;
}

IUTEST_IPP_INLINE bool TestEnv::ParseYesNoFlagCommandLine(const char* str, TestFlag::Kind flag, int def)
{
	const char* option = ParseOptionSettingStr(str);
	const int yesno = option != NULL ? ParseYesNoOption(option) : def;
	if( yesno < 0 )
	{
		return false;
	}
	TestFlag::SetFlag(flag, yesno ? TestFlag::MASK : ~(flag) );
	return true;
}

IUTEST_IPP_INLINE int TestEnv::ParseYesNoOption(const char* option)
{
	if( option != NULL )
	{
		if( IsYes(option) )
		{
			return 1;
		}
		if( IsNo(option) )
		{
			return 0;
		}
	}

	return -1;
}

IUTEST_IPP_INLINE bool TestEnv::IsYes(const char* option)
{
	if( detail::IsStringCaseEqual(option, "yes")
		|| detail::IsStringCaseEqual(option, "y")
		|| detail::IsStringCaseEqual(option, "on")
		|| detail::IsStringCaseEqual(option, "true")
		|| detail::IsStringCaseEqual(option, "t")
		|| detail::IsStringEqual(option, "1") )
	{
		return true;
	}
	return false;
}

IUTEST_IPP_INLINE bool TestEnv::IsNo(const char* option)
{
	if( detail::IsStringCaseEqual(option, "no")
		|| detail::IsStringCaseEqual(option, "n")
		|| detail::IsStringCaseEqual(option, "off")
		|| detail::IsStringCaseEqual(option, "false")
		|| detail::IsStringCaseEqual(option, "f")
		|| detail::IsStringEqual(option, "0") )
	{
		return true;
	}
	return false;
}

}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_ENV_IPP_F4017EAB_6CA3_4E6E_8983_059393DADD04_
