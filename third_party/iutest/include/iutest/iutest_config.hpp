//======================================================================
//-----------------------------------------------------------------------
/**
 * @file		iutest_config.hpp
 * @brief		iris unit test config ファイル
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
#ifndef INCG_IRIS_IUTEST_CONFIG_HPP_F0151607_0D9D_4055_B94D_ED4A0FFA748B_
#define INCG_IRIS_IUTEST_CONFIG_HPP_F0151607_0D9D_4055_B94D_ED4A0FFA748B_

//======================================================================
// include

/**
 * @defgroup	IUTEST_CONFIG	IUTEST_CONFIG
 * @brief		設定マクロ
 * @note		ユーザーが事前定義することで変更可能
 * @{
*/

//! Library を使うかどうか
#ifndef IUTEST_HAS_LIB
#  if	defined(IUTEST_NO_LIB)
#    define IUTEST_HAS_LIB			0
#  elif	defined(IUTEST_USE_LIB) || defined(IUTEST_AUTO_LINK)
#    define IUTEST_HAS_LIB			1
#  endif
#endif

#ifndef IUTEST_HAS_LIB
#  define IUTEST_HAS_LIB			0
#endif

/**
 * @defgroup	IUTEST_CONFIG_OUTPUT	OUTPUT
 * @brief		出力関係 CONFIG
 * @{
*/

#ifndef IUTEST_VPRINTF
#  define IUTEST_VPRINTF(f, a)	vprintf(f, a)	//!< vprintf 呼び出しマクロ
#endif

#ifndef IUTEST_REPORT_SKIPPED
#  define IUTEST_REPORT_SKIPPED		1			//!< xml 出力に skipped タグを含めるかどうか
#endif

/**
 * @}
*/


/**
 * @defgroup	IUTEST_CONFIG_TEST	TEST
 * @brief		テスト関係 CONFIG
 * @{
*/

#ifndef IUTEST_HAS_PARAM_METHOD_TEST
//! パラメタライズ関数コールテストが使用可能かどうか
#  ifndef IUTEST_NO_VARIADIC_MACROS
#    define IUTEST_HAS_PARAM_METHOD_TEST	1
#  else
#    define IUTEST_HAS_PARAM_METHOD_TEST	0
#  endif
#endif

#ifndef IUTEST_HAS_PARAM_TEST
#  define IUTEST_HAS_PARAM_TEST			1	//!< 値をパラメータ化したテストが使用可能かどうか
#endif

#ifndef IUTEST_HAS_TYPED_TEST
//! 型付けテストが使用可能かどうか
#  if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
#    define IUTEST_HAS_TYPED_TEST		1
#  else
#    define IUTEST_HAS_TYPED_TEST		0
#  endif
#endif

#ifndef IUTEST_HAS_TYPED_TEST_P
//! 型をパラメータ化したテストが使用可能かどうか
#  if IUTEST_HAS_TYPED_TEST && !defined(IUTEST_NO_VARIADIC_MACROS) && !defined(IUTEST_NO_TEMPLATE_TEMPLATES)
#    define IUTEST_HAS_TYPED_TEST_P	1
#  else
#    define IUTEST_HAS_TYPED_TEST_P	0
#  endif
#endif

#ifndef IUTEST_TYPED_TEST_P_STRICT
//! Typed Test のより厳しい構文チェックを行うかどうか
#  define IUTEST_TYPED_TEST_P_STRICT	1
#endif

#if IUTEST_HAS_TYPED_TEST_P
#  if IUTEST_TYPED_TEST_P_STRICT
#    include <set>
#  endif
#endif

#ifndef IUTEST_HAS_IGNORE_TEST
/**
 * @brief	コンパイルが通らなくてもよいテスト生成マクロが使用可能かどうか
*/
#  if	defined(_MSC_VER) && _MSC_VER > 1200
#    define IUTEST_HAS_IGNORE_TEST	1
#  elif	defined(__GNUC__) || defined(__clang__)
#    define IUTEST_HAS_IGNORE_TEST	0
#  else
#    define IUTEST_HAS_IGNORE_TEST	0
#  endif
#endif

#ifndef IUTEST_HAS_VALUESGEN
//! ::iutest::ValuesGen が使用可能かどうか
#  if IUTEST_HAS_PARAM_TEST
#    define IUTEST_HAS_VALUESGEN	1
#  else
#    define IUTEST_HAS_VALUESGEN	0
#  endif
#endif

#ifndef IUTEST_HAS_RANDOMVALUES
//! ::iutest::RandomValues が使用可能かどうか
#  if IUTEST_HAS_PARAM_TEST && IUTEST_HAS_VALUESGEN
#    define IUTEST_HAS_RANDOMVALUES	1
#  else
#    define IUTEST_HAS_RANDOMVALUES	0
#  endif
#endif

#if IUTEST_HAS_RANDOMVALUES
#  if !IUTEST_HAS_PARAM_TEST || !IUTEST_HAS_VALUESGEN
#    undef IUTEST_HAS_RANDOMVALUES
#    define IUTEST_HAS_RANDOMVALUES	0
#  endif
#endif

#ifndef IUTEST_HAS_COMBINE
//! ::iutest::Combine が使用可能かどうか
#  if IUTEST_HAS_PARAM_TEST && IUTEST_HAS_TUPLE
#    define IUTEST_HAS_COMBINE		1
#  else
#    define IUTEST_HAS_COMBINE		0
#  endif
#endif

#ifndef IUTEST_HAS_PAIRWISE
//! ::iutest::Pairwise が使用可能かどうか
#  if IUTEST_HAS_COMBINE
#    define IUTEST_HAS_PAIRWISE		1
#  else
#    define IUTEST_HAS_PAIRWISE		0
#  endif
#endif

#ifndef IUTEST_HAS_CONCAT
//! ::iutest::Concat が使用可能かどうか
# define IUTEST_HAS_CONCAT			1
#endif

#ifndef IUTEST_HAS_VARIADIC_VALUES
//! 可変長引数に対応した ::iutest::Values が使用可能かどうか
#  if IUTEST_HAS_PARAM_TEST && IUTEST_HAS_VARIADIC_TEMPLATES && IUTEST_HAS_TUPLE
#    define IUTEST_HAS_VARIADIC_VALUES	1
#  else
#    define IUTEST_HAS_VARIADIC_VALUES	0
#  endif
#endif

#ifndef IUTEST_HAS_VARIADIC_COMBINE
//! 可変長引数に対応した ::iutest::Combine が使用可能かどうか
#  if IUTEST_HAS_COMBINE && IUTEST_HAS_VARIADIC_TEMPLATES && IUTEST_HAS_TUPLE
#    define IUTEST_HAS_VARIADIC_COMBINE	1
#  else
#    define IUTEST_HAS_VARIADIC_COMBINE	0
#  endif
#endif

#ifndef IUTEST_HAS_VARIADIC_PAIRWISE
//! 可変長引数に対応した ::iutest::Pairwise が使用可能かどうか
#  if IUTEST_HAS_PAIRWISE && IUTEST_HAS_VARIADIC_TEMPLATES && IUTEST_HAS_TUPLE
#    define IUTEST_HAS_VARIADIC_PAIRWISE	1
#  else
#    define IUTEST_HAS_VARIADIC_PAIRWISE	0
#  endif
#endif

#ifndef IUTEST_USE_THROW_ON_ASSERT_FAILURE
/**
 * @brief	ASSERT マクロで失敗時に例外を throw します。
 * @note	サブ関数にアサーションを記述しても、その時点でテストが中断されるようになります
*/
#  if !defined(IUTEST_NO_VOID_RETURNS)
#    define IUTEST_USE_THROW_ON_ASSERT_FAILURE	0
#  else
#    define IUTEST_USE_THROW_ON_ASSERT_FAILURE	1
#  endif
#endif

#ifndef IUTEST_HAS_SPI_LAMBDA_SUPPORT
//! spi マクロで lambda を使って変数にアクセス可能かどうか
#if IUTEST_HAS_LAMBDA && !defined(IUTEST_NO_LAMBDA_SCOPE_RESOLUTION)
#  define IUTEST_HAS_SPI_LAMBDA_SUPPORT		1
#else
#  define IUTEST_HAS_SPI_LAMBDA_SUPPORT		0
#endif
#endif

#ifndef IUTEST_HAS_GENRAND
#  define IUTEST_HAS_GENRAND		1	//!< テストごとの genrand 関数が使用可能かどうか
#endif

#ifndef IUTEST_HAS_PACKAGE
//! パッケージ機能があるかどうか
#  if !defined(IUTEST_NO_ARGUMENT_DEPENDENT_LOOKUP)
#    define IUTEST_HAS_PACKAGE		1
#  else
#    define IUTEST_HAS_PACKAGE		0
#  endif
#endif

// peep
#ifndef IUTEST_HAS_PEEP
//! private メンバーへのアクセスマクロが使用可能かどうか
#  define IUTEST_HAS_PEEP			1
#endif

#ifndef IUTEST_HAS_PEEP_FUNC
//! private メンバー関数へのアクセスが可能かどうか
#  if IUTEST_HAS_PEEP && IUTEST_EXPLICIT_INSTANTIATION_ACCESS_PRIVATE_MEMBER_FUNCTION
#    define IUTEST_HAS_PEEP_FUNC	1
#  else
#    define IUTEST_HAS_PEEP_FUNC	0
#  endif
#endif

#ifndef IUTEST_HAS_PEEP_STATIC_FUNC
//! private static メンバー関数へのアクセスが可能かどうか
#  if IUTEST_HAS_PEEP && IUTEST_EXPLICIT_INSTANTIATION_ACCESS_PRIVATE_STATIC_MEMBER_FUNCTION
#    define IUTEST_HAS_PEEP_STATIC_FUNC	1
#  else
#    define IUTEST_HAS_PEEP_STATIC_FUNC	0
#  endif
#endif

#ifndef IUTEST_HAS_STATIC_ASSERT_TYPEEQ
//! StaticAssertTypeEq が使用可能かどうか
#if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
#    define IUTEST_HAS_STATIC_ASSERT_TYPEEQ		1
#  else
#    define IUTEST_HAS_STATIC_ASSERT_TYPEEQ		0
#  endif
#endif

#ifndef IUTEST_HAS_PRINT_TO
//! PrintTo が使用可能かどうか
#  if !defined(IUTEST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(IUTEST_NO_FUNCTION_TEMPLATE_ORDERING)
#    define IUTEST_HAS_PRINT_TO		1
#  else
#    define IUTEST_HAS_PRINT_TO		0
#  endif
#endif

#ifndef IUTEST_HAS_ASSERTION_RETURN
//! ::iutest::AssertionReturn が使用可能かどうか
#  if !defined(IUTEST_NO_FUNCTION_TEMPLATE_ORDERING)
#    define IUTEST_HAS_ASSERTION_RETURN		1
#  else
#    define IUTEST_HAS_ASSERTION_RETURN		0
#  endif
#endif

#ifndef IUTEST_HAS_TESTNAME_ALIAS
//! テスト名の別名指定記法が使用できるかどうか
#  if !defined(IUTEST_NO_VARIADIC_MACROS)
#    define IUTEST_HAS_TESTNAME_ALIAS		1
#  else
#    define IUTEST_HAS_TESTNAME_ALIAS		0
#  endif
#endif

#ifndef IUTEST_HAS_TESTNAME_ALIAS_JP
//! テスト名の別名に日本語が使用できるかどうか
#  if IUTEST_HAS_TESTNAME_ALIAS
#    define IUTEST_HAS_TESTNAME_ALIAS_JP	1
#  else
#    define IUTEST_HAS_TESTNAME_ALIAS_JP	0
#  endif
#endif

#ifndef IUTEST_HAS_ANY_PARAM_TEST
//! any を利用した値のパラメータ化テストが使用可能かどうか
#  if IUTEST_HAS_PARAM_TEST && IUTEST_HAS_TESTNAME_ALIAS
#    define IUTEST_HAS_ANY_PARAM_TEST		1
#  else
#    define IUTEST_HAS_ANY_PARAM_TEST		0
#  endif
#endif


/**
 * @}
*/


/**
 * @defgroup	IUTEST_CONFIG_FILE	FILE
 * @brief		ファイル関係 CONFIG
 * @{
*/

// file
#ifndef IUTEST_HAS_FOPEN
#  define IUTEST_HAS_FOPEN			1	//!< fopen が使用可能かどうか
#endif

#ifndef IUTEST_HAS_FILE_STAT
#  define IUTEST_HAS_FILE_STAT		0	//!< stat が使用可能かどうか
#endif

/**
 * @}
*/


/**
 * @defgroup	IUTEST_CONFIG_TIME	TIME
 * @brief		TIME関係 CONFIG
 * @{
*/

//! time 関数が使用可能かどうか
#ifndef IUTEST_HAS_CTIME
#  if	defined(IUTEST_OS_WINDOWS_MOBILE)
#    define IUTEST_HAS_CTIME		0
#  else
#    define IUTEST_HAS_CTIME		1
#  endif
#endif

//! gettimeofday 関数が使用可能かどうか
#ifndef IUTEST_HAS_GETTIMEOFDAY
#  if IUTEST_HAS_HDR_SYSTIME
#    define IUTEST_HAS_GETTIMEOFDAY	1
#  endif
#endif

#ifndef IUTEST_HAS_GETTIMEOFDAY
#  define IUTEST_HAS_GETTIMEOFDAY	0
#endif

//! clock 関数が使用可能かどうか
#ifndef IUTEST_HAS_CLOCK
#  if defined(CLOCKS_PER_SEC)
#    define IUTEST_HAS_CLOCK		1
#  endif
#endif

#ifndef IUTEST_HAS_CLOCK
#  define IUTEST_HAS_CLOCK			0
#endif

/**
 * @}
*/


/**
 * @defgroup	IUTEST_CONFIG_OTHER	OTHER
 * @brief		その他の CONFIG
 * @{
*/

#ifndef IUTEST_HAS_MINIDUMP
//! MiniDump 出力が有効かどうか
#  if defined(_MSC_VER) && _MSC_VER >= 1310 && IUTEST_HAS_EXCEPTIONS && IUTEST_HAS_SEH
#    define IUTEST_HAS_MINIDUMP		1
#  else
#    define IUTEST_HAS_MINIDUMP		0
#  endif
#endif

#ifndef IUTEST_HAS_SOCKET
//! ソケット通信可能かどうか
#  if defined(IUTEST_OS_WINDOWS)
#    if !defined(_MSC_VER) || _MSC_VER >= 1310
#      define IUTEST_HAS_SOCKET	1
#    endif
#  elif defined(IUTEST_OS_CYGWIN) || defined(IUTEST_OS_LINUX)
#    define IUTEST_HAS_SOCKET	1
#  else
#    define IUTEST_HAS_SOCKET	0
#  endif
#endif

#ifndef IUTEST_HAS_STRINGSTREAM
//! std::stringstream が使用可能かどうか
#  if !defined(_STLP_NO_IOSTREAMS)
#    define IUTEST_HAS_STRINGSTREAM	1	
#  else
#    define IUTEST_HAS_STRINGSTREAM	0
#  endif
#endif

/**
 * @brief	std::strstream が使用可能かどうか
 * @note	IUTEST_HAS_STRINGSTREAM が優先されます
*/
#ifndef IUTEST_HAS_STRSTREAM
#  define IUTEST_HAS_STRSTREAM		0
#endif

/**
 * @}
*/

/**
 * @defgroup	IUTEST_CONFIG_LISTENER	LISTENER
 * @brief		Listener 関係の CONFIG
 * @{
*/

#ifndef IUTEST_HAS_STREAM_RESULT
//! stream result が使用可能かどうか
#  if IUTEST_HAS_SOCKET
#    define IUTEST_HAS_STREAM_RESULT	1
#  else
#    define IUTEST_HAS_STREAM_RESULT	0
#  endif
#endif

/**
 * @}
*/

// Doxygen 用
#ifdef IUTEST_BUILD_DOXYGEN

/**
 * @ingroup	IUTEST_CONFIG_TIME	TIME
 * @{
*/

#define IUTEST_GetMillisec()	//!< 現在時刻のミリ秒取得関数を独自定義

/**
* @}
*/


/**
 * @ingroup	IUTEST_CONFIG_OTHER	OTHER
 * @{
*/

/**
 * @brief	iutest で t1/tuple.h をインクルードするかどうか
 * @details	1: std::tr1 名前空間にある tuple を使用します。
 *			tuple.h はユーザーがインクルードします。
 *			0: 指定なし iutest が自動で判断します。
*/
#define IUTEST_USE_EXTERNAL_TR1_TUPLE	0

/**
 * @brief	iutest で tuple.h をインクルードするかどうか
 * @details	1: std 名前空間にある tuple を使用します。
 *			tuple.h はユーザーがインクルードします。
 *			0: 指定なし iutest が自動で判断します。
*/
#define IUTEST_USE_EXTERNAL_STD_TUPLE	0

/**
 * @brief	iutest で使用する乱数エンジンを指定します
 * @details	未定義の場合は iutest が実装しているエンジンを使用します(XorShift 128)
*/
#define IUTEST_USE_RANDOM_ENGINE_TYPENAME	::std::default_random_engine

#define IUTEST_NO_GETENV	//!< getenv 関数がない場合は定義
#define IUTEST_NO_PUTENV	//!< putenv 関数がない場合は定義
#define IUTEST_NO_GETCWD	//!< getcwd 関数がない場合は定義

/**
 * @}
*/

#define IUTEST_USE_LIB		//!< ライブラリ版を使用する
#define IUTEST_AUTO_LINK	//!< ライブラリ版を使用し、自動リンクする

#endif

/**
 * @}
*/

/**
 * @private
 * @{
*/

// include only
#if IUTEST_HAS_LIB
#  define IUTEST_IPP_INLINE
#else
#  define IUTEST_IPP_INLINE		inline
#endif

// auto link
#if IUTEST_HAS_LIB && defined(IUTEST_AUTO_LINK)

#if defined(_MSC_VER)

#if defined(_MT) || defined(__MT__)
#  if	defined(_DLL)
#    define IUTEST_LIB_THREAD_OPT	"md"
#  else
#    define IUTEST_LIB_THREAD_OPT	"mt"
#  endif
#else
#  define IUTEST_LIB_THREAD_OPT	""
#endif

#if		_MSC_VER == 1400
#  define IUTEST_LIB_TOOLSET	"vc80"
#elif	_MSC_VER == 1500
#  define IUTEST_LIB_TOOLSET	"vc90"
#elif	_MSC_VER == 1600
#  define IUTEST_LIB_TOOLSET	"vc100"
#elif	_MSC_VER == 1700
#  define IUTEST_LIB_TOOLSET	"vc110"
#elif	_MSC_VER == 1800
#  define IUTEST_LIB_TOOLSET	"vc120"
#else
#  error unkown _MSC_VER.
#endif

#if		defined(_DEBUG)
#  define IUTEST_LIB_RT_OPT		"d"
#else
#  define IUTEST_LIB_RT_OPT		""
#endif

#define IUTEST_LIB_NAME(name)	#name "-" IUTEST_LIB_TOOLSET "-" IUTEST_LIB_THREAD_OPT IUTEST_LIB_RT_OPT ".lib"

#pragma comment(lib, IUTEST_LIB_NAME(libiutest) )

#endif

#endif

/**
 * @}
*/

#endif
