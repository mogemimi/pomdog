// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PRIMITIVETOPOLOGY_46F3C71C_626F_43D4_8CBD_F4FC28EE1933_HPP
#define POMDOG_PRIMITIVETOPOLOGY_46F3C71C_626F_43D4_8CBD_F4FC28EE1933_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class PrimitiveTopology: std::uint8_t {
	///@brief A triangle list.
	TriangleList,

	///@brief A triangle strip.
	TriangleStrip,

	///@brief A line list.
	LineList,

	///@brief A line strip.
	LineStrip,
};

}// namespace Pomdog

#endif // !defined(POMDOG_PRIMITIVETOPOLOGY_46F3C71C_626F_43D4_8CBD_F4FC28EE1933_HPP)
