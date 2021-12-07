// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
