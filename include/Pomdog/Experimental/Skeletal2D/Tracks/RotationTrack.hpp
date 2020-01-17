// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/AnimationTimeInterval.hpp"
#include "Pomdog/Experimental/Skeletal2D/CompressedFloat.hpp"
#include "Pomdog/Experimental/Skeletal2D/JointIndex.hpp"
#include "Pomdog/Experimental/Skeletal2D/Tracks/AnimationTrack.hpp"
#include <vector>

namespace Pomdog::Skeletal2D::Detail {

struct RotationKeyframe final {
    AnimationTimeInterval Time;
    CompressedFloat<std::int16_t, 4500> Rotation;

    static_assert(decltype(Rotation)::Max() > 3.1416f * 2.0f, "");
    static_assert(decltype(Rotation)::Min() < -3.1416f * 2.0f, "");
};

class RotationTrack final : public AnimationTrack {
public:
    RotationTrack(std::vector<RotationKeyframe>&& keys, JointIndex&& jointIndex);

    void Apply(
        const AnimationTimeInterval& time,
        const Skeleton& skeleton,
        SkeletonPose& skeletonPose,
        Skin* skin) override;

    AnimationTimeInterval GetLength() const override;

private:
    std::vector<RotationKeyframe> keys;
    JointIndex jointIndex;
};

} // namespace Pomdog::Skeletal2D::Detail
