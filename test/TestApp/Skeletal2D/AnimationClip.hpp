//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANIMATIONCLIP_5D3DFD31_D6B5_4405_B247_232C105BB927_HPP
#define POMDOG_ANIMATIONCLIP_5D3DFD31_D6B5_4405_B247_232C105BB927_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <vector>
#include <memory>
#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class AnimationTimeline;
class Skeleton;

class AnimationClip {
public:
	AnimationClip() = default;

	explicit AnimationClip(std::vector<std::unique_ptr<AnimationTimeline>> && timelinesIn);

	void Apply(Skeleton & skeleton, DurationSeconds const& time);

private:
	std::vector<std::unique_ptr<AnimationTimeline>> timelines;
};

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONCLIP_5D3DFD31_D6B5_4405_B247_232C105BB927_HPP)
