// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_ANIMATIONBLENDINPUT_304DF276_HPP
#define POMDOG_ANIMATIONBLENDINPUT_304DF276_HPP

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

#endif // POMDOG_ANIMATIONBLENDINPUT_304DF276_HPP
