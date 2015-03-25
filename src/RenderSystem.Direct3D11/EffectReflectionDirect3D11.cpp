// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "EffectReflectionDirect3D11.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "Pomdog/Graphics/EffectConstantDescription.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

static ID3D11ShaderReflection* ReflectShaderBytecode(ShaderBytecode const& shaderBytecode)
{
    ID3D11ShaderReflection* shaderReflector = nullptr;

    HRESULT hr = D3DReflect(shaderBytecode.Code, shaderBytecode.ByteLength,
        IID_ID3D11ShaderReflection, reinterpret_cast<void**>(&shaderReflector));

    if (FAILED(hr))
    {
        // FUS RO DAH!!
        ///@todo throw exception
    }

    return std::move(shaderReflector);
}
//-----------------------------------------------------------------------
static EffectVariableType ToEffectVariableType(D3D_SHADER_VARIABLE_TYPE variableType)
{
    switch (variableType) {
    case D3D10_SVT_FLOAT: return EffectVariableType::Float;
    case D3D10_SVT_INT: return EffectVariableType::Int32;
    case D3D10_SVT_BOOL: return EffectVariableType::Bool;
    case D3D10_SVT_VOID: return EffectVariableType::Void;
    case D3D10_SVT_TEXTURE2D: return EffectVariableType::Texture2D;
    case D3D10_SVT_TEXTURECUBE: return EffectVariableType::TextureCube;
    case D3D10_SVT_STRING: return EffectVariableType::String;
    default:
        break;
    }
    return EffectVariableType::Float;
}
//-----------------------------------------------------------------------
static EffectVariableClass ToEffectVariableClass(D3D_SHADER_VARIABLE_CLASS variableClass)
{
    switch (variableClass) {
    case D3D10_SVC_SCALAR: return EffectVariableClass::Scalar;
    case D3D10_SVC_VECTOR: return EffectVariableClass::Vector;
    case D3D10_SVC_MATRIX_ROWS: return EffectVariableClass::Matrix;
    case D3D10_SVC_MATRIX_COLUMNS: return EffectVariableClass::Matrix;
    case D3D10_SVC_OBJECT: return EffectVariableClass::Object;
    case D3D10_SVC_STRUCT: return EffectVariableClass::Struct;
    default:
        break;
    }
    return EffectVariableClass::Scalar;
}
//-----------------------------------------------------------------------
static std::vector<EffectVariable> EnumerateEffectVariables(
    ID3D11ShaderReflectionConstantBuffer* constantBufferReflector)
{
    POMDOG_ASSERT(constantBufferReflector);

    D3D11_SHADER_BUFFER_DESC constantBufferDesc;
    HRESULT hr = constantBufferReflector->GetDesc(&constantBufferDesc);

    if (FAILED(hr)) {
        // FUS RO DAH!!
        ///@todo throw exception
    }

    std::vector<EffectVariable> variables;

    if (constantBufferDesc.Variables > 0) {
        variables.reserve(constantBufferDesc.Variables);
    }

    for (UINT index = 0; index < constantBufferDesc.Variables; ++index)
    {
        D3D11_SHADER_VARIABLE_DESC shaderVariableDesc;
        D3D11_SHADER_TYPE_DESC shaderVariableTypeDesc;

        {
            auto variableReflector = constantBufferReflector->GetVariableByIndex(index);
            hr = variableReflector->GetDesc(&shaderVariableDesc);

            if (FAILED(hr)) {
                // FUS RO DAH!!
                ///@todo throw exception
            }

            auto shaderVariableType = variableReflector->GetType();
            hr = shaderVariableType->GetDesc(&shaderVariableTypeDesc);

            if (FAILED(hr)) {
                // FUS RO DAH!!
                ///@todo throw exception
            }
        }

        EffectAnnotation annotation;
        annotation.VariableClass = ToEffectVariableClass(shaderVariableTypeDesc.Class);
        annotation.VariableType = ToEffectVariableType(shaderVariableTypeDesc.Type);
        annotation.ColumnCount = static_cast<uint8_t>(shaderVariableTypeDesc.Columns);
        annotation.RowCount = static_cast<uint8_t>(shaderVariableTypeDesc.Rows);
        annotation.Elements = static_cast<uint8_t>(shaderVariableTypeDesc.Elements);

        POMDOG_ASSERT(shaderVariableTypeDesc.Columns <= std::numeric_limits<decltype(annotation.ColumnCount)>::max());
        POMDOG_ASSERT(shaderVariableTypeDesc.Rows <= std::numeric_limits<decltype(annotation.RowCount)>::max());
        POMDOG_ASSERT(shaderVariableTypeDesc.Elements <= std::numeric_limits<decltype(annotation.Elements)>::max());

        EffectVariable effectVariable;
        effectVariable.Name = shaderVariableDesc.Name;
        effectVariable.StartOffset = shaderVariableDesc.StartOffset;
        effectVariable.Annotation = std::move(annotation);

        variables.push_back(std::move(effectVariable));
    }

    return std::move(variables);
}
//-----------------------------------------------------------------------
static void EnumerateConstantBuffer(ID3D11ShaderReflection* shaderReflector,
    std::vector<EffectConstantDescription> & output)
{
    POMDOG_ASSERT(shaderReflector);

    D3D11_SHADER_DESC shaderDesc;
    HRESULT hr = shaderReflector->GetDesc(&shaderDesc);

    if (FAILED(hr))
    {
        // FUS RO DAH!!
        ///@todo throw exception
    }

    for (UINT index = 0; index < shaderDesc.ConstantBuffers; ++index)
    {
        POMDOG_ASSERT(shaderReflector);
        auto constantBufferReflector = shaderReflector->GetConstantBufferByIndex(index);

        D3D11_SHADER_BUFFER_DESC bufferDesc;
        hr = constantBufferReflector->GetDesc(&bufferDesc);

        if (FAILED(hr))
        {
            // FUS RO DAH!!
            ///@todo throw exception
            continue;
        }

        //if (D3D11_CT_CBUFFER != constantBufferDesc.Type)
        //{
        //    ///@todo Not implemented
        //    //tbuffer
        //    //continue;
        //}

        EffectConstantDescription description;
        description.Name = bufferDesc.Name;
        description.ByteSize = bufferDesc.Size;
        description.Variables = EnumerateEffectVariables(constantBufferReflector);
        output.push_back(std::move(description));
    }
}

}// unnamed namespace
//-----------------------------------------------------------------------
EffectReflectionDirect3D11::EffectReflectionDirect3D11(
    ShaderBytecode const& vertexShaderBytecode,
    ShaderBytecode const& pixelShaderBytecode)
{
    vertexShaderReflector = ReflectShaderBytecode(vertexShaderBytecode);
    pixelShaderReflector = ReflectShaderBytecode(pixelShaderBytecode);
}
//-----------------------------------------------------------------------
std::vector<EffectConstantDescription> EffectReflectionDirect3D11::GetConstantBuffers() const
{
    std::vector<EffectConstantDescription> result;

    EnumerateConstantBuffer(vertexShaderReflector.Get(), result);
    EnumerateConstantBuffer(pixelShaderReflector.Get(), result);

    std::sort(std::begin(result), std::end(result),
        [](EffectConstantDescription const& a, EffectConstantDescription const& b) {
            return a.Name < b.Name;
        });

    auto equal = [](EffectConstantDescription const& a, EffectConstantDescription const& b) {
    #if defined(DEBUG) && !defined(NDEBUG)
        if (a.Name == b.Name)
        {
            POMDOG_ASSERT(a.ByteSize == b.ByteSize);
            POMDOG_ASSERT(a.Variables.size() == b.Variables.size());

            for (std::size_t i = 0; i < a.Variables.size(); ++i) {
                POMDOG_ASSERT(a.Variables[i].Name == b.Variables[i].Name);
                POMDOG_ASSERT(a.Variables[i].StartOffset == b.Variables[i].StartOffset);
                POMDOG_ASSERT(a.Variables[i].Annotation.VariableClass == b.Variables[i].Annotation.VariableClass);
                POMDOG_ASSERT(a.Variables[i].Annotation.VariableType == b.Variables[i].Annotation.VariableType);
                POMDOG_ASSERT(a.Variables[i].Annotation.Elements == b.Variables[i].Annotation.Elements);
                POMDOG_ASSERT(a.Variables[i].Annotation.ColumnCount == b.Variables[i].Annotation.ColumnCount);
                POMDOG_ASSERT(a.Variables[i].Annotation.RowCount == b.Variables[i].Annotation.RowCount);
            }
        }
    #endif
        return a.Name == b.Name;
    };

    result.erase(std::unique(std::begin(result), std::end(result), equal), std::end(result));
    result.shrink_to_fit();

    return std::move(result);
}
//-----------------------------------------------------------------------
}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog
