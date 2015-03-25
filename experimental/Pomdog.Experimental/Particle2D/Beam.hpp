// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BEAM_463AA6E5_5F50_4C38_B5F1_5722855B9805_HPP
#define POMDOG_BEAM_463AA6E5_5F50_4C38_B5F1_5722855B9805_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Color.hpp"
#include <vector>

namespace Pomdog {

class Beam {
public:
	std::vector<Vector2> Points;
	Color Color;
	float Thickness;
	float TimeToLive;
};

}// namespace Pomdog

#endif // !defined(POMDOG_BEAM_463AA6E5_5F50_4C38_B5F1_5722855B9805_HPP)
