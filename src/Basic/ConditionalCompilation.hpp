// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

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
// CPU architecture detection
//----------------------------
#if defined(_M_IX86) || defined(__i386__)
#   define POMDOG_ARCHITECTURE_X86
#elif defined(_M_IA64) || defined(__ia64__)
#   define POMDOG_ARCHITECTURE_IA64
#elif defined(_M_AMD64) || defined(__amd64__)
#   define POMDOG_ARCHITECTURE_AMD64
#elif defined(_M_ARM) || defined(__arm__)
#   define POMDOG_ARCHITECTURE_ARM
#elif defined(__aarch64__) || defined(__arm64__)
#   define POMDOG_ARCHITECTURE_ARM64
#elif defined(__powerpc64__)
#   define POMDOG_ARCHITECTURE_POWERPC64
#elif defined(_M_PPC) || defined(__ppc__)
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

//-------------------------------------------------------------
// Check the minimum version of the target platform required.
//-------------------------------------------------------------
#ifdef __APPLE_CC__
#include <AvailabilityMacros.h>
#include <TargetConditionals.h>
#endif

// Specify the minimum supported iOS version
#if defined(__APPLE_CC__) && defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
    #if (__IPHONE_OS_VERSION_MIN_REQUIRED < 90300)
        #error "Minimum required OS X version: Apple iOS 9.3 or later"
    #endif
#endif

// Specify the minimum supported OS X version
#if defined(__APPLE_CC__) && defined(TARGET_OS_MAC) && TARGET_OS_MAC
    #if (MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_14)
        #error "Minimum required OS X version: macOS 10.14 or later"
    #endif
#endif

//----------------------------------
// Support for C++14 features
//----------------------------------
#if defined(__clang__) && defined(__has_feature)
#if __has_feature(cxx_lambdas) \
    && __has_feature(cxx_nullptr) \
    && __has_feature(cxx_static_assert) \
    && __has_feature(cxx_strong_enums) \
    && __has_feature(cxx_defaulted_functions) \
    && __has_feature(cxx_rvalue_references)
// C++14 support in Clang
// See http://clang.llvm.org/docs/LanguageExtensions.html
#else
#error "C++14 is not supported on this older version of clang."
#endif
#elif defined(_MSC_VER) && (_MSC_VER >= 1900)
// C++14 support in MSVC
// See https://msdn.microsoft.com/en-us/library/hh567368.aspx
#elif defined(__GNUC__) && (__cplusplus >= 201402L)
// C++14 support in GCC
// See https://gcc.gnu.org/projects/cxx-status.html#cxx14
#else
#error "C++14 is not supported."
#endif

//----------------------------
// Build configuration
//----------------------------
#if defined(DEBUG) && defined(NDEBUG)
#error "Both DEBUG and NDEBUG are defined."
#endif
