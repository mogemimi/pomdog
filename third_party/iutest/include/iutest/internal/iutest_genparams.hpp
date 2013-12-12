//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_genparams.hpp
 * @brief		iris unit test parameter generator 定義 ファイル
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
#ifndef INCG_IRIS_IUTEST_GENPARAMS_HPP_7845F59A_825C_426A_B451_573245408998_
#define INCG_IRIS_IUTEST_GENPARAMS_HPP_7845F59A_825C_426A_B451_573245408998_

//======================================================================
// include
#include "iutest_random.hpp"

#if IUTEST_HAS_PARAM_TEST

namespace iutest {
namespace detail
{

//======================================================================
// declare
#if IUTEST_HAS_CONCAT
template<typename G1, typename G2>class iuConcatParamHolder;
#endif

//======================================================================
// class
/**
 * @brief	パラメータ生成器インターフェイス
*/
template<typename T>
class iuIParamGenerator
{
	typedef iuIParamGenerator<T>	_Myt;
public:
	typedef T	type;
public:
	typedef _Myt*	(*Generator)(void);
public:
	virtual	~iuIParamGenerator(void) {}
public:
	virtual	void	Begin(void) = 0;	//!< パラメータリストの先頭に移動
	virtual T		GetCurrent(void) const = 0;	// 現在のパラメータを取得
	virtual void	Next(void)	= 0;	//!< パラメータを取得して次に移動
	virtual bool	IsEnd(void) const = 0;	//!< パラメータリストの終端にいるかどうか
};

/**
 * @brief	パラメータ生成器保持クラス
*/
template<typename T>
class iuParamGenerator : public iuIParamGenerator<T>
{
	typedef iuIParamGenerator<T>	_Interface;
	typedef iuParamGenerator<T>		_Myt;
public:
	typedef T	type;
public:
	iuParamGenerator(_Interface* pInterface=NULL) : m_pInterface(pInterface) {}

public:
	operator iuIParamGenerator<T>* (void) const { return m_pInterface; }

public:
	template<typename Other>
	iuConcatParamHolder<_Myt, Other> operator + (const Other& g) const
	{
		return iuConcatParamHolder<_Myt, Other>(*this, g);
	}

public:
	virtual	void	Begin(void)	{ m_pInterface->Begin(); }	//!< パラメータリストの先頭に移動
	virtual T		GetCurrent(void) const { return m_pInterface->GetCurrent(); }	// 現在のパラメータを取得
	virtual void	Next(void)	{ m_pInterface->Next(); }	//!< パラメータを取得して次に移動
	virtual bool	IsEnd(void) const { return m_pInterface->IsEnd(); }	//!< パラメータリストの終端にいるかどうか
private:
	_Interface*		m_pInterface;
};

/**
 * @brief	範囲パラメータ生成器
 * @tparam T	= パラメータ型
*/
template<typename T>
class iuRangeParamsGenerator : public iuIParamGenerator<T>
{
	T	m_begin;
	T	m_end;
	T	m_step;
	T	m_cur;
public:
	/**
	 * @brief	コンストラクタ
	 * @param [in]	begin	= 開始値
	 * @param [in]	end		= 終了値
	 * @param [in]	step	= 増値
	*/
	iuRangeParamsGenerator(T begin, T end, T step=1)
		: m_begin(begin)
		, m_end(end)
		, m_step(step)
		, m_cur(begin)
	{
	}

public:
	virtual	void	Begin(void) { m_cur = m_begin; }
	virtual T		GetCurrent(void) const	{ return m_cur; }
	virtual void	Next(void)	{ m_cur += m_step; }
	virtual bool	IsEnd(void) const	{ return !( m_cur < m_end ); }
};

/**
 * @breif	真偽値パラメータ生成器
 * @tparam T	= パラメータ型
*/
template<typename T>
class iuBoolParamsGenerator : public iuIParamGenerator<T>
{
	T	m_cur;
	int m_n;
public:
	iuBoolParamsGenerator(void)
		: m_n(0)
	{}

public:
	virtual	void	Begin(void) { m_cur = 0; m_n = 0; }
	virtual T		GetCurrent(void) const	{ return m_cur; }
	virtual void	Next(void)	{ ++m_n; m_cur = !m_cur; }
	virtual bool	IsEnd(void) const	{ return (m_n >= 2); }
};

/**
 * @brief	値配列パラメータ生成器
 * @tparam T	= パラメータ型
*/
template<typename T>
class iuValuesInParamsGenerator : public iuIParamGenerator<T>
{
	typedef ::std::vector<T>	params_t;
	params_t	m_values;
	typename params_t::const_iterator m_it;
public:
	iuValuesInParamsGenerator(const params_t& values)
		: m_values(values) {}
	template<typename Container>
	iuValuesInParamsGenerator(const Container& values)
	{
		for( typename Container::const_iterator it = values.begin(), end=values.end(); it != end; ++it )
		{
			m_values.push_back(static_cast<T>(*it));
		}
	}
#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
	template<typename TT, size_t SIZE>
	iuValuesInParamsGenerator(const TT (&values)[SIZE])
	{
		const TT* begin = values;
		for( const TT* end = values+SIZE; begin != end; ++begin )
		{
			m_values.push_back(static_cast<T>(*begin));
		}
	}
#endif

	template<typename Ite>
	iuValuesInParamsGenerator(Ite begin, Ite end)
	{
		for( ; begin != end; ++begin )
		{
			m_values.push_back(static_cast<T>(*begin));
		}
	}

#if IUTEST_HAS_INITIALIZER_LIST
	iuValuesInParamsGenerator(::std::initializer_list<T> l)
	{
		m_values.insert(m_values.end(), l.begin(), l.end());
	}
#endif

public:
	virtual	void	Begin(void) { m_it = m_values.begin(); }
	virtual T		GetCurrent(void) const	{ return *m_it; }
	virtual void	Next(void)	{ ++m_it; }
	virtual bool	IsEnd(void) const	{ return (m_it == m_values.end()); }
};


#if IUTEST_HAS_CONCAT
/**
 * @brief	パラメータ生成器加算保持クラス
*/
template<typename G1, typename G2>
class iuConcatParamHolder
{
	typedef iuConcatParamHolder<G1, G2>	_Myt;
public:
	iuConcatParamHolder(const G1& g1, const G2& g2)
		: m_g1(g1), m_g2(g2) {}

public:
	template<typename T>
	operator iuIParamGenerator<T>* (void)
	{
		params_t<T> params;
		params.append(m_g1);
		params.append(m_g2);
		return new iuValuesInParamsGenerator<T>(params.val);
	}

	template<typename Other>
	iuConcatParamHolder<_Myt, Other> operator + (const Other& g) const
	{
		return iuConcatParamHolder<_Myt, Other>(*this, g);
	}

private:
	template<typename T>
	struct params_t
	{
		::std::vector<T> val;

		void append(iuIParamGenerator<T>* gen)
		{
			for( gen->Begin(); !gen->IsEnd(); gen->Next() )
			{
				val.push_back(gen->GetCurrent());
			}
		}
		template<typename U>
		void append(iuParamGenerator<U>& gen)
		{
			for( gen.Begin(); !gen.IsEnd(); gen.Next() )
			{
				val.push_back(static_cast<T>(gen.GetCurrent()));
			}
		}
	};
private:
	G1 m_g1;
	G2 m_g2;
};
#endif


#if IUTEST_HAS_VARIADIC_VALUES
template<typename... Args>
class iuValueArray
{
	typedef tuples::tuple<Args...>	_MyTuple;
	typedef iuValueArray<Args...>	_Myt;

	template<typename T>
	struct make_array
	{
		T val[tuples::tuple_size<_MyTuple>::value];
		make_array(const _MyTuple& t)
		{
			make<tuples::tuple_size<_MyTuple>::value, 0>(t);
		};

		template<int N, int I>
		void	make(const _MyTuple& t, typename detail::enable_if<(I < N), void>::type*& = detail::enabler::value )
		{
			val[I] = tuples::get<I>(t);
			make<N, I+1>(t);
		}
		template<int N, int I>
		void	make(const _MyTuple& t, typename detail::disable_if<(I < N), void>::type*& = detail::enabler::value )
		{
			IUTEST_UNUSED_VAR(t);
		}
	};
public:
	iuValueArray(const Args&... args)
		: v(args...)
	{}
public:
	template<typename T>
	operator iuIParamGenerator<T>* (void) const
	{
		make_array<T> ar(v);
		return new iuValuesInParamsGenerator<T>(ar.val);
	}

public:
	template<typename Other>
	iuConcatParamHolder<_Myt, Other> operator + (const Other& g) const
	{
		return iuConcatParamHolder<_Myt, Other>(*this, g);
	}

private:
	_MyTuple v;
};

#else

/*
template<typename A1, typename A2>
class iuValueArray2
{
	typedef iuValueArray2<A1, A2> _Myt;
public:
	iuValueArray2(A1 a1, A2 a2) : v1(a1), v2(a2)
	{}
public:
	template<typename T>
	operator	iuIParamGenerator<T>* (void) const
	{
		const T val[] = { static_cast<T>(v1), static_cast<T>(v2)
		};
		return new iuValuesInParamsGenerator<T>(val);
	}
public:
	template<typename Other>
	iuConcatParamHolder<_Myt, Other> operator + (const Other& g) const
	{
		return iuConcatParamHolder<_Myt, Other>(*this, g);
	}
private:
	A1	v1;	A2	v2;
};
*/
#define IIUT_DECL_VALUEARRAY_CONSTRUCT_(i, p1, p2)		IUTEST_PP_CAT(p1, i)(IUTEST_PP_CAT(p2, i))
#define IIUT_DECL_VALUEARRAY_STATICCAST_(i, p1, p2)		static_cast<p1>(IUTEST_PP_CAT(p2, i))
#define IIUT_DECL_VALUEARRAY_VARIABLE_(i, p1, p2)		IUTEST_PP_CAT(p1, i) IUTEST_PP_CAT(p2, i);
#define IIUT_DECL_VALUEARRAY_(n)						\
	template< IUTEST_PP_ENUM_PARAMS(n, typename A) >	\
	class IUTEST_PP_CAT(iuValueArray, n) {				\
		typedef IUTEST_PP_CAT(iuValueArray, n)< IUTEST_PP_ENUM_PARAMS(n, A) > _Myt;	\
	public:																			\
		IUTEST_PP_CAT(iuValueArray, n)( IUTEST_PP_ENUM_BINARY_PARAMS(n, A, a) )		\
		: IUTEST_PP_ENUM_BINARY(n, IIUT_DECL_VALUEARRAY_CONSTRUCT_, v, a) {}		\
		template<typename T>operator iuIParamGenerator<T>* (void) const {			\
			const T val[] = { IUTEST_PP_ENUM_BINARY(n, IIUT_DECL_VALUEARRAY_STATICCAST_, T, v) };		\
			return new iuValuesInParamsGenerator<T>(val);							\
		}																			\
		template<typename Other> iuConcatParamHolder<_Myt, Other> operator + (const Other& g) const {	\
			return iuConcatParamHolder<_Myt, Other>(*this, g);						\
		}																			\
	private: IUTEST_PP_REPEAT_BINARY(n, IIUT_DECL_VALUEARRAY_VARIABLE_, A, v)		\
	}

IIUT_DECL_VALUEARRAY_(1);
IIUT_DECL_VALUEARRAY_(2);
IIUT_DECL_VALUEARRAY_(3);
IIUT_DECL_VALUEARRAY_(4);
IIUT_DECL_VALUEARRAY_(5);
IIUT_DECL_VALUEARRAY_(6);
IIUT_DECL_VALUEARRAY_(7);
IIUT_DECL_VALUEARRAY_(8);
IIUT_DECL_VALUEARRAY_(9);
IIUT_DECL_VALUEARRAY_(10);
IIUT_DECL_VALUEARRAY_(11);
IIUT_DECL_VALUEARRAY_(12);
IIUT_DECL_VALUEARRAY_(13);
IIUT_DECL_VALUEARRAY_(14);
IIUT_DECL_VALUEARRAY_(15);
IIUT_DECL_VALUEARRAY_(16);
IIUT_DECL_VALUEARRAY_(17);
IIUT_DECL_VALUEARRAY_(18);
IIUT_DECL_VALUEARRAY_(19);
IIUT_DECL_VALUEARRAY_(20);
IIUT_DECL_VALUEARRAY_(21);
IIUT_DECL_VALUEARRAY_(22);
IIUT_DECL_VALUEARRAY_(23);
IIUT_DECL_VALUEARRAY_(24);
IIUT_DECL_VALUEARRAY_(25);
IIUT_DECL_VALUEARRAY_(26);
IIUT_DECL_VALUEARRAY_(27);
IIUT_DECL_VALUEARRAY_(28);
IIUT_DECL_VALUEARRAY_(29);
IIUT_DECL_VALUEARRAY_(30);
IIUT_DECL_VALUEARRAY_(31);
IIUT_DECL_VALUEARRAY_(32);
IIUT_DECL_VALUEARRAY_(33);
IIUT_DECL_VALUEARRAY_(34);
IIUT_DECL_VALUEARRAY_(35);
IIUT_DECL_VALUEARRAY_(36);
IIUT_DECL_VALUEARRAY_(37);
IIUT_DECL_VALUEARRAY_(38);
IIUT_DECL_VALUEARRAY_(39);
IIUT_DECL_VALUEARRAY_(40);
IIUT_DECL_VALUEARRAY_(41);
IIUT_DECL_VALUEARRAY_(42);
IIUT_DECL_VALUEARRAY_(43);
IIUT_DECL_VALUEARRAY_(44);
IIUT_DECL_VALUEARRAY_(45);
IIUT_DECL_VALUEARRAY_(46);
IIUT_DECL_VALUEARRAY_(47);
IIUT_DECL_VALUEARRAY_(48);
IIUT_DECL_VALUEARRAY_(49);
IIUT_DECL_VALUEARRAY_(50);

#undef IIUT_DECL_VALUEARRAY_CONSTRUCT_
#undef IIUT_DECL_VALUEARRAY_STATICCAST_
#undef IIUT_DECL_VALUEARRAY_VARIABLE_
#undef IIUT_DECL_VALUEARRAY_

#endif

#if IUTEST_HAS_COMBINE

#if IUTEST_HAS_VARIADIC_COMBINE

template<typename... Args>
class iuCartesianProductGenerator : public iuIParamGenerator< tuples::tuple<Args...> >
{
	typedef tuples::tuple< iuParamGenerator<Args>... > _MyTuple;
	static const int count = tuples::tuple_size<_MyTuple>::value;

	template<int index, int end, typename Tuple>
	void begin_foreach(Tuple& t, typename detail::enable_if<index != end, void>::type*& = detail::enabler::value )
	{
		tuples::get<index>(t).Begin();
		begin_foreach<index+1, end>(t);
	}
	template<int index, int end, typename Tuple>
	void begin_foreach(Tuple& , typename detail::enable_if<index == end, void>::type*& = detail::enabler::value )
	{
	}

	template<int index, int end, typename Tuple>
	bool is_end_foreach(Tuple& t, typename detail::enable_if<index != end, void>::type*& = detail::enabler::value ) const
	{
		bool b = tuples::get<index>(t).IsEnd();
		return b && is_end_foreach<index+1, end>(t);
	}
	template<int index, int end, typename Tuple>
	bool is_end_foreach(Tuple& , typename detail::enable_if<index == end, void>::type*& = detail::enabler::value ) const
	{
		return true;
	}

	template<int index, int end, typename Tuple>
	void next_foreach(Tuple& t, typename detail::enable_if<index != end, void>::type*& = detail::enabler::value )
	{
		next_foreach<index+1, end>(t);
		if( is_end_foreach<index+1, end>(t) )
		{
			tuples::get<index>(t).Next();
			if( !tuples::get<index>(t).IsEnd() ) begin_foreach<index+1, end>(t);
		}
	}
	template<int index, int end, typename Tuple>
	void next_foreach(Tuple& , typename detail::enable_if<index == end, void>::type*& = detail::enabler::value )
	{
	}

	template<int index, int end, typename T1, typename ...TArgs>
	tuples::tuple<T1, TArgs...> current_foreach(typename detail::enable_if<index != end-1, void>::type*& = detail::enabler::value ) const
	{
		return ::std::tuple_cat( tuples::tuple<T1>(tuples::get<index>(v).GetCurrent())
			, current_foreach<index+1, end, TArgs...>());
	}
	template<int index, int end, typename T1, typename ...TArgs>
	tuples::tuple<T1>  current_foreach(typename detail::enable_if<index == end-1, void>::type*& = detail::enabler::value ) const
	{
		return tuples::tuple<T1>(tuples::get<index>(v).GetCurrent());
	}

public:
	typedef tuples::tuple<Args...>	ParamType;
public:
	iuCartesianProductGenerator(void)
	{}

public:
	virtual	void	Begin(void)
	{
		begin_foreach<0, count>(v);
	}
	virtual void	Next(void)
	{
		if( IsEnd() ) return;
		next_foreach<0, count>(v);
	}
	virtual bool	IsEnd(void) const
	{
		return is_end_foreach<0, count>(v);
	}
	virtual ParamType	GetCurrent(void) const
	{
		return current_foreach<0, count, Args...>();
	}

	_MyTuple&	generators(void) { return v; }
private:
	_MyTuple v;
};

template<typename... Generator>
class iuCartesianProductHolder
{
	typedef iuCartesianProductHolder<Generator...> _Myt;

	typedef tuples::tuple<const Generator...>	_MyTuple;

	template<int index, int end, typename ArgTuple, typename SrcTuple, typename DstTuple>
	void set_foreach(SrcTuple& src, DstTuple& dst, typename detail::enable_if<index != end, void>::type*& = detail::enabler::value ) const
	{
		tuples::get<index>(dst) = static_cast< typename tuples::tuple_element<index, DstTuple>::type >(tuples::get<index>(src));
		set_foreach<index+1, end, ArgTuple>(src, dst);
	}
	template<int index, int end, typename ArgTuple, typename SrcTuple, typename DstTuple>
	void set_foreach(SrcTuple& , DstTuple& , typename detail::enable_if<index == end, void>::type*& = detail::enabler::value ) const
	{
	}

public:
	iuCartesianProductHolder(const Generator&... generators)
		: v(generators...) {}

public:
	template<typename... Args>
	operator iuIParamGenerator< tuples::tuple<Args...> >* (void) const 
	{
		typedef tuples::tuple<Args...> ArgTuple;
		iuCartesianProductGenerator<Args...>* p = new iuCartesianProductGenerator<Args...>();
		set_foreach<0, tuples::tuple_size<ArgTuple>::value, ArgTuple>(v, p->generators());
		return p;
	}

public:
	template<typename Other>
	iuConcatParamHolder<_Myt, Other> operator + (const Other& g) const
	{
		return iuConcatParamHolder<_Myt, Other>(*this, g);
	}

private:
	void	operator = (const _Myt&);
private:
	_MyTuple v;
};


#else

template<typename Generator1, typename Generator2, typename ParamType>
class iuICartesianProductGeneratorBase : public iuIParamGenerator< ParamType >
{
public:
	iuICartesianProductGeneratorBase(const Generator1& g1, const Generator2& g2)
		: m_g1(g1), m_g2(g2)
	{}
public:
	virtual	void	Begin(void)
	{
		m_g1.Begin();
		m_g2.Begin();
	}
	virtual void	Next(void)
	{
		if( m_g2.IsEnd() ) return;
		m_g2.Next();
		if( m_g2.IsEnd() )
		{
			m_g1.Next();
			if( !m_g1.IsEnd() ) m_g2.Begin();
		}
	}
	virtual bool	IsEnd(void) const
	{
		return m_g1.IsEnd() && m_g2.IsEnd();
	}

protected:
	Generator1	m_g1;
	Generator2	m_g2;
};

template<typename T1, typename T2>
class iuCartesianProductGenerator2 : public iuICartesianProductGeneratorBase<iuParamGenerator<T1>, iuParamGenerator<T2>, tuples::tuple<T1, T2> >
{
	typedef iuICartesianProductGeneratorBase<iuParamGenerator<T1>, iuParamGenerator<T2>, tuples::tuple<T1, T2> > _Mybase;
	typedef iuParamGenerator<T1> Generator1;
	typedef iuParamGenerator<T2> Generator2;
public:
	typedef tuples::tuple<T1, T2>	ParamType;

public:
	iuCartesianProductGenerator2(const Generator1 &g1, const Generator2 &g2)
		: _Mybase(g1, g2)
	{}

public:
	virtual ParamType	GetCurrent(void) const
	{
		return ParamType(this->m_g1.GetCurrent(), this->m_g2.GetCurrent());
	}
};

/*
template<typename T1, typename T2, typename T3>
class iuCartesianProductGenerator3 : public iuICartesianProductGeneratorBase<iuParamGenerator<T1>
																			, iuCartesianProductGenerator2<T2, T3>
																			, tuples::tuple<T1, T2, T3> >
{
	typedef iuICartesianProductGeneratorBase<iuParamGenerator<T1>, iuCartesianProductGenerator2<T2, T3>, tuples::tuple<T1, T2, T3> >	_Mybase;
	typedef iuParamGenerator<T1> Generator1;
	typedef iuParamGenerator<T2> Generator2;
	typedef iuParamGenerator<T3> Generator3;
public:
	typedef tuples::tuple<T1, T2, T3>	ParamType;
public:
	iuCartesianProductGenerator3(const Generator1& g1, const Generator2& g2, const Generator3& g3)
		: _Mybase(g1, iuCartesianProductGenerator2<T2, T3>(g2, g3))
	{}

public:
	virtual ParamType	GetCurrent(void) const
	{
		tuples::tuple<T2, T3> param(this->m_g2.GetCurrent());
		return ParamType(this->m_g1.GetCurrent(), tuples::get<0>(param), tuples::get<1>(param) );
	}
};
*/

#define IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_TYPEDEF_(i, p1, p2)		\
	typedef iuParamGenerator<IUTEST_PP_CAT(p1, i)> IUTEST_PP_CAT(p2, i);

#define IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_TUPLEGET_(i, param)		\
	tuples::get<i>(param)

#define IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_BASE_(n)					\
	iuICartesianProductGeneratorBase< iuParamGenerator<T0>				\
		, IUTEST_PP_CAT(iuCartesianProductGenerator, IUTEST_PP_DEC(n))< \
			IUTEST_PP_ENUM_SHIFTED_PARAMS(IUTEST_PP_DEC(n), T) >		\
		, tuples::tuple< IUTEST_PP_ENUM_PARAMS(n, T) > >

#define IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_(n)							\
	template< IUTEST_PP_ENUM_PARAMS(n, typename T) >						\
	class IUTEST_PP_CAT(iuCartesianProductGenerator, n)						\
	: public IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_BASE_(n) {				\
		typedef IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_BASE_(n) _Mybase;		\
		IUTEST_PP_REPEAT_BINARY(n, IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_TYPEDEF_, T, Generator)			\
	public:																	\
		typedef tuples::tuple< IUTEST_PP_ENUM_PARAMS(n, T) > ParamType;		\
		IUTEST_PP_CAT(iuCartesianProductGenerator, n)( IUTEST_PP_ENUM_BINARY_PARAMS(n, const Generator, &g) ) \
		: _Mybase(g0, IUTEST_PP_CAT(iuCartesianProductGenerator, IUTEST_PP_DEC(n))<							\
		IUTEST_PP_ENUM_SHIFTED_PARAMS(IUTEST_PP_DEC(n), T)>					\
		( IUTEST_PP_ENUM_SHIFTED_PARAMS(IUTEST_PP_DEC(n), g) ) ) {}			\
		virtual ParamType	GetCurrent(void) const {						\
			tuples::tuple< IUTEST_PP_ENUM_SHIFTED_PARAMS(IUTEST_PP_DEC(n), T) >	\
			params(this->m_g2.GetCurrent());								\
			return ParamType(this->m_g1.GetCurrent(), IUTEST_PP_ENUM(IUTEST_PP_DEC(n)	\
				, IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_TUPLEGET_, params) );			\
		}																	\
	}

IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_(3);
IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_(4);
IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_(5);
IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_(6);
IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_(7);
IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_(8);
IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_(9);

#undef IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_TYPEDEF_
#undef IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_TUPLEGET_
#undef IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_BASE_
#undef IIUT_DECL_CARTESIAN_PRODUCT_GENERATOR_

// iuCartesianProductHolder

/*
template<typename Generator1, typename Generator2>
class iuCartesianProductHolder2
{
	typedef iuCartesianProductHolder2<Generator1, Generator2> _Myt;
public:
	iuCartesianProductHolder2(const Generator1& g1, const Generator2& g2)
		: m_g1(g1), m_g2(g2) {}

public:
	template<typename T1, typename T2>
	operator iuIParamGenerator< tuples::tuple<T1, T2> >* () const 
	{
		return new iuCartesianProductGenerator2<T1, T2>(
			static_cast< iuIParamGenerator<T1>* >(m_g1)
			, static_cast< iuIParamGenerator<T2>* >(m_g2)
		);
	}

public:
	template<typename Other>
	iuConcatParamHolder<_Myt, Other> operator + (const Other& g) const
	{
		return iuConcatParamHolder<_Myt, Other>(*this, g);
	}

private:
	void	operator = (const _Myt&) {}
private:
	const Generator1	m_g1;
	const Generator2	m_g2;
};
*/

#define IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_CONSTRUCT_(i, p1, p2)	IUTEST_PP_CAT(p1, i)(IUTEST_PP_CAT(p2, i))
#define IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_STATICCAST_(i, p1, p2)	\
	static_cast< iuIParamGenerator< IUTEST_PP_CAT(p1, i) >* >(IUTEST_PP_CAT(p2, i))
#define IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_VARIABLE_(i, p1, p2)	IUTEST_PP_CAT(p1, i) IUTEST_PP_CAT(p2, i);

#define IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_(n)					\
	template< IUTEST_PP_ENUM_PARAMS(n, typename Generator) >	\
	class IUTEST_PP_CAT(iuCartesianProductHolder, n) {			\
		typedef IUTEST_PP_CAT(iuCartesianProductHolder, n)< IUTEST_PP_ENUM_PARAMS(n, Generator) > _Myt;		\
	public:														\
		IUTEST_PP_CAT(iuCartesianProductHolder, n)( IUTEST_PP_ENUM_BINARY_PARAMS(n, const Generator, &g) )	\
		: IUTEST_PP_ENUM_BINARY(n, IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_CONSTRUCT_, m_g, g) {}				\
		template< IUTEST_PP_ENUM_PARAMS(n, typename T) >		\
		operator iuIParamGenerator< tuples::tuple< IUTEST_PP_ENUM_PARAMS(n, T) > >* (void) const {			\
			return new IUTEST_PP_CAT(iuCartesianProductGenerator, n)< IUTEST_PP_ENUM_PARAMS(n, T) >(		\
				IUTEST_PP_ENUM_BINARY(n, IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_STATICCAST_, T, m_g) );			\
		}														\
		template<typename Other> iuConcatParamHolder<_Myt, Other> operator + (const Other& g) const {		\
			return iuConcatParamHolder<_Myt, Other>(*this, g);	\
		}														\
	private: void operator = (const _Myt&) {}					\
		IUTEST_PP_REPEAT_BINARY(n, IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_VARIABLE_, const Generator, m_g)		\
	}

IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_(2);
IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_(3);
IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_(4);
IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_(5);
IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_(6);
IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_(7);
IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_(8);
IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_(9);

#undef IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_CONSTRUCT_
#undef IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_STATICCAST_
#undef IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_VARIABLE_
#undef IIUT_DECL_CARTESIAN_PRODUCT_HOLDER_

#endif

#endif

#if IUTEST_HAS_PAIRWISE

class iuPairwiseGeneratorBase
{
protected:
	template<int N>
	struct ParamIndexes
	{ 
		int index[N];
		ParamIndexes(void) { for( int i=0; i < N; ++i ) index[i] = -1; }
	};

private:
	struct PairInfo {
		int raw1, raw2;	// 列のペア
		int idx1, idx2;	// インデックスのペア
	};
protected:
	template<typename T1>
	static void MakeParamVector( ::std::vector<T1>& list, iuParamGenerator<T1>& g1)
	{
		for( g1.Begin(); !g1.IsEnd(); g1.Next() )
		{
			list.push_back(g1.GetCurrent());
		}
	}

	template<typename T1, typename T2>
	static void MakePairList( ::std::vector< std::pair<T1, T2> >& list, iuParamGenerator<T1>& g1, iuParamGenerator<T2>& g2)
	{
		for( g1.Begin(); !g1.IsEnd(); g1.Next() )
		{
			T1 t1 = g1.GetCurrent();
			for( g2.Begin(); !g2.IsEnd(); g2.Next() )
			{
				list.push_back(std::pair<T1, T2>(t1, g2.GetCurrent()));
			}
		}
	}

	template<int N>
	static void MakeIndexList( ::std::vector< ParamIndexes<N> >& list, int* count_list)
	{
		typedef typename ::std::vector< ParamIndexes<N> >::iterator list_iterator;
		list.clear();

		// ペアを列挙
		::std::vector<PairInfo> pair_list;
		for( int i=0; i < N; ++i )
		{
			int l = count_list[i];
			for( int j=i+1; j < N; ++j )
			{
				int r = count_list[j];
				for( int li=0; li < l; ++li )
				{
					for( int ri=0; ri < r; ++ri )
					{
						PairInfo info = { i, j, li, ri };
						pair_list.push_back(info);
					}
				}
			}
		}

		// シャッフル
		iuRandom random;
		unsigned int seed = TestEnv::get_random_seed();
		if( seed != 0 )
		{
			random.init(seed);
		}
		std::random_shuffle(pair_list.begin(), pair_list.end(), random);

		for( ::std::vector<PairInfo>::const_iterator it=pair_list.begin(); it != pair_list.end(); ++it )
		{
			const PairInfo& pair_info = *it;
			list_iterator find = Find(list, pair_info, list.begin());
			if( find == list.end() )
			{
				find = FindFree(list, pair_info, list.begin());
				if( find == list.end() )
				{
					// 空きが無いので作る
					ParamIndexes<N> params;
					params.index[pair_info.raw1] = pair_info.idx1;
					params.index[pair_info.raw2] = pair_info.idx2;
					list.push_back(params);
				}
				else
				{
					// 埋める
					ParamIndexes<N>& params = *find;
					params.index[pair_info.raw1] = pair_info.idx1;
					params.index[pair_info.raw2] = pair_info.idx2;
				}
			}
		}

		//for( list_iterator it=list.begin(), end=list.end(); it != end; ++it )
		//{
		//	for( int i=0; i < N; ++i ) printf("%2d ", it->index[i]);
		//	printf("\n");
		//}
	}

	template<int N, typename Fn>
	static int GetParamIndex(const ParamIndexes<N>& indexes, int raw, size_t count, Fn& func)
	{
		return indexes.index[raw] == -1 ? func(count)
			: indexes.index[raw];
	}

	template<int N, typename T>
	static T GetParam(const ::std::vector<T>& params, const ParamIndexes<N>& indexes, int raw)
	{
		const int index = GetParamIndex(indexes, raw, params.size(), TestEnv::genrand());
		return params[index];
	}

private:
	template<int N>
	static typename ::std::vector< ParamIndexes<N> >::iterator Find( ::std::vector< ParamIndexes<N> >& list, const PairInfo& pair_info
		, typename ::std::vector< ParamIndexes<N> >::iterator start)
	{
		typedef typename ::std::vector< ParamIndexes<N> >::iterator iterator;
		for( iterator it = start, end=list.end(); it != end; ++it )
		{
			ParamIndexes<N>& indexes = *it;
			if( indexes.index[pair_info.raw1] == pair_info.idx1 
				&& indexes.index[pair_info.raw2] == pair_info.idx2 )
			{
				return it;
			}
		}
		return list.end();
	}

	template<int N>
	static typename ::std::vector< ParamIndexes<N> >::iterator FindFree( ::std::vector< ParamIndexes<N> >& list, const PairInfo& pair_info
		, typename ::std::vector< ParamIndexes<N> >::iterator start)
	{
		// 入れそうなとこを探す
		typedef typename ::std::vector< ParamIndexes<N> >::iterator iterator;
		iterator find = list.end();
		UInt32 max_overlap = static_cast<UInt32>(-1);
		for( iterator it = start, end=list.end(); it != end; ++it )
		{
			ParamIndexes<N>& indexes = *it;
			int free_raw = -1;
			int free_idx = -1;
			if( indexes.index[pair_info.raw1] == -1 && indexes.index[pair_info.raw2] == pair_info.idx2 )
			{
				free_raw = pair_info.raw1;
				free_idx = pair_info.idx1;
			}
			if( indexes.index[pair_info.raw2] == -1 && indexes.index[pair_info.raw1] == pair_info.idx1 )
			{
				free_raw = pair_info.raw2;
				free_idx = pair_info.idx2;
			}
			if( free_raw != -1 )
			{
#if 0
				IUTEST_UNUSED_VAR(free_idx);
				IUTEST_UNUSED_VAR(max_overlap);
				return it;
#else
				// 仮に入ったとして重複がないか調べる
				UInt32 overlap = 0;
				for( int i=0; i < N; ++i )
				{
					if( indexes.index[i] == -1 || i == free_raw )
					{
						continue;
					}
					PairInfo tmp = { i, free_raw, indexes.index[i], free_idx };
					iterator it2 = Find(list, tmp, list.begin());
					while(it2 != end)
					{
						++overlap;
						++it2;
						it2 = Find(list, tmp, it2);
					}
				}
				if( overlap == 0 )
				{
					return it;
				}
				if( find == list.end()
					|| (overlap < max_overlap) )
				{
					find = it;
					max_overlap = overlap;
				}
#endif
			}
		}
		if( find != list.end() )
		{
			return find;
		}

		typedef typename ::std::vector< ParamIndexes<N> >::iterator iterator;
		for( iterator it = start, end=list.end(); it != end; ++it )
		{
			ParamIndexes<N>& indexes = *it;
			if( indexes.index[pair_info.raw1] == -1 && indexes.index[pair_info.raw2] == -1 )
			{
				return it;
			}
		}
		return list.end();
	}
};

#if IUTEST_HAS_VARIADIC_PAIRWISE

template<typename... Args>
class iuPairwiseGenerator : public iuPairwiseGeneratorBase
{
	typedef tuples::tuple< Args... > ParamType;
	typedef tuples::tuple< iuParamGenerator<Args>... > GeneratorTuple;
	static const int RAW_COUNT = tuples::tuple_size<ParamType>::value;

	typedef ParamIndexes<RAW_COUNT>		_MyParamIndexes;
	typedef ::std::vector< _MyParamIndexes >	ParamIndexesList;

	typedef tuples::tuple< ::std::vector<Args>... > ParamsTuple;

public:
	static iuIParamGenerator< ParamType >* Create(GeneratorTuple& generators)
	{
		ParamIndexesList list;
		ParamsTuple params_list;

		MakeParamVecotrs<0>(params_list, generators);

		int count_list[RAW_COUNT] = { 0 };
		GetCountList<0>(params_list, count_list);

		MakeIndexList(list, count_list);

		::std::vector<ParamType> params;

		for( typename ParamIndexesList::const_iterator it=list.begin(), end=list.end(); it != end; ++it )
		{
			const _MyParamIndexes& indexes = *it;
			params.push_back( MakeParam<0, Args...>(params_list, indexes) );
		}

		return new iuValuesInParamsGenerator< ParamType >(params);
	}
private:
	template<int N, typename T1, typename... TArgs>
	static tuples::tuple<T1, TArgs...> MakeParam(ParamsTuple& list, const _MyParamIndexes& indexes, typename detail::disable_if<N == RAW_COUNT-1, void>::type*& = detail::enabler::value)
	{
		return ::std::tuple_cat( tuples::tuple<T1>(GetParam(tuples::get<N>(list), indexes, N))
			, MakeParam<N+1, TArgs...>(list, indexes) );
	}
	template<int N, typename T1, typename... TArgs>
	static tuples::tuple<T1> MakeParam(ParamsTuple& list, const _MyParamIndexes& indexes, typename detail::enable_if<N == RAW_COUNT-1, void>::type*& = detail::enabler::value)
	{
		return tuples::tuple<T1>( GetParam( tuples::get<N>(list), indexes, N) );
	}

	template<int N>
	static void MakeParamVecotrs(ParamsTuple& list, GeneratorTuple& generators, typename detail::disable_if<N == RAW_COUNT-1, void>::type*& = detail::enabler::value)
	{
		MakeParamVector(tuples::get<N>(list), tuples::get<N>(generators));
		MakeParamVecotrs<N+1>(list, generators);
	}
	template<int N>
	static void MakeParamVecotrs(ParamsTuple& list, GeneratorTuple& generators, typename detail::enable_if<N == RAW_COUNT-1, void>::type*& = detail::enabler::value)
	{
		MakeParamVector(tuples::get<N>(list), tuples::get<N>(generators));
	}

	template<int N>
	static void GetCountList(ParamsTuple& list, int* count_list, typename detail::disable_if<N == RAW_COUNT-1, void>::type*& = detail::enabler::value)
	{
		count_list[N] = static_cast<int>(tuples::get<N>(list).size());
		GetCountList<N+1>(list, count_list);
	}
	template<int N>
	static void GetCountList(ParamsTuple& list, int* count_list, typename detail::enable_if<N == RAW_COUNT-1, void>::type*& = detail::enabler::value)
	{
		count_list[N] = static_cast<int>(tuples::get<N>(list).size());
	}
};

template<typename... Generator>
class iuPairwiseHolder
{
	typedef iuPairwiseHolder<Generator...> _Myt;

	typedef tuples::tuple<const Generator...>	_MyTuple;

	template<int index, int end, typename ArgTuple, typename SrcTuple, typename DstTuple>
	void set_foreach(SrcTuple& src, DstTuple& dst, typename detail::enable_if<index != end, void>::type*& = detail::enabler::value ) const
	{
		tuples::get<index>(dst) = static_cast< typename tuples::tuple_element<index, DstTuple>::type >(tuples::get<index>(src));
		set_foreach<index+1, end, ArgTuple>(src, dst);
	}
	template<int index, int end, typename ArgTuple, typename SrcTuple, typename DstTuple>
	void set_foreach(SrcTuple& , DstTuple& , typename detail::enable_if<index == end, void>::type*& = detail::enabler::value ) const
	{
	}

public:
	iuPairwiseHolder(const Generator&... generators)
		: v(generators...) {}

public:
	template<typename... Args>
	operator iuIParamGenerator< tuples::tuple<Args...> >* (void) const
	{
		typedef tuples::tuple<Args...> ArgTuple;
		tuples::tuple< iuParamGenerator<Args>... > generators;
		set_foreach<0, tuples::tuple_size<ArgTuple>::value, ArgTuple>(v, generators);

		return iuPairwiseGenerator<Args...>::Create(generators);
	}

public:
	template<typename Other>
	iuConcatParamHolder<_Myt, Other> operator + (const Other& g) const
	{
		return iuConcatParamHolder<_Myt, Other>(*this, g);
	}

private:
	void	operator = (const _Myt&);
private:
	_MyTuple v;
};

#else

template<typename T1, typename T2>
class iuPairwiseGenerator2 : public iuIParamGenerator< tuples::tuple<T1, T2> >
{
	typedef iuParamGenerator<T1> Generator1;
	typedef iuParamGenerator<T2> Generator2;
public:
	typedef tuples::tuple<T1, T2>	ParamType;

public:
	iuPairwiseGenerator2(const Generator1& g1, const Generator2& g2)
		: m_g1(g1), m_g2(g2)
	{}

	static iuIParamGenerator< ParamType >* Create(const Generator1& g1, const Generator2& g2)
	{
		return new iuPairwiseGenerator2<T1, T2>(g1, g2);
	}
public:
	virtual	void	Begin(void)
	{
		m_g1.Begin();
		m_g2.Begin();
	}
	virtual void	Next(void)
	{
		if( m_g2.IsEnd() ) 
		{
			return;
		}
		m_g2.Next();
		if( m_g2.IsEnd() )
		{
			m_g1.Next();
			if( !m_g1.IsEnd() )
			{
				m_g2.Begin();
			}
		}
	}
	virtual bool	IsEnd(void) const
	{
		return m_g1.IsEnd() && m_g2.IsEnd();
	}
	virtual ParamType	GetCurrent(void) const
	{
		return ParamType(this->m_g1.GetCurrent(), this->m_g2.GetCurrent());
	}
private:
	Generator1	m_g1;
	Generator2	m_g2;
};

/*
template<typename T1, typename T2, typename T3>
class iuPairwiseGenerator3 : public iuPairwiseGeneratorBase
{
	typedef iuParamGenerator<T1> Generator1;
	typedef iuParamGenerator<T2> Generator2;
	typedef iuParamGenerator<T3> Generator3;

	static const int RAW_COUNT = 3;
	typedef ParamIndexes<RAW_COUNT>		_MyParamIndexes;
	typedef ::std::vector< _MyParamIndexes >	ParamIndexesList;

public:
	typedef tuples::tuple<T1, T2, T3>	ParamType;
public:
	static iuIParamGenerator< ParamType >* Create(Generator1 g1, Generator2 g2, Generator3 g3)
	{
		ParamIndexesList list;
		::std::vector<T1> params1;
		::std::vector<T2> params2;
		::std::vector<T3> params3;

		MakeParamVector(params1, g1);
		MakeParamVector(params2, g2);
		MakeParamVector(params3, g3);

		int count_list[] = {
			static_cast<int>(params1.size())
			, static_cast<int>(params2.size())
			, static_cast<int>(params3.size())
		};
		MakeIndexList(list, count_list);

		::std::vector<ParamType> params;

		for( typename ParamIndexesList::const_iterator it=list.begin(), end=list.end(); it != end; ++it )
		{
			const _MyParamIndexes& indexes = *it;
			params.push_back( ParamType(
				GetParam(params1, indexes, 0)
				, GetParam(params2, indexes, 1)
				, GetParam(params3, indexes, 2)
				) );
		}

		return new iuValuesInParamsGenerator< ParamType >(params);
	}
};
*/

#define IIUT_DECL_PAIRWISE_GENERATOR_TEMPLATE_T_(i, p1, p2)			\
	p1<IUTEST_PP_CAT(T, i)> IUTEST_PP_CAT(p2, i);
#define IIUT_DECL_PAIRWISE_GENERATOR_MAKEPARAM_VECTOR_(i, p1, p2)	\
	MakeParamVector( IUTEST_PP_CAT(p1, i), IUTEST_PP_CAT(p2, i) );
#define IIUT_DECL_PAIRWISE_GENERATOR_PARAM_SIZE_(i, param)			\
	static_cast<int>( IUTEST_PP_CAT(param, i).size() )
#define IIUT_DECL_PAIRWISE_GENERATOR_GETPARAM_(i, param)			\
	GetParam( IUTEST_PP_CAT(param, i), indexes, i)

#define IIUT_DECL_PAIRWISE_GENERATOR_(n)				\
	template< IUTEST_PP_ENUM_PARAMS(n, typename T) >	\
	class IUTEST_PP_CAT(iuPairwiseGenerator, n) : public iuPairwiseGeneratorBase {	\
		IUTEST_PP_REPEAT_BINARY(n, IIUT_DECL_PAIRWISE_GENERATOR_TEMPLATE_T_, typedef iuParamGenerator, Generator)	\
		typedef ParamIndexes<n> _MyParamIndexes;							\
		typedef ::std::vector< _MyParamIndexes > ParamIndexesList;			\
	public: typedef tuples::tuple< IUTEST_PP_ENUM_PARAMS(n, T) > ParamType;	\
		static iuIParamGenerator< ParamType >* Create(						\
			IUTEST_PP_ENUM_BINARY_PARAMS(n, Generator, g) ) {				\
			ParamIndexesList list;											\
			IUTEST_PP_REPEAT_BINARY(n, IIUT_DECL_PAIRWISE_GENERATOR_TEMPLATE_T_, ::std::vector, params)	\
			IUTEST_PP_REPEAT_BINARY(n, IIUT_DECL_PAIRWISE_GENERATOR_MAKEPARAM_VECTOR_, params, g)		\
			int count_list[] = { IUTEST_PP_ENUM(n, IIUT_DECL_PAIRWISE_GENERATOR_PARAM_SIZE_, params) };	\
			MakeIndexList(list, count_list);								\
			::std::vector<ParamType> params;								\
			for( typename ParamIndexesList::const_iterator it=list.begin(), end=list.end(); it != end; ++it ) {		\
				const _MyParamIndexes& indexes = *it;						\
				params.push_back( ParamType( IUTEST_PP_ENUM(n, IIUT_DECL_PAIRWISE_GENERATOR_GETPARAM_, params) ) );	\
			}																\
			return new iuValuesInParamsGenerator< ParamType >(params);		\
		}																	\
	}

IIUT_DECL_PAIRWISE_GENERATOR_(3);
IIUT_DECL_PAIRWISE_GENERATOR_(4);
IIUT_DECL_PAIRWISE_GENERATOR_(5);
IIUT_DECL_PAIRWISE_GENERATOR_(6);
IIUT_DECL_PAIRWISE_GENERATOR_(7);
IIUT_DECL_PAIRWISE_GENERATOR_(8);
IIUT_DECL_PAIRWISE_GENERATOR_(9);

#undef IIUT_DECL_PAIRWISE_GENERATOR_TEMPLATE_T_
#undef IIUT_DECL_PAIRWISE_GENERATOR_MAKEPARAM_VECTOR_
#undef IIUT_DECL_PAIRWISE_GENERATOR_PARAM_SIZE_
#undef IIUT_DECL_PAIRWISE_GENERATOR_GETPARAM_
#undef IIUT_DECL_PAIRWISE_GENERATOR_

/*
template<typename Generator1, typename Generator2>
class iuPairwiseHolder2
{
	typedef iuPairwiseHolder2<Generator1, Generator2> _Myt;
public:
	iuPairwiseHolder2(const Generator1& g1, const Generator2& g2)
		: m_g1(g1), m_g2(g2) {}

public:
	template<typename T1, typename T2>
	operator iuIParamGenerator< tuples::tuple<T1, T2> >* () const 
	{
		return iuPairwiseGenerator2<T1, T2>::Create(
			static_cast< iuIParamGenerator<T1>* >(m_g1)
			, static_cast< iuIParamGenerator<T2>* >(m_g2)
			);
	}

public:
	template<typename Other>
	iuConcatParamHolder<_Myt, Other> operator + (const Other& g) const
	{
		return iuConcatParamHolder<_Myt, Other>(*this, g);
	}

private:
	void	operator = (const _Myt&) {}
private:
	const Generator1	m_g1;
	const Generator2	m_g2;
};
*/

#define IIUT_DECL_PAIRWISE_HOLDER_CONSTRUCT_(i, p1, p2)	IUTEST_PP_CAT(p1, i)(IUTEST_PP_CAT(p2, i))
#define IIUT_DECL_PAIRWISE_HOLDER_STATICCAST_(i, p1, p2)	\
	static_cast< iuIParamGenerator< IUTEST_PP_CAT(p1, i) >* >(IUTEST_PP_CAT(p2, i))
#define IIUT_DECL_PAIRWISE_HOLDER_VARIABLE_(i, p1, p2)	IUTEST_PP_CAT(p1, i) IUTEST_PP_CAT(p2, i);

#define IIUT_DECL_PAIRWISE_HOLDER_(n)	\
	template< IUTEST_PP_ENUM_PARAMS(n, typename Generator) >	\
	class IUTEST_PP_CAT(iuPairwiseHolder, n) {					\
		typedef IUTEST_PP_CAT(iuPairwiseHolder, n)< IUTEST_PP_ENUM_PARAMS(n, Generator) > _Myt;		\
	public: IUTEST_PP_CAT(iuPairwiseHolder, n)(					\
		IUTEST_PP_ENUM_BINARY_PARAMS(n, const Generator, &g) )	\
		: IUTEST_PP_ENUM_BINARY(n, IIUT_DECL_PAIRWISE_HOLDER_CONSTRUCT_, m_g, g) {}					\
		template< IUTEST_PP_ENUM_PARAMS(n, typename T) >		\
		operator iuIParamGenerator< tuples::tuple< IUTEST_PP_ENUM_PARAMS(n, T) > >* (void) const {	\
			return IUTEST_PP_CAT(iuPairwiseGenerator, n)< IUTEST_PP_ENUM_PARAMS(n, T) >::Create(	\
				IUTEST_PP_ENUM_BINARY(n, IIUT_DECL_PAIRWISE_HOLDER_STATICCAST_, T, m_g) );			\
		}														\
		template<typename Other> iuConcatParamHolder<_Myt, Other> operator + (const Other& g) const { \
			return iuConcatParamHolder<_Myt, Other>(*this, g);	\
		}														\
	private: void operator = (const _Myt&) {}					\
		IUTEST_PP_REPEAT_BINARY(n, IIUT_DECL_PAIRWISE_HOLDER_VARIABLE_, const Generator, m_g)		\
	}

IIUT_DECL_PAIRWISE_HOLDER_(2);
IIUT_DECL_PAIRWISE_HOLDER_(3);
IIUT_DECL_PAIRWISE_HOLDER_(4);
IIUT_DECL_PAIRWISE_HOLDER_(5);
IIUT_DECL_PAIRWISE_HOLDER_(6);
IIUT_DECL_PAIRWISE_HOLDER_(7);
IIUT_DECL_PAIRWISE_HOLDER_(8);
IIUT_DECL_PAIRWISE_HOLDER_(9);

#undef IIUT_DECL_PAIRWISE_HOLDER_CONSTRUCT_
#undef IIUT_DECL_PAIRWISE_HOLDER_STATICCAST_
#undef IIUT_DECL_PAIRWISE_HOLDER_VARIABLE_
#undef IIUT_DECL_PAIRWISE_HOLDER_

#endif

#endif

#if IUTEST_HAS_VALUESGEN

/**
 * @breif	パラメータ生成器
 * @tparam G	= パラメータ生成器
*/
template<typename G>
class iuValuesParamsGeneratorHolder
{
	typedef iuValuesParamsGeneratorHolder<G> _Myt;
public:
	iuValuesParamsGeneratorHolder(size_t num, const G& g)
		: m_num(num), m_g(g)
	{}
public:
	template<typename T>
	operator iuIParamGenerator<T>* (void) const
	{
		::std::vector<T> params(m_num);
		::std::generate(params.begin(), params.end(), m_g);
		return new iuValuesInParamsGenerator<T>( params );
	}

public:
	template<typename Other>
	iuConcatParamHolder<_Myt, Other> operator + (const Other& g) const
	{
		return iuConcatParamHolder<_Myt, Other>(*this, g);
	}

private:
	size_t m_num;
	G m_g;
};

/**
 * @brief	乱数ジェネレータ
*/
template<typename T, typename F>
class iuRandomFilterParamGenerator
{
	typedef T	type;
public:
	iuRandomFilterParamGenerator(const F& fn, unsigned int seed)
		: m_fn(fn), m_rnd(seed) {}

	type operator ()(void)
	{
		for(;;)
		{
			type val =  m_rnd.genrand();
			if( (m_fn)(val) )
			{
				return val;
			}
		}
	}
private:
	F m_fn;
	iuTypedRandom<type> m_rnd;
};

#endif

#if IUTEST_HAS_RANDOMVALUES

/**
 * @breif	乱数パラメータ生成器
*/
class iuRandomParamsHolder
{
public:
	iuRandomParamsHolder(size_t num, unsigned int seed=0) IUTEST_CXX_NOEXCEPT_SPEC
		: m_num(num), m_seed(seed) {}
public:
	template<typename T>
	operator iuIParamGenerator<T>* (void) const
	{
		unsigned int seed = m_seed;
		if( seed == 0 )
		{
			seed = GetIndefiniteValue();
		}
		iuValuesParamsGeneratorHolder< iuTypedRandom<T> > gen( m_num, iuTypedRandom<T>(seed) );
		return gen;
	}

public:
	template<typename Other>
	iuConcatParamHolder<iuRandomParamsHolder, Other> operator + (const Other& g) const
	{
		return iuConcatParamHolder<iuRandomParamsHolder, Other>(*this, g);
	}

private:
	size_t m_num;
	unsigned int m_seed;
};

#endif

}	// end of namespace detail
}	// end of namespace iutest

#endif

#endif
