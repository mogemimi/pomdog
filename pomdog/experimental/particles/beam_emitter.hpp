// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/application/duration.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/color.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class BeamEmitter final {
public:
    pomdog::Duration Duration{5.0};
    bool Looping = true;

    Color StartColor = Color::White;
    Color EndColor = Color::White;

    ///@note [0, std::numelic_limits<T>::max()]
    std::uint32_t InterpolationPoints = 90;

    std::uniform_real_distribution<float> SwayRange{-8.0f, 8.0f};

    float ShapeWidth = 0;

    float StartThickness = 1.0f;

    ///@note [0.0, 5.0]
    float Jaggedness = 0.7f;

    float StartLifetime = 1;

    std::uint16_t MaxBeams = 16;

    std::uint16_t EmissionRate = 4;
};

} // namespace pomdog
