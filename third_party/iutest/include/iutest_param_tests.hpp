//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_param_tests.hpp
 * @brief		iris unit test parameter 定義 ファイル
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
#ifndef INCG_IRIS_IUTEST_PARAM_TESTS_HPP_3DBBF55E_A076_4207_B49E_34734EE333EE_
#define INCG_IRIS_IUTEST_PARAM_TESTS_HPP_3DBBF55E_A076_4207_B49E_34734EE333EE_

//======================================================================
// include
#include "iutest_core.hpp"
#include "iutest_any.hpp"

#if IUTEST_HAS_PARAM_TEST
#include "internal/iutest_pool.hpp"

//======================================================================
// define
/**
 * @defgroup	VALUE_PARAMETERIZED_TEST	値のパラメータ化テスト
 * @brief		値のパラメータ化テスト
*/

/**
 * @ingroup	VALUE_PARAMETERIZED_TEST
 * @def		IUTEST_INSTANTIATE_TEST_CASE_P
 * @brief	パラメータテストインスタンス化マクロ
 * @param	prefix_			= インスタンス名
 * @param	testfixture_	= テストフィクスチャ
 * @param	generator_		= Range, Bool, Values, ValuesIn, Combine, Pairwise ...
*/
#define IUTEST_INSTANTIATE_TEST_CASE_P(prefix_, testfixture_, generator_)	\
	IIUT_INSTANTIATE_TEST_CASE_P_(prefix_, testfixture_, generator_)

/**
 * @ingroup	VALUE_PARAMETERIZED_TEST
 * @def		IUTEST_P(testfixture_, testname_)
 * @brief	パラメータユーザー指定テスト関数定義マクロ
 * @param	testfixture_	= テストフィクスチャ
 * @param	testname_		= テスト名
 * @note	
 *	class TestCaseName : public ::iutest::TestWithParam<int> {};\n
 *  IUTEST_P(TestCaseName, TestName) {}\n
 *  IUTEST_INSTANTIATE_TEST_CASE_P(InstantiateName, TestCaseName, ParamGenerator);\n
*/
#define IUTEST_P(testfixture_, testname_)		IIUT_TEST_P_(testfixture_, testname_)


#if IUTEST_HAS_ANY_PARAM_TEST

/**
 * @ingroup	VALUE_PARAMETERIZED_TEST
 * @def		IUTEST_INSTANTIATE_TEST_CASE_AP
 * @brief	パラメータテストインスタンス化マクロ
 * @param	prefix_		= インスタンス名
 * @param	testcase_	= テストケース名
 * @param	generator_	= Range, Bool, Values, ValuesIn, Combine, Pairwise ...
*/
#define IUTEST_INSTANTIATE_TEST_CASE_AP(prefix_, testcase_, generator_)	\
	IIUT_INSTANTIATE_TEST_CASE_P_(prefix_, IUTEST_ALIAS_TESTNAME_F(iuTestWithAny, testcase_), generator_)

/**
 * @ingroup	VALUE_PARAMETERIZED_TEST
 * @def		IUTEST_AP(testcase_, testname_)
 * @brief	パラメータユーザー指定テスト関数定義マクロ
 * @param	testcase_	= テストケース名
 * @param	testname_	= テスト名
 * @note	
 *  IUTEST_AP(TestCaseName, TestName) {}\n
 *  IUTEST_INSTANTIATE_TEST_CASE_AP(InstantiateName, TestCaseName, ParamGenerator);\n
*/
#define IUTEST_AP(testcase_, testname_)			IIUT_TEST_P_(IUTEST_ALIAS_TESTNAME_F(iuTestWithAny, testcase_), testname_)


#endif


/**
 * @private
 * @{
*/

#if !defined(IUTEST_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS)
#  define IIUT_GETTESTCASEPATTERNHOLDER(T, testcase_, package_)	\
	::iutest::UnitTest::GetInstance()->parameterized_test_registry().GetTestCasePatternHolder<T>(testcase_, package_)
#else
#  define IIUT_GETTESTCASEPATTERNHOLDER(T, testcase_, package_)	\
	::iutest::UnitTest::GetInstance()->parameterized_test_registry().GetTestCasePatternHolder(testcase_, package_, &::iutest::detail::type<T>())
#endif

#if IUTEST_HAS_IF_EXISTS
#define IIUT_TEST_P_FIXTURE_DECL_(testcase_)	IIUT_TEST_P_FIXTURE_DECL_I(IIUT_TO_VARNAME_(testcase_))
#define IIUT_TEST_P_FIXTURE_DECL_I(testcase_)	IUTEST_IF_NOT_EXISTS(testcase_, typedef ::iutest::TestWithAny testcase_;)
#else
#define IIUT_TEST_P_FIXTURE_DECL_(testcase_)
#endif


/**
 * @brief	パラメータテスト登録
*/
#define IIUT_INSTANTIATE_TEST_CASE_P_(prefix_, testcase_, generator_)						\
	IIUT_TEST_P_FIXTURE_DECL_(testcase_)													\
	static ::iutest::detail::iuIParamGenerator< IIUT_TO_VARNAME_(testcase_)::ParamType >*	\
		IUTEST_TEST_P_EVALGENERATOR_NAME_(prefix_, testcase_)(void) { return generator_; }	\
		int IUTEST_TEST_P_INSTANTIATIONREGISTER_NAME_(prefix_, testcase_)(void) {			\
			::iutest::detail::ParamTestCaseInfo< IIUT_TO_VARNAME_(testcase_) >* p =			\
				IIUT_GETTESTCASEPATTERNHOLDER( IIUT_TO_VARNAME_(testcase_)					\
					, IIUT_TO_NAME_STR_(testcase_), IUTEST_GET_PACKAGENAME_());				\
			return p->AddTestCaseInstantiation(#prefix_, IUTEST_TEST_P_EVALGENERATOR_NAME_(prefix_, testcase_));	\
		} IUTEST_TEST_P_INSTANTIATIONREGISTER_(prefix_, testcase_)

/**
 * @brief	パラメータテストクラス定義
*/
#define IIUT_TEST_P_(testcase_, testname_)															\
	IIUT_TEST_P_FIXTURE_DECL_(testcase_)															\
	class IUTEST_TEST_CLASS_NAME_(testcase_, testname_) : public IIUT_TO_VARNAME_(testcase_) {		\
		public: IUTEST_TEST_CLASS_NAME_(testcase_, testname_)(void) {}								\
		protected: virtual void Body(void);															\
		private: static int AddRegister(void) {														\
			static ::iutest::detail::ParamTestInstance< IUTEST_TEST_CLASS_NAME_(testcase_			\
				, testname_) > testinfo(IIUT_TO_NAME_STR_(testname_));								\
			IIUT_GETTESTCASEPATTERNHOLDER(IIUT_TO_VARNAME_(testcase_), IIUT_TO_NAME_STR_(testcase_)	\
					, IUTEST_GET_PACKAGENAME_())->AddTestPattern(&testinfo); return 0;				\
		}																							\
		static int dummy_;																			\
		IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(IUTEST_TEST_CLASS_NAME_(testcase_, testname_));			\
	};																								\
	int IUTEST_TEST_CLASS_NAME_(testcase_, testname_)::dummy_ IUTEST_ATTRIBUTE_UNUSED_				\
		= IUTEST_TEST_CLASS_NAME_(testcase_, testname_)::AddRegister();								\
	void IUTEST_TEST_CLASS_NAME_(testcase_, testname_)::Body(void)

#define IIUT_TEST_P_IGNORE_(testcase_, testname_)													\
	class IUTEST_TEST_CLASS_NAME_(testcase_, testname_) : public IIUT_TO_VARNAME_(testcase_) {		\
		public: IUTEST_TEST_CLASS_NAME_(testcase_, testname_)(void) {}								\
		protected: virtual void Body(void) { IUTEST_SKIP() << "ignored test..."; }					\
		template<typename T>void Body(void);														\
		private: static int AddRegister(void) {														\
			static ::iutest::detail::ParamTestInstance< IUTEST_TEST_CLASS_NAME_(testcase_			\
				, testname_) > testinfo(IIUT_TO_NAME_STR_(testname_));								\
			IIUT_GETTESTCASEPATTERNHOLDER(IIUT_TO_VARNAME_(testcase_), IIUT_TO_NAME_STR_(testcase_)	\
				, IUTEST_GET_PACKAGENAME_())->AddTestPattern(&testinfo); return 0;					\
		}																							\
		static int dummy_;																			\
		IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(IUTEST_TEST_CLASS_NAME_(testcase_, testname_));			\
	};																								\
	int IUTEST_TEST_CLASS_NAME_(testcase_, testname_)::dummy_ IUTEST_ATTRIBUTE_UNUSED_				\
		= IUTEST_TEST_CLASS_NAME_(testcase_, testname_)::AddRegister();								\
	template<typename T>void IUTEST_TEST_CLASS_NAME_(testcase_, testname_)::Body(void)

#define IUTEST_TEST_P_EVALGENERATOR_NAME_(prefix_, testcase_)	IIUT_TEST_P_EVALGENERATOR_NAME_I(prefix_, IIUT_TO_VARNAME_(testcase_))
#define IIUT_TEST_P_EVALGENERATOR_NAME_I(prefix_, testcase_)	IIUT_TEST_P_EVALGENERATOR_NAME_I_(prefix_, testcase_)
#define IIUT_TEST_P_EVALGENERATOR_NAME_I_(prefix_, testcase_)	s_##prefix_##_##testcase_##_EvalGenerator_

#define IUTEST_TEST_P_INSTANTIATIONREGISTER_NAME_(prefix_, testcase_)	IIUT_TEST_P_INSTANTIATIONREGISTER_NAME_I(prefix_, IIUT_TO_VARNAME_(testcase_))
#define IIUT_TEST_P_INSTANTIATIONREGISTER_NAME_I(prefix_, testcase_)	IIUT_TEST_P_INSTANTIATIONREGISTER_NAME_I_(prefix_, testcase_)
#define IIUT_TEST_P_INSTANTIATIONREGISTER_NAME_I_(prefix_, testcase_)	prefix_##_##testcase_##_TestCaseInstantiationRegister

#define IUTEST_TEST_P_INSTANTIATIONREGISTER_(prefix_, testcase_)			IIUT_TEST_P_INSTANTIATIONREGISTER_I(prefix_, IIUT_TO_VARNAME_(testcase_), IUTEST_TEST_P_INSTANTIATIONREGISTER_NAME_(prefix_, testcase_))
#define IIUT_TEST_P_INSTANTIATIONREGISTER_I(prefix_, testcase_, register_)	IIUT_TEST_P_INSTANTIATIONREGISTER_I_(prefix_, testcase_, register_)
#define IIUT_TEST_P_INSTANTIATIONREGISTER_I_(prefix_, testcase_, register_)	int s_##prefix_##_##testcase_##_dummy = register_()

/**
 * @}
*/

namespace iutest {
namespace detail
{

//======================================================================
// class
/**
 * @private
 * @brief	パラメータ単体テスト TestInfo 情報インスタンス
*/
template<typename T>
class ParamTestInstance : public IParamTestInfoData<typename T::ParamType>
{
	typedef T								Tester;
	typedef typename Tester::ParamType		ParamType;
	typedef detail::iuParamTestFactory<T>	Factory;
	typedef IParamTestInfoData<ParamType>	_Mybase;

	// 各テストのインスタンス
	class EachTest
	{
	public:
		EachTest(TestCase* testcase, const char* name, ParamType param)
			: m_mediator(testcase)
			, m_factory(param)
			, m_info(&m_mediator, name, &m_factory)
		{
			UnitTest::instance().AddTestInfo(testcase, &m_info);
			m_info.set_value_param(PrintToString(param).c_str());
		}
	private:
		TestCaseMediator	m_mediator;
		Factory				m_factory;
		TestInfo			m_info;
	};
public:
	ParamTestInstance(const char* testcase_name) : _Mybase(testcase_name) {}

private:
	// テストケースの作成
	virtual TestCase* MakeTestCase(const char* testcase_name, TestTypeId id, SetUpMethod setup, TearDownMethod teardown) const IUTEST_CXX_OVERRIDE
	{
#if !defined(IUTEST_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS)
		return UnitTest::instance().AddTestCase<TestCase>(testcase_name, id, setup, teardown);
#else
		return UnitTest::instance().AddTestCase(testcase_name, id, setup, teardown, &detail::type<TestCase>());
#endif
	}

	// テストの作成登録
	virtual void RegisterTest(TestCase* testcase, ParamType param, int index) const IUTEST_CXX_OVERRIDE
	{
		EachTest* test = new EachTest(testcase, detail::MakeIndexTestName(this->m_name.c_str(), index).c_str(), param);
		// new オブジェクトを管理してもらう
		detail::iuPool<EachTest>::GetInstance().push(test);
	}
};

}	// end of namespace detail

/**
 * @ingroup	PARAMETERIZED_TEST
 * @brief	any パラメータテストフィクスチャ
*/
class TestWithAny : public Test, public WithParamInterface<any>
{
public:
	/**
	 * @brief	パラメータの取得
	*/
	static const ParamType& GetParam(void) { return WithParamInterface<any>::GetParam(); }

	/**
	 * @brief	パラメータの取得
	*/
	template<typename T>
	static T GetParam(void) { return unsafe_any_cast<T>(WithParamInterface<any>::GetParam()); }
};

//======================================================================
// function
/**
 * @ingroup		VALUE_PARAMETERIZED_TEST
 * @defgroup	PARAM_GENERATOR	Parameter Generator
 * @brief		パラメータ生成器
 * @detail		IUTEST_INSTANTIATE_TEST_CASE_P に使用可能なパラメータ生成器
 * @{
*/

/**
 * @brief	範囲パラメータ
*/
template<typename T>
detail::iuParamGenerator<T> IUTEST_ATTRIBUTE_UNUSED_ Range(T begin, T end, T step=1)
{
	return new detail::iuRangeParamsGenerator<T>(begin, end, step);
}

/**
 * @brief	真偽値パラメータ
*/
inline detail::iuParamGenerator<bool> IUTEST_ATTRIBUTE_UNUSED_ Bool(void)
{
	return new detail::iuBoolParamsGenerator<bool>();
}

#if IUTEST_HAS_VALUESGEN

/**
 * @brief	ユーザー定義ジェネレータ利用パラメータ
*/
template<typename Generator>
inline detail::iuValuesParamsGeneratorHolder<Generator> IUTEST_ATTRIBUTE_UNUSED_ ValuesGen(size_t num, const Generator& gen)
{
	return detail::iuValuesParamsGeneratorHolder<Generator>(num, gen);
}

/**
 * @brief	乱数ジェネレータ
*/
template<typename T, typename F>
inline detail::iuRandomFilterParamGenerator<T, F> IUTEST_ATTRIBUTE_UNUSED_ RandomGenerator(const F& fn, unsigned int seed=0)
{
	if( seed == 0 )
	{
		seed = detail::GetIndefiniteValue();
	}
	return detail::iuRandomFilterParamGenerator<T, F>(fn, seed);
}

#endif

#if IUTEST_HAS_RANDOMVALUES
/**
 * @brief	乱数値パラメータ
*/
inline detail::iuRandomParamsHolder IUTEST_ATTRIBUTE_UNUSED_ RandomValues(size_t num, unsigned int seed=0)
{
	return detail::iuRandomParamsHolder(num, seed);
}
#endif

/**
 * @brief	値配列パラメータ
*/
template<typename Container>
inline detail::iuParamGenerator< typename Container::value_type > IUTEST_ATTRIBUTE_UNUSED_ ValuesIn(Container containor)
{
	return new detail::iuValuesInParamsGenerator< typename Container::value_type >(containor);
}

#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
/** @overload */
template<typename T, size_t SIZE>
inline detail::iuParamGenerator<T> IUTEST_ATTRIBUTE_UNUSED_ ValuesIn(const T (&v)[SIZE])
{
	return new detail::iuValuesInParamsGenerator<T>(v, v+SIZE);
}
/** @overload */
template<typename Ite>
inline detail::iuParamGenerator< typename detail::IteratorTraits<Ite>::type > IUTEST_ATTRIBUTE_UNUSED_ ValuesIn(Ite begin, Ite end)
{
	return new detail::iuValuesInParamsGenerator< typename detail::IteratorTraits<Ite>::type >(begin, end);
}
#if IUTEST_HAS_INITIALIZER_LIST
/** @overload */
template<typename T>
inline detail::iuParamGenerator<T> IUTEST_ATTRIBUTE_UNUSED_ ValuesIn(::std::initializer_list<T> l)
{
	return new detail::iuValuesInParamsGenerator<T>(l);
}
#endif

#endif

#if IUTEST_HAS_VARIADIC_VALUES
/**
 * @brief	値配列パラメータ
*/
template<typename ...Args>
inline detail::iuValueArray<Args...> IUTEST_ATTRIBUTE_UNUSED_ Values(Args... args)
{
	return detail::iuValueArray<Args...>(args...);
}

#else

#define IIUT_DECL_VALUES_(n)	template< IUTEST_PP_ENUM_PARAMS(n, typename T) >					\
	inline detail::iuValueArray##n< IUTEST_PP_ENUM_PARAMS(n, T) >									\
	IUTEST_ATTRIBUTE_UNUSED_ Values( IUTEST_PP_ENUM_BINARY_PARAMS(n, T, t) )	{					\
	return detail::iuValueArray##n< IUTEST_PP_ENUM_PARAMS(n, T) >( IUTEST_PP_ENUM_PARAMS(n, t) );	\
	}

/**
 * @brief	値配列パラメータ
 * @note	50引数まで使用可能
*/
IIUT_DECL_VALUES_(1)
IIUT_DECL_VALUES_(2)
IIUT_DECL_VALUES_(3)
IIUT_DECL_VALUES_(4)
IIUT_DECL_VALUES_(5)
IIUT_DECL_VALUES_(6)
IIUT_DECL_VALUES_(7)
IIUT_DECL_VALUES_(8)
IIUT_DECL_VALUES_(9)
IIUT_DECL_VALUES_(10)
IIUT_DECL_VALUES_(11)
IIUT_DECL_VALUES_(12)
IIUT_DECL_VALUES_(13)
IIUT_DECL_VALUES_(14)
IIUT_DECL_VALUES_(15)
IIUT_DECL_VALUES_(16)
IIUT_DECL_VALUES_(17)
IIUT_DECL_VALUES_(18)
IIUT_DECL_VALUES_(19)
IIUT_DECL_VALUES_(20)
IIUT_DECL_VALUES_(21)
IIUT_DECL_VALUES_(22)
IIUT_DECL_VALUES_(23)
IIUT_DECL_VALUES_(24)
IIUT_DECL_VALUES_(25)
IIUT_DECL_VALUES_(26)
IIUT_DECL_VALUES_(27)
IIUT_DECL_VALUES_(28)
IIUT_DECL_VALUES_(29)
IIUT_DECL_VALUES_(30)
IIUT_DECL_VALUES_(31)
IIUT_DECL_VALUES_(32)
IIUT_DECL_VALUES_(33)
IIUT_DECL_VALUES_(34)
IIUT_DECL_VALUES_(35)
IIUT_DECL_VALUES_(36)
IIUT_DECL_VALUES_(37)
IIUT_DECL_VALUES_(38)
IIUT_DECL_VALUES_(39)
IIUT_DECL_VALUES_(40)
IIUT_DECL_VALUES_(41)
IIUT_DECL_VALUES_(42)
IIUT_DECL_VALUES_(43)
IIUT_DECL_VALUES_(44)
IIUT_DECL_VALUES_(45)
IIUT_DECL_VALUES_(46)
IIUT_DECL_VALUES_(47)
IIUT_DECL_VALUES_(48)
IIUT_DECL_VALUES_(49)
IIUT_DECL_VALUES_(50)

#undef IIUT_DECL_VALUES_

#endif

#if IUTEST_HAS_CONCAT
/**
 * @brief	パラメータの結合
*/
template<typename G1, typename G2>
detail::iuConcatParamHolder<G1, G2> Concat(const G1& g1, const G2& g2)
{
	return detail::iuConcatParamHolder<G1, G2>(g1, g2);
}
#endif

#if IUTEST_HAS_COMBINE

#if IUTEST_HAS_VARIADIC_COMBINE
/**
 * @brief	複合条件パラメータ化
*/
template<typename ...Generator>
detail::iuCartesianProductHolder<Generator...> Combine(const Generator&... generators)
{
	return detail::iuCartesianProductHolder<Generator...>(generators...);
}

#else

#define IIUT_DECL_COMBINE_(n)	template< IUTEST_PP_ENUM_PARAMS(n, typename T) >				\
	inline IUTEST_PP_CAT(detail::iuCartesianProductHolder, n)< IUTEST_PP_ENUM_PARAMS(n, T) >	\
	Combine(IUTEST_PP_ENUM_BINARY_PARAMS(n, const T, &t)) {										\
		return IUTEST_PP_CAT(detail::iuCartesianProductHolder, n)								\
				< IUTEST_PP_ENUM_PARAMS(n, T) >(IUTEST_PP_ENUM_PARAMS(n, t));					\
	}

/**
 * @brief	複合条件パラメータ化
 * @note	9引数まで使用可能
*/
IIUT_DECL_COMBINE_(2)
IIUT_DECL_COMBINE_(3)
IIUT_DECL_COMBINE_(4)
IIUT_DECL_COMBINE_(5)
IIUT_DECL_COMBINE_(6)
IIUT_DECL_COMBINE_(7)
IIUT_DECL_COMBINE_(8)
IIUT_DECL_COMBINE_(9)

#undef IIUT_DECL_COMBINE_

#endif

#endif

#if IUTEST_HAS_PAIRWISE

#if IUTEST_HAS_VARIADIC_PAIRWISE

/**
 * @brief	複合条件パラメータ化(オールペア法)
*/
template<typename ...Generator>
detail::iuPairwiseHolder<Generator...> Pairwise(const Generator&... generators)
{
	return detail::iuPairwiseHolder<Generator...>(generators...);
}

#else

#define IIUT_DECL_PAIRWISE_(n)	template< IUTEST_PP_ENUM_PARAMS(n, typename T) >			\
	inline IUTEST_PP_CAT(detail::iuPairwiseHolder, n)< IUTEST_PP_ENUM_PARAMS(n, T) >		\
	Pairwise(IUTEST_PP_ENUM_BINARY_PARAMS(n, const T, &t)) {								\
		return IUTEST_PP_CAT(detail::iuPairwiseHolder, n)									\
				< IUTEST_PP_ENUM_PARAMS(n, T) >(IUTEST_PP_ENUM_PARAMS(n, t));				\
	}

/**
 * @brief	複合条件パラメータ化(オールペア法)
 * @note	9引数まで使用可能
*/
IIUT_DECL_PAIRWISE_(2)
IIUT_DECL_PAIRWISE_(3)
IIUT_DECL_PAIRWISE_(4)
IIUT_DECL_PAIRWISE_(5)
IIUT_DECL_PAIRWISE_(6)
IIUT_DECL_PAIRWISE_(7)
IIUT_DECL_PAIRWISE_(8)
IIUT_DECL_PAIRWISE_(9)

#undef IIUT_DECL_PAIRWISE__

#endif

#endif

/**
 * @}
*/

}	// end of namespace iutest

#if IUTEST_HAS_ANY_PARAM_TEST
/**
 * @ingroup	VALUE_PARAMETERIZED_TEST
 * @brief	any param test fixture
*/
typedef ::iutest::TestWithAny iuTestWithAny;
#endif

#endif // IUTEST_HAS_PARAM_TEST

#endif // INCG_IRIS_IUTEST_PARAM_TESTS_HPP_3DBBF55E_A076_4207_B49E_34734EE333EE_
