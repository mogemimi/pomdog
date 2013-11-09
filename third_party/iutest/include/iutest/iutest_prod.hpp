//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_prod.hpp
 * @brief		iris unit test production code 対応用 ファイル
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
 * @par			copyright
 * Copyright (C) 2012-2013, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_iutest_prod_HPP_7A316C18_042D_4e48_BC31_E6AE8B6C2E28_
#define INCG_IRIS_iutest_prod_HPP_7A316C18_042D_4e48_BC31_E6AE8B6C2E28_

//======================================================================
// define
/**
 * @brief	テストから見えるように fried 登録
*/
#define IUTEST_FRIEND_TEST(test_case_name, test_name)	\
	friend class IUTEST_TEST_CLASS_NAME_(test_case_name, test_name)

#if IUTEST_HAS_TYPED_TEST 

#if !defined(_MSC_VER) || _MSC_VER > 1200

/**
 * @brief	テストから見えるように fried 登録(IUTEST_TYPED_TEST用)
*/
#define IUTEST_FRIEND_TYPED_TEST(test_case_name, test_name)	\
	template<typename T>IUTEST_FRIEND_TEST(test_case_name, test_name)

#endif

#endif

namespace iutest {
namespace detail
{

//======================================================================
// struct
/** 
 * @brief	private メンバー保持構造体
*/
template<typename Tag>
struct peep_tag
{
	static typename Tag::type	value;
};
template<typename Tag>
typename Tag::type peep_tag<Tag>::value;

}	// end of namespace detail

//======================================================================
// define
/**
 * @brief	private メンバーへのアクセス権を作成
 * @param	member_type	= 型
 * @param	class_name	= クラス
 * @param	member_name	= メンバー名
*/
#define IUTEST_MAKE_PEEP(member_type, class_name, member_name)		\
	IUTEST_MAKE_PEEP_TAG_(member_type, class_name, member_name);	\
	template struct IUTEST_PEEP_SETTER_NAME_(class_name, member_name)<class_name,	\
		IUTEST_PEEP_TAG_NAME_(class_name, member_name)<class_name>, &class_name::member_name>

/**
 * @brief	private メンバーへのアクセス権を作成
 * @param	member_type	= 型
 * @param	scope_name	= スコープ
 * @param	class_name	= クラス名
 * @param	member_name	= メンバー名
 * @deprecated	このマクロは非推奨です。 IUTEST_MAKE_PEEP を使用してください。
 * @note	過去互換のために scope_name::class_name を using して class_name だけで参照できるようにしています。
 *			scope_name::class_name 型を using するので使用する際には注意してください。
*/
#define IUTEST_MAKE_SCOPED_PEEP(member_type, scope_name, class_name, member_name)				\
	using scope_name::class_name;																\
	IUTEST_MAKE_PEEP_TAG_(member_type, class_name, member_name);								\
	template struct IUTEST_PEEP_SETTER_NAME_(class_name, member_name)< scope_name::class_name,	\
		IUTEST_PEEP_TAG_NAME_(class_name, member_name)<scope_name::class_name>, &scope_name::class_name::member_name>

/**
 * @brief	private	メンバーへのアクセス
 * @param	v			= オブジェクトインスタンス
 * @param	class_name	= クラス名
 * @param	member_name	= メンバー名
*/
#define IUTEST_PEEP_GET(v, class_name, member_name)		(v.*::iutest::detail::peep_tag< IUTEST_PEEP_TAG_NAME_(class_name, member_name)<class_name> >::value)

/**
 * @brief	static private	メンバーへのアクセス
 * @param	class_name	= クラス名
 * @param	member_name	= メンバー名
*/
#define IUTEST_PEEP_STATIC_GET(class_name, member_name)	(*::iutest::detail::peep_tag< IUTEST_PEEP_TAG_NAME_(class_name, member_name)<class_name> >::value)

/**
 * @brief	private メンバーへのアクセスクラス宣言
 * @param	class_name	= クラス名
 * @param	member_name	= メンバー名
*/
#define IUTEST_PEEP(class_name, member_name)	::iutest::Peep< class_name, IUTEST_PEEP_TAG_NAME_(class_name, member_name)<class_name> >::type

/**
 * @brief	private メンバーへのアクセスクラス宣言
 * @param	scope_name	= スコープ
 * @param	class_name	= クラス名
 * @param	member_name	= メンバー名
 * @deprecated	このマクロは廃止予定です。 IUTEST_PEEP を使用してください。
*/
#define IUTEST_SCOPED_PEEP(scope_name, class_name, member_name)	::iutest::Peep< scope_name::class_name, IUTEST_PEEP_TAG_NAME_(class_name, member_name)<scope_name::class_name> >::type

/**
 * @private
 * @{
*/
#define IUTEST_MAKE_PEEP_TAG_(member_type, class_name, member_name)	\
template<typename T>struct IUTEST_PEEP_TAG_NAME_(class_name, member_name) { typedef member_type type; };				\
	template<typename T, typename Tag, typename Tag::type X>struct IUTEST_PEEP_SETTER_NAME_(class_name, member_name) {	\
	IUTEST_PEEP_SETTER_NAME_(class_name, member_name)(void) { ::iutest::detail::peep_tag<Tag>::value = X; }				\
	static IUTEST_PEEP_SETTER_NAME_(class_name, member_name)	instance;												\
	};	\
	template<typename T, typename Tag, typename Tag::type X>IUTEST_PEEP_SETTER_NAME_(class_name, member_name)<T, Tag, X>	\
	IUTEST_PEEP_SETTER_NAME_(class_name, member_name)<T, Tag, X>::instance

#define IUTEST_PEEP_TAG_NAME_(class_name, member_name)			iu_peep_tag_##member_name
#define IUTEST_PEEP_SETTER_NAME_(class_name, member_name)		iu_peep_set_##member_name
/**
 * @}
*/

#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

/**
 * @brief	private	メンバーアクセスオブジェクト
*/
template<typename T, typename Tag>
class Peep
{
private:
	typedef Tag	peep_tag;
	typedef typename Tag::type	peep_type;

private:
	template<typename U, typename Type>
	class peep_member_function_impl
	{
	private:
		typedef typename type_traits::function_return_type<Type>::type return_type;

	private:
		U*	m_ptr;
	public:
		explicit peep_member_function_impl(U* ptr) : m_ptr(ptr) {}

#if IUTEST_HAS_VARIADIC_TEMPLATES
	public:
		template<typename ...Args>
		return_type operator () (Args... args) { return ((*m_ptr).*detail::peep_tag<peep_tag>::value)( std::forward<Args>(args)...); }
#else

#define PEEP_DECL_MEMBER_FUNC_(n)	\
	template<IUTEST_PP_ENUM_PARAMS(n, typename T)>return_type operator () (		\
		IUTEST_PP_ENUM_BINARY_PARAMS(n, T, t)) {				\
		return ((*m_ptr).*detail::peep_tag<peep_tag>::value)(IUTEST_PP_ENUM_PARAMS(n, t)); }

		return_type operator () (void) { return ((*m_ptr).*detail::peep_tag<peep_tag>::value)(); }

		PEEP_DECL_MEMBER_FUNC_(1)
		PEEP_DECL_MEMBER_FUNC_(2)
		PEEP_DECL_MEMBER_FUNC_(3)
		PEEP_DECL_MEMBER_FUNC_(4)
		PEEP_DECL_MEMBER_FUNC_(5)
		PEEP_DECL_MEMBER_FUNC_(6)
		PEEP_DECL_MEMBER_FUNC_(7)
		PEEP_DECL_MEMBER_FUNC_(8)
		PEEP_DECL_MEMBER_FUNC_(9)
		PEEP_DECL_MEMBER_FUNC_(10)
		PEEP_DECL_MEMBER_FUNC_(11)
		PEEP_DECL_MEMBER_FUNC_(12)
		PEEP_DECL_MEMBER_FUNC_(13)
		PEEP_DECL_MEMBER_FUNC_(14)
		PEEP_DECL_MEMBER_FUNC_(15)
		PEEP_DECL_MEMBER_FUNC_(16)
		PEEP_DECL_MEMBER_FUNC_(17)
		PEEP_DECL_MEMBER_FUNC_(18)
		PEEP_DECL_MEMBER_FUNC_(19)
		PEEP_DECL_MEMBER_FUNC_(20)

#undef IUTEST_DECL_PEEP_MEMBER_FUNC_
#endif
	};
	template<typename U, typename Type, bool is_const>
	class peep_member_object_impl
	{
		typedef peep_member_object_impl<U, Type, false>	_Myt;
		typedef Type value_type;
	private:
		U*	m_ptr;
	public:
		explicit peep_member_object_impl(U* ptr) : m_ptr(ptr) {}
	public:
		operator value_type (void) const { return (*m_ptr).*detail::peep_tag<peep_tag>::value; }
	};
	template<typename U, typename Type>
	class peep_member_object_impl<U, Type, false>
	{
		typedef peep_member_object_impl<U, Type, false>	_Myt;
		typedef Type value_type;
	private:
		U*	m_ptr;
	public:
		explicit peep_member_object_impl(U* ptr) : m_ptr(ptr) {}
	public:
		operator value_type (void) const { return (*m_ptr).*detail::peep_tag<peep_tag>::value; }
		operator value_type& (void) { return (*m_ptr).*detail::peep_tag<peep_tag>::value; }
		_Myt&	operator = (const value_type& value) { (*m_ptr).*detail::peep_tag<peep_tag>::value = value; return *this; }
	};


	template<typename U, typename Type, bool Func>
	struct peep_member_impl
	{
		typedef peep_member_function_impl<U, Type> type;
	};
	template<typename U, typename Type>
	struct peep_member_impl<U, Type U::*, false>
	{
		typedef peep_member_object_impl<U, Type, type_traits::is_const<Type>::value> type;
	};

private:
	template<typename U, typename Type, bool Func>
	class peep_static_impl
	{
		typedef peep_static_impl<U, Type, Func>	_Myt;
		typedef typename type_traits::remove_ptr<Type>::type value_type;
	public:
		peep_static_impl(void) {}
		peep_static_impl(const value_type& value) { *detail::peep_tag<peep_tag>::value = value; }
		peep_static_impl(const _Myt&) {}
	public:
		operator value_type (void) const { return *detail::peep_tag<peep_tag>::value; }
		operator value_type& (void) { return *detail::peep_tag<peep_tag>::value; }
		_Myt&	operator = (const value_type& value) { *detail::peep_tag<peep_tag>::value = value; return *this; }
	};

	template<typename U, typename Type>
	class peep_static_impl<U, Type, true>
	{
	public:
		operator Type (void)	{ return *detail::peep_tag<peep_tag>::value; }
	};
private:
	template<typename U, typename Type, bool is_member_ptr>
	struct peep_impl
	{
		typedef peep_static_impl<U, Type, type_traits::is_function_pointer<Type>::value > type;
	};
	template<typename U, typename Type>
	struct peep_impl<U, Type, true>
	{
		typedef typename peep_member_impl<U, Type, type_traits::is_member_function_pointer<Type>::value >::type type;
	};
public:
	typedef typename peep_impl<T, peep_type, type_traits::is_member_pointer<peep_type>::value >::type	type;	//!< private メンバーアクセスオブジェクト型
};

#endif

}	// end of namespace iutest

#endif
