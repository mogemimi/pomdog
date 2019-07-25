// Copyright (c) 2013-2019 mogemimi. Distributed under the MIT license.

#pragma once

#include "AnimationBlendInputType.hpp"
#include <string>

namespace Pomdog {

class AnimationBlendInput {
public:
    ///@todo replace by CRC32
    std::string Name;

    AnimationBlendInputType Type;
};

} // namespace Pomdog
