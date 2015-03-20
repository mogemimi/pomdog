// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PARTICLEEMITTER_69E1B461_D54C_4634_B9DF_19F6366F4E83_HPP
#define POMDOG_PARTICLEEMITTER_69E1B461_D54C_4634_B9DF_19F6366F4E83_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <cstdint>

namespace Pomdog {

class ParticleEmitter {
public:
	float StartDelay = 0;

	float StartLifetime = 1;

	float GravityModifier = 0;

	///@brief 発生させるパーティクルの最大数
	std::uint16_t MaxParticles = 128;

	///@brief 1 秒間に発生させるパーティクルの数
	std::uint16_t EmissionRate = 16;

	//float InheritVelocity;

	//ParticleSimulationSpace SimulationSpace;

	bool Looping = true;

	//bool Prewarm = false;
};

}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLEEMITTER_69E1B461_D54C_4634_B9DF_19F6366F4E83_HPP)
