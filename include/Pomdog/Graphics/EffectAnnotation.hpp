// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "EffectVariableClass.hpp"
#include "EffectVariableType.hpp"

namespace Pomdog {

struct EffectAnnotation {
    EffectVariableClass VariableClass;
    EffectVariableType VariableType;
    std::uint8_t ColumnCount;
    std::uint8_t RowCount;
    std::uint8_t Elements;
};

} // namespace Pomdog
