// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "PipelineStateDirect3D11.hpp"
#include "GraphicsDeviceDirect3D11.hpp"
#include "ShaderDirect3D11.hpp"
#include "../RenderSystem.DXGI/DXGIFormatHelper.hpp"
#include "../RenderSystem.Direct3D/PrerequisitesDirect3D.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "Pomdog/Graphics/InputLayoutDescription.hpp"
#include "Pomdog/Graphics/PipelineStateDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <algorithm>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace Direct3D11 {
namespace {

using Microsoft::WRL::ComPtr;

D3D11_BLEND_OP ToBlendOperation(BlendOperation blendOperation) noexcept
{
    switch (blendOperation) {
    case BlendOperation::Add: return D3D11_BLEND_OP_ADD;
    case BlendOperation::Subtract: return D3D11_BLEND_OP_SUBTRACT;
    case BlendOperation::ReverseSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
    case BlendOperation::Min: return D3D11_BLEND_OP_MIN;
    case BlendOperation::Max: return D3D11_BLEND_OP_MAX;
    }
    return D3D11_BLEND_OP_ADD;
}

D3D11_BLEND ToBlend(Blend blend) noexcept
{
    switch (blend) {
    case Blend::Zero: return D3D11_BLEND_ZERO;
    case Blend::One: return D3D11_BLEND_ONE;
    case Blend::SourceColor: return D3D11_BLEND_SRC_COLOR;
    case Blend::InverseSourceColor: return D3D11_BLEND_INV_SRC_COLOR;
    case Blend::SourceAlpha: return D3D11_BLEND_SRC_ALPHA;
    case Blend::InverseSourceAlpha: return D3D11_BLEND_INV_SRC_ALPHA;
    case Blend::DestinationAlpha: return D3D11_BLEND_DEST_ALPHA;
    case Blend::InverseDestinationAlpha: return D3D11_BLEND_INV_DEST_ALPHA;
    case Blend::DestinationColor: return D3D11_BLEND_DEST_COLOR;
    case Blend::InverseDestinationColor: return D3D11_BLEND_INV_DEST_COLOR;
    case Blend::SourceAlphaSaturation: return D3D11_BLEND_SRC_ALPHA_SAT;
    case Blend::BlendFactor: return D3D11_BLEND_BLEND_FACTOR;
    case Blend::InvereseBlendFactor: return D3D11_BLEND_INV_BLEND_FACTOR;
    case Blend::Source1Color: return D3D11_BLEND_SRC1_COLOR;
    case Blend::InverseSource1Color: return D3D11_BLEND_INV_SRC1_COLOR;
    case Blend::Source1Alpha: return D3D11_BLEND_SRC1_ALPHA;
    case Blend::InverseSource1Alpha: return D3D11_BLEND_INV_SRC1_ALPHA;
    }
    return D3D11_BLEND_ONE;
}

D3D11_STENCIL_OP ToStencilOperation(StencilOperation operation) noexcept
{
    switch (operation) {
    case StencilOperation::Keep: return D3D11_STENCIL_OP_KEEP;
    case StencilOperation::Zero: return D3D11_STENCIL_OP_ZERO;
    case StencilOperation::Replace: return D3D11_STENCIL_OP_REPLACE;
    case StencilOperation::IncrementSaturation: return D3D11_STENCIL_OP_INCR_SAT;
    case StencilOperation::DecrementSaturation: return D3D11_STENCIL_OP_DECR_SAT;
    case StencilOperation::Invert: return D3D11_STENCIL_OP_INVERT;
    case StencilOperation::Increment: return D3D11_STENCIL_OP_INCR;
    case StencilOperation::Decrement: return D3D11_STENCIL_OP_DECR;
    }
    return D3D11_STENCIL_OP_KEEP;
}

D3D11_COMPARISON_FUNC ToComparisonFunction(ComparisonFunction compareFunction) noexcept
{
    switch (compareFunction) {
    case ComparisonFunction::Never: return D3D11_COMPARISON_NEVER;
    case ComparisonFunction::Less:  return D3D11_COMPARISON_LESS;
    case ComparisonFunction::Equal: return D3D11_COMPARISON_EQUAL;
    case ComparisonFunction::LessEqual: return D3D11_COMPARISON_LESS_EQUAL;
    case ComparisonFunction::Greater:   return D3D11_COMPARISON_GREATER;
    case ComparisonFunction::NotEqual:  return D3D11_COMPARISON_NOT_EQUAL;
    case ComparisonFunction::GreaterEqual: return D3D11_COMPARISON_GREATER_EQUAL;
    case ComparisonFunction::Always: return D3D11_COMPARISON_ALWAYS;
    }
    return D3D11_COMPARISON_LESS_EQUAL;
}

D3D11_CULL_MODE ToCullMode(CullMode cullMode) noexcept
{
    switch (cullMode) {
    case CullMode::ClockwiseFace: return D3D11_CULL_FRONT;
    case CullMode::CounterClockwiseFace: return D3D11_CULL_BACK;
    case CullMode::None: return D3D11_CULL_NONE;
    }
    return D3D11_CULL_BACK;
}

D3D11_FILL_MODE ToFillMode(FillMode fillMode) noexcept
{
    switch (fillMode) {
    case FillMode::WireFrame: return D3D11_FILL_WIREFRAME;
    case FillMode::Solid: return D3D11_FILL_SOLID;
    }
    return D3D11_FILL_SOLID;
}

D3D11_INPUT_CLASSIFICATION ToD3D11InputClassification(
    InputClassification slotClass) noexcept
{
    switch (slotClass) {
    case InputClassification::InputPerVertex: return D3D11_INPUT_PER_VERTEX_DATA;
    case InputClassification::InputPerInstance: return D3D11_INPUT_PER_INSTANCE_DATA;
    }
    return D3D11_INPUT_PER_VERTEX_DATA;
}

inline BOOL ToD3D11Boolean(bool is) noexcept
{
    return is ? TRUE : FALSE;
}

void ToD3D11Desc(
    const RenderTargetBlendDescription& desc,
    D3D11_RENDER_TARGET_BLEND_DESC & result) noexcept
{
    result.BlendEnable = ToD3D11Boolean(desc.BlendEnable);
    result.BlendOp = ToBlendOperation(desc.ColorBlendOperation);
    result.BlendOpAlpha = ToBlendOperation(desc.AlphaBlendOperation);
    result.DestBlend = ToBlend(desc.ColorDestinationBlend);
    result.DestBlendAlpha = ToBlend(desc.AlphaDestinationBlend);
    result.SrcBlend = ToBlend(desc.ColorSourceBlend);
    result.SrcBlendAlpha = ToBlend(desc.AlphaSourceBlend);
    result.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
}

ComPtr<ID3D11BlendState> CreateBlendState(
    ID3D11Device* nativeDevice,
    const BlendDescription& description)
{
    D3D11_BLEND_DESC blendDesc;
    ::ZeroMemory(&blendDesc, sizeof(blendDesc));
    blendDesc.AlphaToCoverageEnable = ToD3D11Boolean(description.AlphaToCoverageEnable);
    blendDesc.IndependentBlendEnable = ToD3D11Boolean(description.IndependentBlendEnable);

    const auto renderTargetCount = std::min<int>(
        description.RenderTargets.size(),
        D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

    for (int i = 0; i < renderTargetCount; ++i) {
        ToD3D11Desc(description.RenderTargets[i], blendDesc.RenderTarget[i]);
    }

    POMDOG_ASSERT(nativeDevice);

    ComPtr<ID3D11BlendState> blendState;
    HRESULT hr = nativeDevice->CreateBlendState(&blendDesc, &blendState);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create ID3D11BlendState");
    }

    return std::move(blendState);
}

ComPtr<ID3D11DepthStencilState> CreateDepthStencilState(
    ID3D11Device* nativeDevice,
    const DepthStencilDescription& description)
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    depthStencilDesc.DepthEnable = ToD3D11Boolean(description.DepthBufferEnable);
    depthStencilDesc.DepthFunc = ToComparisonFunction(description.DepthBufferFunction);
    depthStencilDesc.DepthWriteMask = (description.DepthBufferWriteEnable
        ? D3D11_DEPTH_WRITE_MASK_ALL
        : D3D11_DEPTH_WRITE_MASK_ZERO);

    depthStencilDesc.StencilEnable = ToD3D11Boolean(description.StencilEnable);
    depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    depthStencilDesc.BackFace.StencilDepthFailOp = ToStencilOperation(description.CounterClockwiseFace.StencilDepthBufferFail);
    depthStencilDesc.BackFace.StencilFailOp = ToStencilOperation(description.CounterClockwiseFace.StencilFail);
    depthStencilDesc.BackFace.StencilPassOp = ToStencilOperation(description.CounterClockwiseFace.StencilPass);
    depthStencilDesc.BackFace.StencilFunc = ToComparisonFunction(description.CounterClockwiseFace.StencilFunction);

    depthStencilDesc.FrontFace.StencilDepthFailOp = ToStencilOperation(description.ClockwiseFace.StencilDepthBufferFail);
    depthStencilDesc.FrontFace.StencilFailOp = ToStencilOperation(description.ClockwiseFace.StencilFail);
    depthStencilDesc.FrontFace.StencilPassOp = ToStencilOperation(description.ClockwiseFace.StencilPass);
    depthStencilDesc.FrontFace.StencilFunc = ToComparisonFunction(description.ClockwiseFace.StencilFunction);

    POMDOG_ASSERT(nativeDevice);

    ComPtr<ID3D11DepthStencilState> depthStencilState;
    HRESULT hr = nativeDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create ID3D11DepthStencilState");
    }

    return std::move(depthStencilState);
}

ComPtr<ID3D11RasterizerState> CreateRasterizerState(
    ID3D11Device* nativeDevice,
    const RasterizerDescription& description)
{
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

    rasterizerDesc.CullMode = ToCullMode(description.CullMode);
    rasterizerDesc.FillMode = ToFillMode(description.FillMode);
    rasterizerDesc.FrontCounterClockwise = FALSE;

    rasterizerDesc.DepthBias = static_cast<INT>(description.DepthBias);
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias = description.SlopeScaledDepthBias;
    rasterizerDesc.AntialiasedLineEnable = FALSE;
    rasterizerDesc.MultisampleEnable = ToD3D11Boolean(description.MultisampleEnable);
    rasterizerDesc.ScissorEnable = ToD3D11Boolean(description.ScissorTestEnable);

    POMDOG_ASSERT(!rasterizerDesc.AntialiasedLineEnable
        || (rasterizerDesc.AntialiasedLineEnable && !rasterizerDesc.MultisampleEnable));

    POMDOG_ASSERT(nativeDevice);

    ComPtr<ID3D11RasterizerState> rasterizerState;
    HRESULT hr = nativeDevice->CreateRasterizerState(&rasterizerDesc, &rasterizerState);

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create ID3D11RasterizerState");
    }

    return std::move(rasterizerState);
}

void ReflectShaderBytecode(
    const ShaderBytecode& shaderBytecode,
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> & shaderReflector,
    D3D11_SHADER_DESC & shaderDesc)
{
    HRESULT hr = D3DReflect(
        shaderBytecode.Code,
        shaderBytecode.ByteLength,
        IID_PPV_ARGS(&shaderReflector));

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

//struct ConstantBufferBindDesc {
//    std::string Name;
//    UINT BindPoint;
//};
//
//void EnumerateConstantBuffers(
//    const ShaderBytecode& shaderBytecode,
//    std::vector<ConstantBufferBindDesc> & output)
//{
//    POMDOG_ASSERT(shaderBytecode.Code);
//
//    ComPtr<ID3D11ShaderReflection> shaderReflector;
//    D3D11_SHADER_DESC shaderDesc;
//
//    ReflectShaderBytecode(shaderBytecode, shaderReflector, shaderDesc);
//
//    for (UINT i = 0; i < shaderDesc.ConstantBuffers; ++i)
//    {
//        POMDOG_ASSERT(shaderReflector);
//        auto constantBufferReflector = shaderReflector->GetConstantBufferByIndex(i);
//
//        D3D11_SHADER_BUFFER_DESC bufferDesc;
//        HRESULT hr = constantBufferReflector->GetDesc(&bufferDesc);
//
//        if (FAILED(hr))
//        {
//            // FUS RO DAH!!
//            ///@todo throw exception
//            continue;
//        }
//
//        //if (D3D_CT_CBUFFER != bufferDesc.Type)
//        //{
//        //    ///@todo Not implemented
//        //    //tbuffer
//        //    //continue;
//        //}
//
//        D3D11_SHADER_INPUT_BIND_DESC shaderInputBindDesc;
//        shaderReflector->GetResourceBindingDescByName(bufferDesc.Name, &shaderInputBindDesc);
//
//        ConstantBufferBindDesc desc;
//        desc.Name = bufferDesc.Name;
//        desc.BindPoint = shaderInputBindDesc.BindPoint;
//
//        POMDOG_ASSERT(shaderInputBindDesc.BindPoint
//            <= D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
//        output.push_back(std::move(desc));
//    }
//}

//std::vector<ConstantBufferBindDesc> CreateConstantBufferBindDescs(
//    const ShaderBytecode& vertexShaderBytecode,
//    const ShaderBytecode& pixelShaderBytecode)
//{
//    using Desc = ConstantBufferBindDesc;
//
//    std::vector<Desc> bindings;
//    EnumerateConstantBuffers(vertexShaderBytecode, bindings);
//    EnumerateConstantBuffers(pixelShaderBytecode, bindings);
//
//    std::sort(std::begin(bindings), std::end(bindings),
//        [](const Desc& a, const Desc& b) { return a.Name < b.Name; });
//
//    bindings.erase(std::unique(std::begin(bindings), std::end(bindings),
//        [](const Desc& a, const Desc& b) { return a.Name == b.Name; }), std::end(bindings));
//
//    bindings.shrink_to_fit();
//
//    return std::move(bindings);
//}

std::vector<D3D11_INPUT_ELEMENT_DESC> BuildInputElements(
    const std::vector<D3D11_SIGNATURE_PARAMETER_DESC>& signatureParameters,
    const InputLayoutDescription& description)
{
    POMDOG_ASSERT(!signatureParameters.empty());
    POMDOG_ASSERT(!description.InputElements.empty());
    POMDOG_ASSERT(signatureParameters.size() == description.InputElements.size());

    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
    inputElements.reserve(description.InputElements.size());

    auto signature = std::begin(signatureParameters);

    for (auto& sourceElement : description.InputElements) {
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

std::vector<D3D11_SIGNATURE_PARAMETER_DESC>
EnumerateSignatureParameters(
    ID3D11ShaderReflection* shaderReflector,
    const D3D11_SHADER_DESC& shaderDesc)
{
    POMDOG_ASSERT(shaderReflector);

    std::vector<D3D11_SIGNATURE_PARAMETER_DESC> signatureParameters;

    for (UINT i = 0; i < shaderDesc.InputParameters; ++i) {
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

Microsoft::WRL::ComPtr<ID3D11InputLayout> CreateInputLayout(
    ID3D11Device* device,
    const ShaderBytecode& vertexShaderBytecode,
    const InputLayoutDescription& description)
{
    POMDOG_ASSERT(device);
    POMDOG_ASSERT(vertexShaderBytecode.Code);

    D3D11_SHADER_DESC shaderDesc;
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> shaderReflector;

    ReflectShaderBytecode(vertexShaderBytecode, shaderReflector, shaderDesc);

    auto signatureParameters = EnumerateSignatureParameters(
        shaderReflector.Get(), shaderDesc);

    auto inputElements = BuildInputElements(
        signatureParameters, description);

    Microsoft::WRL::ComPtr<ID3D11InputLayout> nativeInputLayout;
    HRESULT hr = device->CreateInputLayout(
        inputElements.data(), inputElements.size(),
        vertexShaderBytecode.Code, vertexShaderBytecode.ByteLength,
        &nativeInputLayout);

    if (FAILED(hr)) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create input layout");
    }
    return std::move(nativeInputLayout);
}

} // unnamed namespace

PipelineStateDirect3D11::PipelineStateDirect3D11(
    ID3D11Device* device,
    const PipelineStateDescription& description)
{
    POMDOG_ASSERT(device);

    sampleMask = description.MultiSampleMask;

    blendState = CreateBlendState(device, description.BlendState);
    depthStencilState = CreateDepthStencilState(device, description.DepthStencilState);
    rasterizerState = CreateRasterizerState(device, description.RasterizerState);

    auto vertexShaderD3D = std::dynamic_pointer_cast<VertexShaderDirect3D11>(description.VertexShader);
    if (!vertexShaderD3D) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid vertex shader.");
    }

    vertexShader = vertexShaderD3D->GetShader();
    if (!vertexShader) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "The vertex shader is null");
    }

    auto pixelShaderD3D = std::dynamic_pointer_cast<PixelShaderDirect3D11>(description.PixelShader);
    if (!pixelShaderD3D) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid pixel shader.");
    }

    pixelShader = pixelShaderD3D->GetShader();
    if (!pixelShader) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "The pixel shader is null");
    }

    inputLayout = CreateInputLayout(device,
        vertexShaderD3D->GetShaderBytecode(), description.InputLayout);
}

void PipelineStateDirect3D11::Apply(
    ID3D11DeviceContext * deviceContext,
    FLOAT const blendFactor[4])
{
    POMDOG_ASSERT(deviceContext);
    POMDOG_ASSERT(inputLayout);
    POMDOG_ASSERT(vertexShader);
    POMDOG_ASSERT(pixelShader);
    POMDOG_ASSERT(blendState);
    POMDOG_ASSERT(rasterizerState);
    POMDOG_ASSERT(depthStencilState);

    deviceContext->IASetInputLayout(inputLayout.Get());
    deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
    deviceContext->OMSetBlendState(blendState.Get(), blendFactor, sampleMask);
    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
    deviceContext->RSSetState(rasterizerState.Get());
}

} // namespace Direct3D11
} // namespace Detail
} // namespace Pomdog
