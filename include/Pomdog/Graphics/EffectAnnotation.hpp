// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/EffectVariableClass.hpp"
#include "Pomdog/Graphics/EffectVariableType.hpp"

namespace Pomdog {

struct EffectAnnotation final {
    EffectVariableClass VariableClass;
    EffectVariableType VariableType;
    std::uint8_t ColumnCount;
    std::uint8_t RowCount;
    std::uint8_t Elements;
};

} // namespace Pomdog
