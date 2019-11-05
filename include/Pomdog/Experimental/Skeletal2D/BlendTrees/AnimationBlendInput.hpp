// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Experimental/Skeletal2D/BlendTrees/AnimationBlendInputType.hpp"
#include <string>

namespace Pomdog::Skeletal2D {

class AnimationBlendInput final {
public:
    ///@todo replace by CRC32
    std::string Name;

    AnimationBlendInputType Type;
};

} // namespace Pomdog::Skeletal2D
