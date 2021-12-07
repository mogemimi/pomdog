// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/effect_variable_class.hpp"
#include "pomdog/graphics/effect_variable_type.hpp"

namespace Pomdog {

struct EffectAnnotation final {
    EffectVariableClass VariableClass;
    EffectVariableType VariableType;
    std::uint8_t ColumnCount;
    std::uint8_t RowCount;
    std::uint8_t Elements;
};

} // namespace Pomdog
