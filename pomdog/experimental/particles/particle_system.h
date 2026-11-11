// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/experimental/particles/particle.h"
#include "pomdog/random/xoroshiro128_star_star.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class ParticleClip;

class ParticleSystem final {
public:
    explicit ParticleSystem(const std::shared_ptr<const ParticleClip>& clip);

    void play();
    void pause();
    void stop();

    void simulate(
        const Vector2& emitterPosition,
        const Radian<f32>& emitterRotation,
        const Duration& duration);

    void simulate(
        const Vector3& emitterPosition,
        const Quaternion& emitterRotation,
        const Duration& duration);

    [[nodiscard]] bool
    isAlive() const;

    [[nodiscard]] const std::vector<Particle>&
    getParticles() const { return particles_; }

    [[nodiscard]] std::size_t
    getParticleCount() const { return particles_.size(); }

    [[nodiscard]] bool
    isLoop() const noexcept;

private:
    enum class ParticleSystemState : u8 {
        Paused,
        Playing,
        Stopped
    };

    std::vector<Particle> particles_;
    std::shared_ptr<const ParticleClip> clip_;
    Duration elapsedTime_;
    Duration emissionTimer_;
    random::Xoroshiro128StarStar random_;
    ParticleSystemState state_;
};

} // namespace pomdog
