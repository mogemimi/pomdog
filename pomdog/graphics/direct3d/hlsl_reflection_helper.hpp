// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/direct3d/prerequisites_direct3d.hpp"
#include "pomdog/graphics/forward_declarations.hpp"

namespace Pomdog::Detail::Direct3D {

struct HLSLReflectionHelper final {
    static EffectVariableType
    ToEffectVariableType(D3D_SHADER_VARIABLE_TYPE variableType) noexcept;

    static EffectVariableClass
    ToEffectVariableClass(D3D_SHADER_VARIABLE_CLASS variableClass) noexcept;
};

} // namespace Pomdog::Detail::Direct3D
