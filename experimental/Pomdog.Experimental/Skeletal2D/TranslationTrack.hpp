// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "AnimationTrack.hpp"
#include "AnimationTimeInterval.hpp"
#include "CompressedFloat.hpp"
#include "JointIndex.hpp"
#include <vector>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

struct TranslationKeyframe {
    AnimationTimeInterval Time;
    CompressedFloat<std::int16_t, 7> TranslateX;
    CompressedFloat<std::int16_t, 7> TranslateY;

    static_assert(decltype(TranslateX)::Max() > 1024.0f, "");
    static_assert(decltype(TranslateX)::Min() < -1024.0f, "");
    static_assert(decltype(TranslateY)::Max() > 1024.0f, "");
    static_assert(decltype(TranslateY)::Min() < -1024.0f, "");
};

class TranslationTrack final: public AnimationTrack {
public:
    TranslationTrack(
        std::vector<TranslationKeyframe> && keys, JointIndex && jointIndex);

    void Apply(
        AnimationTimeInterval const& time,
        Skeleton const& skeleton,
        SkeletonPose & skeletonPose) override;

    AnimationTimeInterval Length() const override;

private:
    std::vector<TranslationKeyframe> keys;
    JointIndex jointIndex;
};

} // namespace Skeletal2D
} // namespace Detail
} // namespace Pomdog
