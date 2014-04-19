//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_ANIMATIONLOADER_F321C327_9116_4965_B08F_63A9EE4E820C_HPP
#define POMDOG_ANIMATIONLOADER_F321C327_9116_4965_B08F_63A9EE4E820C_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Pomdog.hpp>

namespace Pomdog {

class AnimationClip;

namespace Details {
namespace Skeletal2D {

class SkeletonDesc;

AnimationClip CreateAnimationClip(SkeletonDesc const& desc);

}// namespace Skeletal2D
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONLOADER_F321C327_9116_4965_B08F_63A9EE4E820C_HPP)
