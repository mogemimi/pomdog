//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_result.hpp
 * @brief		iris unit test result
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2011-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_RESULT_HPP_D27B1599_F42F_4E2D_B3EB_FACE24C2B921_
#define INCG_IRIS_IUTEST_RESULT_HPP_D27B1599_F42F_4E2D_B3EB_FACE24C2B921_

//======================================================================
// include
#include "internal/iutest_message.hpp"

namespace iutest
{

//======================================================================
// declare
namespace detail
{
	class DefaultGlobalTestPartResultReporter;
}

//======================================================================
// class
/**
 * @brief	テスト結果の通知処理インターフェイス
*/
class TestPartResultReporterInterface
{
public:
	virtual ~TestPartResultReporterInterface(void) {}
	/**
	 * @brief	テスト結果通知受け取り関数
	 * @param [in] result	= テスト結果
	*/
	virtual void ReportTestPartResult(const TestPartResult& result) = 0;
};

/**
 * @brief	テスト結果を示すクラス
*/
class TestPartResult : public detail::iuCodeMessage
{
public:
	/**
	 * @brief	結果のタイプ
	*/
	enum Type
	{
		kAssumeFailure = -3,	//!< 前提条件エラー
		kSkip = -2,				//!< スキップ
		kWarning = -1,			//!< 警告
		kSuccess,				//!< 成功
		kNonFatalFailure,		//!< 致命的ではない失敗
		kFatalFailure			//!< 致命的な失敗
	};
public:
	/**
	 * @brief	コンストラクタ
	 * @param [in]	file	= ファイル名
	 * @param [in]	line	= 行番号
	 * @param [in]	message	= メッセージ
	 * @param [in]	type	= 結果のタイプ
	*/
	TestPartResult(const char* file, int line, const char* message, Type type)
		: detail::iuCodeMessage(file, line, message), m_type(type) {}
	//! コピーコンストラクタ
	TestPartResult(const TestPartResult& rhs) : detail::iuCodeMessage(rhs)
		, m_type(rhs.m_type) {}

public:
	/**
	* @brief	失敗かどうか
	*/
	static bool type_is_failed(Type type) IUTEST_CXX_NOEXCEPT_SPEC { return type > kSuccess; }

public:
	/**
	 * @brief	失敗かどうか
	*/
	bool		failed(void) const IUTEST_CXX_NOEXCEPT_SPEC { return m_type > kSuccess; }
	/**
	 * @brief	成功かどうか
	*/
	bool		passed(void) const IUTEST_CXX_NOEXCEPT_SPEC { return !failed(); }
	/**
	 * @brief	警告かどうか
	*/
	bool		warning(void) const IUTEST_CXX_NOEXCEPT_SPEC { return m_type == kWarning; }
	/**
	 * @brief	スキップかどうか
	*/
	bool		skipped(void) const IUTEST_CXX_NOEXCEPT_SPEC { return m_type == kSkip; }
	/**
	 * @brief	前提条件エラーかどうか
	*/
	bool		assume_failed(void) const IUTEST_CXX_NOEXCEPT_SPEC { return m_type == kAssumeFailure; }
	/**
	 * @brief	成功かどうか（警告を除く）
	*/
	bool		succeeded(void) const IUTEST_CXX_NOEXCEPT_SPEC { return m_type == kSuccess; }

	/**
	 * @brief	致命的ではない失敗かどうか
	*/
	bool		nonfatally_failed(void) const IUTEST_CXX_NOEXCEPT_SPEC { return m_type == kNonFatalFailure; }

	/**
	 * @brief	致命的な失敗かどうか
	*/
	bool		fatally_failed(void) const IUTEST_CXX_NOEXCEPT_SPEC { return m_type == kFatalFailure; }

	/**
	 * @brief	理由
	*/
	const char*	summary(void) const { return message(); }

	/**
	 * @brief	結果のタイプ取得
	*/
	Type		type(void) const IUTEST_CXX_NOEXCEPT_SPEC { return m_type; }

private:
	Type m_type;
};

//! TestPartResult print 出力
inline iu_ostream& operator << (iu_ostream& os, const TestPartResult& result)
{
	return os << result.make_message();
}

/**
 * @brief	テストプロパティ
 * @note	XML 属性
*/
class TestProperty
{
public:
	/**
	 * @brief	コンストラクタ
	 * @param [in]	key		= キー
	 * @param [in]	value	= 値
	*/
	TestProperty(const ::std::string& key, const ::std::string& value)
		: m_key(key), m_value(value) {}

public:
	/**
	 * @brief	値の設定
	*/
	void SetValue(const ::std::string& value) { m_value = value; }
	const char*	key(void)	const { return m_key.c_str(); }		//!< キーの取得
	const char*	value(void)	const { return m_value.c_str(); }	//!< 値の取得

public:
	/**
	 * @brief	有効なキーかどうかチェック
	 * @retval	true=有効
	 * @retval	false=無効
	*/
	bool Validate(const char** ban_list, size_t size) const
	{
		return ValidateName(m_key, ban_list, ban_list+size);
	}
	/**
	 * @brief	有効なキーかどうかチェック
	 * @retval	true=有効
	 * @retval	false=無効
	*/
	template<typename Ite>
	static bool ValidateName(const ::std::string& name, Ite begin, Ite end)
	{
		return ::std::find(begin, end, name) == end;
	}
#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
	/** @overload */
	template<typename T, size_t N>
	static bool ValidateName(const ::std::string& name, T (&ar)[N])
	{
		return ValidateName(name, ar, ar + N);
	}
#endif

private:
	friend class TestResult;
	::std::string m_key;
	::std::string m_value;
};

/**
 * @brief	テスト結果を示すクラス
*/
class TestResult
{
	typedef ::std::vector<TestPartResult>	TestPartResults;
	typedef ::std::vector<TestProperty>		TestPropertys;
public:
	TestResult(void)
		: m_elapsedmsec(0) {}

public:
	/**
	 * @brief	成功したかどうか
	 * @return	真偽値
	*/
	bool		Passed(void) const { return !Failed(); }
	/**
	 * @brief	失敗したかどうか
	 * @return	真偽値
	*/
	bool		Failed(void) const
	{
		for( TestPartResults::const_iterator it=m_test_part_results.begin(), end=m_test_part_results.end(); it != end; ++it )
		{
			if( it->failed() )
			{
				return true;
			}
		}
		return false;
	}
	/**
	 * @brief	スキップしたかどうか
	 * @return	真偽値
	*/
	bool		Skipped(void) const
	{
		for( TestPartResults::const_iterator it=m_test_part_results.begin(), end=m_test_part_results.end(); it != end; ++it )
		{
			if( it->skipped() || it->assume_failed() )
			{
				return Passed();
			}
		}
		return false;
	}

	/**
	 * @brief	致命的なエラーがあるかどうか
	 * @return	真偽値
	*/
	bool		HasFatalFailure(void) const { return HasResult(TestPartResult::kFatalFailure); }

	/**
	 * @brief	致命的でないエラーがあるかどうか
	 * @return	真偽値
	*/
	bool		HasNonfatalFailure(void) const { return HasResult(TestPartResult::kNonFatalFailure); }

	/**
	 * @brief	前提条件エラーがあるかどうか
	 * @return	真偽値
	*/
	bool		HasAssumeFailure(void) const { return HasResult(TestPartResult::kAssumeFailure); }

	/**
	 * @brief	テストの実行時間の取得
	 * @return	テストの実行時間
	*/
	TimeInMillisec	elapsed_time(void) const IUTEST_CXX_NOEXCEPT_SPEC { return m_elapsedmsec; }

	/**
	 * @brief	結果の数を取得
	 * @return	結果の数
	*/
	int			total_part_count(void) const { return static_cast<int>(m_test_part_results.size()); }

	/**
	 * @brief	プロパティ総数の取得
	 * @return	総数
	*/
	int			test_property_count(void) const { return static_cast<int>(m_test_propertys.size()); }

	/**
	 * @brief	テスト結果の取得
	 * @param [in]	index	= インデックス
	 * @return	テスト結果
	*/
	const TestPartResult&	GetTestPartResult(int index) const { return m_test_part_results[index]; }

	/**
	 * @brief	プロパティの取得
	 * @param [in]	index	= インデックス
	 * @return	プロパティの
	*/
	const TestProperty&		GetTestProperty(int index) const { return m_test_propertys[index]; }

public:
	/**
	 * @brief	失敗の数を取得
	 * @return	失敗の数
	*/
	int total_error_count(void) const
	{
		int count = 0;
		for( TestPartResults::const_iterator it=m_test_part_results.begin(), end=m_test_part_results.end(); it != end; ++it )
		{
			if( it->failed() )
			{
				++count;
			}
		}
		return count;
	}

private:
	void AddTestPartResult(const TestPartResult& result) { m_test_part_results.push_back(result); }
	void set_elapsed_time(TimeInMillisec time) IUTEST_CXX_NOEXCEPT_SPEC { m_elapsedmsec = time; }

private:
	void RecordProperty(const TestProperty& prop)
	{
		for( TestPropertys::iterator it=m_test_propertys.begin(), end=m_test_propertys.end(); it != end; ++it )
		{
			if( detail::IsStringEqual(it->key(), prop.key()) )
			{
				it->m_value = prop.m_value;
				return;
			}
		}
		m_test_propertys.push_back(prop);
	}

	void ClearResult(void)
	{
		m_test_part_results.clear();
	}
	void Clear(void)
	{
		m_test_part_results.clear();
		m_test_propertys.clear();
		m_elapsedmsec = 0;
	}
	bool HasResult(TestPartResult::Type eType) const
	{
		for( TestPartResults::const_iterator it=m_test_part_results.begin(), end=m_test_part_results.end(); it != end; ++it )
		{
			if( it->type() == eType ) return true;
		}
		return false;
	}
private:
	friend class UnitTestImpl;
	friend class TestInfo;
	friend class TestCase;
	friend class detail::DefaultGlobalTestPartResultReporter;

	TestPartResults	m_test_part_results;
	TestPropertys	m_test_propertys;
	TimeInMillisec	m_elapsedmsec;

	IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(TestResult);
};

}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_RESULT_HPP_D27B1599_F42F_4E2D_B3EB_FACE24C2B921_
