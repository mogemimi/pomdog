// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Math/Vector3.hpp"
#include <tuple>

namespace Pomdog::Random {
class Xoroshiro128StarStar;
} // namespace Pomdog::Random

namespace Pomdog::Detail::Particles {

class ParticleEmitterShape {
public:
    virtual ~ParticleEmitterShape() = default;

    virtual std::tuple<Vector3, Vector3>
    Compute(Random::Xoroshiro128StarStar& random) const = 0;
};

} // namespace Pomdog::Detail::Particles
