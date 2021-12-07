// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/experimental/skeletal2d/blendtrees/animation_blend_input_type.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <string>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Skeletal2D {

class AnimationBlendInput final {
public:
    ///@todo replace by CRC32
    std::string Name;

    AnimationBlendInputType Type;
};

} // namespace Pomdog::Skeletal2D
