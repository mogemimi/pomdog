//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_defs.hpp
 * @brief		iris unit test 定義 ファイル
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
#ifndef INCG_IRIS_iutest_defs_HPP_839F06DB_E0B6_4e6a_84F2_D99C0A44E06C_
#define INCG_IRIS_iutest_defs_HPP_839F06DB_E0B6_4e6a_84F2_D99C0A44E06C_

//======================================================================
// include
#include "internal/iutest_compiler.hpp"
#include "internal/iutest_pp.hpp"
#include "internal/iutest_stdlib.hpp"
#include "iutest_config.hpp"
#include <stdio.h>
#include <vector>
#include <algorithm>
#include "internal/iutest_debug.hpp"

namespace iutest
{

//======================================================================
// struct
namespace detail
{

/**
 * @brief	type_least_t
*/
template<int SIZE>
struct type_least_t {};

/** type_least_t<1> */
template<>
struct type_least_t<1>
{
	typedef char				Int;
	typedef unsigned char		UInt;
};

/** type_least_t<2> */
template<>
struct type_least_t<2>
{
	typedef short				Int;
	typedef unsigned short		UInt;
};

/** type_least_t<4> */
template<>
struct type_least_t<4>
{
	typedef int					Int;
	typedef unsigned int		UInt;
};

/** type_least_t<8> */
template<>
struct type_least_t<8>
{
#if defined(_MSC_VER)
	typedef __int64				Int;
	typedef unsigned __int64	UInt;
#else
	typedef long long			Int;
	typedef unsigned long long	UInt;
#endif
};

}

namespace internal
{

typedef void*	TypeId;		//!< テスト識別型

namespace helper
{

//======================================================================
// struct
/**
 * @brief	TypeId Generator Implementation
*/
template<typename T>
struct TestTypeIdHelper { public: static bool _dummy; };

template<typename T>bool TestTypeIdHelper<T>::_dummy = false;

}

//======================================================================
// function
/**
  * @internal
  * @brief	TypeId Generator
*/
template<typename T>
inline IUTEST_CXX_CONSTEXPR TypeId	GetTypeId(void)
{
	return &(helper::TestTypeIdHelper<T>::_dummy);
}

/**
  * @internal
  * @brief	TypeId Generator
*/
inline IUTEST_CXX_CONSTEXPR TypeId	GetTestTypeId(void)
{
	return 0;
}

}

//======================================================================
// class
/**
 * @brief	浮動小数点数
*/
template<typename RawType>
class floating_point
{
private:
	typedef floating_point<RawType>	_Myt;

	typedef typename detail::type_least_t<sizeof(RawType)>	type;
	typedef typename type::Int	Int;
	typedef typename type::UInt	UInt;
	union FInt
	{
		Int		iv;
		UInt	uv;
		RawType	fv;
	};

public:
	/**
	 * @brief	コンストラクタ
	*/
	floating_point(void)
	{
		m_v.uv = 0;
	}

	/**
	 * @brief	コンストラクタ
	 * @param [in]	f	= 浮動小数点数
	*/
	floating_point(RawType f)
	{
		m_v.fv = f;
	}

	/**
	 * @brief	コンストラクタ
	*/
	floating_point(const _Myt& rhs)
		: m_v(rhs.m_v)
	{
	}

public:
	/**
	 * @brief	浮動小数点数がほぼ一致するかどうか
	 * @sa		http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
	*/
	bool	AlmostEquals(const _Myt& rhs) const
	{
		const Int v1 = m_v.iv < 0 ? ~m_v.iv + 1 : m_v.iv;
		const Int v2 = rhs.m_v.iv < 0 ? ~rhs.m_v.iv + 1 : rhs.m_v.iv;
		const Int diff = (v1 > v2) ? v1 - v2 : v2 - v1;
		if( diff <= kMaxUlps ) return true;
		return false;
	}

public:
	/**
	 * @brief	ビット列の取得
	*/
	UInt	bit(void) const	{ return m_v.uv; }

	/**
	 * @brief	raw データの取得
	*/
	RawType	raw(void) const	{ return m_v.fv; }

public:
	//! ＋ inf
	static _Myt	PINF(void)
	{
		_Myt f;
		f.m_v.uv = ((1<<EXP)-1);
		f.m_v.uv <<= FRAC;
		return f;
	}
	//! － inf
	static _Myt	NINF(void)
	{
		_Myt f = PINF();
		f.m_v.uv |= static_cast<UInt>(1u) << (EXP+FRAC);
		return f;
	}
	//! ＋ nan
	static _Myt	PNAN(void)
	{
		_Myt f = PINF();
		f.m_v.uv |= 1;
		return f;
	}
	//! － nan
	static _Myt	NNAN(void)
	{
		_Myt f = NINF();
		f.m_v.uv |= 1;
		return f;
	}
	//! ＋ qnan
	static _Myt	PQNAN(void)
	{
		_Myt f;
		f.m_v.uv = ((1<<(EXP+1))-1);
		f.m_v.uv <<= FRAC-1;
		return f;
	}
	//! － qnan
	static _Myt	NQNAN(void)
	{
		_Myt f = PQNAN();
		f.m_v.uv |= static_cast<UInt>(1u) << (EXP+FRAC);
		return f;
	}

public:
	operator RawType (void) const	{ return m_v.fv; }	//!< cast to RawType
	_Myt&	operator = (RawType f)	{ m_v.fv = f; return *this; }	//!< 代入

	bool	operator == (const _Myt& rhs) const	{ return m_v.uv == rhs.m_v.uv; }	//!< 比較

private:
	template<size_t N, typename DMY>
	struct ieee 
	{
		enum
		{
			  EXP  = 8
			, FRAC = 23
		};
	};
	template<typename DMY>
	struct ieee<8, DMY>
	{
		enum
		{
			  EXP  = 11
			, FRAC = 52
		};
	};
	enum
	{
		  EXP  = ieee<sizeof(RawType), void>::EXP
		, FRAC = ieee<sizeof(RawType), void>::FRAC
	};
	enum
	{
		kMaxUlps = 4
	};
private:
	FInt	m_v;
};

//======================================================================
// typedef
typedef detail::type_least_t<4>::Int	Int32;	//!< 32 bit 符号付き整数型
typedef detail::type_least_t<4>::UInt	UInt32;	//!< 32 bit 符号なし整数型
typedef detail::type_least_t<8>::Int	Int64;	//!< 64 bit 符号付き整数型
typedef detail::type_least_t<8>::UInt	UInt64;	//!< 64 bit 符号なし整数型

typedef internal::TypeId TestTypeId;	//!< テスト識別型

typedef void (*SetUpMethod)(void);		//!< SetUp 関数型
typedef void (*TearDownMethod)(void);	//!< TearDown 関数型

typedef detail::type_least_t<8>::UInt	TimeInMillisec;	//!< ミリ秒単位を扱う型
typedef detail::type_least_t<8>::Int	BiggestInt;		//!< Biggest Int

}	// end of namespace iutest

#endif
