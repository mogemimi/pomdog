//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_core_impl.hpp
 * @brief		iris unit test UnitTest 実装 ファイル
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
#ifndef INCG_IRIS_IUTEST_CORE_IMPL_HPP_D5ABC7DE_C751_4AC0_922F_547880163891_
#define INCG_IRIS_IUTEST_CORE_IMPL_HPP_D5ABC7DE_C751_4AC0_922F_547880163891_

//======================================================================
// include
#include "iutest_internal.hpp"
#include "iutest_option_message.hpp"
#include "../iutest_case.hpp"

namespace iutest
{

//======================================================================
// class
class UnitTestImpl
{
protected:
	typedef detail::iu_list<TestCase>	iuTestCases;
	//typedef ::std::vector<TestCase*>	iuTestCases;
	typedef ::std::vector<Environment*>	iuEnvironmentList;
protected:
	UnitTestImpl(void) : m_total_test_num(0), m_disable_num(0), m_should_run_num(0)
		, m_current_testcase(NULL), m_elapsedmsec(0)
	{
		ptr() = this;
	}
	~UnitTestImpl(void) { TerminateImpl(); }

public:
	/**
	 * @brief	テスト中のテストの TestResult の取得
	*/
	static TestResult*	current_test_result(void);

public:
	/**
	 * @brief	有効なプロパティ名かどうかチェック
	 * @param [in] name	= プロパティ名
	 * @retval	true=有効
	 * @retval	false=無効
	*/
	static bool ValidateTestPropertyName(const ::std::string& name)
	{
		const char* ban[] = { "name", "tests", "failures", "disabled", "skip", "errors", "time", "timestamp", "random_seed" };
#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
		return TestProperty::ValidateName(name, ban);
#else
		return TestProperty::ValidateName(name, ban, ban+IUTEST_PP_COUNTOF(ban));
#endif
	}

public:
	/** @private */
	template<typename T>
	TestCase*	AddTestCase(const char* testcase_name, TestTypeId id, SetUpMethod setup, TearDownMethod teardown
		IUTEST_APPEND_EXPLICIT_TEMPLATE_TYPE_(T)
		)
	{
		TestCase::FindOp func = { id, testcase_name };
		iuTestCases& list = m_testcases;
		TestCase* p = detail::FindList(list, func);
		if( p == NULL )
		{
			p = new T (testcase_name, id, setup, teardown);
			list.push_back(p);
		}
		return p;
	}
	/** @private */
	void AddTestInfo(TestCase* pCase, TestInfo* pInfo);
	/** @private */
	static void SkipTest(void);

protected:
	/**
	 * @brief	テストのリストアップ
	*/
	int		Listup(void) const;

	/**
	 * @brief	テストのリストアップ(with where)
	*/
	int		ListupWithWhere(void) const;

	/**
	 * @brief	事前処理
	*/
	bool	PreRunner(void);

	/**
	 * @brief	テスト結果のクリア
	*/
	void	ClearNonAdHocTestResult(void);

	/**
	 * @brief	ad_hoc_testresult のクリア
	*/
	void	ClearAdHocTestResult(void)
	{
		m_ad_hoc_testresult.Clear();
	}

private:
	/**
	 * @brief	RecordProperty
	*/
	static 	void RecordProperty(const TestProperty& prop);

private:
	/**
	 * @brief	セットアップ
	*/
	void	InitializeImpl(void)
	{
#if IUTEST_HAS_SEH

#if !defined(IUTEST_OS_WINDOWS_MOBILE)
		SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
#endif

#if	(defined(_MSC_VER) || IUTEST_OS_WINDOWS_MINGW) && !defined(IUTEST_OS_WINDOWS_MOBILE)
		_set_error_mode(_OUT_TO_STDERR);
#endif

#endif

#if defined(_MSC_VER) && _MSC_VER >= 1400 && !defined(IUTEST_OS_WINDOWS_MOBILE)
		if( !TestFlag::IsEnableFlag(TestFlag::BREAK_ON_FAILURE) )
		{
			_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
		}
#endif

#if IUTEST_HAS_EXCEPTIONS && (defined(_MSC_VER) && (_MSC_VER >= 1400)) && !defined(IUTEST_OS_WINDOWS_MOBILE)
		_set_invalid_parameter_handler(OnInvalidParameter);
#endif
	}
	/**
	 * @brief	後片付け
	*/
	void	TerminateImpl(void);

private:
#if IUTEST_HAS_EXCEPTIONS && (defined(_MSC_VER) && (_MSC_VER >= 1400)) && !defined(IUTEST_OS_WINDOWS_MOBILE)

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_BEGIN()

	// _invalid_parameter_handler
	static void OnInvalidParameter(const wchar_t * expression, const wchar_t * function
		, const wchar_t * file, unsigned int line, uintptr_t pReserved);

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_END()

#endif

private:
	static UnitTestImpl*& ptr(void) IUTEST_CXX_NOEXCEPT_SPEC
	{
		static UnitTestImpl* ptr = NULL;
		return ptr;
	}
protected:
	friend class UnitTestSource;
	friend class Test::TestRecordPropertyHelper;

	int				m_total_test_num;	//!< 登録されたテスト総数
	int				m_disable_num;		//!< 無視したテスト総数
	int				m_should_run_num;	//!< 実行すべきテスト総数
	TestCase*		m_current_testcase;	//!< 現在実行中のテストケース
	TimeInMillisec	m_elapsedmsec;		//!< テストの実行時間
	iuTestCases		m_testcases;		//!< テストケースリスト
	TestResult		m_ad_hoc_testresult;	//!< テストが実行中でないときのリザルト
};

namespace detail
{

/**
 * @brief	テスト名の作成
 * @param [in]	basename	= ベース名
 * @param [in]	index		= インデックス
*/
::std::string MakeIndexTestName(const char* basename, int index);

/**
 * @brief	テスト名の作成
 * @param [in]	basename	= ベース名
 * @param [in]	index		= インデックス
*/
template<typename T>
::std::string MakeIndexTypedTestName(const char* basename, int index)
{
#if IUTEST_HAS_RTTI
	::std::string name = basename;
	iuStringStream::type strm; strm << index;
	name += "/";
	name += GetTypeName<T>();
	return name;
#else
	return MakeIndexTestName(basename, index);
#endif
}

/**
 * @brief	テスト名の作成
 * @param [in]	prefix		= prefix
 * @param [in]	basename	= ベース名
 * @param [in]	index		= インデックス
*/
::std::string MakePrefixedIndexTestName(const char* prefix, const char* basename, int index);

/**
 * @brief	テスト名の作成
 * @param [in]	prefix		= prefix
 * @param [in]	basename	= ベース名
 * @param [in]	index		= インデックス
*/
template<typename T>
::std::string MakePrefixedIndexTypedTestName(const char* prefix, const char* basename, int index)
{
#if IUTEST_HAS_RTTI
	::std::string name = prefix;
	if( !name.empty() ) name += "/";
	name += MakeIndexTypedTestName<T>(basename, index);
	return name;
#else
	return MakePrefixedIndexTestName(prefix, basename, index);
#endif
}

}	// end of namespace detail

}	// end of namespace iutest

#if !IUTEST_HAS_LIB
#  include "../impl/iutest_core_impl.ipp"
#endif

#endif
