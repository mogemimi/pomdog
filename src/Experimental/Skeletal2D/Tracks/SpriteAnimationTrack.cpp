// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "Pomdog/Experimental/Skeletal2D/Tracks/SpriteAnimationTrack.hpp"
#include "AnimationKeyHelper.hpp"
#include "Pomdog/Experimental/Skeletal2D/Skin.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <utility>

namespace Pomdog::Skeletal2D::Detail {
namespace {

template <typename ForwardIterator, typename T>
std::pair<ForwardIterator, ForwardIterator> BinarySearchNearestPoints(ForwardIterator first, ForwardIterator last, const T& value)
{
    static_assert(std::is_same<typename std::remove_reference<decltype(*first)>::type, T>::value, "");
    POMDOG_ASSERT(first != last);

    auto it = std::lower_bound(first, last, value, AnimationKeyHelper::Less<T>);

    if (it == last) {
        return std::make_pair(std::prev(last), std::prev(last));
    }
    else if (it == first) {
        return std::make_pair(first, first);
    }
    return std::make_pair(std::prev(it), it);
}

} // namespace

SpriteAnimationTrack::SpriteAnimationTrack(std::vector<SpriteKeyframe>&& keysIn, std::uint32_t slotHashIDIn)
    : keys(std::move(keysIn))
    , slotHashID(slotHashIDIn)
{
    POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<SpriteKeyframe>));
}

void SpriteAnimationTrack::Apply(
    const AnimationTimeInterval& time,
    [[maybe_unused]] const Skeleton& skeleton,
    [[maybe_unused]] SkeletonPose& skeletonPose,
    Skin* skin)
{
    if (skin == nullptr) {
        return;
    }

    SpriteKeyframe point;
    point.Time = time;

    POMDOG_ASSERT(!keys.empty());
    auto pointPair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), point);

    if (!slotIndex) {
        ///@note lazy initialization:
        slotIndex = skin->FindSlotIndexByID(slotHashID);
    }

    POMDOG_ASSERT(slotIndex);
    auto& slot = skin->GetSlotByIndex(*slotIndex);

    slot.Subrect = pointPair.first->Subrect;
    slot.Origin = pointPair.first->Origin;
    slot.TexturePage = pointPair.first->TexturePage;
    slot.TextureRotate = pointPair.first->TextureRotate;
}

AnimationTimeInterval SpriteAnimationTrack::GetLength() const
{
    POMDOG_ASSERT(!keys.empty());
    POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<SpriteKeyframe>));
    POMDOG_ASSERT(keys.front().Time <= keys.back().Time);
    return keys.back().Time;
}

} // namespace Pomdog::Skeletal2D::Detail
