// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Experimental/Particles/Particle.hpp"
#include "Pomdog/Experimental/Random/Xoroshiro128StarStar.hpp"
#include <cstdint>
#include <memory>
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

    void Simulate(
        const Vector3& emitterPosition,
        const Quaternion& emitterRotation,
        const Duration& duration);

    bool IsAlive() const;

    const std::vector<Particle>& GetParticles() const { return particles; }

    std::size_t GetParticleCount() const { return particles.size(); }

    bool IsLoop() const noexcept;

private:
    enum class ParticleSystemState : std::uint8_t {
        Paused,
        Playing,
        Stopped
    };

    std::vector<Particle> particles;
    std::shared_ptr<ParticleClip const> clip;
    Duration erapsedTime;
    Duration emissionTimer;
    Random::Xoroshiro128StarStar random;
    ParticleSystemState state;
};

} // namespace Pomdog
