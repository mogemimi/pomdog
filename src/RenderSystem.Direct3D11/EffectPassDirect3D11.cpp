// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "EffectPassDirect3D11.hpp"
#include "ConstantLayoutDirect3D11.hpp"
#include "GraphicsDeviceDirect3D11.hpp"
#include "InputLayoutDirect3D11.hpp"
#include "ShaderDirect3D11.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "Pomdog/Graphics/EffectPassDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <algorithm>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {
namespace {

using Microsoft::WRL::ComPtr;

static D3D11_BLEND_OP ToBlendFunctionDirect3D11(BlendFunction blendOperation) noexcept
{
    switch (blendOperation) {
    case BlendFunction::Add: return D3D11_BLEND_OP_ADD;
    case BlendFunction::Subtract: return D3D11_BLEND_OP_SUBTRACT;
    case BlendFunction::ReverseSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
    case BlendFunction::Min: return D3D11_BLEND_OP_MIN;
    case BlendFunction::Max: return D3D11_BLEND_OP_MAX;
    };
    return D3D11_BLEND_OP_ADD;
}
//-----------------------------------------------------------------------
static D3D11_BLEND ToBlendDirect3D11(Blend blend) noexcept
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
    };
    return D3D11_BLEND_ONE;
}
//-----------------------------------------------------------------------
static D3D11_STENCIL_OP ToStencilOperationDirect3D11(StencilOperation operation) noexcept
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
//-----------------------------------------------------------------------
static D3D11_COMPARISON_FUNC ToComparisonFunctionDirect3D11(ComparisonFunction compareFunction) noexcept
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
//-----------------------------------------------------------------------
static D3D11_CULL_MODE ToCullModeDirect3D11(CullMode cullMode) noexcept
{
    switch (cullMode) {
    case CullMode::ClockwiseFace: return D3D11_CULL_FRONT;
    case CullMode::CounterClockwiseFace: return D3D11_CULL_BACK;
    case CullMode::None: return D3D11_CULL_NONE;
    };
    return D3D11_CULL_BACK;
}
//-----------------------------------------------------------------------
static D3D11_FILL_MODE ToFillModeDirect3D11(FillMode fillMode) noexcept
{
    switch (fillMode) {
    case FillMode::WireFrame: return D3D11_FILL_WIREFRAME;
    case FillMode::Solid: return D3D11_FILL_SOLID;
    };
    return D3D11_FILL_SOLID;
}
//-----------------------------------------------------------------------
ComPtr<ID3D11BlendState> CreateBlendState(ID3D11Device* nativeDevice,
    BlendDescription const& description)
{
    D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;
    ZeroMemory(&renderTargetBlendDesc, sizeof(renderTargetBlendDesc));

    ///@todo badcode
    renderTargetBlendDesc.BlendEnable =
        (Blend::One == description.ColorSourceBlend) &&
        (Blend::One == description.AlphaSourceBlend) &&
        (Blend::Zero == description.ColorDestinationBlend) &&
        (Blend::Zero == description.AlphaDestinationBlend) ? FALSE : TRUE;

    renderTargetBlendDesc.BlendOp = ToBlendFunctionDirect3D11(description.ColorBlendFunction);
    renderTargetBlendDesc.BlendOpAlpha = ToBlendFunctionDirect3D11(description.AlphaBlendFunction);
    renderTargetBlendDesc.DestBlend = ToBlendDirect3D11(description.ColorDestinationBlend);
    renderTargetBlendDesc.DestBlendAlpha = ToBlendDirect3D11(description.AlphaDestinationBlend);
    renderTargetBlendDesc.SrcBlend = ToBlendDirect3D11(description.ColorSourceBlend);
    renderTargetBlendDesc.SrcBlendAlpha = ToBlendDirect3D11(description.AlphaSourceBlend);

    renderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    D3D11_BLEND_DESC blendDesc;
    ::ZeroMemory(&blendDesc, sizeof(blendDesc));
    blendDesc.AlphaToCoverageEnable = (description.AlphaToCoverageEnable ? TRUE : FALSE);
    blendDesc.IndependentBlendEnable = FALSE;//TRUE;
    blendDesc.RenderTarget[0] = renderTargetBlendDesc;

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
//-----------------------------------------------------------------------
ComPtr<ID3D11DepthStencilState> CreateDepthStencilState(ID3D11Device* nativeDevice,
    DepthStencilDescription const& description)
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    depthStencilDesc.DepthEnable = (description.DepthBufferEnable ? TRUE : FALSE);
    depthStencilDesc.DepthWriteMask = (description.DepthBufferWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO);
    depthStencilDesc.DepthFunc = ToComparisonFunctionDirect3D11(description.DepthBufferFunction);

    depthStencilDesc.StencilEnable = (description.StencilEnable ? TRUE : FALSE);
    depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    depthStencilDesc.BackFace.StencilDepthFailOp = ToStencilOperationDirect3D11(description.CounterClockwiseFace.StencilDepthBufferFail);
    depthStencilDesc.BackFace.StencilFailOp = ToStencilOperationDirect3D11(description.CounterClockwiseFace.StencilFail);
    depthStencilDesc.BackFace.StencilPassOp = ToStencilOperationDirect3D11(description.CounterClockwiseFace.StencilPass);
    depthStencilDesc.BackFace.StencilFunc = ToComparisonFunctionDirect3D11(description.CounterClockwiseFace.StencilFunction);

    depthStencilDesc.FrontFace.StencilDepthFailOp = ToStencilOperationDirect3D11(description.ClockwiseFace.StencilDepthBufferFail);
    depthStencilDesc.FrontFace.StencilFailOp = ToStencilOperationDirect3D11(description.ClockwiseFace.StencilFail);
    depthStencilDesc.FrontFace.StencilPassOp = ToStencilOperationDirect3D11(description.ClockwiseFace.StencilPass);
    depthStencilDesc.FrontFace.StencilFunc = ToComparisonFunctionDirect3D11(description.ClockwiseFace.StencilFunction);

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
//-----------------------------------------------------------------------
ComPtr<ID3D11RasterizerState> CreateRasterizerState(ID3D11Device* nativeDevice,
    RasterizerDescription const& description)
{
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

    rasterizerDesc.CullMode = ToCullModeDirect3D11(description.CullMode);
    rasterizerDesc.FillMode = ToFillModeDirect3D11(description.FillMode);
    rasterizerDesc.FrontCounterClockwise = FALSE;

    rasterizerDesc.DepthBias = static_cast<INT>(description.DepthBias);
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias = static_cast<FLOAT>(description.SlopeScaledDepthBias);
    rasterizerDesc.AntialiasedLineEnable = FALSE;
    rasterizerDesc.MultisampleEnable = (description.MultisampleEnable ? TRUE : FALSE);
    rasterizerDesc.ScissorEnable = (description.ScissorTestEnable ? TRUE : FALSE);

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
//-----------------------------------------------------------------------
static void ReflectShaderBytecode(ShaderBytecode const& shaderBytecode,
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> & shaderReflector, D3D11_SHADER_DESC & shaderDesc)
{
    HRESULT hr = D3DReflect(shaderBytecode.Code, shaderBytecode.ByteLength,
        IID_ID3D11ShaderReflection, reinterpret_cast<void**>(shaderReflector.GetAddressOf()));

    if (FAILED(hr)) {
        // FUS RO DAH!!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to call D3DReflect");
    }

    POMDOG_ASSERT(shaderReflector);
    hr = shaderReflector->GetDesc(&shaderDesc);

    if (FAILED(hr)) {
        // FUS RO DAH!!
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Failed to call ID3D11ShaderReflection::GetDesc");
    }
}
//-----------------------------------------------------------------------
static void EnumerateConstantBuffers(
    ShaderBytecode const& shaderBytecode,
    std::vector<ConstantBufferBindingDirect3D11> & output)
{
    POMDOG_ASSERT(shaderBytecode.Code);

    D3D11_SHADER_DESC shaderDesc;
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> shaderReflector;

    ReflectShaderBytecode(shaderBytecode, shaderReflector, shaderDesc);

    for (std::size_t i = 0; i < shaderDesc.ConstantBuffers; ++i)
    {
        POMDOG_ASSERT(shaderReflector);
        auto constantBufferReflector = shaderReflector->GetConstantBufferByIndex(i);

        D3D11_SHADER_BUFFER_DESC bufferDesc;
        HRESULT hr = constantBufferReflector->GetDesc(&bufferDesc);

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

        D3D11_SHADER_INPUT_BIND_DESC shaderInputBindDesc;
        shaderReflector->GetResourceBindingDescByName(bufferDesc.Name, &shaderInputBindDesc);

        ConstantBufferBindingDirect3D11 binding;
        binding.Name = bufferDesc.Name;
        binding.StartSlot = static_cast<std::uint16_t>(shaderInputBindDesc.BindPoint);

        POMDOG_ASSERT(shaderInputBindDesc.BindPoint < std::numeric_limits<decltype(binding.StartSlot)>::max());
        POMDOG_ASSERT(shaderInputBindDesc.BindPoint < D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT);

        output.push_back(std::move(binding));
    }
}
//-----------------------------------------------------------------------
static std::vector<ConstantBufferBindingDirect3D11> CreateConstantBufferBindings(
    ShaderBytecode const& vertexShaderBytecode,
    ShaderBytecode const& pixelShaderBytecode)
{
    using Desc = ConstantBufferBindingDirect3D11;

    std::vector<Desc> bindings;
    EnumerateConstantBuffers(vertexShaderBytecode, bindings);
    EnumerateConstantBuffers(pixelShaderBytecode, bindings);

    std::sort(std::begin(bindings), std::end(bindings),
        [](Desc const& a, Desc const& b) { return a.Name < b.Name; });

    bindings.erase(std::unique(std::begin(bindings), std::end(bindings),
        [](Desc const& a, Desc const& b) { return a.Name == b.Name; }), std::end(bindings));

    bindings.shrink_to_fit();

    return std::move(bindings);
}

}// unnamed namespace
//-----------------------------------------------------------------------
EffectPassDirect3D11::EffectPassDirect3D11(ID3D11Device* device,
    EffectPassDescription const& description)
{
    POMDOG_ASSERT(device);

    sampleMask = static_cast<UINT>(description.BlendState.MultiSampleMask);

    blendState = CreateBlendState(device, description.BlendState);
    depthStencilState = CreateDepthStencilState(device, description.DepthStencilState);
    rasterizerState = CreateRasterizerState(device, description.RasterizerState);

    vertexShader = std::dynamic_pointer_cast<Direct3D11::VertexShaderDirect3D11>(description.VertexShader);
    if (!vertexShader) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid vertex shader.");
    }

    pixelShader = std::dynamic_pointer_cast<Direct3D11::PixelShaderDirect3D11>(description.PixelShader);
    if (!pixelShader) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid pixel shader.");
    }

    inputLayout = InputLayoutHelper::CreateInputLayout(
        device, vertexShader->GetShaderBytecode(), description.InputLayout);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeConstantLayout> EffectPassDirect3D11::CreateConstantLayout()
{
    POMDOG_ASSERT(vertexShader);
    POMDOG_ASSERT(pixelShader);

    auto bindings = CreateConstantBufferBindings(
        vertexShader->GetShaderBytecode(),
        pixelShader->GetShaderBytecode());

    auto constantLayout = std::make_unique<ConstantLayoutDirect3D11>(std::move(bindings));
    return std::move(constantLayout);
}
//-----------------------------------------------------------------------
void EffectPassDirect3D11::Apply(ID3D11DeviceContext * deviceContext,
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
    deviceContext->VSSetShader(vertexShader->GetNativeShader(), nullptr, 0);
    deviceContext->PSSetShader(pixelShader->GetNativeShader(), nullptr, 0);
    deviceContext->OMSetBlendState(blendState.Get(), blendFactor, sampleMask);
    deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
    deviceContext->RSSetState(rasterizerState.Get());
}
//-----------------------------------------------------------------------
ShaderBytecode EffectPassDirect3D11::GetVertexShaderBlob() const
{
    POMDOG_ASSERT(vertexShader);
    return vertexShader->GetShaderBytecode();
}
//-----------------------------------------------------------------------
ShaderBytecode EffectPassDirect3D11::GetPixelShaderBlob() const
{
    POMDOG_ASSERT(pixelShader);
    return pixelShader->GetShaderBytecode();
}
//-----------------------------------------------------------------------
}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog
