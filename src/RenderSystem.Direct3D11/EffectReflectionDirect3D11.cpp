// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "EffectReflectionDirect3D11.hpp"
#include "../RenderSystem.Direct3D/HLSLReflectionHelper.hpp"
#include "../RenderSystem.Direct3D/PrerequisitesDirect3D.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "Pomdog/Graphics/EffectConstantDescription.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <algorithm>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace Direct3D11 {
namespace {

using Direct3D::HLSLReflectionHelper;

std::vector<EffectVariable> EnumerateEffectVariables(
    ID3D11ShaderReflectionConstantBuffer* constantBufferReflector)
{
    POMDOG_ASSERT(constantBufferReflector != nullptr);

    D3D11_SHADER_BUFFER_DESC constantBufferDesc;
    HRESULT hr = constantBufferReflector->GetDesc(&constantBufferDesc);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to get shader buffer description");
    }

    std::vector<EffectVariable> variables;

    if (constantBufferDesc.Variables > 0) {
        variables.reserve(constantBufferDesc.Variables);
    }

    for (UINT index = 0; index < constantBufferDesc.Variables; ++index) {
        D3D11_SHADER_VARIABLE_DESC shaderVariableDesc;
        D3D11_SHADER_TYPE_DESC shaderVariableTypeDesc;

        {
            auto variableReflector = constantBufferReflector->GetVariableByIndex(index);
            hr = variableReflector->GetDesc(&shaderVariableDesc);

            if (FAILED(hr)) {
                // FUS RO DAH!
                POMDOG_THROW_EXCEPTION(std::runtime_error,
                    "Failed to get shader variable description");
            }

            auto shaderVariableType = variableReflector->GetType();
            hr = shaderVariableType->GetDesc(&shaderVariableTypeDesc);

            if (FAILED(hr)) {
                // FUS RO DAH!
                POMDOG_THROW_EXCEPTION(std::runtime_error,
                    "Failed to get shader type description");
            }
        }

        EffectAnnotation annotation;
        annotation.VariableClass = HLSLReflectionHelper::ToEffectVariableClass(shaderVariableTypeDesc.Class);
        annotation.VariableType = HLSLReflectionHelper::ToEffectVariableType(shaderVariableTypeDesc.Type);
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

void EnumerateConstantBuffer(
    ID3D11ShaderReflection* shaderReflector,
    std::vector<EffectConstantDescription> & output)
{
    POMDOG_ASSERT(shaderReflector);

    D3D11_SHADER_DESC shaderDesc;
    HRESULT hr = shaderReflector->GetDesc(&shaderDesc);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to get shader description");
    }

    for (UINT index = 0; index < shaderDesc.ConstantBuffers; ++index) {
        POMDOG_ASSERT(shaderReflector);
        auto constantBufferReflector = shaderReflector->GetConstantBufferByIndex(index);

        D3D11_SHADER_BUFFER_DESC bufferDesc;
        hr = constantBufferReflector->GetDesc(&bufferDesc);

        if (FAILED(hr)) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to get shader buffer description");
        }

        //if (D3D_CT_CBUFFER != constantBufferDesc.Type)
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

} // unnamed namespace

EffectReflectionDirect3D11::EffectReflectionDirect3D11(
    const ShaderBytecode& vertexShaderBytecode,
    const ShaderBytecode& pixelShaderBytecode)
{
    HRESULT hr = D3DReflect(
        vertexShaderBytecode.Code,
        vertexShaderBytecode.ByteLength,
        IID_PPV_ARGS(&vertexShaderReflector));

    if (FAILED(hr)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to D3DReflect");
    }

    hr = D3DReflect(
        pixelShaderBytecode.Code,
        pixelShaderBytecode.ByteLength,
        IID_PPV_ARGS(&pixelShaderReflector));

    if (FAILED(hr)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to D3DReflect");
    }
}

std::vector<EffectConstantDescription> EffectReflectionDirect3D11::GetConstantBuffers() const
{
    std::vector<EffectConstantDescription> result;

    EnumerateConstantBuffer(vertexShaderReflector.Get(), result);
    EnumerateConstantBuffer(pixelShaderReflector.Get(), result);

    std::sort(std::begin(result), std::end(result),
        [](const EffectConstantDescription& a, const EffectConstantDescription& b) {
            return a.Name < b.Name;
        });

    auto equal = [](const EffectConstantDescription& a, const EffectConstantDescription& b) {
#if defined(DEBUG) && !defined(NDEBUG)
        if (a.Name == b.Name) {
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

} // namespace Direct3D11
} // namespace Detail
} // namespace Pomdog
