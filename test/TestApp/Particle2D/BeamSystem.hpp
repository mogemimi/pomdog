//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_BEAMSYSTEM_616FE7EE_4C71_411A_A44D_C50248FB7B7E_HPP
#define POMDOG_BEAMSYSTEM_616FE7EE_4C71_411A_A44D_C50248FB7B7E_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <vector>
#include <random>
#include <Pomdog/Pomdog.hpp>
#include "Beam.hpp"
#include "BeamEmitter.hpp"

namespace Pomdog {

class BeamSystem {
public:
	BeamSystem();

	void Update(DurationSeconds const& frameDuration, Transform2D const& emitterTransform);
	
public:
	BeamEmitter emitter;
	std::vector<Beam> beams;
	
private:
	DurationSeconds erapsedTime;
	DurationSeconds emissionTimer;
	std::mt19937 random;
};

}// namespace Pomdog

#endif // !defined(POMDOG_BEAMSYSTEM_616FE7EE_4C71_411A_A44D_C50248FB7B7E_HPP)
