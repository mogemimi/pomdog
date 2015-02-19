//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_PARTICLESYSTEM_2AEA7BC5_5CCE_4688_8FAC_D26A97A8F8B7_HPP
#define POMDOG_PARTICLESYSTEM_2AEA7BC5_5CCE_4688_8FAC_D26A97A8F8B7_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "Particle.hpp"
#include "ParticleEmitter.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform2D.hpp"
#include "Pomdog.Experimental/Gameplay/GameObject.hpp"
#include "Pomdog/Application/Duration.hpp"
#include <cstdint>
#include <vector>
#include <random>

namespace Pomdog {

class ParticleClip;

class ParticleSystem: public Component<ParticleSystem> {
public:
	explicit ParticleSystem(std::shared_ptr<ParticleClip const> const& clip);

	void Play();
	void Pause();
	void Stop();

	void Simulate(GameObject & gameObject, Duration const& duration);

	bool IsAlive() const;

	std::vector<Particle> const& Particles() const { return particles; }

	std::size_t ParticleCount() const { return particles.size(); }

	bool Loop() const { return emitter.Looping; }
	void Loop(bool loop) { emitter.Looping = loop; }

	bool EnableEmission() const { return enableEmission; }
	void EnableEmission(bool enableEmissionIn) { this->enableEmission = enableEmissionIn; }

private:
	enum class ParticleSystemState: std::uint8_t {
		Paused,
		Playing,
		Stopped
	};

	ParticleEmitter emitter;
	std::vector<Particle> particles;
	std::shared_ptr<ParticleClip const> clip;
	Duration erapsedTime;
	Duration emissionTimer;
	std::mt19937 random;
	ParticleSystemState state;
	bool enableEmission;
};

}// namespace Pomdog

#endif // !defined(POMDOG_PARTICLESYSTEM_2AEA7BC5_5CCE_4688_8FAC_D26A97A8F8B7_HPP)
