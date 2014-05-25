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
#include "ParticleEmitterShape.hpp"

namespace Pomdog {

class ParticleEmitter {
public:
	DurationSeconds Duration {5.0};
	bool Looping = true;
	float StartDelay = 0; // bool Prewarm;
	
	std::unique_ptr<ParticleParameter<Radian<float>>> StartRotation;
	
	///@note StartSpeed (pixel/second)
	std::unique_ptr<ParticleParameter<float>> StartSpeed;
	
	std::unique_ptr<ParticleEmitterShape> Shape;
	
	///@note on time
	std::unique_ptr<ParticleParameter<Color>> ColorOverLifetime;
	std::unique_ptr<ParticleParameter<float>> SizeOverLifetime;
	
	float StartLifetime = 1;
	float StartSize = 1;
	Color StartColor = Color::White;
	
	float GravityModifier = 0;
	
	///@brief 発生させるパーティクルの最大数
	std::uint16_t MaxParticles = 128;
	
	///@todo もっと細かくカスタマイズしたいパラメータ
	std::uint16_t EmissionRate = 16; // 16 particles/second
	
	//float InheritVelocity;
	//ParticleSimulationSpace SimulationSpace;
};

}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEEMITTER_69E1B461_D54C_4634_B9DF_19F6366F4E83_HPP)
