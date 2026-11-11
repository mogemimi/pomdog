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

struct TranslationKeyframe final {
    AnimationTimeInterval time;
    CompressedFloat<i16, 7> translateX;
    CompressedFloat<i16, 7> translateY;

    static_assert(decltype(translateX)::max() > 1024.0f, "");
    static_assert(decltype(translateX)::min() < -1024.0f, "");
    static_assert(decltype(translateY)::max() > 1024.0f, "");
    static_assert(decltype(translateY)::min() < -1024.0f, "");
};

class TranslationTrack final : public AnimationTrack {
public:
    TranslationTrack(
        std::vector<TranslationKeyframe>&& keys, JointIndex&& jointIndex);

    void apply(
        const AnimationTimeInterval& time,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) override;

    AnimationTimeInterval getLength() const override;

private:
    std::vector<TranslationKeyframe> keys_;
    JointIndex jointIndex_;
};

} // namespace pomdog::skeletal2d::detail
