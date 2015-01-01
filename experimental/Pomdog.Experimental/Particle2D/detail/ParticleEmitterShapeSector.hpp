//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PARTICLEEMITTERSHAPESECTOR_020ABE98_8102_40BE_8EDF_0887FC0D95FD_HPP
#define POMDOG_PARTICLEEMITTERSHAPESECTOR_020ABE98_8102_40BE_8EDF_0887FC0D95FD_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "ParticleEmitterShape.hpp"
#include <random>

namespace Pomdog {
namespace Details {
namespace Particles {

class ParticleEmitterShapeSector final: public ParticleEmitterShape {
private:
	///@note range: [0, 2*Pi]
	Radian<float> centralAngle;
	
public:
	explicit ParticleEmitterShapeSector(Radian<float> && centralAngleIn)
		: centralAngle(std::move(centralAngleIn))
	{}

	void Compute(std::mt19937 & random, Vector2 & emitPosition, Radian<float> & emitAngle) const override
	{
		emitPosition = {0, 0};
		
		std::uniform_real_distribution<float> distribution(-0.5f, 0.5f);
		emitAngle = centralAngle * distribution(random);
	}
};

}// namespace Particles
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEEMITTERSHAPESECTOR_020ABE98_8102_40BE_8EDF_0887FC0D95FD_HPP)
