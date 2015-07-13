// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PLATFORM_C59B59BE_HPP
#define POMDOG_PLATFORM_C59B59BE_HPP

//----------------------------
// Compiler detection
//----------------------------
#if defined(_MSC_VER)
#define POMDOG_COMPILER_MSVC
#elif defined(__clang__)
#define POMDOG_COMPILER_CLANG
#elif defined(__GNUC__)
#define POMDOG_COMPILER_GNUC
#elif defined(__BORLANDC__) && defined(__BCPLUSPLUS__)
#define POMDOG_COMPILER_BORLAND
#else
#error "Compiler undefined or not supported."
#endif

// # NOTE: How to check C++ compiler versions
//
// ## Visual Studio 2015 (MSVC++ 14.0)
// _MSC_VER == 1900
//
// ## Clang 3.6.0
// ((__clang_major__*100) + (__clang_minor__*10) + __clang_patchlevel__)
// == 360
//
// ## GCC 4.9.2
// ((__GNUC__*100) + (__GNUC_MINOR__*10) + __GNUC_PATCHLEVEL__)
// == 492
//
// ## C++Builder XE7
// (__CODEGEARC__) == 0x0690

//----------------------------
// Platform detection
//----------------------------
#ifdef __APPLE_CC__
#include <Availability.h>
#endif

#if defined(linux) || defined(__linux) || defined(__linux__)
#   // Linux
#   define POMDOG_PLATFORM_LINUX
#elif defined(__PS4__) && defined(__FreeBSD__)
#   // PlayStation 4
#   define POMDOG_PLATFORM_PLAYSTATION4
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#   // BSD
#   define POMDOG_PLATFORM_BSD
#elif defined(_XBOX_ONE) && defined(_TITLE)
#   // Xbox One
#   define POMDOG_PLATFORM_XBOX_ONE
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#   // Windows
#   define POMDOG_PLATFORM_WIN32
#elif defined(ANDROID) || defined(__ANDROID__)
#   // Android OS
#   define POMDOG_PLATFORM_ANDROID
#elif defined(__APPLE_CC__) && (__IPHONE_OS_VERSION_MIN_REQUIRED >= 80000)
#   // Apple iOS 8.0 and later
#   define POMDOG_PLATFORM_APPLE_IOS
#elif defined(__APPLE_CC__) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_9)
#   // Mac OS X 10.9 and later
#   define POMDOG_PLATFORM_MACOSX
#elif defined(__QNXNTO__)
#   // QNX Neutrino
#   define POMDOG_PLATFORM_QNXNTO
#elif defined(__native_client__)
#   // Google Native Client (NaCl)
#   define POMDOG_PLATFORM_NACL
#elif defined(__CYGWIN__)
#   // Cygwin
#   define POMDOG_PLATFORM_CYGWIN
#elif defined(sun) || defined(__sun)
#   // Solaris
#   define POMDOG_PLATFORM_SOLARIS
#elif defined(__hpux)
#   // Hewlett-Packard UNIX
#   define POMDOG_PLATFORM_HPUX
#elif defined(__ghs__)
#   // Wii U
#   define POMDOG_PLATFORM_WIIU
#else
#   error "Sorry, this platform is not supported."
#endif

//----------------------------
// CPU architecture detection
//----------------------------
#if defined(_M_IX86) || defined(__i386__) || defined(_X86_)
#   define POMDOG_ARCHITECTURE_X86
#elif defined(_M_IA64) || defined(__ia64__) || defined(_IA64)
#   define POMDOG_ARCHITECTURE_IA64
#elif defined(_M_X64) || defined(__x86_64__) || defined(_AMD64_)
#   define POMDOG_ARCHITECTURE_AMD64
#elif defined(_M_ARM) || defined(__arm__) || defined(_ARM_)
#   define POMDOG_ARCHITECTURE_ARM
#elif defined(__powerpc64__)
#   define POMDOG_ARCHITECTURE_POWERPC64
#elif defined(__ppc__) || defined(__powerpc__)
#   define POMDOG_ARCHITECTURE_POWERPC
#elif defined(_POWER) || defined(_ARCH_PWR) || defined(_ARCH_COM)
#   define POMDOG_ARCHITECTURE_POWER
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__)
#   define POMDOG_ARCHITECTURE_MIPS
#elif defined(__alpha__) || defined(__alpha)
#   define POMDOG_ARCHITECTURE_ALPHA
#else
#   error "Cannot detect CPU architecture."
#endif

//----------------------------
// Detect byte order
//----------------------------
#if defined(POMDOG_ARCHITECTURE_X86)
#   define POMDOG_BYTEORDER_LITTLE_ENDIAN
#elif defined(POMDOG_ARCHITECTURE_AMD64)
#   define POMDOG_BYTEORDER_LITTLE_ENDIAN
#elif defined(POMDOG_ARCHITECTURE_IA64) && (defined(_hpux) || defined(hpux))
#   define POMDOG_BYTEORDER_BIG_ENDIAN
#elif defined(POMDOG_ARCHITECTURE_IA64)
#   define POMDOG_BYTEORDER_LITTLE_ENDIAN
#elif defined(__ARMEB__)
#   define POMDOG_BYTEORDER_BIG_ENDIAN
#elif defined(__ARMEL__)
#   define POMDOG_BYTEORDER_LITTLE_ENDIAN
#elif defined(POMDOG_ARCHITECTURE_POWERPC)
#   define POMDOG_BYTEORDER_BIG_ENDIAN
#elif defined(POMDOG_ARCHITECTURE_POWER)
#   define POMDOG_BYTEORDER_BIG_ENDIAN
#elif defined(MIPSEB) || defined(_MIPSEB) || defined(__MIPSEB)
#   define POMDOG_BYTEORDER_BIG_ENDIAN
#elif defined(MIPSEL) || defined(_MIPSEL) || defined(__MIPSEL)
#   define POMDOG_BYTEORDER_LITTLE_ENDIAN
#elif defined(POMDOG_ARCHITECTURE_ALPHA)
#   define POMDOG_BYTEORDER_LITTLE_ENDIAN
#elif defined(__LITTLE_ENDIAN__)
#   define POMDOG_BYTEORDER_LITTLE_ENDIAN
#elif defined(__BIG_ENDIAN__)
#   define POMDOG_BYTEORDER_BIG_ENDIAN
#elif defined(__BYTE_ORDER) && (__BYTE_ORDER == __LITTLE_ENDIAN)
#   define POMDOG_BYTEORDER_LITTLE_ENDIAN
#elif defined(__BYTE_ORDER) && (__BYTE_ORDER == __BIG_ENDIAN)
#   define POMDOG_BYTEORDER_BIG_ENDIAN
#else
#   error "Byte order undefined or not supported."
#endif

//----------------------------------
// Support for C++11/C++14 features
//----------------------------------
#ifndef __has_feature
// Compatibility with non-clang compilers.
#define __has_feature(x) 0
#endif

#ifndef __has_extension
// Compatibility with pre-3.0 compilers.
#define __has_extension __has_feature
#endif

#if defined(POMDOG_COMPILER_CLANG) \
    && __has_feature(cxx_lambdas) \
    && __has_feature(cxx_nullptr) \
    && __has_feature(cxx_static_assert) \
    && __has_feature(cxx_strong_enums) \
    && __has_feature(cxx_defaulted_functions) \
    && __has_feature(cxx_rvalue_references)
// C++11 support in Clang
// See http://clang.llvm.org/docs/LanguageExtensions.html
#elif defined(POMDOG_COMPILER_MSVC) && (_MSC_VER >= 1900)
// C++11 support in MSVC
// See https://msdn.microsoft.com/en-us/library/hh567368.aspx
#elif defined(POMDOG_COMPILER_GNUC) && (__cplusplus >= 201103L)
// C++11 support in GCC
// See https://gcc.gnu.org/projects/cxx1y.html
#else
#error "C++11 is not supported."
#endif

//----------------------------
// Build configuration
//----------------------------
#if defined(DEBUG) && defined(NDEBUG)
#error "Both DEBUG and NDEBUG are defined."
#endif

#endif // POMDOG_PLATFORM_C59B59BE_HPP
