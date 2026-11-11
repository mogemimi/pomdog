// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_blend_input_type.h"

namespace pomdog::skeletal2d {

class AnimationBlendInput final {
public:
    /// Key is the xxHash32 hash of the parameter name, as computeStringHash32()
    /// computes it. Animator::SetFloat() and Animator::SetBool() take it.
    u32 Key;

    AnimationBlendInputType Type;
};

} // namespace pomdog::skeletal2d
