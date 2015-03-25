// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_COMPARISONFUNCTION_01201CF2_CA64_4AC9_850E_5AA2C4CDC7F6_HPP
#define POMDOG_COMPARISONFUNCTION_01201CF2_CA64_4AC9_850E_5AA2C4CDC7F6_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

enum class ComparisonFunction: std::uint8_t {
	Never,

	Less,

	Equal,

	LessEqual,

	Greater,

	NotEqual,

	GreaterEqual,

	Always,
};

}// namespace Pomdog

#endif // !defined(POMDOG_COMPARISONFUNCTION_01201CF2_CA64_4AC9_850E_5AA2C4CDC7F6_HPP)
