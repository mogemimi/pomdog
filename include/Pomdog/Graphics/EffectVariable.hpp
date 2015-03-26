// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTVARIABLE_B6867EDF_HPP
#define POMDOG_EFFECTVARIABLE_B6867EDF_HPP

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

#endif // POMDOG_EFFECTVARIABLE_B6867EDF_HPP
