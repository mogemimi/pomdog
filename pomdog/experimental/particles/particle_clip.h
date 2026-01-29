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
    pomdog::Duration Duration = std::chrono::seconds{5};

    i32 EmissionRate = 16;

    i32 EmissionRateOverTime = 16;

    i32 MaxParticles = 128;

    /// The initial life time in seconds for particles.
    f32 StartLifetime = 1.0f;

    ///@note StartSpeed (pixel/second)
    std::unique_ptr<detail::particles::ParticleParameter<f32>> StartSpeed;

    std::unique_ptr<detail::particles::ParticleEmitterShape> Shape;

    std::unique_ptr<detail::particles::ParticleParameter<Color>> StartColor;

    std::unique_ptr<detail::particles::ParticleParameter<Color>> ColorOverLifetime;

    std::unique_ptr<detail::particles::ParticleParameter<Radian<f32>>> StartRotation;

    std::unique_ptr<detail::particles::ParticleParameter<Radian<f32>>> RotationOverLifetime;

    std::unique_ptr<detail::particles::ParticleParameter<f32>> StartSize;

    std::unique_ptr<detail::particles::ParticleParameter<f32>> SizeOverLifetime;

    f32 StartDelay = 0.0f;

    f32 GravityModifier = 0.0f;

    bool Looping = true;

    // bool Prewarm = false;
};

} // namespace pomdog
