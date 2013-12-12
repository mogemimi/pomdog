//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_debug.ipp
 * @brief		iris unit test debug help ƒtƒ@ƒCƒ‹
 *
 * @author		t.sirayanagi
 * @version		1.0
 *
 * @par			copyright
 * Copyright (C) 2013, Takazumi Shirayanagi\n
 * This software is released under the new BSD License,
 * see LICENSE
*/
//-----------------------------------------------------------------------
//======================================================================
#ifndef INCG_IRIS_IUTEST_DEBUG_IPP_1575CB44_189A_4248_B305_DB4882E3BFC2_
#define INCG_IRIS_IUTEST_DEBUG_IPP_1575CB44_189A_4248_B305_DB4882E3BFC2_

//======================================================================
// include
#include "../internal/iutest_debug.hpp"

#if defined(_MSC_VER) && _MSC_VER >= 1310 && !defined(IUTEST_OS_WINDOWS_MOBILE)
#  include <DbgHelp.h>
//#  pragma comment (lib, "dbghelp.lib")
#endif

namespace iutest {
namespace detail
{

#if defined(_MSC_VER) && IUTEST_HAS_MINIDUMP

IUTEST_IPP_INLINE MiniDump::MiniDump(void)
	: m_hModule(NULL), m_pfnMiniDumpWriteDump(NULL)
{
	m_hModule = ::LoadLibraryA("dbghelp.dll");
	if( m_hModule != NULL )
	{
		m_pfnMiniDumpWriteDump = ::GetProcAddress(m_hModule, "MiniDumpWriteDump");
	}
}

IUTEST_IPP_INLINE MiniDump::~MiniDump(void)
{
	FreeLibrary(m_hModule);
}

IUTEST_IPP_INLINE bool MiniDump::Dump(HANDLE hFile, EXCEPTION_POINTERS* ep)
{
	if( m_pfnMiniDumpWriteDump == NULL ) return false;
	MINIDUMP_EXCEPTION_INFORMATION mdei;
	mdei.ThreadId = ::GetCurrentThreadId();
	mdei.ExceptionPointers = ep;
	mdei.ClientPointers = FALSE;

	typedef BOOL (WINAPI *pfnMiniDumpWriteDump)(HANDLE hProcess, DWORD dwProcessId, HANDLE hFile
		, MINIDUMP_TYPE DumpType
		, PMINIDUMP_EXCEPTION_INFORMATION
		, PMINIDUMP_USER_STREAM_INFORMATION
		, PMINIDUMP_CALLBACK_INFORMATION
		);
	pfnMiniDumpWriteDump proc = reinterpret_cast<pfnMiniDumpWriteDump>(m_pfnMiniDumpWriteDump);
	return (*proc)( ::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal
		, &mdei, NULL, NULL) ? true : false;
}
IUTEST_IPP_INLINE bool MiniDump::Create(const char* filepath, EXCEPTION_POINTERS* ep)
{
	HANDLE hFile = CreateFileA( filepath, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_WRITE|FILE_SHARE_READ
		, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == NULL || hFile == INVALID_HANDLE_VALUE ) return false;

	static MiniDump dump;
	const bool ret = dump.Dump(hFile, ep);

	CloseHandle(hFile);
	return ret;
}

#endif

}	// end of namespace detail
}	// end of namespace iutest

#endif
