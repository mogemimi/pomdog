//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ASSERT_H
#define POMDOG_ASSERT_H

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "../Config/Platform.hpp"
#include <cassert>

#if defined(POMDOG_COMPILER_MSVC)
#	include <xutility>
#endif

namespace Pomdog {
namespace Details {

/// @addtogroup Framework
/// @{
/// @addtogroup Utility
/// @{
/// @addtogroup Preprocessor
/// @{

// @code
// //how to use:
// POMDOG_ASSERT(expr);
// POMDOG_ASSERT_MESSAGE(expr, "message");
// @endcode

#if defined(POMDOG_COMPILER_MSVC)
#	// MSVC	
#	define POMDOG_REPORT_ASSERTION_FAILURE(condition, message) \
		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW); \
		_ASSERT(message && condition)
#else
#	// std assertion failure
#	define POMDOG_REPORT_ASSERTION_FAILURE(condition, message) \
		assert(message && condition)
#endif

#if defined(DEBUG)
#	// Debug mode
#	if defined(_MSC_VER)
#		define POMDOG_DEBUGBREAK() __debugbreak()
#	elif defined(linux) || defined(__linux) || defined(__linux__)
#		define POMDOG_DEBUGBREAK() raise(SIGTRAP)
#	elif defined(POMDOG_ARCHITECTURE_POWERPC)
#		define define POMDOG_DEBUGBREAK() asm {trap}
#	else
		// MS-style inline assembly:
#		define POMDOG_DEBUGBREAK() __asm { int 3 }
#	endif
#	define POMDOG_ASSERT(condition) POMDOG_ASSERT_MESSAGE(condition, "POMDOG_ASSERT")
#	define POMDOG_ASSERT_MESSAGE(condition, message) \
		if (!(condition))\
		{\
			POMDOG_REPORT_ASSERTION_FAILURE(condition, message);\
			POMDOG_DEBUGBREAK(); \
		}
#else //!defined(DEBUG) || defined(NDEBUG)
#	// Release mode
#	define POMDOG_DEBUGBREAK()
#	define POMDOG_ASSERT(condition)
#	define POMDOG_ASSERT_MESSAGE(condition, message)
#endif

/// @}
/// @}
/// @}

}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ASSERT_H)
