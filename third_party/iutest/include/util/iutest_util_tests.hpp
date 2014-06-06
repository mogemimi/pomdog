//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_util_tests.hpp
 * @brief		iris unit test テスト情報 utility ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2012-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_UTIL_TESTS_HPP_4095FF9B_D6B8_4CD3_BF86_43DFED1760EA_
#define INCG_IRIS_IUTEST_UTIL_TESTS_HPP_4095FF9B_D6B8_4CD3_BF86_43DFED1760EA_

//======================================================================
// include
namespace iuutil
{

//======================================================================
// function

/**
 * @brief	テスト名を取得
*/
inline ::std::string TestFullName(const ::iutest::TestInfo* test_info)
{
	::std::string fullname = test_info->test_case_name();
	fullname += ".";
	fullname += test_info->name();
	return fullname;
}

/**
 * @brief	インデックスを除いたテスト名を取得
*/
inline ::std::string TestNameRemoveIndexName(const char* name)
{
	const char* const p = strrchr(name, '/');
	if( p == NULL )
	{
		return name;
	}
	return ::std::string(name, p);
}

/**
 * @brief	インデックスを除いたテストケース名を取得
*/
inline ::std::string TestCaseNameRemoveIndexName(const char* name)
{
	return TestNameRemoveIndexName(name);
}

/**
 * @brief	インスタンス名とインデックスを除いたテストケース名を取得
*/
inline ::std::string TestCaseNameRemoveInstantiateAndIndexName(const char* name)
{
	// パッケージ名があれば取得
	const char* const pkg = strrchr(name, '.');
	// 先頭にインスタンス名がある
	const char* const p1 = strchr(name, '/');
	if( p1 == NULL )
	{
		return name;
	}
	if( pkg == NULL )
	{
		return TestCaseNameRemoveIndexName(p1 + 1);
	}
	else
	{
		return ::std::string(name, pkg + 1) + TestCaseNameRemoveIndexName(p1 + 1);
	}
}

/**
 * @brief	TestCase の検索
*/
inline const ::iutest::TestCase* FindTestCase(const char* testcase_name)
{
	if( testcase_name == NULL )
	{
		return NULL;
	}
	const int testcase_count = ::iutest::UnitTest::GetInstance()->total_test_case_count();
	for( int i=0; i < testcase_count; ++i )
	{
		const ::iutest::TestCase* testcase = ::iutest::UnitTest::GetInstance()->GetTestCase(i);
		if( strcmp(testcase->name(), testcase_name) == 0 )
		{
			return testcase;
		}
	}
	return NULL;
}

/**
 * @brief	TestCase の検索
*/
inline const ::iutest::TestCase* FindParamTestCase(const char* testcase_name, const ::iutest::TestCase* begin=NULL)
{
	if( testcase_name == NULL )
	{
		return NULL;
	}
	const int testcase_count = ::iutest::UnitTest::GetInstance()->total_test_case_count();
	int i=0;
	if( begin != NULL )
	{
		for( ; i < testcase_count; ++i )
		{
			const ::iutest::TestCase* testcase = ::iutest::UnitTest::GetInstance()->GetTestCase(i);
			if( testcase == begin )
			{
				break;
			}
		}
		++i;
	}
	for( ; i < testcase_count; ++i )
	{
		const ::iutest::TestCase* testcase = ::iutest::UnitTest::GetInstance()->GetTestCase(i);
		const char* testcase_origin_name = strchr(testcase->name(), '/');
		if( testcase_origin_name != NULL )
		{
			if( strcmp(testcase_origin_name+1, testcase_name) == 0 )
			{
				return testcase;
			}
		}
	}
	return NULL;
}

/**
 * @brief	Typed Test の TestCase の検索
*/
inline const ::iutest::TestCase* FindTypedTestCase(const char* testcase_name, const ::iutest::TestCase* begin=NULL)
{
	if( testcase_name == NULL )
	{
		return NULL;
	}
	const int testcase_count = ::iutest::UnitTest::GetInstance()->total_test_case_count();
	int i=0;
	if( begin != NULL )
	{
		for( ; i < testcase_count; ++i )
		{
			const ::iutest::TestCase* testcase = ::iutest::UnitTest::GetInstance()->GetTestCase(i);
			if( testcase == begin )
			{
				break;
			}
		}
		++i;
	}
	for( ; i < testcase_count; ++i )
	{
		const ::iutest::TestCase* testcase = ::iutest::UnitTest::GetInstance()->GetTestCase(i);
		if( testcase != NULL )
		{
			const char* name = testcase->name();
			if( name != NULL
				&& strstr(name, testcase_name) == name
				&& name[strlen(testcase_name)] == '/' )
			{
				return testcase;
			}
		}
	}
	return NULL;
}

/**
 * @brief	Type Parameter Test の TestCase の検索
*/
inline const ::iutest::TestCase* FindParamTypedTestCase(const char* testcase_name, const ::iutest::TestCase* begin=NULL)
{
	if( testcase_name == NULL )
	{
		return NULL;
	}
	const int testcase_count = ::iutest::UnitTest::GetInstance()->total_test_case_count();
	int i=0;
	if( begin != NULL )
	{
		for( ; i < testcase_count; ++i )
		{
			const ::iutest::TestCase* testcase = ::iutest::UnitTest::GetInstance()->GetTestCase(i);
			if( testcase == begin )
			{
				break;
			}
		}
		++i;
	}
	for( ; i < testcase_count; ++i )
	{
		const ::iutest::TestCase* testcase = ::iutest::UnitTest::GetInstance()->GetTestCase(i);
		const char* name = strchr(testcase->name(), '/');
		if( name != NULL )
		{
			++name;
			if( strstr(name, testcase_name) == name
				&& name[strlen(testcase_name)] == '/' )
			{
				return testcase;
			}
		}
	}
	return NULL;
}

/**
 * @brief	TestInfo の検索
*/
inline const ::iutest::TestInfo* FindTestInfo(const ::iutest::TestCase* testcase, const char* testinfo_name)
{
	if( testcase == NULL || testinfo_name == NULL )
	{
		return NULL;
	}

	int testinfo_count = testcase->total_test_count();
	for( int i=0; i < testinfo_count; ++i )
	{
		const ::iutest::TestInfo* testinfo = testcase->GetTestInfo(i);
		if( strcmp(testinfo->name(), testinfo_name) == 0 )
		{
			return testinfo;
		}
	}
	return NULL;
}

/**
 * @brief	TestInfo の検索
*/
inline const ::iutest::TestInfo* FindTestInfo(const char* testcase_name, const char* testinfo_name)
{
	if( testcase_name == NULL || testinfo_name == NULL )
	{
		return NULL;
	}
	const ::iutest::TestCase* testcase = FindTestCase(testcase_name);
	return FindTestInfo(testcase, testinfo_name);
}

/**
 * @brief	TestInfo の検索
*/
inline const ::iutest::TestInfo* FindParamTestInfo(const ::iutest::TestCase* testcase, const char* testinfo_name, const ::iutest::TestInfo* begin=NULL)
{
	if( testcase == NULL || testinfo_name == NULL )
	{
		return NULL;
	}

	const int testinfo_count = testcase->total_test_count();
	int i=0;
	if( begin != NULL )
	{
		for( ; i < testinfo_count; ++i )
		{
			const ::iutest::TestInfo* testinfo = testcase->GetTestInfo(i);
			if( testinfo == begin )
			{
				break;
			}
		}
		++i;
	}

	for( ; i < testinfo_count; ++i )
	{
		const ::iutest::TestInfo* testinfo = testcase->GetTestInfo(i);
		if( testinfo != NULL )
		{
			const char* name = testinfo->name();
			if( name != NULL
				&& strstr(name, testinfo_name) == name
				&& name[strlen(testinfo_name)] == '/' )
			{
				return testinfo;
			}
		}
	}
	return NULL;
}

}	// end of namespace iuutil

#endif // INCG_IRIS_IUTEST_UTIL_TESTS_HPP_4095FF9B_D6B8_4CD3_BF86_43DFED1760EA_
