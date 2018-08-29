// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

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
                assert(message && (expression));\
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
#    elif defined(__ppc__) || defined(__powerpc__)
#        define POMDOG_DEBUGBREAK() asm {trap}
#    else
#        define POMDOG_DEBUGBREAK() asm("int $3")
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

} // namespace Detail
} // namespace Pomdog
