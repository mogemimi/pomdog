// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_PARTICLECLIP_CC5B71B4_5A11_4D2A_8225_5633A4BCD516_HPP
#define POMDOG_PARTICLECLIP_CC5B71B4_5A11_4D2A_8225_5633A4BCD516_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "detail/ParticleEmitterShape.hpp"
#include "detail/ParticleParameter.hpp"
#include "ParticleEmitter.hpp"
#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Radian.hpp"
#include <cstdint>
#include <memory>

namespace Pomdog {

class ParticleClip {
public:
	ParticleEmitter Emitter;

	///@note StartSpeed (pixel/second)
	std::unique_ptr<Detail::Particles::ParticleParameter<float>> StartSpeed;

	std::unique_ptr<Detail::Particles::ParticleEmitterShape> Shape;

	std::unique_ptr<Detail::Particles::ParticleParameter<Color>> StartColor;

	std::unique_ptr<Detail::Particles::ParticleParameter<Color>> ColorOverLifetime;

	std::unique_ptr<Detail::Particles::ParticleParameter<Radian<float>>> StartRotation;

	std::unique_ptr<Detail::Particles::ParticleParameter<Radian<float>>> RotationOverLifetime;

	std::unique_ptr<Detail::Particles::ParticleParameter<float>> StartSize;

	std::unique_ptr<Detail::Particles::ParticleParameter<float>> SizeOverLifetime;

	Pomdog::Duration Duration = std::chrono::seconds(5);
};

}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLECLIP_CC5B71B4_5A11_4D2A_8225_5633A4BCD516_HPP)
