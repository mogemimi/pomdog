//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PARTICLEEMITTERSHAPEBOX_21E87DCC_D7C6_4395_9858_0E3B76B20A5E_HPP
#define POMDOG_PARTICLEEMITTERSHAPEBOX_21E87DCC_D7C6_4395_9858_0E3B76B20A5E_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "ParticleEmitterShape.hpp"
#include <random>

namespace Pomdog {
namespace Details {
namespace Particles {

class ParticleEmitterShapeBox final: public ParticleEmitterShape {
private:
	float width;
	float height;
	
public:
	ParticleEmitterShapeBox(float widthIn, float heightIn)
		: width(widthIn)
		, height(heightIn)
	{}

	void Compute(std::mt19937 & random, Vector2 & emitPosition, Radian<float> & emitAngle) const override
	{
		POMDOG_ASSERT(width >= 0.0f);
		POMDOG_ASSERT(height >= 0.0f);
		
		std::uniform_real_distribution<float> distribution(-0.5f, 0.5f);
		emitPosition.X = width * distribution(random);
		emitPosition.Y = height * distribution(random);
		emitAngle = 0.0f;
	}
};

}// namespace Particles
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEEMITTERSHAPEBOX_21E87DCC_D7C6_4395_9858_0E3B76B20A5E_HPP)
