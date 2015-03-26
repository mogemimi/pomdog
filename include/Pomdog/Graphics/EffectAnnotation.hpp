// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_EFFECTANNOTATION_5BD74CC6_417B_4D9C_B287_1006E96FB624_HPP
#define POMDOG_EFFECTANNOTATION_5BD74CC6_417B_4D9C_B287_1006E96FB624_HPP

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

#endif // !defined(POMDOG_EFFECTANNOTATION_5BD74CC6_417B_4D9C_B287_1006E96FB624_HPP)
