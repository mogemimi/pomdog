// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Experimental/Particle2D/Particle.hpp"
#include "Pomdog/Experimental/Particle2D/ParticleEmitter.hpp"
#include <cstdint>
#include <memory>
#include <random>
#include <vector>

namespace Pomdog {

class ParticleClip;

class ParticleSystem final {
public:
    explicit ParticleSystem(const std::shared_ptr<ParticleClip const>& clip);

    void Play();
    void Pause();
    void Stop();

    void Simulate(
        const Vector2& emitterPosition,
        const Radian<float>& emitterRotation,
        const Duration& duration);

    bool IsAlive() const;

    const std::vector<Particle>& GetParticles() const { return particles; }

    std::size_t GetParticleCount() const { return particles.size(); }

    bool IsLoop() const noexcept { return emitter.Looping; }
    void SetLoop(bool loop) noexcept { emitter.Looping = loop; }

private:
    enum class ParticleSystemState : std::uint8_t {
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
};

} // namespace Pomdog
