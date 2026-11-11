// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/blendtrees/weight_blending_helper.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/joint_pose.h"
#include "pomdog/math/math_functions.h"
#include "pomdog/utility/assert.h"

namespace pomdog::skeletal2d::detail::WeightBlendingHelper {

void lerp(
    const std::vector<JointPose>& sourcePoses1,
    const std::vector<JointPose>& sourcePoses2,
    f32 weight,
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

        result.scale = math::lerp(pose1.scale, pose2.scale, weight);
        result.rotation = math::lerp(pose1.rotation.get(), pose2.rotation.get(), weight);
        result.translate = math::lerp(pose1.translate, pose2.translate, weight);
    }
}

} // namespace pomdog::skeletal2d::detail::WeightBlendingHelper
