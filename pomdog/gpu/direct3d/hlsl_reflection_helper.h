// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/direct3d/prerequisites_direct3d.h"
#include "pomdog/gpu/forward_declarations.h"

namespace pomdog::detail::direct3d {

struct HLSLReflectionHelper final {
    static EffectVariableType
    ToEffectVariableType(D3D_SHADER_VARIABLE_TYPE variableType) noexcept;

    static EffectVariableClass
    ToEffectVariableClass(D3D_SHADER_VARIABLE_CLASS variableClass) noexcept;
};

} // namespace pomdog::detail::direct3d
