// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/chrono/duration.hpp"
#include "pomdog/experimental/particles/particle.hpp"
#include "pomdog/experimental/random/xoroshiro128_star_star.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

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
    random::Xoroshiro128StarStar random;
    ParticleSystemState state;
};

} // namespace pomdog
