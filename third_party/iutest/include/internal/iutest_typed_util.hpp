//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_typed_util.hpp
 * @brief		iris unit test typed util
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
#ifndef INCG_IRIS_IUTEST_TYPED_UTIL_HPP_32A6F7CF_36C6_4EDE_AD19_04965FBFBB37_
#define INCG_IRIS_IUTEST_TYPED_UTIL_HPP_32A6F7CF_36C6_4EDE_AD19_04965FBFBB37_

//======================================================================
// include
#include "iutest_internal_defs.hpp"
#include "iutest_typelist.hpp"

namespace iutest
{

//======================================================================
// struct
#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

/**
 * @ingroup	TYPED_TEST TYPE_PARAMETERIZED_TEST
 * @brief	Type List
 * @tparam	Args	= 型リスト
 * @{
*/
#if IUTEST_HAS_VARIADIC_TEMPLATES

template<typename ...Args>
struct Types
{
	typedef detail::VariadicTypeList<Args...> type;
	template<size_t N>struct get : public detail::typelist_get<type, N> {};
};

#else

#define IIUT_DECL_DEFAULT_ARG_(i, param)	IUTEST_PP_CAT(param,i)=detail::None

template< IUTEST_PP_ENUM(50, IIUT_DECL_DEFAULT_ARG_, typename T) >
struct Types
{
	typedef detail::TypeList50< IUTEST_PP_ENUM_PARAMS(50, T) > type;
	template<size_t N>struct get : public detail::typelist_get<type, N> {};
};

#undef IIUT_DECL_DEFAULT_ARG_

#define IIUT_DECL_SPEC_NONE_(i, param)	param
#define IIUT_DECL_TYPES_(n, m)								\
	template< IUTEST_PP_ENUM_PARAMS(n, typename T) >		\
	struct Types< IUTEST_PP_ENUM_PARAMS(n, T)				\
		, IUTEST_PP_ENUM(m, IIUT_DECL_SPEC_NONE_, detail::None) > {	\
		typedef IUTEST_PP_CAT(detail::TypeList, n)<			\
			IUTEST_PP_ENUM_PARAMS(n, T) > type;				\
		template<size_t N>struct get : public detail::typelist_get<type, N> {};	\
	}

template<>
struct Types< IUTEST_PP_ENUM(50, IIUT_DECL_SPEC_NONE_, detail::None) >
{
	typedef detail::TypeList0 type;
};

IIUT_DECL_TYPES_( 1, 49);
IIUT_DECL_TYPES_( 2, 48);
IIUT_DECL_TYPES_( 3, 47);
IIUT_DECL_TYPES_( 4, 46);
IIUT_DECL_TYPES_( 5, 45);
IIUT_DECL_TYPES_( 6, 44);
IIUT_DECL_TYPES_( 7, 43);
IIUT_DECL_TYPES_( 8, 42);
IIUT_DECL_TYPES_( 9, 41);
IIUT_DECL_TYPES_(10, 40);
IIUT_DECL_TYPES_(11, 39);
IIUT_DECL_TYPES_(12, 38);
IIUT_DECL_TYPES_(13, 37);
IIUT_DECL_TYPES_(14, 36);
IIUT_DECL_TYPES_(15, 35);
IIUT_DECL_TYPES_(16, 34);
IIUT_DECL_TYPES_(17, 33);
IIUT_DECL_TYPES_(18, 32);
IIUT_DECL_TYPES_(19, 31);
IIUT_DECL_TYPES_(20, 30);
IIUT_DECL_TYPES_(21, 29);
IIUT_DECL_TYPES_(22, 28);
IIUT_DECL_TYPES_(23, 27);
IIUT_DECL_TYPES_(24, 26);
IIUT_DECL_TYPES_(25, 25);
IIUT_DECL_TYPES_(26, 24);
IIUT_DECL_TYPES_(27, 23);
IIUT_DECL_TYPES_(28, 22);
IIUT_DECL_TYPES_(29, 21);
IIUT_DECL_TYPES_(30, 20);
IIUT_DECL_TYPES_(31, 19);
IIUT_DECL_TYPES_(32, 18);
IIUT_DECL_TYPES_(33, 17);
IIUT_DECL_TYPES_(34, 16);
IIUT_DECL_TYPES_(35, 15);
IIUT_DECL_TYPES_(36, 14);
IIUT_DECL_TYPES_(37, 13);
IIUT_DECL_TYPES_(38, 12);
IIUT_DECL_TYPES_(39, 11);
IIUT_DECL_TYPES_(40, 10);
IIUT_DECL_TYPES_(41,  9);
IIUT_DECL_TYPES_(42,  8);
IIUT_DECL_TYPES_(43,  7);
IIUT_DECL_TYPES_(44,  6);
IIUT_DECL_TYPES_(45,  5);
IIUT_DECL_TYPES_(46,  4);
IIUT_DECL_TYPES_(47,  3);
IIUT_DECL_TYPES_(48,  2);
IIUT_DECL_TYPES_(49,  1);

#undef IIUT_DECL_SPEC_NONE_
#undef IIUT_DECL_TYPES_

#endif

/**
 * @}
*/

namespace detail
{

#if !defined(IUTEST_NO_TEMPLATE_TEMPLATES)

/**
 * @brief	Templates
 * @{
*/
#if IUTEST_HAS_VARIADIC_TEMPLATE_TEMPLATES
template<IUTEST_TEMPLATE_TPARAM1 ...Types>
struct Templates
{
	typedef VariadicTemplateTypeList<Types...> type;
};

#else

#define IIUT_DECL_DEFAULT_ARG_(i, param)	IUTEST_PP_CAT(param,i)=detail::NoneT1

template< IUTEST_PP_ENUM(50, IIUT_DECL_DEFAULT_ARG_, IUTEST_TEMPLATE_TPARAM1 T) >
struct Templates
{
	typedef detail::TemplateTypeList50< IUTEST_PP_ENUM_PARAMS(50, T) > type;
};

#undef IIUT_DECL_DEFAULT_ARG_

#define IIUT_DECL_SPEC_NONE_(i, param)	param
#define IIUT_DECL_TEMPLATES_(n, m)										\
	template< IUTEST_PP_ENUM_PARAMS(n, IUTEST_TEMPLATE_TPARAM1 T) >		\
	struct Templates< IUTEST_PP_ENUM_PARAMS(n, T)						\
		, IUTEST_PP_ENUM(m, IIUT_DECL_SPEC_NONE_, detail::NoneT1) > {	\
		typedef IUTEST_PP_CAT(detail::TemplateTypeList, n)<				\
			IUTEST_PP_ENUM_PARAMS(n, T) > type;							\
	}

template<>
struct Templates< IUTEST_PP_ENUM(50, IIUT_DECL_SPEC_NONE_, detail::NoneT1) >
{
	typedef detail::TemplateTypeList0 type;
};

IIUT_DECL_TEMPLATES_( 1, 49);
IIUT_DECL_TEMPLATES_( 2, 48);
IIUT_DECL_TEMPLATES_( 3, 47);
IIUT_DECL_TEMPLATES_( 4, 46);
IIUT_DECL_TEMPLATES_( 5, 45);
IIUT_DECL_TEMPLATES_( 6, 44);
IIUT_DECL_TEMPLATES_( 7, 43);
IIUT_DECL_TEMPLATES_( 8, 42);
IIUT_DECL_TEMPLATES_( 9, 41);
IIUT_DECL_TEMPLATES_(10, 40);
IIUT_DECL_TEMPLATES_(11, 39);
IIUT_DECL_TEMPLATES_(12, 38);
IIUT_DECL_TEMPLATES_(13, 37);
IIUT_DECL_TEMPLATES_(14, 36);
IIUT_DECL_TEMPLATES_(15, 35);
IIUT_DECL_TEMPLATES_(16, 34);
IIUT_DECL_TEMPLATES_(17, 33);
IIUT_DECL_TEMPLATES_(18, 32);
IIUT_DECL_TEMPLATES_(19, 31);
IIUT_DECL_TEMPLATES_(20, 30);
IIUT_DECL_TEMPLATES_(21, 29);
IIUT_DECL_TEMPLATES_(22, 28);
IIUT_DECL_TEMPLATES_(23, 27);
IIUT_DECL_TEMPLATES_(24, 26);
IIUT_DECL_TEMPLATES_(25, 25);
IIUT_DECL_TEMPLATES_(26, 24);
IIUT_DECL_TEMPLATES_(27, 23);
IIUT_DECL_TEMPLATES_(28, 22);
IIUT_DECL_TEMPLATES_(29, 21);
IIUT_DECL_TEMPLATES_(30, 20);
IIUT_DECL_TEMPLATES_(31, 19);
IIUT_DECL_TEMPLATES_(32, 18);
IIUT_DECL_TEMPLATES_(33, 17);
IIUT_DECL_TEMPLATES_(34, 16);
IIUT_DECL_TEMPLATES_(35, 15);
IIUT_DECL_TEMPLATES_(36, 14);
IIUT_DECL_TEMPLATES_(37, 13);
IIUT_DECL_TEMPLATES_(38, 12);
IIUT_DECL_TEMPLATES_(39, 11);
IIUT_DECL_TEMPLATES_(40, 10);
IIUT_DECL_TEMPLATES_(41,  9);
IIUT_DECL_TEMPLATES_(42,  8);
IIUT_DECL_TEMPLATES_(43,  7);
IIUT_DECL_TEMPLATES_(44,  6);
IIUT_DECL_TEMPLATES_(45,  5);
IIUT_DECL_TEMPLATES_(46,  4);
IIUT_DECL_TEMPLATES_(47,  3);
IIUT_DECL_TEMPLATES_(48,  2);
IIUT_DECL_TEMPLATES_(49,  1);

#undef IIUT_DECL_SPEC_NONE_
#undef IIUT_DECL_TEMPLATES_

#endif

/**
 * @}
*/

#endif // #if !defined(IUTEST_NO_TEMPLATE_TEMPLATES)

template<typename T>
struct TypeList
{
	typedef detail::TypeList1<T> type;
};

#if IUTEST_HAS_VARIADIC_TEMPLATES

template<typename ...Args>
struct TypeList< Types<Args...> >
{
	typedef typename Types<Args...>::type type;
};

#else

template< IUTEST_PP_ENUM_PARAMS(50, typename T) >
struct TypeList< Types< IUTEST_PP_ENUM_PARAMS(50, T) > >
{
	typedef typename Types< IUTEST_PP_ENUM_PARAMS(50, T) >::type type;
};

#endif

}	// end of namespace detail

#endif // #if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

}	// end of namespace iutest

#endif // INCG_IRIS_IUTEST_TYPED_UTIL_HPP_32A6F7CF_36C6_4EDE_AD19_04965FBFBB37_
