// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ANIMATIONLOADER_F321C327_HPP
#define POMDOG_ANIMATIONLOADER_F321C327_HPP

namespace Pomdog {

class AnimationClip;

namespace Spine {

class SkeletonDesc;

AnimationClip CreateAnimationClip(SkeletonDesc const& desc, char const* name);

}// namespace Spine
}// namespace Pomdog

#endif // POMDOG_ANIMATIONLOADER_F321C327_HPP
