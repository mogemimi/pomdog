//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_params_util.hpp
 * @brief		iris unit test parameter 定義 ファイル
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
#ifndef INCG_IRIS_iutest_params_util_HPP_19F0C0BB_EEAE_4e8b_B269_A09A4A45E890_
#define INCG_IRIS_iutest_params_util_HPP_19F0C0BB_EEAE_4e8b_B269_A09A4A45E890_

//======================================================================
// include
#include "iutest_genparams.hpp"

#if IUTEST_HAS_PARAM_TEST

namespace iutest
{

//======================================================================
// class
/**
 * @brief	パラメータ単体テスト TestInfo データクラス
*/
template<typename ParamType>
class IParamTestInfoData
{
public:
	IParamTestInfoData(const char* name) : m_name(name) {}
	virtual TestCase*	MakeTestCase(const char* testcase_name, TestTypeId id, SetUpMethod setup, TearDownMethod teardown) const = 0;
	virtual void RegisterTest(TestCase* , ParamType param, int index) const = 0;
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
	virtual void	RegisterTests(void) const = 0;

	::std::string	GetTestCaseBaseName(void)	const	{ return m_testcase_base_name; }
	::std::string	GetPackageName(void)		const	{ return m_package_name; }

public:
	bool is_same(const ::std::string& base_name, const ::std::string& package_name)
	{
		return m_testcase_base_name == base_name && m_package_name == package_name;
	}
protected:
	::std::string	m_testcase_base_name;
	::std::string	m_package_name;
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
	typedef IParamTestInfoData<ParamType>			TestInfoData;

	typedef ParamGenerator* (pfnCreateGeneratorFunc)();

public:
	/// コンストラクタ
	ParamTestCaseInfo(const ::std::string& testcase_name, const ::std::string& package_name)
		: IParamTestCaseInfo(testcase_name, package_name)
	{
	}
	virtual ~ParamTestCaseInfo(void) {}

	/**
	 * @brief	テストパターンの登録
	*/
	void	AddTestPattern(TestInfoData* testinfo)
	{
		m_testinfos.push_back(testinfo);
	};
	/**
	 * @brief	インスタンスの登録
	*/
	int	AddTestCaseInstantiation(::std::string name, pfnCreateGeneratorFunc* func)
	{
		m_instantiation.push_back(InstantiationPair(name, func));
		return 0;
	}

	/**
	 * @brief	テストの作成
	*/
	virtual void	RegisterTests(void) const
	{
		for( typename TestInfoContainer::const_iterator it=m_testinfos.begin(), end=m_testinfos.end(); it != end; ++it )
		{
			for( typename InstantiationContainer::const_iterator gen_it=m_instantiation.begin(), gen_end=m_instantiation.end(); gen_it != gen_end; ++gen_it )
			{
				// パラメータ生成器の作成
				detail::auto_ptr<ParamGenerator> p = (gen_it->second)();

				::std::string testcase_name = m_package_name;
				if( !gen_it->first.empty() )
				{
					testcase_name += gen_it->first;
					testcase_name += "/";
				}
				testcase_name += m_testcase_base_name;
				TestCase* testcase = (*it)->MakeTestCase(testcase_name.c_str()
					, internal::GetTypeId<Tester>()
					, Tester::SetUpTestCase
					, Tester::TearDownTestCase);

				if( p.ptr() != NULL )
				{
					int i=0;
					for( p->Begin(); !p->IsEnd(); p->Next() )
					{
						// パラメータがなくなるまでテストを作る
						(*it)->RegisterTest(testcase, p->GetCurrent(), i);
						++i;
					}
				}
			}
		}
	}
private:
	typedef ::std::vector<TestInfoData*>	TestInfoContainer;
	typedef ::std::pair< ::std::string, pfnCreateGeneratorFunc* >	InstantiationPair;
	typedef ::std::vector<InstantiationPair>						InstantiationContainer;
	TestInfoContainer		m_testinfos;
	InstantiationContainer	m_instantiation;
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
	ParamTestCaseInfo<T>*	GetTestCasePatternHolder(const ::std::string& testcase, const ::std::string& package
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
	size_t	count(void)	const { return m_testcase_infos.size(); }

private:
	struct RegisterTestsFunctor
	{
		inline void operator ()(const IParamTestCaseInfo* p) const
		{
			p->RegisterTests();
		}
	};

	// テストを登録
	void	RegisterTests(void)
	{
		::std::for_each(m_testcase_infos.begin(), m_testcase_infos.end(), RegisterTestsFunctor());
	}
private:
	friend class UnitTest;
	typedef ::std::vector<IParamTestCaseInfo*>	TestCaseInfoContainer;
	TestCaseInfoContainer	m_testcase_infos;
};

}	// end of namespace iutest

#endif

#endif
