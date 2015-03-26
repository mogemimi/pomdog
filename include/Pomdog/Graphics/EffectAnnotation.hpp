// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTANNOTATION_5BD74CC6_HPP
#define POMDOG_EFFECTANNOTATION_5BD74CC6_HPP

#include "EffectVariableClass.hpp"
#include "EffectVariableType.hpp"

namespace Pomdog {

class EffectAnnotation {
public:
    EffectVariableClass VariableClass;

    EffectVariableType VariableType;

    std::uint8_t ColumnCount;

    std::uint8_t RowCount;

    std::uint8_t Elements;
    //std::optional<std::uint8_t> Elements;
};

}// namespace Pomdog

#endif // POMDOG_EFFECTANNOTATION_5BD74CC6_HPP
