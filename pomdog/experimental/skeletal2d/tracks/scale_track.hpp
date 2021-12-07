// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/skeletal2d/animation_time_interval.hpp"
#include "pomdog/experimental/skeletal2d/compressed_float.hpp"
#include "pomdog/experimental/skeletal2d/joint_index.hpp"
#include "pomdog/experimental/skeletal2d/tracks/animation_track.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D::Detail {

struct ScaleKeyframe final {
    AnimationTimeInterval Time;
    CompressedFloat<std::int16_t, 1500> Scale;

    static_assert(decltype(Scale)::Max() > 20.0f, "");
    static_assert(decltype(Scale)::Min() < -20.0f, "");
};

class ScaleTrack final : public AnimationTrack {
public:
    ScaleTrack(std::vector<ScaleKeyframe>&& keys, JointIndex&& jointIndex);

    void Apply(
        const AnimationTimeInterval& time,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) override;

    AnimationTimeInterval GetLength() const override;

private:
    std::vector<ScaleKeyframe> keys;
    JointIndex jointIndex;
};

} // namespace Pomdog::Skeletal2D::Detail
