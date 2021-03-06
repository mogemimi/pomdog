// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "PipelineStateDirect3D11.hpp"
#include "FormatHelper.hpp"
#include "GraphicsDeviceDirect3D11.hpp"
#include "ShaderDirect3D11.hpp"
#include "../Graphics.Backends/ShaderBytecode.hpp"
#include "../Graphics.DXGI/DXGIFormatHelper.hpp"
#include "../Graphics.Direct3D/PrerequisitesDirect3D.hpp"
#include "Pomdog/Basic/ConditionalCompilation.hpp"
#include "Pomdog/Graphics/InputLayoutDescription.hpp"
#include "Pomdog/Graphics/PipelineStateDescription.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Utility/Assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Direct3D11 {
namespace {

using Microsoft::WRL::ComPtr;

D3D11_PRIMITIVE_TOPOLOGY
ToPrimitiveTopology(PrimitiveTopology primitiveTopology) noexcept
{
    switch (primitiveTopology) {
    case PrimitiveTopology::TriangleStrip:
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    case PrimitiveTopology::TriangleList:
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case PrimitiveTopology::LineList:
        return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    case PrimitiveTopology::LineStrip:
        return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
    }
    return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
}

D3D11_BLEND_OP ToBlendOperation(BlendOperation blendOperation) noexcept
{
    switch (blendOperation) {
    case BlendOperation::Add:
        return D3D11_BLEND_OP_ADD;
    case BlendOperation::Subtract:
        return D3D11_BLEND_OP_SUBTRACT;
    case BlendOperation::ReverseSubtract:
        return D3D11_BLEND_OP_REV_SUBTRACT;
    case BlendOperation::Min:
        return D3D11_BLEND_OP_MIN;
    case BlendOperation::Max:
        return D3D11_BLEND_OP_MAX;
    }
    return D3D11_BLEND_OP_ADD;
}

D3D11_BLEND ToBlend(Blend blend) noexcept
{
    switch (blend) {
    case Blend::Zero:
        return D3D11_BLEND_ZERO;
    case Blend::One:
        return D3D11_BLEND_ONE;
    case Blend::SourceColor:
        return D3D11_BLEND_SRC_COLOR;
    case Blend::InverseSourceColor:
        return D3D11_BLEND_INV_SRC_COLOR;
    case Blend::SourceAlpha:
        return D3D11_BLEND_SRC_ALPHA;
    case Blend::InverseSourceAlpha:
        return D3D11_BLEND_INV_SRC_ALPHA;
    case Blend::DestinationAlpha:
        return D3D11_BLEND_DEST_ALPHA;
    case Blend::InverseDestinationAlpha:
        return D3D11_BLEND_INV_DEST_ALPHA;
    case Blend::DestinationColor:
        return D3D11_BLEND_DEST_COLOR;
    case Blend::InverseDestinationColor:
        return D3D11_BLEND_INV_DEST_COLOR;
    case Blend::SourceAlphaSaturation:
        return D3D11_BLEND_SRC_ALPHA_SAT;
    case Blend::BlendFactor:
        return D3D11_BLEND_BLEND_FACTOR;
    case Blend::InvereseBlendFactor:
        return D3D11_BLEND_INV_BLEND_FACTOR;
    case Blend::Source1Color:
        return D3D11_BLEND_SRC1_COLOR;
    case Blend::InverseSource1Color:
        return D3D11_BLEND_INV_SRC1_COLOR;
    case Blend::Source1Alpha:
        return D3D11_BLEND_SRC1_ALPHA;
    case Blend::InverseSource1Alpha:
        return D3D11_BLEND_INV_SRC1_ALPHA;
    }
    return D3D11_BLEND_ONE;
}

D3D11_STENCIL_OP ToStencilOperation(StencilOperation operation) noexcept
{
    switch (operation) {
    case StencilOperation::Keep:
        return D3D11_STENCIL_OP_KEEP;
    case StencilOperation::Zero:
        return D3D11_STENCIL_OP_ZERO;
    case StencilOperation::Replace:
        return D3D11_STENCIL_OP_REPLACE;
    case StencilOperation::IncrementSaturation:
        return D3D11_STENCIL_OP_INCR_SAT;
    case StencilOperation::DecrementSaturation:
        return D3D11_STENCIL_OP_DECR_SAT;
    case StencilOperation::Invert:
        return D3D11_STENCIL_OP_INVERT;
    case StencilOperation::Increment:
        return D3D11_STENCIL_OP_INCR;
    case StencilOperation::Decrement:
        return D3D11_STENCIL_OP_DECR;
    }
    return D3D11_STENCIL_OP_KEEP;
}

D3D11_CULL_MODE ToCullMode(CullMode cullMode) noexcept
{
    switch (cullMode) {
    case CullMode::ClockwiseFace:
        return D3D11_CULL_FRONT;
    case CullMode::CounterClockwiseFace:
        return D3D11_CULL_BACK;
    case CullMode::None:
        return D3D11_CULL_NONE;
    }
    return D3D11_CULL_BACK;
}

D3D11_FILL_MODE ToFillMode(FillMode fillMode) noexcept
{
    switch (fillMode) {
    case FillMode::WireFrame:
        return D3D11_FILL_WIREFRAME;
    case FillMode::Solid:
        return D3D11_FILL_SOLID;
    }
    return D3D11_FILL_SOLID;
}

D3D11_INPUT_CLASSIFICATION ToD3D11InputClassification(
    InputClassification slotClass) noexcept
{
    switch (slotClass) {
    case InputClassification::InputPerVertex:
        return D3D11_INPUT_PER_VERTEX_DATA;
    case InputClassification::InputPerInstance:
        return D3D11_INPUT_PER_INSTANCE_DATA;
    }
    return D3D11_INPUT_PER_VERTEX_DATA;
}

inline BOOL ToD3D11Boolean(bool is) noexcept
{
    return is ? TRUE : FALSE;
}

void ToD3D11Desc(
    const RenderTargetBlendDescription& desc,
    D3D11_RENDER_TARGET_BLEND_DESC& result) noexcept
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

[[nodiscard]] std::tuple<ComPtr<ID3D11BlendState>, std::unique_ptr<Error>>
CreateBlendState(
    ID3D11Device* nativeDevice,
    const BlendDescription& description) noexcept
{
    D3D11_BLEND_DESC blendDesc;
    ::ZeroMemory(&blendDesc, sizeof(blendDesc));
    blendDesc.AlphaToCoverageEnable = ToD3D11Boolean(description.AlphaToCoverageEnable);
    blendDesc.IndependentBlendEnable = ToD3D11Boolean(description.IndependentBlendEnable);

    const auto renderTargetCount = std::min<int>(
        static_cast<int>(description.RenderTargets.size()),
        D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

    for (int i = 0; i < renderTargetCount; ++i) {
        ToD3D11Desc(description.RenderTargets[i], blendDesc.RenderTarget[i]);
    }

    POMDOG_ASSERT(nativeDevice != nullptr);

    ComPtr<ID3D11BlendState> blendState;
    HRESULT hr = nativeDevice->CreateBlendState(&blendDesc, &blendState);

    if (FAILED(hr)) {
        return std::make_tuple(nullptr, Errors::New("failed to create ID3D11BlendState"));
    }

    return std::make_tuple(std::move(blendState), nullptr);
}

[[nodiscard]] std::tuple<ComPtr<ID3D11DepthStencilState>, std::unique_ptr<Error>>
CreateDepthStencilState(
    ID3D11Device* nativeDevice,
    const DepthStencilDescription& description) noexcept
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

    POMDOG_ASSERT(nativeDevice != nullptr);

    ComPtr<ID3D11DepthStencilState> depthStencilState;
    HRESULT hr = nativeDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

    if (FAILED(hr)) {
        return std::make_tuple(nullptr, Errors::New("failed to create ID3D11DepthStencilState"));
    }

    return std::make_tuple(std::move(depthStencilState), nullptr);
}

[[nodiscard]] std::tuple<ComPtr<ID3D11RasterizerState>, std::unique_ptr<Error>>
CreateRasterizerState(
    ID3D11Device* nativeDevice,
    const RasterizerDescription& description) noexcept
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

    // NOTE: Modern graphics APIs (Direct3D 12, Metal and Vulkan) don't include
    // a parameter like ScissorEnable to enable/disable scissor test. So we
    // always set ScissorEnable to true in Direct3D 11.
    rasterizerDesc.ScissorEnable = TRUE;

    POMDOG_ASSERT(!rasterizerDesc.AntialiasedLineEnable ||
        (rasterizerDesc.AntialiasedLineEnable && !rasterizerDesc.MultisampleEnable));

    POMDOG_ASSERT(nativeDevice != nullptr);

    ComPtr<ID3D11RasterizerState> rasterizerState;
    HRESULT hr = nativeDevice->CreateRasterizerState(&rasterizerDesc, &rasterizerState);

    if (FAILED(hr)) {
        return std::make_tuple(nullptr, Errors::New("failed to create ID3D11RasterizerState"));
    }

    return std::make_tuple(std::move(rasterizerState), nullptr);
}

[[nodiscard]] std::unique_ptr<Error>
ReflectShaderBytecode(
    const ShaderBytecode& shaderBytecode,
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection>& shaderReflector,
    D3D11_SHADER_DESC& shaderDesc) noexcept
{
    if (auto hr = D3DReflect(
            shaderBytecode.Code,
            shaderBytecode.ByteLength,
            IID_PPV_ARGS(&shaderReflector));
        FAILED(hr)) {
        return Errors::New("D3DReflect() failed");
    }

    POMDOG_ASSERT(shaderReflector != nullptr);

    if (auto hr = shaderReflector->GetDesc(&shaderDesc); FAILED(hr)) {
        return Errors::New("failed to get shader description");
    }

    return nullptr;
}

[[nodiscard]] std::tuple<std::vector<D3D11_INPUT_ELEMENT_DESC>, std::unique_ptr<Error>>
BuildInputElements(
    const std::vector<D3D11_SIGNATURE_PARAMETER_DESC>& signatureParameters,
    const InputLayoutDescription& description) noexcept
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
            return std::make_tuple(std::move(inputElements), Errors::New("invalid input elements"));
        }

        POMDOG_ASSERT(sourceElement.InstanceStepRate == 0 ||
            sourceElement.InputSlotClass == InputClassification::InputPerInstance);

        D3D11_INPUT_ELEMENT_DESC elementDesc;
        elementDesc.SemanticName = signature->SemanticName;
        elementDesc.SemanticIndex = signature->SemanticIndex;
        elementDesc.Format = DXGI::ToDXGIFormat(sourceElement.Format);
        elementDesc.InputSlot = sourceElement.InputSlot;
        elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        elementDesc.InputSlotClass = ToD3D11InputClassification(sourceElement.InputSlotClass);
        elementDesc.InstanceDataStepRate = sourceElement.InstanceStepRate;

        POMDOG_ASSERT(elementDesc.InputSlot <= D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
        static_assert(std::is_unsigned_v<decltype(elementDesc.InstanceDataStepRate)>, "elementDesc.InstanceDataStepRate >= 0");

        inputElements.push_back(std::move(elementDesc));

        ++signature;
    }

    return std::make_tuple(std::move(inputElements), nullptr);
}

[[nodiscard]] std::vector<D3D11_SIGNATURE_PARAMETER_DESC>
EnumerateSignatureParameters(
    ID3D11ShaderReflection* shaderReflector,
    const D3D11_SHADER_DESC& shaderDesc) noexcept
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
        default:
            break;
        }
        signatureParameters.push_back(signatureDesc);
    }
    return std::move(signatureParameters);
}

[[nodiscard]] std::tuple<ComPtr<ID3D11InputLayout>, std::unique_ptr<Error>>
CreateInputLayout(
    ID3D11Device* device,
    const ShaderBytecode& vertexShaderBytecode,
    const InputLayoutDescription& description) noexcept
{
    POMDOG_ASSERT(device);
    POMDOG_ASSERT(vertexShaderBytecode.Code);

    D3D11_SHADER_DESC shaderDesc;
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> shaderReflector;

    if (auto err = ReflectShaderBytecode(
            vertexShaderBytecode,
            shaderReflector,
            shaderDesc);
        err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "ReflectShaderBytecode() failed"));
    }

    auto signatureParameters = EnumerateSignatureParameters(
        shaderReflector.Get(), shaderDesc);

    auto [inputElements, buildErr] = BuildInputElements(signatureParameters, description);
    if (buildErr != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(buildErr), "BuildInputElements() failed"));
    }

    Microsoft::WRL::ComPtr<ID3D11InputLayout> nativeInputLayout;
    HRESULT hr = device->CreateInputLayout(
        inputElements.data(),
        static_cast<UINT>(inputElements.size()),
        vertexShaderBytecode.Code,
        vertexShaderBytecode.ByteLength,
        &nativeInputLayout);

    if (FAILED(hr)) {
        return std::make_tuple(nullptr, Errors::New("failed to create ID3D11InputLayout"));
    }

    return std::make_tuple(std::move(nativeInputLayout), nullptr);
}

} // namespace

std::unique_ptr<Error>
PipelineStateDirect3D11::Initialize(
    ID3D11Device* device,
    const PipelineStateDescription& description) noexcept
{
    POMDOG_ASSERT(device);

    sampleMask = description.MultiSampleMask;

    if (auto [result, err] = CreateBlendState(device, description.BlendState); err != nullptr) {
        return Errors::Wrap(std::move(err), "CreateBlendState() failed");
    }
    else {
        blendState = std::move(result);
    }

    if (auto [result, err] = CreateDepthStencilState(device, description.DepthStencilState); err != nullptr) {
        return Errors::Wrap(std::move(err), "CreateDepthStencilState() failed");
    }
    else {
        depthStencilState = std::move(result);
    }

    if (auto [result, err] = CreateRasterizerState(device, description.RasterizerState); err != nullptr) {
        return Errors::Wrap(std::move(err), "CreateRasterizerState() failed");
    }
    else {
        rasterizerState = std::move(result);
    }

    auto vertexShaderD3D = std::dynamic_pointer_cast<VertexShaderDirect3D11>(description.VertexShader);
    if (vertexShaderD3D == nullptr) {
        return Errors::New("invalid vertex shader");
    }

    vertexShader = vertexShaderD3D->GetShader();
    if (vertexShader == nullptr) {
        return Errors::New("vertexShader must be != nullptr");
    }

    auto pixelShaderD3D = std::dynamic_pointer_cast<PixelShaderDirect3D11>(description.PixelShader);
    if (pixelShaderD3D == nullptr) {
        return Errors::New("invalid pixel shader");
    }

    pixelShader = pixelShaderD3D->GetShader();
    if (pixelShader == nullptr) {
        return Errors::New("pixelShader must be != nullptr");
    }

    if (auto [result, err] = CreateInputLayout(
            device,
            vertexShaderD3D->GetShaderBytecode(),
            description.InputLayout);
        err != nullptr) {
        return Errors::Wrap(std::move(err), "CreateBlendState() failed");
    }
    else {
        inputLayout = std::move(result);
    }

    primitiveTopology = ToPrimitiveTopology(description.PrimitiveTopology);

    return nullptr;
}

void PipelineStateDirect3D11::Apply(
    ID3D11DeviceContext* deviceContext,
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
    deviceContext->IASetPrimitiveTopology(primitiveTopology);
    deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
    deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
    deviceContext->OMSetBlendState(blendState.Get(), blendFactor, sampleMask);
    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
    deviceContext->RSSetState(rasterizerState.Get());
}

} // namespace Pomdog::Detail::Direct3D11
