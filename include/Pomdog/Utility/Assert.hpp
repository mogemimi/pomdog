// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ASSERT_7D111D58_HPP
#define POMDOG_ASSERT_7D111D58_HPP

#include "Pomdog/Basic/Platform.hpp"
#include <cassert>

#if defined(_MSC_VER)
#include <xutility>
#endif
#if defined(linux) || defined(__linux) || defined(__linux__)
#include <csignal>
#endif

namespace Pomdog {
namespace Detail {

// How to use:
// POMDOG_ASSERT(expr);
// POMDOG_ASSERT_MESSAGE(expr, "message");

#if defined(DEBUG) && defined(__APPLE_CC__)
#    // Debug mode under Xcode
#    define POMDOG_ASSERT(expression) assert(expression)
#    define POMDOG_ASSERT_MESSAGE(expression, message) \
        do {\
            if (!(expression)) {\
                assert(message && expression);\
            }\
        } while(false)
#elif defined(DEBUG) && defined(_MSC_VER)
#    // Debug mode under Visual Studio
#    define POMDOG_ASSERT(expression) \
        static_cast<void>((!!(expression)) \
            || (_CrtDbgBreak(), _ASSERT(expression), false))
#    define POMDOG_ASSERT_MESSAGE(expression, message) \
        static_cast<void>((!!(expression)) \
            || (1 != _CrtDbgReportW(_CRT_ASSERT, _CRT_WIDE(__FILE__), __LINE__, nullptr, L"%s", message)) \
            || (_CrtDbgBreak(), false))
#elif defined(DEBUG)
#    // Debug mode
#    if defined(_MSC_VER)
#        define POMDOG_DEBUGBREAK() __debugbreak()
#    elif defined(linux) || defined(__linux) || defined(__linux__)
#        define POMDOG_DEBUGBREAK() raise(SIGTRAP)
#    elif defined(POMDOG_ARCHITECTURE_POWERPC)
#        define POMDOG_DEBUGBREAK() asm {trap}
#    else
#        define POMDOG_DEBUGBREAK() __asm { int 3 } // MS-style inline assembly
#    endif
#    define POMDOG_ASSERT(expression) assert(expression)
#    define POMDOG_ASSERT_MESSAGE(expression, message) \
        do {\
            if (!(expression)) {\
                assert(message && expression); \
                POMDOG_DEBUGBREAK(); \
            }\
        } while(false)
#else
#    // Release mode
#    define POMDOG_ASSERT(expression)
#    define POMDOG_ASSERT_MESSAGE(expression, message)
#endif

namespace Assertion {

inline constexpr bool ConstexprAssert(
    bool condition,
    char const* expression)
{
    return (assert(condition), condition);
}

} // namespace Assertion

#if defined(DEBUG)
#   // Debug mode
#   define POMDOG_CONSTEXPR_ASSERT(expression) \
        static_cast<void>(Pomdog::Detail::Assertion::ConstexprAssert( \
            static_cast<bool>(expression), #expression))
#else
#   // Release mode
#   define POMDOG_CONSTEXPR_ASSERT(expression) static_cast<void const>(0)
#endif

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_ASSERT_7D111D58_HPP
