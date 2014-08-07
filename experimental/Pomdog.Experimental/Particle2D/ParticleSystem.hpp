//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PARTICLESYSTEM_2AEA7BC5_5CCE_4688_8FAC_D26A97A8F8B7_HPP
#define POMDOG_PARTICLESYSTEM_2AEA7BC5_5CCE_4688_8FAC_D26A97A8F8B7_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include "Particle.hpp"
#include "ParticleEmitter.hpp"
#include <Pomdog.Experimental/Gameplay2D/Transform2D.hpp>
#include <Pomdog/Pomdog.hpp>
#include <cstdint>
#include <vector>
#include <random>

namespace Pomdog {

class ParticleSystem {
public:
	ParticleSystem();

	void Update(DurationSeconds const& frameDuration, Transform2D const& emitterTransform);
	
public:
	ParticleEmitter emitter;
	std::vector<Particle> particles;
	bool enableEmission;
	
	void ResetEmission()
	{
		erapsedTime = DurationSeconds{0};
	}
	
private:
	DurationSeconds erapsedTime;
	DurationSeconds emissionTimer;
	std::mt19937 random;
};

}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLESYSTEM_2AEA7BC5_5CCE_4688_8FAC_D26A97A8F8B7_HPP)
