// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/graphics/direct3d/prerequisites_direct3d.hpp"
#include "pomdog/graphics/forward_declarations.hpp"

namespace pomdog::detail::direct3d {

struct HLSLReflectionHelper final {
    static EffectVariableType
    ToEffectVariableType(D3D_SHADER_VARIABLE_TYPE variableType) noexcept;

    static EffectVariableClass
    ToEffectVariableClass(D3D_SHADER_VARIABLE_CLASS variableClass) noexcept;
};

} // namespace pomdog::detail::direct3d
