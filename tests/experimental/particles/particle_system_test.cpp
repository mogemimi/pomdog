// Copyright mogemimi. Distributed under the MIT license.

#include "tests/testing/testing.h"

// FIXME: Suppress warnings for the ParticleClip class, whose copy operations
// are implicitly deleted.
POMDOG_MSVC_SUPPRESS_WARNING_PUSH
POMDOG_MSVC_SUPPRESS_WARNING(4625)
POMDOG_MSVC_SUPPRESS_WARNING(4626)
#include "pomdog/experimental/particles/particle_clip.h"
POMDOG_MSVC_SUPPRESS_WARNING_POP

#include "pomdog/chrono/duration.h"
#include "pomdog/experimental/particles/emitter_shapes/particle_emitter_shape_sector.h"
#include "pomdog/experimental/particles/parameters/particle_parameter_constant.h"
#include "pomdog/experimental/particles/particle_system.h"
#include "pomdog/math/color.h"
#include "pomdog/math/turn.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_BEGIN
#include <doctest/doctest.h>
#include <array>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_TESTING_HEADERS_END

using pomdog::Color;
using pomdog::Duration;
using pomdog::f32;
using pomdog::ParticleClip;
using pomdog::ParticleSystem;
using pomdog::Turn32;
using pomdog::Vector2;
using pomdog::detail::particles::ParticleEmitterShapeSector;
using pomdog::detail::particles::ParticleParameterConstant;

namespace {

/// Creates a clip whose particles all leave along +X at 10 units per second.
[[nodiscard]] std::shared_ptr<const ParticleClip>
createStraightClip()
{
    auto clip = std::make_shared<ParticleClip>();
    clip->startLifetime = 10.0f;
    clip->startSpeed = std::make_unique<ParticleParameterConstant<f32>>(10.0f);
    clip->shape = std::make_unique<ParticleEmitterShapeSector>(Turn32{0.0f});
    clip->startColor = std::make_unique<ParticleParameterConstant<Color>>(Color::createWhite());
    clip->colorOverLifetime = std::make_unique<ParticleParameterConstant<Color>>(Color::createWhite());
    clip->startRotation = std::make_unique<ParticleParameterConstant<Turn32>>(Turn32{0.0f});
    clip->rotationOverLifetime = std::make_unique<ParticleParameterConstant<Turn32>>(Turn32{0.0f});
    clip->startSize = std::make_unique<ParticleParameterConstant<f32>>(1.0f);
    clip->sizeOverLifetime = std::make_unique<ParticleParameterConstant<f32>>(1.0f);
    return clip;
}

} // namespace

TEST_CASE("ParticleSystem")
{
    SUBCASE("simulate rotates emission by the emitter rotation in turns")
    {
        struct Expectation final {
            Turn32 rotation;
            f32 velocityX;
            f32 velocityY;
        };
        const std::array<Expectation, 5> expectations = {{
            {Turn32{0.0f}, 10.0f, 0.0f},
            {Turn32{0.25f}, 0.0f, 10.0f},
            {Turn32{0.5f}, -10.0f, 0.0f},
            {Turn32{-0.25f}, 0.0f, -10.0f},
            {Turn32{0.125f}, 7.0710678f, 7.0710678f},
        }};
        for (const auto& expectation : expectations) {
            CAPTURE(expectation.rotation.get());
            ParticleSystem particleSystem{createStraightClip()};
            particleSystem.simulate(Vector2{0.0f, 0.0f}, expectation.rotation, Duration{0.5});
            REQUIRE(particleSystem.getParticleCount() > 0);

            for (const auto& particle : particleSystem.getParticles()) {
                REQUIRE(particle.velocity.x == doctest::Approx(expectation.velocityX).epsilon(1e-5));
                REQUIRE(particle.velocity.y == doctest::Approx(expectation.velocityY).epsilon(1e-5));
                REQUIRE(particle.velocity.z == doctest::Approx(0.0f).epsilon(1e-5));
            }
        }
    }
}
