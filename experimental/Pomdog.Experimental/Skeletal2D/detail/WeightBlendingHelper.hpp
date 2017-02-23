// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include <vector>

namespace Pomdog {

struct JointPose;

namespace Detail {
namespace Skeletal2D {

struct WeightBlendingHelper {
    static void Lerp(
        std::vector<JointPose> const& sourcePoses1,
        std::vector<JointPose> const& sourcePoses2,
        float weight,
        std::vector<JointPose> & output);
};

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
