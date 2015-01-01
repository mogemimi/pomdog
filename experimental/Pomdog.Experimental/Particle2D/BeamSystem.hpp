//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_BEAMSYSTEM_616FE7EE_4C71_411A_A44D_C50248FB7B7E_HPP
#define POMDOG_BEAMSYSTEM_616FE7EE_4C71_411A_A44D_C50248FB7B7E_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "Beam.hpp"
#include "BeamEmitter.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform2D.hpp"
#include <Pomdog/Pomdog.hpp>
#include <cstdint>
#include <vector>
#include <random>

namespace Pomdog {

class BeamSystem {
public:
	BeamSystem();

	void Update(DurationSeconds const& frameDuration, Transform2D const& emitterTransform, Vector2 const& target);
	
	static Vector2 CreateTarget(Transform2D const& emitterTransform, float distance);
	
public:
	BeamEmitter emitter;
	BeamBranching branching;
	
	std::vector<Beam> beams;
	
private:
	DurationSeconds erapsedTime;
	DurationSeconds emissionTimer;
	std::mt19937 random;
};

}// namespace Pomdog

#endif // !defined(POMDOG_BEAMSYSTEM_616FE7EE_4C71_411A_A44D_C50248FB7B7E_HPP)
