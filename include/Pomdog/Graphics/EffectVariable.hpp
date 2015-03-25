// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTVARIABLE_B6867EDF_BF55_45D6_AB68_A610A5557AEA_HPP
#define POMDOG_EFFECTVARIABLE_B6867EDF_BF55_45D6_AB68_A610A5557AEA_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "EffectAnnotation.hpp"
#include <cstdint>
#include <string>

namespace Pomdog {

class EffectVariable {
public:
	std::string Name;
	EffectAnnotation Annotation;
	std::uint32_t StartOffset;
};

}// namespace Pomdog

#endif // !defined(POMDOG_EFFECTVARIABLE_B6867EDF_BF55_45D6_AB68_A610A5557AEA_HPP)
