// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Particle.hpp"
#include "ParticleEmitter.hpp"
#include "Pomdog.Experimental/Gameplay2D/Transform.hpp"
#include "Pomdog.Experimental/Gameplay/Entity.hpp"
#include "Pomdog/Application/Duration.hpp"
#include <cstdint>
#include <vector>
#include <random>

namespace Pomdog {

class ParticleClip;

class ParticleSystem final : public Component {
public:
    explicit ParticleSystem(std::shared_ptr<ParticleClip const> const& clip);

    void Play();
    void Pause();
    void Stop();

    void Simulate(Entity & entity, Duration const& duration);

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

} // namespace Pomdog
