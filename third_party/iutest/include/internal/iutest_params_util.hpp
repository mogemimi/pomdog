//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_params_util.hpp
 * @brief		iris unit test parameter 定義 ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2011-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_PARAMS_UTIL_HPP_19F0C0BB_EEAE_4E8B_B269_A09A4A45E890_
#define INCG_IRIS_IUTEST_PARAMS_UTIL_HPP_19F0C0BB_EEAE_4E8B_B269_A09A4A45E890_

//======================================================================
// include
#include "iutest_genparams.hpp"

#if IUTEST_HAS_PARAM_TEST
#include "iutest_pool.hpp"

namespace iutest {
namespace detail
{

//======================================================================
// class
/**
 * @brief	パラメータ単体テスト TestInfo データクラス
*/
class IParamTestInfoData
{
public:
	class EachTestBase : public iuIObject {};
	template<typename T>
	class ParamEachTestBase : public EachTestBase
	{
	public:
		virtual void SetParam(const T& param) = 0;
	};
public:
	IParamTestInfoData(const char* name) : m_name(name) {}
	virtual TestCase* MakeTestCase(const char* , TestTypeId , SetUpMethod , TearDownMethod ) const = 0;
	virtual EachTestBase* RegisterTest(TestCase* , int ) const = 0;
	const char* GetName(void) const { return m_name.c_str(); }
protected:
	::std::string m_name;
};

/**
 * @brief	パラメータ単体テストインスタンスインターフェイス
*/
class IParamTestCaseInfo
{
public:
	virtual ~IParamTestCaseInfo(void) {}
protected:
	IParamTestCaseInfo(const ::std::string& base_name, const ::std::string& package_name)
		: m_testcase_base_name(base_name), m_package_name(package_name) {}
	
public:
	/**
	 * @brief	テストパターンの登録
	*/
	void AddTestPattern(IParamTestInfoData* testinfo)
	{
		m_testinfos.push_back(testinfo);
	};

public:
	void RegisterTests(void) const
	{
		for( TestInfoContainer::const_iterator it=m_testinfos.begin(), end=m_testinfos.end(); it != end; ++it )
		{
			OnRegisterTests(*it);
		}
	}
	
	::std::string GetTestCaseBaseName(void)	const { return m_testcase_base_name; }
	::std::string GetPackageName(void)		const { return m_package_name; }

public:
	bool is_same(const ::std::string& base_name, const ::std::string& package_name)
	{
		return m_testcase_base_name == base_name && m_package_name == package_name;
	}
	
private:
	virtual void OnRegisterTests(IParamTestInfoData*) const = 0;
private:
	typedef ::std::vector<IParamTestInfoData*> TestInfoContainer;
	TestInfoContainer m_testinfos;
protected:
	::std::string m_testcase_base_name;
	::std::string m_package_name;
};

/**
 * @brief	パラメータ単体テストインスタンス
 * @tparam	T	= テストクラス
*/
template<class T>
class ParamTestCaseInfo : public IParamTestCaseInfo
{
	typedef T										Tester;
	typedef typename Tester::ParamType				ParamType;
	typedef detail::iuIParamGenerator<ParamType>	ParamGenerator;
	typedef typename ParamGenerator::Generator		Generator;
	typedef IParamTestInfoData::ParamEachTestBase<ParamType> EachTest;

	typedef ::std::vector<IParamTestInfoData*> TestInfoContainer;

	typedef ParamGenerator* (pfnCreateGeneratorFunc)();

public:
	/// コンストラクタ
	ParamTestCaseInfo(const ::std::string& testcase_name, const ::std::string& package_name)
		: IParamTestCaseInfo(testcase_name, package_name)
	{
	}
	virtual ~ParamTestCaseInfo(void) {}

	/**
	 * @brief	インスタンスの登録
	*/
	int	AddTestCaseInstantiation(::std::string name, pfnCreateGeneratorFunc* func)
	{
#if IUTEST_HAS_STD_EMPLACE
		m_instantiation.emplace_back(name, func);
#else
		m_instantiation.push_back(InstantiationPair(name, func));
#endif
		return 0;
	}

	/**
	 * @brief	テストの作成
	*/
	virtual void OnRegisterTests(IParamTestInfoData* infodata) const IUTEST_CXX_OVERRIDE
	{
		for( typename InstantiationContainer::const_iterator gen_it=m_instantiation.begin(), gen_end=m_instantiation.end(); gen_it != gen_end; ++gen_it )
		{
			// パラメータ生成器の作成
			detail::scoped_ptr<ParamGenerator> p((gen_it->second)());

			::std::string testcase_name = m_package_name;
			if( !gen_it->first.empty() )
			{
				testcase_name += gen_it->first;
				testcase_name += "/";
			}
			testcase_name += m_testcase_base_name;
			TestCase* testcase = infodata->MakeTestCase(testcase_name.c_str()
				, internal::GetTypeId<Tester>()
				, Tester::SetUpTestCase
				, Tester::TearDownTestCase);

			if( p.get() != NULL )
			{
				int i=0;
				for( p->Begin(); !p->IsEnd(); p->Next() )
				{
					EachTest* test = static_cast<EachTest*>(infodata->RegisterTest(testcase, i));
					test->SetParam(p->GetCurrent());
					++i;
				}
			}
		}
	}
private:
	typedef ::std::pair< ::std::string, pfnCreateGeneratorFunc* > InstantiationPair;
	typedef ::std::vector<InstantiationPair> InstantiationContainer;
	InstantiationContainer m_instantiation;
};

/**
 * @brief	パラメータテストパターン保管庫
*/
class ParamTestCaseHolder
{
private:
	~ParamTestCaseHolder(void)
	{
		// 解放
		for( TestCaseInfoContainer::const_iterator it=m_testcase_infos.begin(), end=m_testcase_infos.end(); it != end; ++it )
		{
			delete *it;
		}
	}
public:
	template<typename T>
	ParamTestCaseInfo<T>* GetTestCasePatternHolder(const ::std::string& testcase, const ::std::string& package
		IUTEST_APPEND_EXPLICIT_TEMPLATE_TYPE_(T)
		)
	{
		for( TestCaseInfoContainer::iterator it=m_testcase_infos.begin(), end=m_testcase_infos.end(); it != end; ++it )
		{
			if( (*it)->is_same(testcase, package) )
			{
				return static_cast<ParamTestCaseInfo<T>*>(*it);
			}
		}
		ParamTestCaseInfo<T>* p = new ParamTestCaseInfo<T>(testcase, package);
		m_testcase_infos.push_back(p);
		return p;
	}

public:
	size_t count(void) const { return m_testcase_infos.size(); }

private:
	struct RegisterTestsFunctor
	{
		inline void operator ()(const IParamTestCaseInfo* p) const
		{
			p->RegisterTests();
		}
	};

	// テストを登録
	void RegisterTests(void)
	{
		::std::for_each(m_testcase_infos.begin(), m_testcase_infos.end(), RegisterTestsFunctor());
	}
private:
	friend class ::iutest::UnitTest;
	typedef ::std::vector<IParamTestCaseInfo*> TestCaseInfoContainer;
	TestCaseInfoContainer m_testcase_infos;
};

}	// end of namespace detail
}	// end of namespace iutest

#endif

#endif // INCG_IRIS_IUTEST_PARAMS_UTIL_HPP_19F0C0BB_EEAE_4E8B_B269_A09A4A45E890_
