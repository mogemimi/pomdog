//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PARTICLE_3943827E_5585_430A_B0B0_51463FA3DA07_HPP
#define POMDOG_PARTICLE_3943827E_5585_430A_B0B0_51463FA3DA07_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Pomdog/Math/Vector2.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Radian.hpp"
#include <cstdint>

namespace Pomdog {

class Particle {
public:
	Vector2 Position {0, 0};
	Vector2 Velocity {1, 1};

	Color StartColor = Color::White;
	Color Color = Color::White;
	float ColorVariance = 0;

	Radian<float> Rotation = 0;
	float RotationVariance = 0;

	float StartSize = 1;
	float Size = 1;
	float SizeVariance = 0;

	float TimeToLive = 2;
};

}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLE_3943827E_5585_430A_B0B0_51463FA3DA07_HPP)
