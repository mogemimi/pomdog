//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_pool.hpp
 * @brief		iris unit test memory pool ファイル
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2011-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_POOL_HPP_60F7CC27_EA15_4AC7_BCA6_80FFDCBC890E_
#define INCG_IRIS_IUTEST_POOL_HPP_60F7CC27_EA15_4AC7_BCA6_80FFDCBC890E_

//======================================================================
// include
#include <vector>

#if IUTEST_HAS_LIB && IUTEST_HAS_EXTERN_TEMPLATE

namespace iutest { namespace detail { class iuIObject; } }

IUTEST_PRAGMA_EXTERN_TEMPLATE_WARN_DISABLE_BEGIN()

extern template class ::std::vector< ::iutest::detail::iuIObject* >;

IUTEST_PRAGMA_EXTERN_TEMPLATE_WARN_DISABLE_END()

#endif

namespace iutest {
namespace detail
{

/**
 * @brief	pool で管理するオブジェクトのベースクラス
*/
class iuIObject
{
public:
	virtual ~iuIObject() {}
};

/**
 * @brief	new したオブジェクトを管理
*/
class iuPool
{
	typedef ::std::vector<iuIObject*> pool;
	pool m_pool;
public:
	typedef iuIObject *value_ptr;

public:
	~iuPool(void)
	{
		// すべて解放する
		for( pool::iterator it=m_pool.begin(); it != m_pool.end(); )
		{
			value_ptr p = *it;
			it = m_pool.erase(it);
			delete p;
		}
	}
public:
	void push(value_ptr ptr) { m_pool.push_back(ptr); }
public:
	static iuPool& GetInstance(void) { static iuPool inst; return inst; }
};

}	// end of namespace detail
}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_POOL_HPP_60F7CC27_EA15_4AC7_BCA6_80FFDCBC890E_
