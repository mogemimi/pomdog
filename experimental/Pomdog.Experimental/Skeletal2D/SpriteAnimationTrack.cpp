//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "SpriteAnimationTrack.hpp"
#include "detail/AnimationKeyHelper.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <utility>

namespace Pomdog {
namespace Details {
namespace Skeletal2D {
namespace {

template <typename ForwardIterator, typename T>
static
std::pair<ForwardIterator, ForwardIterator> BinarySearchNearestPoints(ForwardIterator first, ForwardIterator last, T const& value)
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

}// unnamed namespace
//-----------------------------------------------------------------------
SpriteAnimationTrack::SpriteAnimationTrack(std::vector<SpriteKeyframe> && keysIn, std::uint32_t slotHashIdIn)
	: keys(std::move(keysIn))
	, slotHashId(slotHashIdIn)
{
	POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<SpriteKeyframe>));
}
//-----------------------------------------------------------------------
void SpriteAnimationTrack::Apply(Skin & skin, AnimationTimeInterval const& time)
{
	SpriteKeyframe point;
	point.Time = time;
	
	POMDOG_ASSERT(!keys.empty());
	auto pointPair = BinarySearchNearestPoints(std::begin(keys), std::end(keys), point);

	if (!slotIndex) {
		///@note lazy initialization:
		slotIndex = skin.FindSlotIndexById(slotHashId);
	}

	POMDOG_ASSERT(slotIndex);
	auto & slot = skin.SlotByIndex(*slotIndex);

	slot.Subrect = pointPair.first->Subrect;
	slot.Origin = pointPair.first->Origin;
	slot.TexturePage = pointPair.first->TexturePage;
	slot.TextureRotate = pointPair.first->TextureRotate;
}
//-----------------------------------------------------------------------
AnimationTimeInterval SpriteAnimationTrack::Length() const
{
	POMDOG_ASSERT(!keys.empty());
	POMDOG_ASSERT(std::is_sorted(std::begin(keys), std::end(keys), AnimationKeyHelper::Less<SpriteKeyframe>));
	POMDOG_ASSERT(keys.front().Time <= keys.back().Time);
	return keys.back().Time;
}
//-----------------------------------------------------------------------
}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog
