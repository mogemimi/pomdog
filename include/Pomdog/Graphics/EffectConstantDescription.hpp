// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTCONSTANTDESCRIPTION_5EBD0C28_B36E_485D_AD3C_2D1108556F9C_HPP
#define POMDOG_EFFECTCONSTANTDESCRIPTION_5EBD0C28_B36E_485D_AD3C_2D1108556F9C_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "EffectVariable.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace Pomdog {

class EffectConstantDescription {
public:
	std::vector<EffectVariable> Variables;
	std::string Name;
	std::uint32_t ByteSize;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTCONSTANTDESCRIPTION_5EBD0C28_B36E_485D_AD3C_2D1108556F9C_HPP)
