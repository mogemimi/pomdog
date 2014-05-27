//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PARTICLE_3943827E_5585_430A_B0B0_51463FA3DA07_HPP
#define POMDOG_PARTICLE_3943827E_5585_430A_B0B0_51463FA3DA07_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <Pomdog/Pomdog.hpp>

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
