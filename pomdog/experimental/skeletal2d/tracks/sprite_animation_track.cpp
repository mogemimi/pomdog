// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/experimental/skeletal2d/tracks/sprite_animation_track.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/skin.h"
#include "pomdog/experimental/skeletal2d/tracks/animation_key_helper.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::skeletal2d::detail {
namespace {

template <typename ForwardIterator, typename T>
std::pair<ForwardIterator, ForwardIterator> binarySearchNearestPoints(ForwardIterator first, ForwardIterator last, const T& value)
{
    static_assert(std::is_same<typename std::remove_reference<decltype(*first)>::type, T>::value, "");
    POMDOG_ASSERT(first != last);

    auto it = std::lower_bound(first, last, value, AnimationKeyHelper::less<T>);

    if (it == last) {
        return std::make_pair(std::prev(last), std::prev(last));
    }
    else if (it == first) {
        return std::make_pair(first, first);
    }
    return std::make_pair(std::prev(it), it);
}

} // namespace

SpriteAnimationTrack::SpriteAnimationTrack(std::vector<SpriteKeyframe>&& keysIn, u32 slotHashIDIn)
    : keys_(std::move(keysIn))
    , slotHashID_(slotHashIDIn)
{
    POMDOG_ASSERT(std::is_sorted(std::begin(keys_), std::end(keys_), AnimationKeyHelper::less<SpriteKeyframe>));
}

void SpriteAnimationTrack::apply(
    const AnimationTimeInterval& time,
    [[maybe_unused]] const Skeleton& skeleton,
    [[maybe_unused]] SkeletonPose& skeletonPose,
    Skin* skin)
{
    if (skin == nullptr) {
        return;
    }

    SpriteKeyframe point;
    point.time = time;

    POMDOG_ASSERT(!keys_.empty());
    auto pointPair = binarySearchNearestPoints(std::begin(keys_), std::end(keys_), point);

    if (!slotIndex_) {
        ///@note lazy initialization:
        slotIndex_ = skin->findSlotIndexByID(slotHashID_);
    }

    POMDOG_ASSERT(slotIndex_);
    auto& slot = skin->getSlotByIndex(*slotIndex_);

    slot.subrect = pointPair.first->subrect;
    slot.origin = pointPair.first->origin;
    slot.texturePage = pointPair.first->texturePage;
    slot.textureRotate = pointPair.first->textureRotate;
}

AnimationTimeInterval SpriteAnimationTrack::getLength() const
{
    POMDOG_ASSERT(!keys_.empty());
    POMDOG_ASSERT(std::is_sorted(std::begin(keys_), std::end(keys_), AnimationKeyHelper::less<SpriteKeyframe>));
    POMDOG_ASSERT(keys_.front().time <= keys_.back().time);
    return keys_.back().time;
}

} // namespace pomdog::skeletal2d::detail
