// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Math/Vector3.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Random {
class Xoroshiro128StarStar;
} // namespace Pomdog::Random

namespace Pomdog::Detail::Particles {

class ParticleEmitterShape {
public:
    virtual ~ParticleEmitterShape();

    virtual std::tuple<Vector3, Vector3>
    Compute(Random::Xoroshiro128StarStar& random) const = 0;
};

} // namespace Pomdog::Detail::Particles
