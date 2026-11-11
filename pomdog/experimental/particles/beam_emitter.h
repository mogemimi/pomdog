// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/chrono/duration.h"
#include "pomdog/math/color.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class BeamEmitter final {
public:
    Duration duration{5.0};

    /// The number of points that bend a beam between its ends. A branch gets
    /// fewer points in proportion to its length.
    u32 interpolationPoints = 90;

    std::uniform_real_distribution<f32> swayRange{-8.0f, 8.0f};

    f32 shapeWidth = 0;

    f32 startThickness = 1.0f;

    /// How sharply a beam zigzags, within [0, 5]. Zero makes a straight beam.
    f32 jaggedness = 0.7f;

    f32 startLifetime = 1;

    Color startColor = Color::createWhite();
    Color endColor = Color::createWhite();

    u16 maxBeams = 16;

    u16 emissionRate = 4;

    bool looping = true;
};

} // namespace pomdog
