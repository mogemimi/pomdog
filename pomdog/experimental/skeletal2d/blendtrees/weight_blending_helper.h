// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d {
struct JointPose;
} // namespace pomdog::skeletal2d

namespace pomdog::skeletal2d::detail::WeightBlendingHelper {

void lerp(
    const std::vector<JointPose>& sourcePoses1,
    const std::vector<JointPose>& sourcePoses2,
    f32 weight,
    std::vector<JointPose>& output);

} // namespace pomdog::skeletal2d::detail::WeightBlendingHelper
