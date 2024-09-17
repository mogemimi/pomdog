// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/basic/compilation_targets.h"
#include "pomdog/basic/conditional_compilation.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#ifdef __APPLE_CC__
#include <AvailabilityMacros.h>
#include <TargetConditionals.h>
#endif
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

//---------------------------------------------------------------------------
// NOTE: Compiler detection
//---------------------------------------------------------------------------
#if defined(_MSC_VER)
// NOTE: cl (Visual C++ Compiler)
#elif defined(__clang__)
// NOTE: clang++ (LLVM Clang)
#elif defined(__GNUC__)
// NOTE: g++ (GNU Compiler Collection)
#elif defined(__INTEL_COMPILER)
// NOTE: icc (Intel C++ Compiler)
#elif defined(__BORLANDC__) && defined(__BCPLUSPLUS__)
// NOTE: bcc (Borland C++ Compiler)
#else
#error "Compiler undefined or not supported."
#endif

//---------------------------------------------------------------------------
// NOTE: How to check C++ compiler versions
//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
// Support for C++14 features
//---------------------------------------------------------------------------
#if defined(__clang__) && defined(__has_feature)
#if __has_feature(cxx_lambdas) && \
    __has_feature(cxx_nullptr) && \
    __has_feature(cxx_static_assert) && \
    __has_feature(cxx_strong_enums) && \
    __has_feature(cxx_defaulted_functions) && \
    __has_feature(cxx_rvalue_references)
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

//---------------------------------------------------------------------------
// NOTE: Check the minimum version of the target platform required.
//---------------------------------------------------------------------------

// NOTE: Specify the minimum supported iOS version
#if defined(__APPLE_CC__) && defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE
#if (__IPHONE_OS_VERSION_MIN_REQUIRED < 90300)
#error "Minimum required OS X version: Apple iOS 9.3 or later"
#endif
#endif

// NOTE: Specify the minimum supported OS X version
#if defined(__APPLE_CC__) && defined(TARGET_OS_MAC) && TARGET_OS_MAC
#if (MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_VERSION_11_0)
#error "Minimum required OS X version: macOS 11.0 or later"
#endif
#endif

#if defined(DEBUG) && defined(NDEBUG)
#error "Both DEBUG and NDEBUG are defined."
#endif

} // namespace pomdog
