// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape.h"
#include "pomdog/experimental/particles/parameters/particle_parameter.h"
#include "pomdog/math/color.h"
#include "pomdog/math/radian.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class ParticleClip final {
public:
    pomdog::Duration Duration = std::chrono::seconds{5};

    int EmissionRate = 16;

    int EmissionRateOverTime = 16;

    int MaxParticles = 128;

    /// The initial life time in seconds for particles.
    float StartLifetime = 1.0f;

    ///@note StartSpeed (pixel/second)
    std::unique_ptr<detail::particles::ParticleParameter<float>> StartSpeed;

    std::unique_ptr<detail::particles::ParticleEmitterShape> Shape;

    std::unique_ptr<detail::particles::ParticleParameter<Color>> StartColor;

    std::unique_ptr<detail::particles::ParticleParameter<Color>> ColorOverLifetime;

    std::unique_ptr<detail::particles::ParticleParameter<Radian<float>>> StartRotation;

    std::unique_ptr<detail::particles::ParticleParameter<Radian<float>>> RotationOverLifetime;

    std::unique_ptr<detail::particles::ParticleParameter<float>> StartSize;

    std::unique_ptr<detail::particles::ParticleParameter<float>> SizeOverLifetime;

    float StartDelay = 0.0f;

    float GravityModifier = 0.0f;

    bool Looping = true;

    // bool Prewarm = false;
};

} // namespace pomdog
