//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_BEAM_463AA6E5_5F50_4C38_B5F1_5722855B9805_HPP
#define POMDOG_BEAM_463AA6E5_5F50_4C38_B5F1_5722855B9805_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <vector>
#include <Pomdog/Math/Vector2.hpp>
#include <Pomdog/Math/Color.hpp>

namespace Pomdog {

class Beam {
public:
	std::vector<Vector2> JaggedLine;
	Color Color;
	float TimeToLive;
};

}// namespace Pomdog

#endif // !defined(POMDOG_BEAM_463AA6E5_5F50_4C38_B5F1_5722855B9805_HPP)
