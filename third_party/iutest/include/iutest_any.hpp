//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_any.hpp
 * @brief		iris unit test any ファイル
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
 * @par			copyright
 * Copyright (C) 2013-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_ANY_HPP_8DB2417F_568A_4E01_95AD_21164565B975_
#define INCG_IRIS_IUTEST_ANY_HPP_8DB2417F_568A_4E01_95AD_21164565B975_

//======================================================================
// include
#include "internal/iutest_internal_defs.hpp"
#include "internal/iutest_exception.hpp"

//======================================================================
// class
namespace iutest
{

/**
 * @brief	any
*/
class any
{
	typedef internal::TypeId type_id;
public:
	any(void) : content(NULL) {}
	template<typename T>
	any(const T& rhs) : content(new holder<T>(rhs)) {}
	any(const any& rhs) : content(rhs.content == NULL ? NULL : rhs.content->clone()) {}
	~any(void) { delete content; }
public:
	/**
	 * @brief	swap
	*/
	any& swap(any& rhs)
	{
		::std::swap(content, rhs.content);
		return *this;
	}
	/**
	 * @brief	空かどうか
	 * @retval	true = 空
	*/
	bool empty(void) const
	{
		return content == NULL;
	}
	/**
	 * @brief	要素のクリア
	*/
	void clear(void)
	{
		any().swap(*this);
	}
	/**
	 * @brief	型IDの取得
	 * @return	型ID
	*/
	type_id type(void) const
	{
		return content == NULL ? internal::GetTypeId<void>() : content->type();
	}
	/**
	 * @brief	型の比較
	 * @retval	true = 同一
	*/
	template<typename T>
	bool type_equal(void) const
	{
		return type() == internal::GetTypeId<T>();
	}

public:
	template<typename T>
	any& operator = (const T& rhs)
	{
		any(rhs).swap(*this);
		return *this;
	}
	any& operator = (const any& rhs)
	{
		any(rhs).swap(*this);
		return *this;
	}

	template<typename T>
	friend T* any_cast(any*);
	template<typename T>
	friend T* unsafe_any_cast(any*);

private:
	class placeholder
	{
	public:
		virtual ~placeholder() {};
		virtual type_id type(void) const = 0;
		virtual placeholder* clone(void) const = 0;
	};
	template<typename T>
	class holder : public placeholder
	{
	public:
		holder(const T& v) : held(v) {}
	public:
		virtual type_id type(void) const IUTEST_CXX_OVERRIDE
		{
			return internal::GetTypeId<T>();
		}
		virtual placeholder* clone(void) const IUTEST_CXX_OVERRIDE
		{
			return new holder<T>(held);
		}
	public:
		T held;
	private:
		holder& operator = (const holder&);
	};
private:
	placeholder* content;
};

#if IUTEST_HAS_EXCEPTIONS
/**
 * @brief	any_cast の失敗の例外
*/
class bad_any_cast : public ::std::bad_cast {};
#endif

inline void swap(any& lhs, any& rhs) { lhs.swap(rhs); }

/**
 * @brief	型を考慮したキャスト
*/
template<typename T>
T* any_cast(any* p)
{
	return p != NULL && p->type_equal<T>() ?
		&static_cast< any::holder<T>* >(p->content)->held : NULL;
}
/** @overload */
template<typename T>
inline const T* any_cast(const any* p)
{
	return any_cast<T>(const_cast<any*>(p));
}
/** @overload */
template<typename T>
inline T any_cast(any& value)
{
	typedef typename type_traits::remove_reference<T>::type nonref_t;
	nonref_t* p = any_cast<nonref_t>(&value);
#if IUTEST_HAS_EXCEPTIONS
	if( p == NULL ) {
		throw bad_any_cast();
	}
#endif

	return static_cast<nonref_t&>(*p);
}
/** @overload */
template<typename T>
inline T any_cast(const any& value)
{
	return any_cast<T>(const_cast<any&>(value));
}

/**
 * @brief	型を考慮せずキャスト
*/
template<typename T>
T* unsafe_any_cast(any* p)
{
	return p != NULL ?
		&static_cast< any::holder<T>* >(p->content)->held : NULL;
}
/** @overload */
template<typename T>
inline const T* unsafe_any_cast(const any* p)
{
	return unsafe_any_cast<T>(const_cast<any*>(p));
}
/** @overload */
template<typename T>
inline T unsafe_any_cast(any& value)
{
	typedef typename type_traits::remove_reference<T>::type nonref_t;
	nonref_t* p = unsafe_any_cast<nonref_t>(&value);
	return static_cast<nonref_t&>(*p);
}
/** @overload */
template<typename T>
inline T unsafe_any_cast(const any& value)
{
	return unsafe_any_cast<T>(const_cast<any&>(value));
}

}	// end of namespace iutest

#endif	// INCG_IRIS_IUTEST_ANY_HPP_8DB2417F_568A_4E01_95AD_21164565B975_
