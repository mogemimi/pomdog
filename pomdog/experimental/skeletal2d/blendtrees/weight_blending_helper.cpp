// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/blendtrees/weight_blending_helper.h"
#include "pomdog/experimental/skeletal2d/joint_pose.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/utility/assert.h"

namespace pomdog::skeletal2d::detail::WeightBlendingHelper {

void Lerp(
    const std::vector<JointPose>& sourcePoses1,
    const std::vector<JointPose>& sourcePoses2,
    float weight,
    std::vector<JointPose>& output)
{
    POMDOG_ASSERT(!sourcePoses1.empty());
    POMDOG_ASSERT(!sourcePoses2.empty());
    POMDOG_ASSERT(sourcePoses1.size() == sourcePoses2.size());

    for (std::size_t i = 0; i < sourcePoses1.size(); ++i) {
        auto& pose1 = sourcePoses1[i];
        auto& pose2 = sourcePoses2[i];

        POMDOG_ASSERT(!output.empty());
        POMDOG_ASSERT(i < output.size());
        auto& result = output[i];

        result.Scale = math::lerp(pose1.Scale, pose2.Scale, weight);
        result.Rotation = math::lerp(pose1.Rotation.get(), pose2.Rotation.get(), weight);
        result.Translate = math::lerp(pose1.Translate, pose2.Translate, weight);
    }
}

} // namespace pomdog::skeletal2d::detail::WeightBlendingHelper
