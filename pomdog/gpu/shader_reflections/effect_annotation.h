// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/shader_reflections/effect_variable_class.h"
#include "pomdog/gpu/shader_reflections/effect_variable_type.h"

namespace pomdog::gpu {

struct EffectAnnotation final {
    EffectVariableClass VariableClass;
    EffectVariableType VariableType;
    std::uint8_t ColumnCount;
    std::uint8_t RowCount;
    std::uint8_t Elements;
};

} // namespace pomdog::gpu
