//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_case.hpp
 * @brief		iris unit test testcase ファイル
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
#ifndef INCG_IRIS_IUTEST_CASE_HPP_F57C9B7E_7CAA_4429_BE75_FCAAEED1B220_
#define INCG_IRIS_IUTEST_CASE_HPP_F57C9B7E_7CAA_4429_BE75_FCAAEED1B220_

//======================================================================
// include
#include "iutest_info.hpp"

namespace iutest
{

//======================================================================
// class
/**
 * @brief	テストケース
*/
class TestCase
	: public detail::iu_list_node<TestCase>
{
protected:
	typedef detail::iu_list<TestInfo>	iuTestInfos;	//!< TestInfo リスト
	//typedef ::std::vector<TestInfo*>	iuTestInfos;	//!< TestInfo リスト
protected:
	/**
	 * @brief	コンストラクタ
	 * @param [in]	testcase_name	= テストケース名
	 * @param [in]	id				= テストケースID
	 * @param [in]	setup			= テスト事前実行関数
	 * @param [in]	teardown		= テスト事後実行関数
	*/
	TestCase(const char* testcase_name, TestTypeId id, SetUpMethod setup, TearDownMethod teardown)
	: m_testcase_name(testcase_name)
	, m_setup(setup), m_teardown(teardown)
	, m_id(id), m_disable_num(0)
	, m_should_run_num(0)
	, m_elapsedmsec(0)
	, m_disable(false)
	{
		if( detail::IsStringForwardMatching(testcase_name, "DISABLED_") )
		{
			m_disable = true;
		}
	}

public:
	/**
	 * @brief	デストラクタ
	*/
	virtual ~TestCase(void) {}

public:
	/** test case 名の取得 */
	const char*		name(void)					const { return m_testcase_name.c_str(); }

	/** テスト総数 */
	int				total_test_count(void)		const IUTEST_CXX_NOEXCEPT_SPEC { return m_testinfos.size(); }
	/** レポート対象のテスト総数 */
	int				reportable_test_count(void)	const;
	/** 実行したテスト総数 */
	int				test_to_run_count(void)		const IUTEST_CXX_NOEXCEPT_SPEC { return m_should_run_num; }
	/** 失敗テスト総数 */
	int				failed_test_count(void)		const;
	/** 無効テスト総数 */
	int				disabled_test_count(void)	const IUTEST_CXX_NOEXCEPT_SPEC { return m_disable_num; }
	/** レポート対象の無効テスト総数 */
	int				reportable_disabled_test_count(void) const;
	/** 成功テスト総数 */
	int				successful_test_count(void)	const;
	/** スキップテスト総数 */
	int				skip_test_count(void)		const;
	/** レポート対象のスキップテスト総数 */
	int				reportable_skip_test_count(void) const;
	/** 明示的にスキップされたテスト総数 */
	int				test_run_skipped_count(void) const;
	/** レポート対象の明示的にスキップされたテスト総数 */
	int				reportable_test_run_skipped_count(void) const;
	/** テストの実行ミリ秒 */
	TimeInMillisec	elapsed_time(void)			const IUTEST_CXX_NOEXCEPT_SPEC { return m_elapsedmsec; }

	/** TestInfo の取得 */
	const TestInfo*	GetTestInfo(int index)		const { return m_testinfos[index]; }
	/** should_run */
	bool			should_run(void)			const IUTEST_CXX_NOEXCEPT_SPEC { return m_should_run_num != 0; }

	/** テストが成功したかどうか */
	bool			Passed(void)				const { return failed_test_count() == 0 && m_ad_hoc_testresult.Passed(); }
	/** テストが失敗したかどうか */
	bool			Failed(void)				const { return !Passed(); }

	/** type param 文字列の取得 */
	virtual const char*	type_param(void)		const { return NULL; }

	/** TestCase 出力情報の取得 */
	::std::string testcase_name_with_where(void) const
	{
		::std::string str = m_testcase_name;
		if( type_param() != NULL )
		{
			str += ", where TypeParam = ";
			str += type_param();
		}
		return str;
	}

	/** テスト実行中じゃないときのリザルトの取得 */
	const TestResult* ad_hoc_testresult(void) const IUTEST_CXX_NOEXCEPT_SPEC { return &m_ad_hoc_testresult; }

public:
	/**
	 * @brief	有効なプロパティ名かどうかチェック
	 * @param [in] name	= プロパティ名
	 * @retval	true=有効
	 * @retval	false=無効
	*/
	static bool ValidateTestPropertyName(const ::std::string& name)
	{
		const char* ban[] = { "name", "tests", "failures", "disabled", "skip", "errors", "time" };
#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
		return TestProperty::ValidateName(name, ban);
#else
		return TestProperty::ValidateName(name, ban, ban+IUTEST_PP_COUNTOF(ban));
#endif
	}

private:
	/**
	 * @brief	テストの実行
	 * @return	成否
	*/
	bool Run(void);

	/**
	 * @brief	実行
	 * @return	成否
	*/
	bool RunImpl(void);

public:
	/**
	 * @private
	 * @brief	テストケース比較用オブジェクト
	*/
	struct FindOp
	{
		TestTypeId	m_id;
		const char*	m_name;

		bool operator () (const TestCase* p) const
		{
			if( p->get_typeid() == m_id && (strcmp(p->name(), m_name) == 0) )
			{
				return true;
			}
			return false;
		}
	};
private:
	/**
	 * @brief	テストのクリア
	*/
	void clear(void);
	/*
	 * @brief	テストのフィルタリング
	 * @return	実行する場合は真
	*/
	bool filter(void);

private:
	friend bool operator == (const TestCase& lhs, const TestCase& rhs)
	{
		return (lhs.m_id == rhs.m_id) && (strcmp(lhs.name(), rhs.name()) == 0);
	}

	void push_back(TestInfo* p) { m_testinfos.push_back(p); }

private:
	iuTestInfos::const_iterator	begin(void)	const		{ return m_testinfos.begin(); }
	iuTestInfos::const_iterator	end(void)	const		{ return m_testinfos.end(); }
	TestTypeId					get_typeid(void) const IUTEST_CXX_NOEXCEPT_SPEC	{ return m_id; }

private:
	static bool IsSuccessfulTest(const TestInfo* p) { return p->is_ran() && p->Passed(); }
	static bool IsFaildTest(const TestInfo* p) { return p->should_run() && p->HasFailure(); }
	static bool IsSkipTest(const TestInfo* p) { return !p->is_ran() || p->is_skipped(); }
	static bool IsReportableSkipTest(const TestInfo* p) { return p->is_reportable() && IsSkipTest(p); }
	static bool IsRunSkippedTest(const TestInfo* p) { return p->should_run() && p->is_skipped(); }
	static bool IsReportableRunSkippedTest(const TestInfo* p) { return p->is_reportable() && IsRunSkippedTest(p); }
	static bool IsReportableDisabledTest(const TestInfo* p) { return p->is_reportable() && p->is_disabled_test(); }

private:
	friend class UnitTestImpl;
	friend class UnitTest;

#if defined(IUTEST_NO_PRIVATE_IN_AGGREGATE)
	friend struct FindOp;
#endif

	::std::string	m_testcase_name;	//!< テストケース名
	iuTestInfos		m_testinfos;		//!< テスト情報リスト
	SetUpMethod		m_setup;			//!< SetUp 関数
	TearDownMethod	m_teardown;			//!< TearDown 関数
	TestTypeId		m_id;				//!< TypeID
	int				m_disable_num;		//!< 無視したテストの総数
	int				m_should_run_num;	//!< 実行すべきテストの総数
	TimeInMillisec	m_elapsedmsec;		//!< テスト時間
	bool			m_disable;			//!< 無効真偽値
	TestResult		m_ad_hoc_testresult;	//!< テストが実行中でないときのリザルト

	IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(TestCase);
};

/**
 * @brief	型つきテストケース
*/
template<typename T>
class TypedTestCase : public TestCase
{
public:
	typedef T	TypeParam;	//!< 型

protected:
	/**
	 * @brief	コンストラクタ
	 * @param [in]	testcase_name	= テストケース名
	 * @param [in]	id				= テストケースID
	 * @param [in]	setup			= テスト事前実行関数
	 * @param [in]	teardown		= テスト事後実行関数
	*/
	TypedTestCase(const char* testcase_name, TestTypeId id, SetUpMethod setup, TearDownMethod teardown)
		: TestCase(testcase_name, id, setup, teardown)
		, m_type_param(detail::GetTypeName<TypeParam>())
	{}

public:
	/** type param 文字列の取得 */
	virtual const char* type_param(void) const IUTEST_CXX_OVERRIDE { return m_type_param.empty() ? NULL : m_type_param.c_str(); }

private:
	friend class UnitTestImpl;

	::std::string m_type_param;		//!< type param string
};

namespace detail
{

/**
 * @brief	テストケース仲介者
*/
class TestCaseMediator IUTEST_CXX_FINAL : public detail::iuITestCaseMediator
{
public:
	TestCaseMediator(TestCase* p) IUTEST_CXX_NOEXCEPT_SPEC : iuITestCaseMediator(p) {}
public:
	virtual const char* test_case_name(void) const IUTEST_CXX_OVERRIDE { return m_test_case->name(); }
	virtual const char* type_param(void)	 const IUTEST_CXX_OVERRIDE { return m_test_case->type_param(); }
};

}	// end of namespace detail
}	// end of namespace iutest

#if !IUTEST_HAS_LIB
#  include "impl/iutest_case.ipp"
#endif

#endif // INCG_IRIS_IUTEST_CASE_HPP_F57C9B7E_7CAA_4429_BE75_FCAAEED1B220_
