// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Particles/EmitterShapes/ParticleEmitterShapeBox.hpp"
#include "Pomdog/Experimental/Random/Xoroshiro128StarStar.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <random>

namespace Pomdog::Detail::Particles {

ParticleEmitterShapeBox::ParticleEmitterShapeBox(const Vector3& size)
    : halfSize(size * 0.5f)
{
}

std::tuple<Vector3, Vector3>
ParticleEmitterShapeBox::Compute(Random::Xoroshiro128StarStar& random) const
{
    POMDOG_ASSERT(halfSize.X >= 0.0f);
    POMDOG_ASSERT(halfSize.Y >= 0.0f);
    POMDOG_ASSERT(halfSize.Z >= 0.0f);

    std::uniform_real_distribution<float> distX(-halfSize.X, halfSize.X);
    std::uniform_real_distribution<float> distY(-halfSize.Y, halfSize.Y);
    std::uniform_real_distribution<float> distZ(-halfSize.Z, halfSize.Z);

    auto emitPosition = Vector3{distX(random), distY(random), distZ(random)};
    auto emitDirection = Vector3{1.0f, 0.0f, 0.0f};
    return std::make_tuple(std::move(emitPosition), std::move(emitDirection));
}

} // namespace Pomdog::Detail::Particles
