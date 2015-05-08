// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_TRANSLATIONTRACK_6A0FA1BF_HPP
#define POMDOG_TRANSLATIONTRACK_6A0FA1BF_HPP

#include "AnimationTrack.hpp"
#include "AnimationTimeInterval.hpp"
#include "CompressedFloat.hpp"
#include "JointIndex.hpp"
#include "Pomdog.Experimental/MSVCSupport.hpp"
#include <vector>

namespace Pomdog {
namespace Detail {
namespace Skeletal2D {

class TranslationKeyframe {
public:
    AnimationTimeInterval Time;
    CompressedFloat<std::int16_t, 7> TranslateX;
    CompressedFloat<std::int16_t, 7> TranslateY;

#ifndef POMDOG_MSVC2015_SUPPORT
    static_assert(decltype(TranslateX)::Max() > 1024.0f, "");
    static_assert(decltype(TranslateX)::Min() < -1024.0f, "");
    static_assert(decltype(TranslateY)::Max() > 1024.0f, "");
    static_assert(decltype(TranslateY)::Min() < -1024.0f, "");
#endif
};

class TranslationTrack final: public AnimationTrack {
public:
    TranslationTrack(std::vector<TranslationKeyframe> && keys, JointIndex && jointIndex);

    void Apply(AnimationTimeInterval const& time, Skeleton const& skeleton,
        SkeletonPose & skeletonPose) override;

    AnimationTimeInterval Length() const override;

private:
    std::vector<TranslationKeyframe> keys;
    JointIndex jointIndex;
};

}// namespace Skeletal2D
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_TRANSLATIONTRACK_6A0FA1BF_HPP
