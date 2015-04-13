// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "InputLayoutDirect3D11.hpp"
#include "../RenderSystem.DXGI/DXGIFormatHelper.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "Pomdog/Graphics/InputLayoutDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <d3dcompiler.h>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

static D3D11_INPUT_CLASSIFICATION ToD3D11InputClassification(
    InputClassification slotClass) noexcept
{
    switch (slotClass) {
    case InputClassification::InputPerVertex: return D3D11_INPUT_PER_VERTEX_DATA;
    case InputClassification::InputPerInstance: return D3D11_INPUT_PER_INSTANCE_DATA;
    }
    return D3D11_INPUT_PER_VERTEX_DATA;
}
//-----------------------------------------------------------------------
static std::vector<D3D11_INPUT_ELEMENT_DESC> BuildInputElements(
    std::vector<D3D11_SIGNATURE_PARAMETER_DESC> const& signatureParameters,
    InputLayoutDescription const& description)
{
    POMDOG_ASSERT(!signatureParameters.empty());
    POMDOG_ASSERT(!description.InputElements.empty());
    POMDOG_ASSERT(signatureParameters.size() == description.InputElements.size());

    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
    inputElements.reserve(description.InputElements.size());

    auto signature = std::begin(signatureParameters);

    for (auto & sourceElement: description.InputElements)
    {
        POMDOG_ASSERT(signature != std::end(signatureParameters));

        if (signature == std::end(signatureParameters)) {
            ///@todo throw exception
            // error: FUS RO DAH!
            break;
        }

        POMDOG_ASSERT(sourceElement.InstanceStepRate == 0 ||
            sourceElement.InputSlotClass == InputClassification::InputPerInstance);

        using DXGI::DXGIFormatHelper;

        D3D11_INPUT_ELEMENT_DESC elementDesc;
        elementDesc.SemanticName = signature->SemanticName;
        elementDesc.SemanticIndex = signature->SemanticIndex;
        elementDesc.Format = DXGIFormatHelper::ToDXGIFormat(sourceElement.Format);
        elementDesc.InputSlot = sourceElement.InputSlot;
        elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        elementDesc.InputSlotClass = ToD3D11InputClassification(sourceElement.InputSlotClass);
        elementDesc.InstanceDataStepRate = sourceElement.InstanceStepRate;

        POMDOG_ASSERT(elementDesc.InputSlot <= D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
        POMDOG_ASSERT(elementDesc.InstanceDataStepRate >= 0);

        inputElements.push_back(std::move(elementDesc));

        ++signature;
    }

    return std::move(inputElements);
}
//-----------------------------------------------------------------------
static std::vector<D3D11_SIGNATURE_PARAMETER_DESC> EnumerateSignatureParameters(
    ID3D11ShaderReflection* shaderReflector,
    D3D11_SHADER_DESC const& shaderDesc)
{
    POMDOG_ASSERT(shaderReflector);

    std::vector<D3D11_SIGNATURE_PARAMETER_DESC> signatureParameters;

    for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
    {
        D3D11_SIGNATURE_PARAMETER_DESC signatureDesc;
        shaderReflector->GetInputParameterDesc(i, &signatureDesc);

        switch (signatureDesc.SystemValueType) {
        case D3D_NAME_INSTANCE_ID:
        case D3D_NAME_VERTEX_ID:
        case D3D_NAME_PRIMITIVE_ID:
            continue;
        }
        signatureParameters.push_back(signatureDesc);
    }

    return std::move(signatureParameters);
}
//-----------------------------------------------------------------------
static void ReflectShaderBytecode(
    ShaderBytecode const& shaderBytecode,
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> & shaderReflector,
    D3D11_SHADER_DESC & shaderDesc)
{
    HRESULT hr = D3DReflect(shaderBytecode.Code, shaderBytecode.ByteLength,
        IID_ID3D11ShaderReflection, reinterpret_cast<void**>(shaderReflector.GetAddressOf()));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to D3DReflect");
    }

    POMDOG_ASSERT(shaderReflector);
    hr = shaderReflector->GetDesc(&shaderDesc);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to get shader description");
    }
}

} // unnamed namespace
//-----------------------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D11InputLayout> InputLayoutHelper::CreateInputLayout(
    ID3D11Device* device,
    ShaderBytecode const& vertexShaderBytecode,
    InputLayoutDescription const& description)
{
    POMDOG_ASSERT(device);
    POMDOG_ASSERT(vertexShaderBytecode.Code);

    D3D11_SHADER_DESC shaderDesc;
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> shaderReflector;

    ReflectShaderBytecode(vertexShaderBytecode, shaderReflector, shaderDesc);

    auto signatureParameters = EnumerateSignatureParameters(shaderReflector.Get(), shaderDesc);
    auto inputElements = BuildInputElements(signatureParameters, description);

    Microsoft::WRL::ComPtr<ID3D11InputLayout> nativeInputLayout;
    HRESULT hr = device->CreateInputLayout(inputElements.data(), inputElements.size(),
        vertexShaderBytecode.Code, vertexShaderBytecode.ByteLength, &nativeInputLayout);

    if (FAILED(hr)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to create InputLayout");
    }

    return std::move(nativeInputLayout);
}
//-----------------------------------------------------------------------
} // namespace Direct3D11
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog
