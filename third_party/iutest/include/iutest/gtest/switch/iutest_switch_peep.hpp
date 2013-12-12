//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_switch_peep.hpp
 * @brief		IUTEST_PEEP 切り替え定義 ファイル
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
#ifndef INCG_IRIS_IUTEST_SWITCH_PEEP_HPP_316C2127_8DF8_4B99_9C07_9170B403D6AD_
#define INCG_IRIS_IUTEST_SWITCH_PEEP_HPP_316C2127_8DF8_4B99_9C07_9170B403D6AD_

#if !defined(IUTEST_USE_GTEST)

//======================================================================
// define

#else

//======================================================================
// define
#ifndef IUTEST_HAS_PEEP
#  define IUTEST_HAS_PEEP				0
#endif
#ifndef IUTEST_HAS_PEEP_FUNC
#  define IUTEST_HAS_PEEP_FUNC			0
#endif
#ifndef IUTEST_HAS_PEEP_STATIC_FUNC
#  define IUTEST_HAS_PEEP_STATIC_FUNC	0
#endif

#endif

#endif
