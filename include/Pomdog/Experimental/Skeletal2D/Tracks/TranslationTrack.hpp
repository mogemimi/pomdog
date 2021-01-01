// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Experimental/Skeletal2D/AnimationTimeInterval.hpp"
#include "Pomdog/Experimental/Skeletal2D/CompressedFloat.hpp"
#include "Pomdog/Experimental/Skeletal2D/JointIndex.hpp"
#include "Pomdog/Experimental/Skeletal2D/Tracks/AnimationTrack.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D::Detail {

struct TranslationKeyframe final {
    AnimationTimeInterval Time;
    CompressedFloat<std::int16_t, 7> TranslateX;
    CompressedFloat<std::int16_t, 7> TranslateY;

    static_assert(decltype(TranslateX)::Max() > 1024.0f, "");
    static_assert(decltype(TranslateX)::Min() < -1024.0f, "");
    static_assert(decltype(TranslateY)::Max() > 1024.0f, "");
    static_assert(decltype(TranslateY)::Min() < -1024.0f, "");
};

class TranslationTrack final : public AnimationTrack {
public:
    TranslationTrack(
        std::vector<TranslationKeyframe>&& keys, JointIndex&& jointIndex);

    void Apply(
        const AnimationTimeInterval& time,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) override;

    AnimationTimeInterval GetLength() const override;

private:
    std::vector<TranslationKeyframe> keys;
    JointIndex jointIndex;
};

} // namespace Pomdog::Skeletal2D::Detail
