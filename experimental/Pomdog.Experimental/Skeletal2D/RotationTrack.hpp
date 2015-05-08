// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ROTATIONTRACK_96A9D69A_HPP
#define POMDOG_ROTATIONTRACK_96A9D69A_HPP

#include "AnimationTrack.hpp"
#include "AnimationTimeInterval.hpp"
#include "CompressedFloat.hpp"
#include "JointIndex.hpp"
#include "Pomdog.Experimental/MSVCSupport.hpp"
#include <vector>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

class RotationKeyframe {
public:
    AnimationTimeInterval Time;
    CompressedFloat<std::int16_t, 4500> Rotation;

#ifndef POMDOG_MSVC2015_SUPPORT
    static_assert(decltype(Rotation)::Max() > 3.1416f*2.0f, "");
    static_assert(decltype(Rotation)::Min() < -3.1416f*2.0f, "");
#endif
};

class RotationTrack final: public AnimationTrack {
public:
    RotationTrack(std::vector<RotationKeyframe> && keys, JointIndex && jointIndex);

    void Apply(AnimationTimeInterval const& time, Skeleton const& skeleton,
        SkeletonPose & skeletonPose) override;

    AnimationTimeInterval Length() const override;

private:
    std::vector<RotationKeyframe> keys;
    JointIndex jointIndex;
};

}// namespace Skeletal2D
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_ROTATIONTRACK_96A9D69A_HPP
