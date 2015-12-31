// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "WeightBlendingHelper.hpp"
#include "Pomdog.Experimental/Skeletal2D/JointPose.hpp"
#include "Pomdog/Math/MathHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

void WeightBlendingHelper::Lerp(std::vector<JointPose> const& sourcePoses1,
    std::vector<JointPose> const& sourcePoses2,
    float weight, std::vector<JointPose> & output)
{
    POMDOG_ASSERT(!sourcePoses1.empty());
    POMDOG_ASSERT(!sourcePoses2.empty());
    POMDOG_ASSERT(sourcePoses1.size() == sourcePoses2.size());

    for (size_t i = 0; i < sourcePoses1.size(); ++i)
    {
        auto & pose1 = sourcePoses1[i];
        auto & pose2 = sourcePoses2[i];

        POMDOG_ASSERT(!output.empty());
        POMDOG_ASSERT(i < output.size());
        auto & result = output[i];

        result.Scale = MathHelper::Lerp(pose1.Scale, pose2.Scale, weight);
        result.Rotation = MathHelper::Lerp(pose1.Rotation.value, pose2.Rotation.value, weight);
        result.Translate = Vector2::Lerp(pose1.Translate, pose2.Translate, weight);
    }
}

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
