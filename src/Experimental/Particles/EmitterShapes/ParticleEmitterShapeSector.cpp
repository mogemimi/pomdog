// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShapeSector.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Random/Xoroshiro128StarStar.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Particles {

ParticleEmitterShapeSector::ParticleEmitterShapeSector(Radian<float>&& centralAngleIn)
    : centralAngle(std::move(centralAngleIn))
{
}

std::tuple<Vector3, Vector3>
ParticleEmitterShapeSector::Compute(Random::Xoroshiro128StarStar& random) const
{
    auto emitPosition = Vector3::Zero;

    std::uniform_real_distribution<float> distribution{-0.5f, 0.5f};
    auto emitAngle = centralAngle * distribution(random);

    auto emitDirection = Vector3{std::cos(emitAngle.value), std::sin(emitAngle.value), 0.0f};
    return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
}

} // namespace Pomdog::Detail::Particles
