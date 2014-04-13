//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_debug.hpp
 * @brief		iris unit test debug 用定義 ファイル
 *
 * @author		t.sirayanagi
 * @debugsion	1.0
 *
 * @par			copyright
 * Copyright (C) 2011-2014, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_DEBUG_HPP_A63366D4_2112_4269_9BAF_BD30A5F2C7F2_
#define INCG_IRIS_IUTEST_DEBUG_HPP_A63366D4_2112_4269_9BAF_BD30A5F2C7F2_

//======================================================================
// include

#ifdef _IUTEST_DEBUG
// iutest 自体のデバッグ用定義

#if defined(_MSC_VER) && !defined(IUTEST_OS_WINDOWS_MOBILE)
#  include <streambuf>
#  include <xlocnum>
#  include <xtree>
#  include <crtdbg.h>
#  ifndef _DEBUG_NEW_
#    define _DEBUG_NEW_		new ( _NORMAL_BLOCK , __FILE__, __LINE__)
#    pragma push_macro("new")
#    define new _DEBUG_NEW_
#  endif
#endif

#endif

namespace iutest {
namespace detail
{

//======================================================================
// function
static void IUTEST_ATTRIBUTE_UNUSED_ iuDebugInitialize(void)
{
#ifdef _IUTEST_DEBUG
#  if defined(_MSC_VER) && !defined(IUTEST_OS_WINDOWS_MOBILE)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#  endif
#endif
}

static void IUTEST_ATTRIBUTE_UNUSED_ iuDebugBreakAlloc(long n)
{
#ifdef _IUTEST_DEBUG
#  if defined(_MSC_VER) && !defined(IUTEST_OS_WINDOWS_MOBILE)
	_CrtSetBreakAlloc(n);
#  endif
#endif
	(void)n;
}

#if defined(_MSC_VER) && IUTEST_HAS_MINIDUMP

/**
 * @brief	minidump 作成クラス
*/
class MiniDump
{
private:
	MiniDump(void);
	~MiniDump(void);

	bool Dump(HANDLE hFile, EXCEPTION_POINTERS* ep);
public:
	/**
	 @brief	minidump 作成
	*/
	static bool Create(const char* filepath, EXCEPTION_POINTERS* ep);

private:
	HMODULE m_hModule;
	FARPROC m_pfnMiniDumpWriteDump;
};

#endif

}	// end of namespace detail
}	// end of namespace iutest

#if !IUTEST_HAS_LIB
#  include "../impl/iutest_debug.ipp"
#endif

#endif // INCG_IRIS_IUTEST_DEBUG_HPP_A63366D4_2112_4269_9BAF_BD30A5F2C7F2_
