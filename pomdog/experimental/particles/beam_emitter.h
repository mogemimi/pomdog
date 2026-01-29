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
    pomdog::Duration Duration{5.0};

    ///@note [0, std::numelic_limits<T>::max()]
    u32 InterpolationPoints = 90;

    std::uniform_real_distribution<f32> SwayRange{-8.0f, 8.0f};

    f32 ShapeWidth = 0;

    f32 StartThickness = 1.0f;

    ///@note [0.0, 5.0]
    f32 Jaggedness = 0.7f;

    f32 StartLifetime = 1;

    Color StartColor = Color::createWhite();
    Color EndColor = Color::createWhite();

    u16 MaxBeams = 16;

    u16 EmissionRate = 4;

    bool Looping = true;
};

} // namespace pomdog
