// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D.hpp"
#include "Pomdog/Graphics/ForwardDeclarations.hpp"

namespace Pomdog::Detail::Direct3D {

struct HLSLReflectionHelper final {
    static EffectVariableType
    ToEffectVariableType(D3D_SHADER_VARIABLE_TYPE variableType) noexcept;

    static EffectVariableClass
    ToEffectVariableClass(D3D_SHADER_VARIABLE_CLASS variableClass) noexcept;
};

} // namespace Pomdog::Detail::Direct3D
