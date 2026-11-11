// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/animation_time_interval.h"
#include "pomdog/experimental/skeletal2d/compressed_float.h"
#include "pomdog/experimental/skeletal2d/joint_index.h"
#include "pomdog/experimental/skeletal2d/tracks/animation_track.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {

struct RotationKeyframe final {
    AnimationTimeInterval time;
    CompressedFloat<i16, 4500> rotation;

    static_assert(decltype(rotation)::max() > 3.1416f * 2.0f, "");
    static_assert(decltype(rotation)::min() < -3.1416f * 2.0f, "");
};

class RotationTrack final : public AnimationTrack {
public:
    RotationTrack(std::vector<RotationKeyframe>&& keys, JointIndex&& jointIndex);

    void apply(
        const AnimationTimeInterval& time,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) override;

    AnimationTimeInterval getLength() const override;

private:
    std::vector<RotationKeyframe> keys_;
    JointIndex jointIndex_;
};

} // namespace pomdog::skeletal2d::detail
