//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_factory.hpp
 * @brief		iris unit test テストファクトリー ファイル
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
#ifndef INCG_IRIS_IUTEST_FACTORY_HPP_9C2B4B94_95A0_479A_9C9D_43FB31845A73_
#define INCG_IRIS_IUTEST_FACTORY_HPP_9C2B4B94_95A0_479A_9C9D_43FB31845A73_

//======================================================================
// include
#include "iutest_internal_defs.hpp"
#include "iutest_list.hpp"

namespace iutest {

//======================================================================
// declare
class Test;

namespace detail
{

//======================================================================
// class
/**
 * @internal
 * @brief	テスト生成クラスインターフェース
*/
class iuFactoryBase
{
	IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(iuFactoryBase);
public:
	iuFactoryBase(void) IUTEST_CXX_NOEXCEPT_SPEC {}
	virtual ~iuFactoryBase(void) {}
public:
	virtual auto_ptr<Test>	Create(void) = 0;
};

/**
 * @internal
 * @brief	テスト生成クラス
 * @tparam	Tester	= テストクラス
*/
template<class Tester>
class iuFactory : public iuFactoryBase
{
public:
	virtual auto_ptr<Test>	Create(void)
	{
		auto_ptr<Test> p = new Tester();
		return p;
	}
};

/**
 * @internal
 * @brief	パラメータセットテスト生成クラスインターフェース
 * @tparam	ParamType	= パラメータの型
*/
template<typename ParamType>
class iuParamTestFactoryBase : public iuFactoryBase
{
public:
	iuParamTestFactoryBase(void) {}
	iuParamTestFactoryBase(ParamType param) : m_param(param) {}
public:
	void				SetParam(ParamType param)	{ m_param = param; }
	const ParamType&	GetParam(void)		const	{ return m_param; }
protected:
	ParamType	m_param;
};


/**
 * @internal
 * @brief	パラメータセットテスト生成クラス
 * @tparam	Tester	= テストクラス
*/
template<class Tester>
class iuParamTestFactory : public iuParamTestFactoryBase<typename Tester::ParamType>
{
	typedef typename Tester::ParamType			ParamType;
	typedef iuParamTestFactoryBase<ParamType>	_Mybase;
public:
	iuParamTestFactory(void) {}
	iuParamTestFactory(ParamType param) : _Mybase(param) {}

public:
	virtual auto_ptr<Test>	Create(void)
	{
		Tester::SetParam(&this->m_param);
		auto_ptr<Test> p = new Tester();
		return p;
	}
};

}	// end of namespace detail
}	// end of namespace iutest

#endif
