// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/experimental/particles/beam.h"
#include "pomdog/experimental/particles/beam_branching.h"
#include "pomdog/experimental/particles/beam_emitter.h"
#include "pomdog/math/vector2.h"
#include "pomdog/random/xoroshiro128_star_star.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class BeamSystem final {
public:
    BeamSystem();

    void update(const Duration& frameDuration, const Vector2& emitterPosition, const Vector2& target);

    [[nodiscard]] static Vector2
    createTarget(
        const Vector2& emitterPosition,
        const Radian<f32>& emitterRotation,
        f32 distance);

public:
    BeamEmitter emitter;
    BeamBranching branching;

    std::vector<Beam> beams;

private:
    Duration elapsedTime_;
    Duration emissionTimer_;
    random::Xoroshiro128StarStar random_;
};

} // namespace pomdog
