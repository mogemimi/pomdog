//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_port.hpp
 * @brief		iris unit test 依存関数 ファイル
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
#ifndef INCG_IRIS_IUTEST_PORT_HPP_7893F685_A1A9_477A_82E8_BF06237697FF_
#define INCG_IRIS_IUTEST_PORT_HPP_7893F685_A1A9_477A_82E8_BF06237697FF_

//======================================================================
// include
#if defined(__MWERKS__)
#  define _MSL_USING_NAMESPACE
#endif

#include "iutest_internal_defs.hpp"

#if defined(IUTEST_OS_LINUX) || defined(IUTEST_OS_CYGWIN) || defined(IUTEST_OS_MAC)
#  include <unistd.h>
#  include <locale.h>
#endif

namespace iutest {

#ifdef IUTEST_OS_NACL
namespace nacl
{

/**
 * @brief	printf
*/
void vprint_message(const char *fmt, va_list va);
void print_message(const char *fmt, ...);

}
#endif

namespace internal {
namespace posix
{

const char* GetEnv(const char* name);
int PutEnv(const char* expr);

const char* GetCWD(char* buf, size_t length);
::std::string GetCWD(void);

void SleepMillisec(unsigned int millisec);

#if defined(IUTEST_OS_WINDOWS_MOBILE)
void Abort(void);
#else
inline void Abort(void) { abort(); }
#endif

}	// end of namespace posix

inline void SleepMilliseconds(int n) { posix::SleepMillisec(static_cast<unsigned int>(n)); }

}	// end of namespace internal

namespace detail
{

namespace posix = internal::posix;

/**
 * @brief	環境変数の設定
*/
bool SetEnvironmentVariable(const char* name, const char* value);


/**
 * @brief	環境変数の取得
 * @param [in]	name	= 環境変数名
 * @param [out]	buf		= 出力バッファ
 * @return	成否
*/
bool GetEnvironmentVariable(const char* name, char* buf, size_t size);

#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template<size_t SIZE>
inline bool GetEnvironmentVariable(const char* name, char (&buf)[SIZE])
{
	return GetEnvironmentVariable(name, buf, SIZE);
}

#endif

/**
 * @brief	環境変数の取得
 * @param [in]	name	= 環境変数名
 * @param [out]	var		= 出力文字列
 * @return	成否
*/
bool IUTEST_ATTRIBUTE_UNUSED_ GetEnvironmentVariable(const char* name, ::std::string& var);

/**
 * @brief	環境変数の取得
 * @param [in]	name	= 環境変数名
 * @param [out]	var		= 出力数値
 * @return	成否
*/
bool IUTEST_ATTRIBUTE_UNUSED_ GetEnvironmentInt(const char* name, int& var);

#if defined(IUTEST_OS_WINDOWS)
namespace win
{

/**
 * @brief	文字列変換
*/
::std::string IUTEST_ATTRIBUTE_UNUSED_ WideStringToMultiByteString(const wchar_t* wide_c_str);

/**
 * @brief	HRESULT のエラー文字列を取得
 * @param [in]	hr	= エラー値
 * @return	文字列
*/
::std::string IUTEST_ATTRIBUTE_UNUSED_ GetHResultString(HRESULT hr);

}	// end of namespace win
#endif

/**
 * @brief	ログ
*/
class IUTestLog
{
public:
	enum Level
	{
		  LOG_INFO
		, LOG_WARNING
		, LOG_ERROR
		, LOG_FATAL
	};
public:
	IUTestLog(Level level, const char* file, int line);

	~IUTestLog(void);

public:
	iuStringStream::type& GetStream(void) { return m_stream; }
private:
	const Level kLevel;
	iuStringStream::type m_stream;

	IUTEST_PP_DISALLOW_COPY_AND_ASSIGN(IUTestLog);
};

#define IUTEST_LOG_(level)	::iutest::detail::IUTestLog(::iutest::detail::IUTestLog::LOG_##level, __FILE__, __LINE__).GetStream()
#define IUTEST_CHECK_(condition)	\
	IUTEST_AMBIGUOUS_ELSE_BLOCKER_	\
	if( ::iutest::detail::IsTrue(condition) )	\
		;										\
	else										\
		IUTEST_LOG_(FATAL) << "Condition " #condition " failed. "


}	// end of namespace detail
}	// end of namespace iutest

#if !IUTEST_HAS_LIB
#  include "../impl/iutest_port.ipp"
#endif

#endif // INCG_IRIS_IUTEST_PORT_HPP_7893F685_A1A9_477A_82E8_BF06237697FF_
