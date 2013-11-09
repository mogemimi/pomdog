//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_typelist.hpp
 * @brief		iris unit test type リスト ファイル
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
#ifndef INCG_IRIS_iutest_typelist_HPP_0AC27A0F_3EFF_48ad_9075_E439B4190DA5_
#define INCG_IRIS_iutest_typelist_HPP_0AC27A0F_3EFF_48ad_9075_E439B4190DA5_

//======================================================================
// include

namespace iutest {
namespace detail
{

//======================================================================
// define
#if !defined(IUTEST_NO_TEMPLATE_TEMPLATES)
#  define IUTEST_TEMPLATE_TPARAM1	template<typename T> class
#endif

//======================================================================
// struct
/**
 * @brief	TypeList
 * @{
*/

// TypeList から 型を取得
template<typename TypeList, size_t N>
class typelist_get
{
	template<typename T, size_t I>
	struct impl
	{
		typedef typename impl<typename T::Tail, I-1>::type type;
	};
	template<typename T>
	struct impl<T, 0>
	{
		typedef typename T::Head type;
	};
public:
	typedef typename impl<TypeList, N>::type type;
};

// type list 終端
struct TypeList0 {};

template<typename T1>
struct TypeList1
{
	typedef T1	Head;
	typedef TypeList0	Tail;
};

#if IUTEST_HAS_VARIADIC_TEMPLATES

template<typename T, typename ...Args>
struct VariadicTypeList
{
	typedef T	Head;
	typedef VariadicTypeList<Args...>	Tail;
};
template<typename T>
struct VariadicTypeList<T>
{
	typedef T	Head;
	typedef TypeList0	Tail;
};

#else

#define IIUT_DECL_TYPELIST_(n)	IIUT_DECL_TYPELIST_I(n, IUTEST_PP_DEC(n))
#define IIUT_DECL_TYPELIST_I(n, m)													\
	template<typename T0, IUTEST_PP_ENUM_SHIFTED_PARAMS(m, typename T)>				\
	struct IUTEST_PP_CAT(TypeList, n) {	typedef T0 Head;							\
	typedef IUTEST_PP_CAT(TypeList, m)< IUTEST_PP_ENUM_SHIFTED_PARAMS(m, T) > Tail;	\
}

IIUT_DECL_TYPELIST_(2);
IIUT_DECL_TYPELIST_(3);
IIUT_DECL_TYPELIST_(4);
IIUT_DECL_TYPELIST_(5);
IIUT_DECL_TYPELIST_(6);
IIUT_DECL_TYPELIST_(7);
IIUT_DECL_TYPELIST_(8);
IIUT_DECL_TYPELIST_(9);
IIUT_DECL_TYPELIST_(10);
IIUT_DECL_TYPELIST_(11);
IIUT_DECL_TYPELIST_(12);
IIUT_DECL_TYPELIST_(13);
IIUT_DECL_TYPELIST_(14);
IIUT_DECL_TYPELIST_(15);
IIUT_DECL_TYPELIST_(16);
IIUT_DECL_TYPELIST_(17);
IIUT_DECL_TYPELIST_(18);
IIUT_DECL_TYPELIST_(19);
IIUT_DECL_TYPELIST_(20);
IIUT_DECL_TYPELIST_(21);
IIUT_DECL_TYPELIST_(22);
IIUT_DECL_TYPELIST_(23);
IIUT_DECL_TYPELIST_(24);
IIUT_DECL_TYPELIST_(25);
IIUT_DECL_TYPELIST_(26);
IIUT_DECL_TYPELIST_(27);
IIUT_DECL_TYPELIST_(28);
IIUT_DECL_TYPELIST_(29);
IIUT_DECL_TYPELIST_(30);
IIUT_DECL_TYPELIST_(31);
IIUT_DECL_TYPELIST_(32);
IIUT_DECL_TYPELIST_(33);
IIUT_DECL_TYPELIST_(34);
IIUT_DECL_TYPELIST_(35);
IIUT_DECL_TYPELIST_(36);
IIUT_DECL_TYPELIST_(37);
IIUT_DECL_TYPELIST_(38);
IIUT_DECL_TYPELIST_(39);
IIUT_DECL_TYPELIST_(40);
IIUT_DECL_TYPELIST_(41);
IIUT_DECL_TYPELIST_(42);
IIUT_DECL_TYPELIST_(43);
IIUT_DECL_TYPELIST_(44);
IIUT_DECL_TYPELIST_(45);
IIUT_DECL_TYPELIST_(46);
IIUT_DECL_TYPELIST_(47);
IIUT_DECL_TYPELIST_(48);
IIUT_DECL_TYPELIST_(49);
IIUT_DECL_TYPELIST_(50);

#undef IIUT_DECL_TYPELIST_

#endif

/**
 * @}
*/


#if !defined(IUTEST_NO_TEMPLATE_TEMPLATES)

/**
 * @brief	TemplateTypeListBind
*/
template<IUTEST_TEMPLATE_TPARAM1 U>
struct TemplateTypeSel
{
	template<typename T>
	struct bind
	{
		typedef U<T>	type;
	};
};

/**
 * @brief	TemplateTypeList
 * @{
*/

// template type list 終端
struct TemplateTypeList0 {};

#if IUTEST_HAS_VARIADIC_TEMPLATE_TEMPLATES

template<IUTEST_TEMPLATE_TPARAM1 T1, IUTEST_TEMPLATE_TPARAM1 ...Types>
struct VariadicTemplateTypeList
{
	typedef TemplateTypeSel<T1>	Head;
	typedef VariadicTemplateTypeList<Types...>	Tail;
};
template<IUTEST_TEMPLATE_TPARAM1 T1>
struct VariadicTemplateTypeList<T1>
{
	typedef TemplateTypeSel<T1>	Head;
	typedef TemplateTypeList0	Tail;
};

#else

template<IUTEST_TEMPLATE_TPARAM1 T1>
struct TemplateTypeList1
{
	typedef TemplateTypeSel<T1> Head;
	typedef TemplateTypeList0	Tail;
};

#define IIUT_DECL_TEMPLATETYPELIST_(n)	IIUT_DECL_TEMPLATETYPELIST_I(n, IUTEST_PP_DEC(n))
#define IIUT_DECL_TEMPLATETYPELIST_I(n, m)																\
	template< IUTEST_TEMPLATE_TPARAM1 T0, IUTEST_PP_ENUM_SHIFTED_PARAMS(m, IUTEST_TEMPLATE_TPARAM1 T) >	\
	struct IUTEST_PP_CAT(TemplateTypeList, n) { typedef TemplateTypeSel<T0> Head;						\
	typedef IUTEST_PP_CAT(TemplateTypeList, m)< IUTEST_PP_ENUM_SHIFTED_PARAMS(m, T) > Tail;				\
}

IIUT_DECL_TEMPLATETYPELIST_(2);
IIUT_DECL_TEMPLATETYPELIST_(3);
IIUT_DECL_TEMPLATETYPELIST_(4);
IIUT_DECL_TEMPLATETYPELIST_(5);
IIUT_DECL_TEMPLATETYPELIST_(6);
IIUT_DECL_TEMPLATETYPELIST_(7);
IIUT_DECL_TEMPLATETYPELIST_(8);
IIUT_DECL_TEMPLATETYPELIST_(9);
IIUT_DECL_TEMPLATETYPELIST_(10);
IIUT_DECL_TEMPLATETYPELIST_(11);
IIUT_DECL_TEMPLATETYPELIST_(12);
IIUT_DECL_TEMPLATETYPELIST_(13);
IIUT_DECL_TEMPLATETYPELIST_(14);
IIUT_DECL_TEMPLATETYPELIST_(15);
IIUT_DECL_TEMPLATETYPELIST_(16);
IIUT_DECL_TEMPLATETYPELIST_(17);
IIUT_DECL_TEMPLATETYPELIST_(18);
IIUT_DECL_TEMPLATETYPELIST_(19);
IIUT_DECL_TEMPLATETYPELIST_(20);
IIUT_DECL_TEMPLATETYPELIST_(21);
IIUT_DECL_TEMPLATETYPELIST_(22);
IIUT_DECL_TEMPLATETYPELIST_(23);
IIUT_DECL_TEMPLATETYPELIST_(24);
IIUT_DECL_TEMPLATETYPELIST_(25);
IIUT_DECL_TEMPLATETYPELIST_(26);
IIUT_DECL_TEMPLATETYPELIST_(27);
IIUT_DECL_TEMPLATETYPELIST_(28);
IIUT_DECL_TEMPLATETYPELIST_(29);
IIUT_DECL_TEMPLATETYPELIST_(30);
IIUT_DECL_TEMPLATETYPELIST_(31);
IIUT_DECL_TEMPLATETYPELIST_(32);
IIUT_DECL_TEMPLATETYPELIST_(33);
IIUT_DECL_TEMPLATETYPELIST_(34);
IIUT_DECL_TEMPLATETYPELIST_(35);
IIUT_DECL_TEMPLATETYPELIST_(36);
IIUT_DECL_TEMPLATETYPELIST_(37);
IIUT_DECL_TEMPLATETYPELIST_(38);
IIUT_DECL_TEMPLATETYPELIST_(39);
IIUT_DECL_TEMPLATETYPELIST_(40);
IIUT_DECL_TEMPLATETYPELIST_(41);
IIUT_DECL_TEMPLATETYPELIST_(42);
IIUT_DECL_TEMPLATETYPELIST_(43);
IIUT_DECL_TEMPLATETYPELIST_(44);
IIUT_DECL_TEMPLATETYPELIST_(45);
IIUT_DECL_TEMPLATETYPELIST_(46);
IIUT_DECL_TEMPLATETYPELIST_(47);
IIUT_DECL_TEMPLATETYPELIST_(48);
IIUT_DECL_TEMPLATETYPELIST_(49);
IIUT_DECL_TEMPLATETYPELIST_(50);

#undef IIUT_DECL_TEMPLATETYPELIST_

#endif

/**
 * @}
*/

#endif

}	// end of namespace detail

}	// end of namespace iutest

#endif
