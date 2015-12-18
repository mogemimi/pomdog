// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_BEAMBRANCHING_1888F4E0_HPP
#define POMDOG_BEAMBRANCHING_1888F4E0_HPP

#include <cstdint>
#include <random>

namespace Pomdog {

class BeamBranching {
public:
    std::uniform_real_distribution<float> SpreadRange {-5.0f, 5.0f};

    float BranchingRate = 0.7f;

    float InheritThickness = 0.7f;

    std::uint8_t MaxBranches = 0;
};

} // namespace Pomdog

#endif // POMDOG_BEAMBRANCHING_1888F4E0_HPP
