// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include <vector>

namespace Pomdog::Skeletal2D {
struct JointPose;
} // namespace Pomdog::Skeletal2D

namespace Pomdog::Skeletal2D::Detail::WeightBlendingHelper {

void Lerp(
    const std::vector<JointPose>& sourcePoses1,
    const std::vector<JointPose>& sourcePoses2,
    float weight,
    std::vector<JointPose>& output);

} // namespace Pomdog::Skeletal2D::Detail::WeightBlendingHelper
