// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <random>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class BeamBranching final {
public:
    std::uniform_real_distribution<f32> SpreadRange{-5.0f, 5.0f};

    f32 BranchingRate = 0.7f;

    f32 InheritThickness = 0.7f;

    u8 MaxBranches = 0;
};

} // namespace pomdog
