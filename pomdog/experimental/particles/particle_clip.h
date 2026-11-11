// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape.h"
#include "pomdog/experimental/particles/parameters/particle_parameter.h"
#include "pomdog/math/color.h"
#include "pomdog/math/radian.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class ParticleClip final {
public:
    Duration duration = std::chrono::seconds{5};

    i32 emissionRate = 16;

    i32 emissionRateOverTime = 16;

    i32 maxParticles = 128;

    /// The initial life time in seconds for particles.
    f32 startLifetime = 1.0f;

    /// The initial speed in pixels per second for particles.
    std::unique_ptr<detail::particles::ParticleParameter<f32>> startSpeed;

    std::unique_ptr<detail::particles::ParticleEmitterShape> shape;

    std::unique_ptr<detail::particles::ParticleParameter<Color>> startColor;

    std::unique_ptr<detail::particles::ParticleParameter<Color>> colorOverLifetime;

    std::unique_ptr<detail::particles::ParticleParameter<Radian<f32>>> startRotation;

    std::unique_ptr<detail::particles::ParticleParameter<Radian<f32>>> rotationOverLifetime;

    std::unique_ptr<detail::particles::ParticleParameter<f32>> startSize;

    std::unique_ptr<detail::particles::ParticleParameter<f32>> sizeOverLifetime;

    f32 startDelay = 0.0f;

    f32 gravityModifier = 0.0f;

    bool looping = true;

    // bool Prewarm = false;
};

} // namespace pomdog
