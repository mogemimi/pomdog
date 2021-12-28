// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/effect_variable_class.h"
#include "pomdog/gpu/effect_variable_type.h"

namespace pomdog {

struct EffectAnnotation final {
    EffectVariableClass VariableClass;
    EffectVariableType VariableType;
    std::uint8_t ColumnCount;
    std::uint8_t RowCount;
    std::uint8_t Elements;
};

} // namespace pomdog
