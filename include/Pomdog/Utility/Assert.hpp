//
//  Copyright (C) 2013-2014 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_ASSERT_7D111D58_5951_4927_A438_EF5F9E62DDFE_HPP
#define POMDOG_ASSERT_7D111D58_5951_4927_A438_EF5F9E62DDFE_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Pomdog/Config/Platform.hpp"
#include <cassert>

#ifdef POMDOG_COMPILER_MSVC
#	include <xutility>
#endif

namespace Pomdog {
namespace Details {
namespace Assertion {

//inline void ReportAssertionFailure(char const* expression,
//	char const* filename, int line, char const* function){
//	// notes: std::cerr in <iostream>
//	std::cerr << "Assertion failed: " << expression << ", function "
//		<< function << ", file " << filename << ", line " << line << ".\n";
//}

inline void RuntimeAssertion(char const* /*expression*/, char const* /*filename*/, int /*line*/)
{
	//ReportAssertionFailure(expression, filename, line, "(unknown)");
	assert(false);
}

inline constexpr bool ConstexprAssert(bool condition,
	char const* expression, char const* filename, int line) {
	return condition ? true: (RuntimeAssertion(expression, filename, line), false);
}

}// namespace Assertion

// @code
// //how to use:
// POMDOG_ASSERT(expr);
// POMDOG_ASSERT_MESSAGE(expr, "message");
// @endcode

#if defined(DEBUG) && defined(__APPLE_CC__)
#	// Debug mode for Xcode 5
#	define POMDOG_ASSERT(expression) POMDOG_ASSERT_MESSAGE(expression, "POMDOG_ASSERT")
#	define POMDOG_ASSERT_MESSAGE(expression, message) \
		do {\
			if (!(expression)) {\
				assert(message && expression);\
			}\
		} while(false)
#	define POMDOG_CONSTEXPR_ASSERT(expression) \
		static_cast<void>(Pomdog::Details::Assertion::ConstexprAssert(\
			static_cast<bool>(expression), #expression, __FILE__, __LINE__))
#elif defined(DEBUG) && defined(_MSC_VER)
#	// Debug mode under Visual Studio
#	define POMDOG_ASSERT(expression) _ASSERT(expression)
#	define POMDOG_ASSERT_MESSAGE(expression, message) \
		static_cast<void>((!!(expression)) \
			|| (1 != _CrtDbgReportW(_CRT_ASSERT, _CRT_WIDE(__FILE__), __LINE__, nullptr, L"%s", message)) \
			|| (_CrtDbgBreak(), false))
#elif defined(DEBUG)
#	// Debug mode
#	if defined(_MSC_VER)
#		define POMDOG_DEBUGBREAK() __debugbreak()
#	elif defined(linux) || defined(__linux) || defined(__linux__)
#		define POMDOG_DEBUGBREAK() raise(SIGTRAP)
#	elif defined(POMDOG_ARCHITECTURE_POWERPC)
#		define POMDOG_DEBUGBREAK() asm {trap}
#	else
#		define POMDOG_DEBUGBREAK() __asm { int 3 } // MS-style inline assembly
#	endif
#	define POMDOG_ASSERT(expression) POMDOG_ASSERT_MESSAGE(expression, "POMDOG_ASSERT")
#	define POMDOG_ASSERT_MESSAGE(expression, message) \
		do {\
			if (!(expression)) {\
				assert(message && expression) \
				POMDOG_DEBUGBREAK(); \
			}\
		} while(false)
#else //!defined(DEBUG) || defined(NDEBUG)
#	// Release mode
#	define POMDOG_ASSERT(expression)
#	define POMDOG_ASSERT_MESSAGE(expression, message)
#	define POMDOG_CONSTEXPR_ASSERT(expression) static_cast<void const>(0)
#endif

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ASSERT_7D111D58_5951_4927_A438_EF5F9E62DDFE_HPP)
