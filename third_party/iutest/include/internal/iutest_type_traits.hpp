//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_type_traits.hpp
 * @brief		iris unit test type traits
 *
 * @author		t.shirayanagi
 * @par			copyright
 * Copyright (C) 2012-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_TYPE_TRAITS_HPP_6F091F15_784A_4F50_BD18_B8F67C5AF0CF_
#define INCG_IRIS_IUTEST_TYPE_TRAITS_HPP_6F091F15_784A_4F50_BD18_B8F67C5AF0CF_

//======================================================================
// include
#include "iutest_pp.hpp"

//======================================================================
// define
#ifndef IUTEST_HAS_HDR_TYPETARITS
#  if defined(__clang__)
#    if __has_include( <type_traits> ) && IUTEST_HAS_CXX11
#      define IUTEST_HAS_HDR_TYPETARITS		1
#    endif
#  elif (defined(__GLIBCXX__) || defined(_LIBCPP_VERSION)) && IUTEST_HAS_CXX11
#    define IUTEST_HAS_HDR_TYPETARITS		1
#  elif defined(_MSC_VER)
#    if _MSC_VER >= 1600
#      define IUTEST_HAS_HDR_TYPETARITS		1
#    endif
#  endif
#endif

#ifndef IUTEST_HAS_HDR_TYPETARITS
#  define IUTEST_HAS_HDR_TYPETARITS			0
#endif

#ifndef IUTEST_HAS_RVALUE_REFS
#  if   (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 2)) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#    define IUTEST_HAS_RVALUE_REFS	1
#  elif defined(_MSC_VER) && (_MSC_VER >= 1700)
#    define IUTEST_HAS_RVALUE_REFS	1
#  endif
#endif

#ifndef IUTEST_HAS_RVALUE_REFS
#  define IUTEST_HAS_RVALUE_REFS	0
#endif

#if IUTEST_HAS_HDR_TYPETARITS
#  include <type_traits>
#endif

/** iutest type traits */
namespace iutest_type_traits
{

#if IUTEST_HAS_HDR_TYPETARITS

using ::std::true_type;
using ::std::false_type;
using ::std::remove_const;
using ::std::remove_volatile;
using ::std::remove_reference;
using ::std::remove_cv;
using ::std::remove_pointer;
using ::std::is_pointer;
using ::std::is_void;
using ::std::is_const;
using ::std::is_same;
using ::std::is_class;
using ::std::is_convertible;
using ::std::is_base_of;
using ::std::add_lvalue_reference;
#if IUTEST_HAS_RVALUE_REFS
using ::std::add_rvalue_reference;
#else
template<typename T>
struct add_rvalue_reference { typedef T type; };
#endif
using ::std::is_function;
using ::std::is_member_function_pointer;
using ::std::is_member_pointer;

template<typename F>
struct is_function_pointer : public ::std::integral_constant<bool
	, ::std::is_pointer<F>::value && ::std::is_function< typename ::std::remove_pointer<F>::type >::value > {};

template<bool B>
struct bool_constant : ::std::integral_constant<bool, B> {};

#else

//======================================================================
// struct

/**
 * @brief	bool constant
*/
template<bool B>
struct bool_constant
{
#if defined(IUTEST_NO_INCLASS_MEMBER_INITIALIZATION)
	enum { value = B };
#else
	static const bool value = B;
#endif
};
#if !defined(IUTEST_NO_INCLASS_MEMBER_INITIALIZATION)
template<bool B>const bool bool_constant<B>::value;
#endif

typedef bool_constant<true>  true_type;
typedef bool_constant<false> false_type;

#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

/**
 * @brief	remove_const
*/
template<typename T>
class remove_const
{
	template<typename U>
	struct impl { typedef U type; };
	template<typename U>
	struct impl<const U> { typedef U type; };
public:
	typedef typename impl<T>::type type;
};

/**
 * @brief	remove_volatile
*/
template<typename T>
class remove_volatile
{
	template<typename U>
	struct impl { typedef U type; };
	template<typename U>
	struct impl<volatile U> { typedef U type; };
public:
	typedef typename impl<T>::type type;
};

/**
 * @brief	remove_reference
*/
template<typename T>
class remove_reference
{
	template<typename U>
	struct impl { typedef U type; };
	template<typename U>
	struct impl<U&> { typedef U type; };
#if IUTEST_HAS_RVALUE_REFS
	template<typename U>
	struct impl<U&&> { typedef U type; };
#endif
public:
	typedef typename impl<T>::type type;
};

/**
 * @brief	remove_cv
*/
template<typename T>
struct remove_cv
{
	typedef typename remove_const< typename remove_volatile<T>::type >::type type;
};

/**
 * @brief	remove_pointer
*/
template<typename T>
class remove_pointer
{
	template<typename U>
	struct impl { typedef U type; };
	template<typename U>
	struct impl<U*> { typedef U type; };
public:
	typedef typename impl<T>::type type;
};

#endif

namespace is_pointer_helper
{

/** @private */
template<typename T>
class is_pointer
{
#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
	template<typename U, typename TMP> struct impl { typedef false_type type; };
	template<typename U, typename TMP> struct impl<U*, TMP> { typedef true_type type; };
	typedef typename remove_cv<T>::type rmcv_type;
public:
	typedef typename impl<rmcv_type, void>::type type;
#else
	typedef T rmcv_type;
	static T& make_t();
	static char IsPointerHelper(const volatile void*);
	static char (&IsPointerHelper(...))[2];

	enum { IsPointer = sizeof(IsPointerHelper(make_t())) == 1 ? true : false };
public:
	typedef bool_constant<IsPointer> type;
#endif
};

}

/**
 * @brief	is_pointer
*/
#if !defined(_MSC_VER) || _MSC_VER >= 1310
template<typename T>
struct is_pointer : public is_pointer_helper::is_pointer<T>::type {};
#else
template<typename T>
struct is_pointer_impl : public is_pointer_helper::is_pointer<T> {};

template<typename T>
struct is_pointer : public is_pointer_impl<T>::type {};
#endif

#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

namespace is_reference_helper
{

/** @private */
template<typename T>
class is_reference
{
	template<typename U, typename TMP> struct impl { typedef false_type type; };
	template<typename U, typename TMP> struct impl<U&, TMP> { typedef true_type type; };
	typedef typename remove_cv<T>::type rmcv_type;
public:
	typedef typename impl<rmcv_type, void>::type type;
};

}

/**
 * @brief	is_reference
*/
template<typename T>
struct is_reference : public is_reference_helper::is_reference<T>::type {};

namespace is_void_helper
{

/** @private */
template<typename T>
class is_void
{
	template<typename U, typename TMP> struct impl { typedef false_type type; };
	template<typename TMP> struct impl<void, TMP> { typedef true_type type; };
	typedef typename remove_cv<T>::type rmcv_type;
public:
	typedef typename impl<rmcv_type, void>::type type;
};

}

/**
 * @brief	is_void
*/
template<typename T>
struct is_void : public is_void_helper::is_void<T>::type {};

namespace is_const_helper
{

/** @private */
template<typename T>
class is_const
{
	template<typename U> struct impl { typedef false_type type; };
	template<typename U> struct impl<U const> { typedef true_type type; };
public:
	typedef typename impl<T>::type type;
};

}

/**
 * @brief	is_const
*/
template<typename T>
struct is_const : public is_const_helper::is_const<T>::type {};

#endif // #if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

namespace is_same_helper
{

#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

/** @private */
template<typename T1, typename T2>
struct is_same { typedef false_type type; };
/** @private */
template<typename T>
struct is_same<T, T> { typedef true_type type; };

#elif IUTEST_HAS_CLASS_MEMBER_TEMPLATE_SPECIALIZATION

/** @private */
template<typename T1, typename T2>
class is_same
{
	template<typename T>
	struct impl { typedef false_type type; };
	template<>
	struct impl<T1> { typedef true_type type; };
public:
	typedef typename impl<T2>::type type;
};

#endif

}

#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) || IUTEST_HAS_CLASS_MEMBER_TEMPLATE_SPECIALIZATION

/**
 * @brief	is_same
*/
template<typename T1, typename T2>
struct is_same : public is_same_helper::is_same<T1, T2>::type {};

#endif

namespace is_class_helper
{

/** @private */
template<typename T>
class is_class
{
	template<typename U>
	static char IsClassHelper(int U::*);
	template<typename U>
	static char(&IsClassHelper(...))[2];

	enum { IsClass = sizeof(IsClassHelper<T>(0)) == 1 ? true : false };
public:
	typedef bool_constant<IsClass> type;
};

}

/**
 * @brief	is class
*/
template<typename T>
struct is_class : public is_class_helper::is_class<T>::type
{
};

namespace is_convertible_helper
{

/** @private */
template<typename From, typename To>
class is_convertible_type
{
	static From MakeFrom(void);

	static char IsConvertibleHelper(To);
	static char (&IsConvertibleHelper(...))[2];

	enum { IsConvertible = sizeof(IsConvertibleHelper(is_convertible_type::MakeFrom())) == 1 ? true : false };
public:
	typedef bool_constant<IsConvertible> type;
};

}

/**
 * @brief	is convertible
*/
template<typename From, typename To>
struct is_convertible : public is_convertible_helper::is_convertible_type<From, To>::type
{
};

namespace is_base_of_helper
{

/** @private */
template<typename Base, typename Derived>
class is_base_of_check
{
	struct no_t { char a[2]; };
	template<typename T>
	static char CheckSig(Derived const volatile *, T);
	static no_t CheckSig(Base const volatile *, int);
	struct host
	{
		operator Base const volatile * () const;
		operator Derived const volatile * ();
	};
	enum { IsBaseAndDerived = sizeof(CheckSig(host(), 0)) == 1 ? true : false };
public:
	typedef bool_constant<IsBaseAndDerived> type;
};

/** @private */
template<bool is_class_b, bool is_class_d, bool is_same>
struct is_base_of_select
{
	template<typename T, typename U>struct rebind { typedef false_type type;  };
};
/** @private */
template<>
struct is_base_of_select<true, true, true>
{
	template<typename T, typename U>struct rebind { typedef true_type type;  };
};
/** @private */
template<>
struct is_base_of_select<true, true, false>
{
	template<typename T, typename U>struct rebind
	{
		typedef typename is_base_of_check<T, U>::type type;
	};
};

/** @private */
template<typename Base, typename Derived>
class is_base_of
{
#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
	typedef typename remove_cv<Base>::type B;
	typedef typename remove_cv<Derived>::type D;
#else
	typedef Base B;
	typedef Derived D;
#endif
	typedef is_base_of_select<
	is_class<Base>::value
	, is_class<Derived>::value
	, is_same<Base, Derived>::value
	> selector;
	typedef typename selector::template rebind<B, D> binder;
public:
	typedef typename binder::type type;
};

}

/**
 * @brief	is base of
*/
template<typename Base, typename Derived>
struct is_base_of : public is_base_of_helper::is_base_of<Base, Derived>::type
{
};


#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

/**
 * @brief	add lvalue reference
*/
template<typename T>
class add_lvalue_reference
{
	template<typename U, bool b>struct impl { typedef U type; };
	template<typename U>struct impl<U, true>
	{
		typedef U& type;
	};

public:
	typedef typename impl<T, !is_void<T>::value && !is_reference<T>::value >::type type;
};

/**
 * @brief	add rvalue reference
*/
template<typename T>
class add_rvalue_reference
{
	template<typename U, bool b>struct impl { typedef U type; };

#if IUTEST_HAS_RVALUE_REFS
	template<typename U>struct impl<U, true>
	{
		typedef U&& type;
	};
#endif

public:
	typedef typename impl<T, !is_void<T>::value && !is_reference<T>::value >::type type;
};

#else

template<typename T>
struct add_rvalue_reference { typedef T type; };

#endif

#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

namespace is_function_pointer_helper
{

/** @private */
template<typename T>
class is_function_pointer
{
	template<typename U>
	struct impl : public false_type {};

#if IUTEST_HAS_VARIADIC_TEMPLATES
	template<typename R, typename ...Args>
	struct impl<R (*)(Args...)> : public true_type {};
	template<typename R, typename ...Args>
	struct impl<R (*)(Args..., ...)> : public true_type {};

#else
	template<typename R>
	struct impl<R (*)()> : public true_type {};
	template<typename R>
	struct impl<R (*)(...)> : public true_type {};

#define IIUT_DECL_IS_FUNCTION_PTR_(n)	\
template<typename R, IUTEST_PP_ENUM_PARAMS(n, typename T)>struct impl<R (*)(IUTEST_PP_ENUM_PARAMS(n, T))> : public true_type {};	\
template<typename R, IUTEST_PP_ENUM_PARAMS(n, typename T)>struct impl<R (*)(IUTEST_PP_ENUM_PARAMS(n, T), ...)> : public true_type {}

	IIUT_DECL_IS_FUNCTION_PTR_(1);
	IIUT_DECL_IS_FUNCTION_PTR_(2);
	IIUT_DECL_IS_FUNCTION_PTR_(3);
	IIUT_DECL_IS_FUNCTION_PTR_(4);
	IIUT_DECL_IS_FUNCTION_PTR_(5);
	IIUT_DECL_IS_FUNCTION_PTR_(6);
	IIUT_DECL_IS_FUNCTION_PTR_(7);
	IIUT_DECL_IS_FUNCTION_PTR_(8);
	IIUT_DECL_IS_FUNCTION_PTR_(9);
	IIUT_DECL_IS_FUNCTION_PTR_(10);
	IIUT_DECL_IS_FUNCTION_PTR_(11);
	IIUT_DECL_IS_FUNCTION_PTR_(12);
	IIUT_DECL_IS_FUNCTION_PTR_(13);
	IIUT_DECL_IS_FUNCTION_PTR_(14);
	IIUT_DECL_IS_FUNCTION_PTR_(15);
	IIUT_DECL_IS_FUNCTION_PTR_(16);
	IIUT_DECL_IS_FUNCTION_PTR_(17);
	IIUT_DECL_IS_FUNCTION_PTR_(18);
	IIUT_DECL_IS_FUNCTION_PTR_(19);
	IIUT_DECL_IS_FUNCTION_PTR_(20);

#undef IIUT_DECL_IS_FUNCTION_PTR_

#endif

	enum { value = impl< typename remove_cv<T>::type >::value };
public:
	typedef bool_constant<value> type;
};

}

/**
 * @brief	is function pointer
*/
template<typename T>
struct is_function_pointer : public is_function_pointer_helper::is_function_pointer<T>::type
{
};

namespace is_member_function_pointer_helper
{

/** @private */
template<typename T>
class is_member_function_pointer
{
	template<typename U>
	struct impl : public false_type {};

#if IUTEST_HAS_VARIADIC_TEMPLATES

#define IIUT_DECL_IS_MEMBER_FUNCTION_PTR_CV_(CV)	\
	template<typename R, typename U, typename ...Args>			\
	struct impl<R (U::*)(Args...) CV> : public true_type {};	\
	template<typename R, typename U, typename ...Args>			\
	struct impl<R (U::*)(Args..., ...) CV> : public true_type {}

	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_CV_(IUTEST_PP_EMPTY());
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_CV_(const);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_CV_(volatile);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_CV_(const volatile);

#undef IIUT_DECL_IS_MEMBER_FUNCTION_PTR_CV_

#else

#define IIUT_DECL_IS_MEMBER_FUNCTION_PTR_VOID_CV_(CV)	\
	template<typename R, typename U>struct impl<R (U::*)() CV> : public true_type {};	\
	template<typename R, typename U>struct impl<R (U::*)(...) CV> : public true_type {}

	template<typename R, typename U>struct impl<R (U::*)()> : public true_type {};
	template<typename R, typename U>struct impl<R (U::*)(...)> : public true_type {};
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_VOID_CV_(const);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_VOID_CV_(volatile);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_VOID_CV_(const volatile);

#undef IIUT_DECL_IS_MEMBER_FUNCTION_PTR_VOID_CV_

#define IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(n)	\
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_I(n, IUTEST_PP_EMPTY());	\
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_I(n, const);	\
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_I(n, volatile);	\
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_I(n, const volatile)

#define IIUT_DECL_IS_MEMBER_FUNCTION_PTR_I(n, CV)	\
	template<typename R, typename U, IUTEST_PP_ENUM_PARAMS(n, typename T)>struct impl<R (U::*)(IUTEST_PP_ENUM_PARAMS(n, T)) CV> : public true_type {};	\
	template<typename R, typename U, IUTEST_PP_ENUM_PARAMS(n, typename T)>struct impl<R (U::*)(IUTEST_PP_ENUM_PARAMS(n, T), ...) CV> : public true_type {}

	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(1);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(2);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(3);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(4);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(5);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(6);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(7);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(8);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(9);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(10);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(11);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(12);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(13);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(14);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(15);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(16);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(17);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(18);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(19);
	IIUT_DECL_IS_MEMBER_FUNCTION_PTR_(20);

#undef IIUT_DECL_IS_MEMBER_FUNCTION_PTR_
#undef IIUT_DECL_IS_MEMBER_FUNCTION_PTR_I

#endif

public:
	typedef bool_constant< impl< typename remove_cv<T>::type >::value > type;
};

}

/**
 * @brief	is member function pointer
*/
template<typename T>
struct is_member_function_pointer : public is_member_function_pointer_helper::is_member_function_pointer<T>::type
{
};

namespace is_member_pointer_helper
{

/** @private */
template<typename T>
class is_member_pointer
{
	template<typename U>
	struct impl : public false_type {};
	template<typename U, typename C>
	struct impl<U C::*> : public true_type {};

public:
	typedef bool_constant< impl< typename remove_cv<T>::type >::value || is_member_function_pointer<T>::value > type;
};

}

/**
 * @brief	is member pointer
*/
template<typename T>
struct is_member_pointer : public is_member_pointer_helper::is_member_pointer<T>::type
{
};

#endif // #if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

#endif // #if IUTEST_HAS_HDR_TYPETRAITS

#if !defined(IUTEST_NO_ARGUMENT_DEPENDENT_LOOKUP)

namespace has_equal_to_operator_helper
{
	typedef char no_t[7];
	template<typename T1, typename T2>
	no_t& operator == (const T1& lhs, const T2& rhs);
}

namespace has_equal_to_operator_impl
{
	using namespace has_equal_to_operator_helper;
	/** @private */
	template<typename T>
	struct has_equal_to_operator
	{
		typedef bool_constant< (sizeof(*(T*)0 == *(T*)0) != sizeof(has_equal_to_operator_helper::no_t) ) > type;
	};
}

/**
 * @brief	has equal to operator
*/
template<typename T>
struct has_equal_to : public has_equal_to_operator_impl::has_equal_to_operator<T>::type
{
};

namespace has_not_equal_to_operator_helper
{
	typedef char no_t[7];
	template<typename T1, typename T2>
	no_t& operator != (const T1& lhs, const T2& rhs);
}

namespace has_not_equal_to_operator_impl
{
	using namespace has_not_equal_to_operator_helper;
	/** @private */
	template<typename T>
	struct has_not_equal_to_operator
	{
		typedef bool_constant< (sizeof(*(T*)0 != *(T*)0) != sizeof(has_not_equal_to_operator_helper::no_t)) > type;
	};
}

/**
 * @brief	has not equal to operator
*/
template<typename T>
struct has_not_equal_to : public has_not_equal_to_operator_impl::has_not_equal_to_operator<T>::type
{
};

#endif

#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

/**
 * @brief	function return type
*/
template<typename T>
class function_return_type
{
	template<typename U> struct impl {};

#if IUTEST_HAS_VARIADIC_TEMPLATES
	template<typename R, typename ...Args>struct impl<R (*)(Args...)>		{ typedef R type; };
	template<typename R, typename ...Args>struct impl<R (*)(Args..., ...)>	{ typedef R type; };
	template<typename R, typename U, typename ...Args>struct impl<R (U::*)(Args...)>		{ typedef R type; };
	template<typename R, typename U, typename ...Args>struct impl<R (U::*)(Args..., ...)>	{ typedef R type; };

#define IIUT_DECL_FUNCTION_RETURN_TYPE_CV_(CV)	\
	template<typename R, typename U, typename ...Args>struct impl<R (U::*)(Args...) CV>		{ typedef R type; };	\
	template<typename R, typename U, typename ...Args>struct impl<R (U::*)(Args..., ...) CV>{ typedef R type; }

	IIUT_DECL_FUNCTION_RETURN_TYPE_CV_(const);
	IIUT_DECL_FUNCTION_RETURN_TYPE_CV_(volatile);
	IIUT_DECL_FUNCTION_RETURN_TYPE_CV_(const volatile);
#undef IIUT_DECL_FUNCTION_RETURN_TYPE_CV_

#else

#define IIUT_DECL_FUNCTION_RETURN_TYPE_(n)	\
	template<typename R, IUTEST_PP_ENUM_PARAMS(n, typename T)>struct impl<R (*)(IUTEST_PP_ENUM_PARAMS(n, T))>	\
	{ typedef R type; };	\
	template<typename R, IUTEST_PP_ENUM_PARAMS(n, typename T)>struct impl<R (*)(IUTEST_PP_ENUM_PARAMS(n, T), ...)>	\
	{ typedef R type; };	\
	IIUT_DECL_FUNCTION_RETURN_TYPE_I(n, IUTEST_PP_EMPTY());	\
	IIUT_DECL_FUNCTION_RETURN_TYPE_I(n, const);	\
	IIUT_DECL_FUNCTION_RETURN_TYPE_I(n, volatile);	\
	IIUT_DECL_FUNCTION_RETURN_TYPE_I(n, const volatile)

#define IIUT_DECL_FUNCTION_RETURN_TYPE_I(n, CV)	\
	template<typename R, typename U, IUTEST_PP_ENUM_PARAMS(n, typename T)>struct impl<R (U::*)(IUTEST_PP_ENUM_PARAMS(n, T)) CV>	\
	{ typedef R type; };	\
	template<typename R, typename U, IUTEST_PP_ENUM_PARAMS(n, typename T)>struct impl<R (U::*)(IUTEST_PP_ENUM_PARAMS(n, T), ...) CV>	\
	{ typedef R type; }

	template<typename R>struct impl<R (*)(void)>	{ typedef R type; };
	template<typename R, typename U>struct impl<R (U::*)(void)>	{ typedef R type; };

#define IIUT_DECL_FUNCTION_RETURN_TYPE_VOID_(CV)	\
	template<typename R, typename U>struct impl<R (U::*)(void) CV>	{ typedef R type; }

	IIUT_DECL_FUNCTION_RETURN_TYPE_VOID_(const);
	IIUT_DECL_FUNCTION_RETURN_TYPE_VOID_(volatile);
	IIUT_DECL_FUNCTION_RETURN_TYPE_VOID_(const volatile);

#undef IIUT_DECL_FUNCTION_RETURN_TYPE_VOID_

	IIUT_DECL_FUNCTION_RETURN_TYPE_(1);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(2);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(3);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(4);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(5);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(6);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(7);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(8);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(9);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(10);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(11);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(12);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(13);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(14);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(15);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(16);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(17);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(18);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(19);
	IIUT_DECL_FUNCTION_RETURN_TYPE_(20);

#undef IIUT_DECL_FUNCTION_RETURN_TYPE_
#undef IIUT_DECL_FUNCTION_RETURN_TYPE_I

#endif

public:
	typedef typename impl< typename remove_cv<T>::type >::type type;
};

#endif // #if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

}	// end of namespace iutest_type_traits

#endif // INCG_IRIS_IUTEST_TYPE_TRAITS_HPP_6F091F15_784A_4F50_BD18_B8F67C5AF0CF_
