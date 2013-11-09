//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_type_traits.hpp
 * @brief		iris unit test type traits ƒtƒ@ƒCƒ‹
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
#ifndef INCG_IRIS_iutest_type_traits_HPP_6F091F15_784A_4f50_BD18_B8F67C5AF0CF_
#define INCG_IRIS_iutest_type_traits_HPP_6F091F15_784A_4f50_BD18_B8F67C5AF0CF_

//======================================================================
// include
#include "iutest_pp.hpp"

namespace iutest_type_traits
{

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
	static const bool	value = B;
#endif
};
#if !defined(IUTEST_NO_INCLASS_MEMBER_INITIALIZATION)
template<bool B>const bool bool_constant<B>::value;
#endif

typedef bool_constant<true>		true_type;
typedef bool_constant<false>	false_type;


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
	typedef typename impl<T>::type	type;
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
	typedef typename impl<T>::type	type;
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
	typedef typename impl<T>::type	type;
};

/**
 * @brief	remove_cv
*/
template<typename T>
class remove_cv
{
public:
	typedef typename remove_const< typename remove_volatile<T>::type >::type	type;
};

/**
 * @brief	remove_ptr
*/
template<typename T>
struct remove_ptr		{ typedef T type; };
template<typename T>
struct remove_ptr<T*>	{ typedef T type; };

#endif

namespace helper
{

template<typename T>
class is_pointer_helper
{
#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
	template<typename U, typename TMP> struct impl { typedef false_type type; };
	template<typename U, typename TMP> struct impl<U*, TMP> { typedef true_type type; };
	typedef typename remove_cv<T>::type	rmcv_type;
public:
	typedef typename impl<rmcv_type, void>::type type;
#else
	typedef T	rmcv_type;
	static T& make_t();
	static char	IsPointerHelper(const volatile void*);
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
template<typename T>
struct is_pointer : public helper::is_pointer_helper<T>::type {};

#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

namespace helper
{

template<typename T>
class is_reference_helper
{
	template<typename U, typename TMP> struct impl { typedef false_type type; };
	template<typename U, typename TMP> struct impl<U&, TMP> { typedef true_type type; };
	typedef typename remove_cv<T>::type	rmcv_type;
public:
	typedef typename impl<rmcv_type, void>::type type;
};

}

/**
 * @brief	is_reference
*/
template<typename T>
struct is_reference : public helper::is_reference_helper<T>::type {};

namespace helper
{
template<typename T>
class is_void_helper
{
	template<typename U, typename TMP> struct impl { typedef false_type type; };
	template<typename TMP> struct impl<void, TMP> { typedef true_type type; };
	typedef typename remove_cv<T>::type	rmcv_type;
public:
	typedef typename impl<rmcv_type, void>::type type;
};

}

/**
 * @brief	is_void
*/
template<typename T>
class is_void : public helper::is_void_helper<T>::type {};

/**
 * @brief	is_const
*/
template<typename T>
struct is_const : public false_type {};
template<typename T>
struct is_const<T const> : public true_type {};

#endif // #if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

namespace helper
{

#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template<typename T1, typename T2>
struct is_same_helper { typedef false_type type; };
template<typename T>
struct is_same_helper<T, T> { typedef true_type type; };

#elif IUTEST_HAS_CLASS_MEMBER_TEMPLATE_SPECIALIZATION

template<typename T1, typename T2>
class is_same_helper
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
struct is_same : public helper::is_same_helper<T1, T2>::type {};

#endif

namespace helper
{

template<typename From, typename To>
class is_convertible_type
{
	static From	MakeFrom(void);

	static char	IsConvertibleHelper(To);
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
class is_convertible : public helper::is_convertible_type<From, To>::type
{
};

#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

/**
 * @brief	add reference
*/
template<typename T>
class add_reference
{
	template<typename U, bool b>struct impl { typedef U type; };
	template<typename U>struct impl<U, true>
	{
		typedef U& type;
	};

public:
	typedef typename impl<T, !is_void<T>::value && !is_reference<T>::value >::type type;
};

#ifndef IUTEST_HAS_RVALUE_REFS
#  if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ > 2)) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#    define IUTEST_HAS_RVALUE_REFS	1
#  elif	defined(_MSC_VER) && (_MSC_VER >= 1700)
#    define IUTEST_HAS_RVALUE_REFS	1
#  endif
#endif

#ifndef IUTEST_HAS_RVALUE_REFS
#  define IUTEST_HAS_RVALUE_REFS	0
#endif

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

/**
 * @brief	is function pointer
*/
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

public:
	enum { value = impl< typename remove_cv<T>::type >::value };
};

/**
 * @brief	is member function pointer
*/
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
	enum { value = impl< typename remove_cv<T>::type >::value };
};

/**
 * @brief	is member pointer
*/
template<typename T>
class is_member_pointer
{
	template<typename U>
	struct impl : public false_type {};
	template<typename U, typename C>
	struct impl<U C::*> : public true_type {};

public:
	enum { value = impl< typename remove_cv<T>::type >::value || is_member_function_pointer<T>::value ? true : false };
};

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
	IIUT_DECL_FUNCTION_RETURN_TYPE_I(n, const volatile)	\

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

#endif	// #if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

}	// end of namespace iutest_type_traits


#endif
