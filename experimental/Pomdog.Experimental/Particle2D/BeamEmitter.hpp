// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_BEAMEMITTER_89CB54AB_HPP
#define POMDOG_BEAMEMITTER_89CB54AB_HPP

#include "Pomdog/Application/Duration.hpp"
#include "Pomdog/Math/Color.hpp"
#include <cstdint>
#include <random>

namespace Pomdog {

class BeamEmitter {
public:
    Pomdog::Duration Duration {5.0};
    bool Looping = true;

    Color StartColor = Color::White;
    Color EndColor = Color::White;

    ///@note [0, std::numelic_limits<T>::max()]
    std::uint32_t InterpolationPoints = 90;

    std::uniform_real_distribution<float> SwayRange {-8.0f, 8.0f};

    float ShapeWidth = 0;

    float StartThickness = 1.0f;

    ///@note [0.0, 5.0]
    float Jaggedness = 0.7f;

    float StartLifetime = 1;

    std::uint16_t MaxBeams = 16;

    std::uint16_t EmissionRate = 4;
};

} // namespace Pomdog

#endif // POMDOG_BEAMEMITTER_89CB54AB_HPP
