//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PLATFORM_C59B59BE_0311_4CB7_96D7_541924F8C06A_HPP
#define POMDOG_PLATFORM_C59B59BE_0311_4CB7_96D7_541924F8C06A_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

namespace Pomdog {
namespace Details {

//----------------------------
// Choose compiler
//----------------------------
#if defined(_MSC_VER)
#   define POMDOG_COMPILER_MSVC
//# define POMDOG_COMPILER_VERSION _MSC_VER
#elif defined(__clang__)
#   define POMDOG_COMPILER_CLANG
//# define POMDOG_COMPILER_VERSION ((__clang_major__*100) + (__clang_minor__*10) + __clang_patchlevel__)
#elif defined(__GNUC__)
#   define POMDOG_COMPILER_GNUC
//# define POMDOG_COMPILER_VERSION ((__GNUC__*100) + (__GNUC_MINOR__*10) + __GNUC_PATCHLEVEL__)
#elid defined(__BORLANDC__) && defined(__BCPLUSPLUS__)
#   define POMDOG_COMPILER_BORLAND
//# define POMDOG_COMPILER_VERSION (__BCPLUSPLUS__)
#else
#	error "Compiler undefined or not supported."
#endif

//----------------------------
// Choose platform
//----------------------------
#if defined(linux) || defined(__linux) || defined(__linux__)
#	// Linux
#	define POMDOG_PLATFORM_LINUX
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#	// BSD
#	define POMDOG_PLATFORM_BSD
#elif defined(_XBOX_ONE) && defined(_TITLE)
#	// Xbox One
#	define POMDOG_PLATFORM_XBOX_ONE
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#	// Windows
#	define POMDOG_PLATFORM_WIN32
#elif defined(ANDROID) || defined(__ANDROID__)
#	// Android OS
#	define POMDOG_PLATFORM_ANDROID
#elif defined(__APPLE_CC__) && ((__IPHONE_OS_VERSION_MIN_REQUIRED >= 60000) || (__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 60000))
#	// Apple iOS 6 or later
#	define POMDOG_PLATFORM_APPLE_IOS
#elif defined(__APPLE_CC__)
#	// Mac OSX
#	define POMDOG_PLATFORM_MACOSX
#elif defined(__QNXNTO__)
#	// QNX Neutrino
#	define POMDOG_PLATFORM_QNXNTO
#elif defined(__native_client__)
#	// Google Native Client(NaCl)
#	define POMDOG_PLATFORM_NACL
#elif defined(__CYGWIN__)
#	// Cygwin
#	define POMDOG_PLATFORM_CYGWIN
#elif defined(sun) || defined(__sun)
#	// Solaris
#	define POMDOG_PLATFORM_SOLARIS
#elif defined(__hpux)
#	// Hewlett-Packard UNIX
#	define POMDOG_PLATFORM_HPUX
#else
#	error "Sorry, this platform is not supported."
#endif

//----------------------------
// Hardware Architecture
//----------------------------
#if defined(_M_IX86) /* msvc */ \
||  defined(_X86_) \
||  defined(__i386__) /* gcc */ \
||  defined(__i386)	/* gcc */ \
||	defined(i386)
#	// x86 processors
#	define POMDOG_ARCHITECTURE_IX86
#elif \
    defined(_M_X64) /* msvc */ \
||  defined(_AMD64_) \
||  defined(__x86_64__) /* 64-bit linux */
#	// x64 processors
#	define POMDOG_ARCHITECTURE_AMD64
#elif \
	defined(_M_IA64) /* msvc */ \
||  defined(__ia64__) \
||  defined(__ia64) \
||  defined(__IA64__) \
||  defined(_IA64) \
||  defined(ia64)
#	// Itanium architecture, 64-bit processors 
#	define POMDOG_ARCHITECTURE_IA64
#elif \
    defined(__arm__) /* gcc */ \
||  defined(__arm) /* gcc */ \
||  defined(_ARM_) /* c compiler */ \
||  defined(ARM) \
||  defined(__ARM__) \
||  defined(_M_ARM) /* windows mobile 5 */
#	// ARM architecture
#	define POMDOG_ARCHITECTURE_ARM
#elif \
    defined(_M_PPC) /* msvc */ \
||  defined(__powerpc__) /* gcc */ \
||  defined(__powerpc) \
||  defined(__POWERPC__) \
||  defined(__PPC) \
||  defined(_ARCH_PPC)
#	///@note __powerpc64__
#	// PowerPC platforms
#	define POMDOG_ARCHITECTURE_POWERPC
#elif \
    defined(_POWER) \
||  defined(_ARCH_PWR) \
||  defined(_ARCH_COM)
#	// Power architecture
#	define POMDOG_ARCHITECTURE_POWER
#elif \
    defined(_M_MRX000) /* msvc */ \
||  defined(__mips__) \
||  defined(__mips) \
||  defined(__MIPS__)
#	// MIPS platforms
#	define POMDOG_ARCHITECTURE_MIPS
#elif \
    defined(_M_ALPHA) /* msvc */ \
||  defined(__alpha__) \
||	defined(__alpha) 
#	// DEC Alpha platforms
#	define POMDOG_ARCHITECTURE_ALPHA
#endif

//----------------------------
// Byte order
//----------------------------
#if defined(POMDOG_ARCHITECTURE_IX86)
#	define POMDOG_BYTEORDER_LITTLE_ENDIAN
#elif defined(POMDOG_ARCHITECTURE_AMD64)
#	define POMDOG_BYTEORDER_LITTLE_ENDIAN
#elif defined(POMDOG_ARCHITECTURE_IA64)
#	if defined(_hpux) || defined(hpux)
#		// HP-UX is big-endian.
#		define POMDOG_BYTEORDER_BIG_ENDIAN
#	else
#		define POMDOG_BYTEORDER_LITTLE_ENDIAN
#	endif
#elif defined(POMDOG_ARCHITECTURE_ARM)
#	if defined(__ARMEB__)
#		// 'EB' is big-endian
#		define POMDOG_BYTEORDER_BIG_ENDIAN
#	else //__ARMEL__
#		define POMDOG_BYTEORDER_LITTLE_ENDIAN
#	endif
#elif defined(POMDOG_ARCHITECTURE_POWERPC)
#	define POMDOG_BYTEORDER_BIG_ENDIAN
#elif defined(POMDOG_ARCHITECTURE_POWER)
#	define POMDOG_BYTEORDER_BIG_ENDIAN
#elif defined(POMDOG_ARCHITECTURE_MIPS)
#	if defined(MIPSEB) || defined(_MIPSEB) || defined(__MIPSEB)
#		define POMDOG_BYTEORDER_BIG_ENDIAN
#	else// MIPSEL, _MIPSEL, __MIPSEL
#		define POMDOG_BYTEORDER_LITTLE_ENDIAN
#	endif
#elif defined(POMDOG_ARCHITECTURE_ALPHA)
#	define POMDOG_BYTEORDER_LITTLE_ENDIAN
#elif defined(POMDOG_COMPILER_GNUC)
#	if (((__GNUC__*100) + (__GNUC_MINOR__*10) + __GNUC_PATCHLEVEL__) >= 310)
#		// include header <endian.h> in the global namespace.
#		if defined (__LITTLE_ENDIAN__) || (__BYTE_ORDER == __LITTLE_ENDIAN)
#			define POMDOG_BYTEORDER_LITTLE_ENDIAN
#		elif defined (__BIG_ENDIAN__) || (__BYTE_ORDER == __BIG_ENDIAN)
#			define POMDOG_BYTEORDER_BIG_ENDIAN
#		endif
#	endif
#else
#	error "Byte order undefined or not supported."
#endif

#if   (defined(POMDOG_BYTEORDER_LITTLE_ENDIAN) && !defined(POMDOG_BYTEORDER_BIG_ENDIAN) && !(POMDOG_BYTEORDER_MIDDLE_ENDIAN))
#elif (!defined(POMDOG_BYTEORDER_LITTLE_ENDIAN) && defined(POMDOG_BYTEORDER_BIG_ENDIAN) && !(POMDOG_BYTEORDER_MIDDLE_ENDIAN))
#elif (!defined(POMDOG_BYTEORDER_LITTLE_ENDIAN) && !defined(POMDOG_BYTEORDER_BIG_ENDIAN) && (POMDOG_BYTEORDER_MIDDLE_ENDIAN))
#else
#	error "Byte order undefined or not supported."
#endif

//----------------------------------
// C++11 support/not supported
//----------------------------------
#ifndef __has_feature         // Optional of course.
#  define __has_feature(x) 0  // Compatibility with non-clang compilers.
#endif
#ifndef __has_extension
#  define __has_extension __has_feature // Compatibility with pre-3.0 compilers.
#endif

#if defined(POMDOG_COMPILER_MSVC) \
	&& (_MSC_VER >= 1600)
#	// Support C++11 under MSVC
#elif defined(POMDOG_COMPILER_CLANG) \
	&& __has_feature(cxx_lambdas) \
	&& __has_feature(cxx_nullptr) \
	&& __has_feature(cxx_static_assert) \
	&& __has_feature(cxx_strong_enums) \
	&& __has_feature(cxx_defaulted_functions)
#	// Support C++11 under Clang++
#	// See also:
#	// http://clang.llvm.org/docs/LanguageExtensions.html#checking_upcoming_features
#elif defined(POMDOG_COMPILER_GNUC) \
	&& defined(__GXX_EXPERIMENTAL_CXX0X__)
	&& __cplusplus >= 201103L
#	// Support C++11 under GCC
#	// See also:
#	// http://gcc.gnu.org/projects/cxx0x.html
#else
#	error "C++11/C++0x is not supported."
#endif

//----------------------------
// debug or release
//----------------------------
#if defined(DEBUG) && defined(NDEBUG)
#	error "Both DEBUG and NDEBUG are defined."
#endif

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_PLATFORM_C59B59BE_0311_4CB7_96D7_541924F8C06A_HPP)
