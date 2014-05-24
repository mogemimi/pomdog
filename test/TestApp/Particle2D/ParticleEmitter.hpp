//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_PARTICLEEMITTER_69E1B461_D54C_4634_B9DF_19F6366F4E83_HPP
#define POMDOG_PARTICLEEMITTER_69E1B461_D54C_4634_B9DF_19F6366F4E83_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <cstdint>
#include <memory>
#include <Pomdog/Pomdog.hpp>
#include "ParticleParameter.hpp"

namespace Pomdog {

enum ParticleSimulationSpace : std::uint8_t {
	Local,
	World
};

class ParticleEmitter {
public:
	DurationSeconds Duration {5.0};
	bool Looping = true;
	float StartDelay = 0; // bool Prewarm;
	
	float StartLifetime = 2; // StartLifetime1 + StartLifetime2
	float StartSize = 1; // StartSize1 + StartSize2
	Radian<float> StartRotation = 0;
	Color StartColor = Color::White;
	
	std::unique_ptr<ParticleParameter<float>> StartSpeed; // StartSpeed (pixel/second)

	std::uint16_t MaxParticles = 128;
	
	///@todo もっと細かくカスタマイズしたいパラメータ
	std::uint16_t EmissionRate = 16; // 16 particles/second
	
	std::unique_ptr<ParticleParameter<Color>> ColorOverLifetime;
	std::unique_ptr<ParticleParameter<float>> SizeOverLifetime;
	
	//float GravityModifier;
	//float InheritVelocity;
	//ParticleSimulationSpace SimulationSpace;
};

}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEEMITTER_69E1B461_D54C_4634_B9DF_19F6366F4E83_HPP)
