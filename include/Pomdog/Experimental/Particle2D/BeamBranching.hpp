// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <cstdint>
#include <random>

namespace Pomdog {

class BeamBranching final {
public:
    std::uniform_real_distribution<float> SpreadRange{-5.0f, 5.0f};

    float BranchingRate = 0.7f;

    float InheritThickness = 0.7f;

    std::uint8_t MaxBranches = 0;
};

} // namespace Pomdog
