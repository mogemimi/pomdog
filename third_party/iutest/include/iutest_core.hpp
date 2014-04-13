//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_core.hpp
 * @brief		iris unit test core
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
#ifndef INCG_IRIS_IUTEST_CORE_HPP_33560431_783B_4AE7_8FC8_6E78355CAF87_
#define INCG_IRIS_IUTEST_CORE_HPP_33560431_783B_4AE7_8FC8_6E78355CAF87_

//======================================================================
// include
#include "internal/iutest_result_reporter.hpp"
#include "internal/iutest_params_util.hpp"
#include "internal/iutest_file.hpp"

namespace iutest
{

//======================================================================
// class
/**
 * @brief	テスト全体の管理者
*/
class UnitTest : public UnitTestImpl
{
public:
	/** @private */
	static UnitTest& instance(void)		{ static UnitTest inst; return inst; }
	/**
	 * @brief	UnitTest インスタンスの取得
	*/
	static UnitTest* GetInstance(void)	{ return &instance(); }

public:
	/**
	 * @brief	テスト中の TestCase の取得
	*/
	const TestCase*		current_test_case(void)	const { return m_current_testcase; }

	/**
	 * @brief	テスト中の TestInfo の取得
	 * @note	互換性のため メンバ関数 にしています。
	*/
	const TestInfo*		current_test_info(void)	const { return Test::GetCurrentTestInfo(); }

	/** 乱数シードの取得 */
	unsigned int		random_seed(void)		const { return TestEnv::current_random_seed(); }

	/** 現在何回目のくり返しか取得 */
	int					repeat_counter(void)	const IUTEST_CXX_NOEXCEPT_SPEC { return m_repeat_counter; }

public:
	/** テスト総数 */
	int				total_test_count(void)		const IUTEST_CXX_NOEXCEPT_SPEC { return m_total_test_num; }
	/** レポート対象のテスト総数 */
	int				reportable_test_count(void)	const;
	/** 実行した/するテスト総数 */
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
	/** 明示的にスキップされたテスト総数 (SKIP, ASSUME) */
	int				test_run_skipped_count(void) const;
	/** レポート対象の明示的にスキップされたテスト総数 (SKIP, ASSUME) */
	int				reportable_test_run_skipped_count(void) const;

	/** テストケース数の総数 */
	int				total_test_case_count(void)	const { return m_testcases.size(); }
	/** 実行したテストケース総数 */
	int				test_case_to_run_count(void) const;
	/** 成功したテストケース総数 */
	int				successful_test_case_count(void) const;
	/** 失敗したテストケース総数 */
	int				failed_test_case_count(void) const;

	/** テスト実行中じゃないときのリザルトの取得 */
	const TestResult* ad_hoc_testresult(void) const IUTEST_CXX_NOEXCEPT_SPEC { return &m_ad_hoc_testresult; }

	/** テストの実行ミリ秒 */
	TimeInMillisec	elapsed_time(void)		const IUTEST_CXX_NOEXCEPT_SPEC { return m_elapsedmsec; }

	/** テスト開始時のタイムスタンプを取得 */
	TimeInMillisec	start_timestamp(void)	const IUTEST_CXX_NOEXCEPT_SPEC { return m_start_timestamp; }

	/** テストケースの取得 */
	const TestCase*	GetTestCase(int index)	const { return m_testcases[index]; }

	/** テストが成功したかどうか */
	bool			Passed(void)			const;
	/** テストが失敗したかどうか */
	bool			Failed(void)			const { return !Passed(); }

	/** イベントリスナーの取得 */
	TestEventListeners&	listeners(void)		const { return TestEnv::event_listeners(); }

protected:
	/**
	 * @brief	テストの実行
	*/
	int Run(void);

private:
#if IUTEST_HAS_EXCEPTIONS && IUTEST_HAS_SEH
	int RunOnMSC(void);
#endif
	int RunImpl(void);

	/**
	 * @brief	テストの実行（１ループ）
	*/
	bool RunOnce(void);

private:
	/**
	 * @brief	プログラムの開始
	*/
	void TestProgramStart(void);

	/**
	 * @brief	イテレーション毎のセットアップ
	*/
	void SetUpTestIteration(void);

	/**
	 * @brief	環境セットアップ
	*/
	void EnvironmentSetUp(void);

	/**
	 * @brief	環境解除
	*/
	void EnvironmentTearDown(void);

	/**
	 * @brief	プログラムの終了
	*/
	void TestProgramEnd(void);

private:
	// C言語の終了コールバック関数
	static void OnExit(void)
	{
		instance().TestProgramEnd();
	}

private:
	UnitTest(void)
		: m_repeat_counter(0)
		, m_init_iutest_count(0)
		, m_test_started(false)
		, m_start_timestamp(0)
	{
		// デフォルトリポーターをセット
		TestEnv::SetGlobalTestPartResultReporter(&m_default_test_part_result_reporter);
		// 環境変数によるテストフラグ指定をロード
		TestEnv::LoadEnviromentVariable();
	}
#if defined(_MSC_VER) && _MSC_VER < 1300
public:	// VC++6.0 bug
#endif
	~UnitTest(void)
	{
		TestEnv::ReleaseGlobalTestEnvironment();
		TestEnv::SetGlobalTestPartResultReporter(NULL);
	}

private:
	// 初期化処理
	void Initialize(void);

#if IUTEST_HAS_PARAM_TEST
public:
	/** @private */
	detail::ParamTestCaseHolder& parameterized_test_registry(void) IUTEST_CXX_NOEXCEPT_SPEC{ return m_param_testcase_holder; }

private:
	detail::ParamTestCaseHolder	m_param_testcase_holder;
#endif

private:
	friend class UnitTestSource;

	int m_repeat_counter;
	int m_init_iutest_count;
	bool m_test_started;
	TimeInMillisec m_start_timestamp;
	detail::DefaultGlobalTestPartResultReporter m_default_test_part_result_reporter;

	IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(UnitTest);
};

namespace detail
{

/**
 * @private
 * @internal
 * @brief	単体テストインスタンス
*/
template<class Tester>
class TestInstance
{
public:
	/** コンストラクタ */
	TestInstance(const char* testcase, const char* name, TestTypeId id, SetUpMethod setup, TearDownMethod teardown)
		: m_mediator(AddTestCase(testcase, id, setup, teardown))
		, m_info(&m_mediator, name, &m_factory)
	{
		UnitTest::instance().AddTestInfo(m_mediator.ptr(), &m_info);
	}
	/** コンストラクタ */
	TestInstance(const char* testcase, const char* name, const char*  value_params, TestTypeId id, SetUpMethod setup, TearDownMethod teardown)
		: m_mediator(AddTestCase(testcase, id, setup, teardown))
		, m_info(&m_mediator, name, &m_factory)
	{
		m_info.set_value_param(value_params);
		UnitTest::instance().AddTestInfo(m_mediator.ptr(), &m_info);
	}

private:
	TestCase* AddTestCase(const char* testcase, TestTypeId id, SetUpMethod setup, TearDownMethod teardown)
	{
#if !defined(IUTEST_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS)
		return UnitTest::instance().AddTestCase<TestCase>(testcase, id, setup, teardown);
#else
		return UnitTest::instance().AddTestCase(testcase, id, setup, teardown, &detail::type<TestCase>());
#endif
	}
private:
	TestCaseMediator	m_mediator;
	TestInfo			m_info;
	iuFactory<Tester>	m_factory;
};

}	// end of namespace detail
}	// end of namespace iutest

#if !IUTEST_HAS_LIB
#  include "impl/iutest_core.ipp"
#endif

#endif // INCG_IRIS_IUTEST_CORE_HPP_33560431_783B_4AE7_8FC8_6E78355CAF87_
