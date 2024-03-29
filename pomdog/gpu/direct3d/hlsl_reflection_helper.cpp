// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d/hlsl_reflection_helper.h"
#include "pomdog/gpu/shader_reflections/effect_variable_class.h"
#include "pomdog/gpu/shader_reflections/effect_variable_type.h"

namespace pomdog::gpu::detail::direct3d {

EffectVariableType HLSLReflectionHelper::ToEffectVariableType(
    D3D_SHADER_VARIABLE_TYPE variableType) noexcept
{
    switch (variableType) {
    case D3D_SVT_FLOAT:
        return EffectVariableType::Float;
    case D3D_SVT_INT:
        return EffectVariableType::Int32;
    case D3D_SVT_BOOL:
        return EffectVariableType::Bool;
    case D3D_SVT_VOID:
        return EffectVariableType::Void;
    case D3D_SVT_TEXTURE2D:
        return EffectVariableType::Texture2D;
    case D3D_SVT_TEXTURECUBE:
        return EffectVariableType::TextureCube;
    case D3D_SVT_STRING:
        return EffectVariableType::String;
    default:
        break;
    }
    return EffectVariableType::Float;
}

EffectVariableClass HLSLReflectionHelper::ToEffectVariableClass(
    D3D_SHADER_VARIABLE_CLASS variableClass) noexcept
{
    switch (variableClass) {
    case D3D_SVC_SCALAR:
        return EffectVariableClass::Scalar;
    case D3D_SVC_VECTOR:
        return EffectVariableClass::Vector;
    case D3D_SVC_MATRIX_ROWS:
        return EffectVariableClass::Matrix;
    case D3D_SVC_MATRIX_COLUMNS:
        return EffectVariableClass::Matrix;
    case D3D_SVC_OBJECT:
        return EffectVariableClass::Object;
    case D3D_SVC_STRUCT:
        return EffectVariableClass::Struct;
    default:
        break;
    }
    return EffectVariableClass::Scalar;
}

} // namespace pomdog::gpu::detail::direct3d
