// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_ANIMATIONBLENDINPUT_304DF276_651C_4D69_8408_F9354A03E7D8_HPP
#define POMDOG_ANIMATIONBLENDINPUT_304DF276_651C_4D69_8408_F9354A03E7D8_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "AnimationBlendInputType.hpp"
#include <string>

namespace Pomdog {

class AnimationBlendInput {
public:
	///@todo replace by CRC32
	std::string Name;

	AnimationBlendInputType Type;
};

}// namespace Pomdog

#endif // !defined(POMDOG_ANIMATIONBLENDINPUT_304DF276_651C_4D69_8408_F9354A03E7D8_HPP)
