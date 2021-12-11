// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/blendtrees/weight_blending_helper.hpp"
#include "pomdog/experimental/skeletal2d/joint_pose.hpp"
#include "pomdog/math/math.hpp"
#include "pomdog/utility/assert.hpp"

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

        result.Scale = math::Lerp(pose1.Scale, pose2.Scale, weight);
        result.Rotation = math::Lerp(pose1.Rotation.value, pose2.Rotation.value, weight);
        result.Translate = Vector2::Lerp(pose1.Translate, pose2.Translate, weight);
    }
}

} // namespace pomdog::skeletal2d::detail::WeightBlendingHelper