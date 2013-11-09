//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_port.ipp
 * @brief		iris unit test 依存関数 ファイル
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
#ifndef INCG_IRIS_iutest_port_IPP_7893F685_A1A9_477a_82E8_BF06237697FF_
#define INCG_IRIS_iutest_port_IPP_7893F685_A1A9_477a_82E8_BF06237697FF_

//======================================================================
// include
#include "../internal/iutest_port.hpp"

#ifdef IUTEST_OS_NACL
#  include <ppapi/cpp/var.h>
#  include <ppapi/cpp/instance.h>
#  include <ppapi/cpp/module.h>
#endif

namespace iutest {

#ifdef IUTEST_OS_NACL
namespace nacl
{

namespace detail
{

IUTEST_IPP_INLINE void	PostMessage(const pp::Var& var)
{
	::pp::Module* module = ::pp::Module::Get();
	if( module != NULL )
	{
		if( module->current_instances().size() > 0 )
		{
			module->current_instances().begin()->second->PostMessage(var);
		}
	}
}

}	// end of namespace detail

IUTEST_IPP_INLINE void	vprint_message(const char *fmt, va_list va)
{
	char msg[1024];
	vsnprintf(msg, sizeof(msg), fmt, va);
	char* tp = strtok(msg, "\n");
	while( tp != NULL )
	{
		detail::PostMessage(pp::Var(tp));
		tp = strtok(NULL, "\n");
	}
}
IUTEST_IPP_INLINE void	print_message(const char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	vprint_message(fmt, va);
	va_end(va);
}

}	// end of namespace nacl
#endif

namespace internal {
namespace posix
{

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_BEGIN()

IUTEST_IPP_INLINE const char* GetEnv(const char* name)
{
#if defined(IUTEST_OS_WINDOWS_MOBILE) || defined(IUTEST_NO_GETENV)
	IUTEST_UNUSED_VAR(name);
	return NULL;
#elif defined(__BORLANDC__) || defined(__SunOS_5_8) || defined(__SunOS_5_9)
	const char* env = getenv(name);
	return (env != NULL && env[0] != '\0') ? env : NULL;
#else
	return getenv(name);
#endif
}

IUTEST_IPP_INLINE int PutEnv(const char* expr)
{
#if defined(IUTEST_OS_WINDOWS_MOBILE) || defined(IUTEST_NO_PUTENV) || defined(__STRICT_ANSI__)
	IUTEST_UNUSED_VAR(expr);
	return -1;
#else
	return putenv(const_cast<char*>(expr));
#endif
}

IUTEST_IPP_INLINE const char* GetCWD(char* buf, size_t length)
{
#if	defined(IUTEST_OS_WINDOWS_MOBILE) || defined(IUTEST_NO_GETCWD)
	if( buf == NULL || length < 3 ) return NULL;
	buf[0] = '.';
	buf[1] = '/';
	buf[2] = '\0';
	return buf;
#elif defined(IUTEST_OS_WINDOWS)
	return ::GetCurrentDirectoryA(static_cast<DWORD>(length), buf) == 0 ? NULL : buf;
#else
	return getcwd(buf, length);
#endif
}

IUTEST_IPP_INLINE ::std::string GetCWD(void)
{
	char buf[260];
	return GetCWD(buf, sizeof(buf));
}

IUTEST_IPP_INLINE void SleepMillisec(unsigned int millisec)
{
#if		defined(IUTEST_OS_WINDOWS)
	Sleep(millisec);
#elif	defined(IUTEST_OS_LINUX) || defined(IUTEST_OS_CYGWIN)

#if	defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 199309L
	const timespec time = { 0, millisec * 1000 * 1000 };
	nanosleep(&time, NULL);
#elif (defined(_BSD_SOURCE) && _BSD_SOURCE) 
	|| (defined(_XOPEN_SOURCE)
		&& (_XOPEN_SOURCE >= 500 || _XOPEN_SOURCE && _XOPEN_SOURCE_EXTENDED)
		&& (!defined(_POSIX_C_SOURCE) || !(_POSIX_C_SOURCE >= 200809L || _XOPEN_SOURCE >= 700)) )
	usleep(millisec*1000);
#else
	usleep(millisec*1000);
#endif

#else
	volatile int x=0;
	for( unsigned int i=0; i < millisec; ++i ) x += 1;
	IUTEST_UNUSED_VAR(x);
#endif
}

#if defined(IUTEST_OS_WINDOWS_MOBILE)
void Abort(void)
{
	DebugBreak();
	TerminateProcess(GetCurrentProcess(), 1);
}
#endif

IUTEST_PRAGMA_CRT_SECURE_WARN_DISABLE_END()

}	// end of namespace posix
}	// end of namespace internal

namespace detail
{

IUTEST_IPP_INLINE bool SetEnvironmentVariable(const char* name, const char* value)
{
#if defined(IUTEST_OS_WINDOWS) && !defined(IUTEST_OS_WINDOWS_MOBILE)
	return ::SetEnvironmentVariableA(name, value) ? true : false;
#else
	::std::string var = name;
	var += "=";
	var += value;
	return internal::posix::PutEnv(var.c_str()) == 0 ? true : false;
#endif
}

IUTEST_IPP_INLINE bool GetEnvironmentVariable(const char* name, char* buf, size_t size)
{
#if defined(IUTEST_OS_WINDOWS) && !defined(IUTEST_OS_WINDOWS_MOBILE)
	DWORD ret = ::GetEnvironmentVariableA(name, buf, static_cast<DWORD>(size));
	if( ret == 0 ) return false;
	if( ret > size ) return false;
	return true;
#else
	IUTEST_UNUSED_VAR(size);
	const char* env = internal::posix::GetEnv(name);
	if( env == NULL ) return false;
	strcpy(buf, env);
	return true;
#endif
}

IUTEST_IPP_INLINE bool GetEnvironmentVariable(const char* name, ::std::string& var)
{
#if defined(IUTEST_OS_WINDOWS) && !defined(IUTEST_OS_WINDOWS_MOBILE)
	char buf[2048];
	if( !GetEnvironmentVariable(name, buf, sizeof(buf)) ) return false;
	var = buf;
	return true;
#else
	const char* env = internal::posix::GetEnv(name);
	if( env == NULL ) return false;
	var = env;
	return true;
#endif
}

IUTEST_IPP_INLINE bool GetEnvironmentInt(const char* name, int& var)
{
#if defined(IUTEST_OS_WINDOWS) && !defined(IUTEST_OS_WINDOWS_MOBILE)
	char buf[128] = {0};
	if( !GetEnvironmentVariable(name, buf, sizeof(buf)) ) return false;
	char* end = NULL;
	var = static_cast<int>(strtol(buf, &end, 0));
	return true;
#else
	const char* env = internal::posix::GetEnv(name);
	if( env == NULL ) return false;
	char* end = NULL;
	var = static_cast<int>(strtol(env, &end, 0));
	return true;
#endif
}

#if defined(IUTEST_OS_WINDOWS)
namespace win
{

IUTEST_IPP_INLINE ::std::string WideStringToMultiByteString(const wchar_t* wide_c_str)
{
	if( wide_c_str == NULL ) return "(null)";
	::std::string str;
	const int length = static_cast<int>(wcslen(wide_c_str)) * 2 + 1;
	char* mbs = new char [length];
	WideCharToMultiByte(932, 0, wide_c_str, static_cast<int>(wcslen(wide_c_str))+1, mbs, length, NULL, NULL);
	str = mbs;
	delete [] mbs;
	return str;
}

IUTEST_IPP_INLINE ::std::string GetHResultString(HRESULT hr)
{
#if defined(IUTEST_OS_WINDOWS_MOBILE)
	LPWSTR buf = NULL;
	if( FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト ユーザー言語
		(LPWSTR)&buf,
		0,
		NULL ) == 0 )
	{
		return "";
	}

	::std::string str = (buf == NULL) ? "" : WideStringToMultiByteString(buf);
#else
	LPSTR buf = NULL;
	if( FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		hr,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // デフォルト ユーザー言語
		(LPSTR)&buf,
		0,
		NULL ) == 0 )
	{
		return "";
	}

	::std::string str = (buf == NULL) ? "" : buf;
#endif
	LocalFree(buf);
	return str;
}

}	// end of namespace win
#endif

// declare
::std::string FormatFileLocation(const char* file, int line);

IUTEST_IPP_INLINE IUTestLog::IUTestLog(Level level, const char* file, int line)
	: kLevel(level)
{
	const char* tag = 
		(level == LOG_INFO   ) ? "[  INFO ] ":
		(level == LOG_WARNING) ? "[WARNING] ":
		(level == LOG_ERROR  ) ? "[ ERROR ] ":
								 "[ FATAL ] ";
	GetStream() << "\r\n" << tag << FormatFileLocation(file, line).c_str() << ": ";
}

IUTEST_IPP_INLINE IUTestLog::~IUTestLog(void)
{
	GetStream() << "\r\n";
	fprintf(stderr, "%s", m_stream.str().c_str());
	if( kLevel == LOG_FATAL )
	{
		fflush(stderr);
		posix::Abort();
	}
}

}	// end of namespace detail
}	// end of namespace iutest

#endif
