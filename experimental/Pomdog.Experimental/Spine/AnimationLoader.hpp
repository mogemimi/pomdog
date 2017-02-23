// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#pragma once

#include <string>

namespace Pomdog {

class AnimationClip;

namespace Spine {

class SkeletonDesc;

AnimationClip CreateAnimationClip(SkeletonDesc const& desc, std::string const& name);

} // namespace Spine
} // namespace Pomdog
