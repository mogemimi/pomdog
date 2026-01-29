// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/experimental/particles/beam.h"
#include "pomdog/experimental/particles/beam_branching.h"
#include "pomdog/experimental/particles/beam_emitter.h"
#include "pomdog/experimental/random/xoroshiro128_star_star.h"
#include "pomdog/math/vector2.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class BeamSystem final {
public:
    BeamSystem();

    void Update(const Duration& frameDuration, const Vector2& emitterPosition, const Vector2& target);

    static Vector2 CreateTarget(
        const Vector2& emitterPosition,
        const Radian<f32>& emitterRotation,
        f32 distance);

public:
    BeamEmitter emitter;
    BeamBranching branching;

    std::vector<Beam> beams;

private:
    Duration erapsedTime;
    Duration emissionTimer;
    random::Xoroshiro128StarStar random;
};

} // namespace pomdog
