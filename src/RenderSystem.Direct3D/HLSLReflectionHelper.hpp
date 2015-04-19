// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_HLSLREFLECTIONHELPER_28AF968C_HPP
#define POMDOG_HLSLREFLECTIONHELPER_28AF968C_HPP

#include "PrerequisitesDirect3D.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D {

struct HLSLReflectionHelper {
    static EffectVariableType
    ToEffectVariableType(D3D_SHADER_VARIABLE_TYPE variableType) noexcept;

    static EffectVariableClass
    ToEffectVariableClass(D3D_SHADER_VARIABLE_CLASS variableClass) noexcept;
};

} // namespace Direct3D
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_HLSLREFLECTIONHELPER_28AF968C_HPP
