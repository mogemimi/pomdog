// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/vector3.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::random {
class Xoroshiro128StarStar;
} // namespace pomdog::random

namespace pomdog::detail::particles {

class ParticleEmitterShape {
public:
    virtual ~ParticleEmitterShape();

    virtual std::tuple<Vector3, Vector3>
    Compute(random::Xoroshiro128StarStar& random) const = 0;
};

} // namespace pomdog::detail::particles
