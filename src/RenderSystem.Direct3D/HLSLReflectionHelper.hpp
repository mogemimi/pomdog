// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Detail {
namespace Direct3D {

struct HLSLReflectionHelper {
    static EffectVariableType
    ToEffectVariableType(D3D_SHADER_VARIABLE_TYPE variableType) noexcept;

    static EffectVariableClass
    ToEffectVariableClass(D3D_SHADER_VARIABLE_CLASS variableClass) noexcept;
};

} // namespace Direct3D
} // namespace Detail
} // namespace Pomdog
