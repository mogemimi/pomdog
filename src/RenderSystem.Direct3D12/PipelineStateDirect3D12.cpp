// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "PipelineStateDirect3D12.hpp"
#include "ConstantLayoutDirect3D12.hpp"
#include "ShaderDirect3D12.hpp"
#include "../RenderSystem.Direct3D/PrerequisitesDirect3D.hpp"
#include "../RenderSystem.DXGI/DXGIFormatHelper.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "Pomdog/Graphics/PipelineStateDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include <d3d12shader.h>
#include <array>
#include <algorithm>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace Direct3D12 {
namespace {

using Microsoft::WRL::ComPtr;
using DXGI::DXGIFormatHelper;

static inline BOOL ToD3D12Boolean(bool is) noexcept
{
    return is ? TRUE : FALSE;
}
//-----------------------------------------------------------------------
static D3D12_BLEND_OP ToBlendFunction(BlendFunction blendOperation) noexcept
{
    switch (blendOperation) {
    case BlendFunction::Add: return D3D12_BLEND_OP_ADD;
    case BlendFunction::Subtract: return D3D12_BLEND_OP_SUBTRACT;
    case BlendFunction::ReverseSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
    case BlendFunction::Min: return D3D12_BLEND_OP_MIN;
    case BlendFunction::Max: return D3D12_BLEND_OP_MAX;
    }
    return D3D12_BLEND_OP_ADD;
}
//-----------------------------------------------------------------------
static D3D12_BLEND ToBlend(Blend blend) noexcept
{
    switch (blend) {
    case Blend::Zero: return D3D12_BLEND_ZERO;
    case Blend::One: return D3D12_BLEND_ONE;
    case Blend::SourceColor: return D3D12_BLEND_SRC_COLOR;
    case Blend::InverseSourceColor: return D3D12_BLEND_INV_SRC_COLOR;
    case Blend::SourceAlpha: return D3D12_BLEND_SRC_ALPHA;
    case Blend::InverseSourceAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
    case Blend::DestinationAlpha: return D3D12_BLEND_DEST_ALPHA;
    case Blend::InverseDestinationAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
    case Blend::DestinationColor: return D3D12_BLEND_DEST_COLOR;
    case Blend::InverseDestinationColor: return D3D12_BLEND_INV_DEST_COLOR;
    case Blend::SourceAlphaSaturation: return D3D12_BLEND_SRC_ALPHA_SAT;
    case Blend::BlendFactor: return D3D12_BLEND_BLEND_FACTOR;
    case Blend::InvereseBlendFactor: return D3D12_BLEND_INV_BLEND_FACTOR;
    case Blend::Source1Color: return D3D12_BLEND_SRC1_COLOR;
    case Blend::InverseSource1Color: return D3D12_BLEND_INV_SRC1_COLOR;
    case Blend::Source1Alpha: return D3D12_BLEND_SRC1_ALPHA;
    case Blend::InverseSource1Alpha: return D3D12_BLEND_INV_SRC1_ALPHA;
    }
    return D3D12_BLEND_ONE;
}
//-----------------------------------------------------------------------
static void ToD3D12Desc(
    RenderTargetBlendDescription const& desc,
    D3D12_RENDER_TARGET_BLEND_DESC & result) noexcept
{
    result.BlendEnable = ToD3D12Boolean(desc.BlendEnable);
    result.BlendOp = ToBlendFunction(desc.ColorBlendFunction);
    result.BlendOpAlpha = ToBlendFunction(desc.AlphaBlendFunction);
    result.DestBlend = ToBlend(desc.ColorDestinationBlend);
    result.DestBlendAlpha = ToBlend(desc.AlphaDestinationBlend);
    result.SrcBlend = ToBlend(desc.ColorSourceBlend);
    result.SrcBlendAlpha = ToBlend(desc.AlphaSourceBlend);
    result.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    result.LogicOp = D3D12_LOGIC_OP_CLEAR;
    result.LogicOpEnable = FALSE;
}
//-----------------------------------------------------------------------
static D3D12_CULL_MODE ToCullMode(CullMode cullMode) noexcept
{
    switch (cullMode) {
    case CullMode::ClockwiseFace: return D3D12_CULL_FRONT;
    case CullMode::CounterClockwiseFace: return D3D12_CULL_BACK;
    case CullMode::None: return D3D12_CULL_NONE;
    }
    return D3D12_CULL_BACK;
}
//-----------------------------------------------------------------------
static D3D12_FILL_MODE ToFillMode(FillMode fillMode) noexcept
{
    switch (fillMode) {
    case FillMode::WireFrame: return D3D12_FILL_WIREFRAME;
    case FillMode::Solid: return D3D12_FILL_SOLID;
    }
    return D3D12_FILL_SOLID;
}
//-----------------------------------------------------------------------
static D3D12_STENCIL_OP ToStencilOperation(StencilOperation operation) noexcept
{
    switch (operation) {
    case StencilOperation::Keep: return D3D12_STENCIL_OP_KEEP;
    case StencilOperation::Zero: return D3D12_STENCIL_OP_ZERO;
    case StencilOperation::Replace: return D3D12_STENCIL_OP_REPLACE;
    case StencilOperation::IncrementSaturation: return D3D12_STENCIL_OP_INCR_SAT;
    case StencilOperation::DecrementSaturation: return D3D12_STENCIL_OP_DECR_SAT;
    case StencilOperation::Invert: return D3D12_STENCIL_OP_INVERT;
    case StencilOperation::Increment: return D3D12_STENCIL_OP_INCR;
    case StencilOperation::Decrement: return D3D12_STENCIL_OP_DECR;
    }
    return D3D12_STENCIL_OP_KEEP;
}
//-----------------------------------------------------------------------
static D3D12_COMPARISON_FUNC ToComparisonFunction(
    ComparisonFunction compareFunction) noexcept
{
    switch (compareFunction) {
    case ComparisonFunction::Never: return D3D12_COMPARISON_NEVER;
    case ComparisonFunction::Less: return D3D12_COMPARISON_LESS;
    case ComparisonFunction::Equal: return D3D12_COMPARISON_EQUAL;
    case ComparisonFunction::LessEqual: return D3D12_COMPARISON_LESS_EQUAL;
    case ComparisonFunction::Greater: return D3D12_COMPARISON_GREATER;
    case ComparisonFunction::NotEqual: return D3D12_COMPARISON_NOT_EQUAL;
    case ComparisonFunction::GreaterEqual: return D3D12_COMPARISON_GREATER_EQUAL;
    case ComparisonFunction::Always: return D3D12_COMPARISON_ALWAYS;
    }
    return D3D12_COMPARISON_LESS_EQUAL;
}
//-----------------------------------------------------------------------
static D3D12_INPUT_CLASSIFICATION ToInputClassification(
    InputClassification slotClass) noexcept
{
    switch (slotClass) {
    case InputClassification::InputPerVertex: return D3D12_INPUT_PER_VERTEX_DATA;
    case InputClassification::InputPerInstance: return D3D12_INPUT_PER_INSTANCE_DATA;
    }
    return D3D12_INPUT_PER_VERTEX_DATA;
}
//-----------------------------------------------------------------------
static DXGI_FORMAT ToDSVFormat(DepthFormat format) noexcept
{
    if (format == DepthFormat::None) {
        return DXGI_FORMAT_UNKNOWN;
    }
    return DXGIFormatHelper::ToDXGIFormat(format);
}
//-----------------------------------------------------------------------
static D3D12_PRIMITIVE_TOPOLOGY_TYPE ToPrimitiveTopologyType(
    PrimitiveTopologyType primitiveTopology) noexcept
{
    switch (primitiveTopology) {
    case PrimitiveTopologyType::Triangle: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case PrimitiveTopologyType::Line: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    }
    return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
}
//-----------------------------------------------------------------------
static void ToBlendState(
    BlendDescription const& desc,
    D3D12_BLEND_DESC & output) noexcept
{
    output.AlphaToCoverageEnable = ToD3D12Boolean(desc.AlphaToCoverageEnable);
    output.IndependentBlendEnable = ToD3D12Boolean(desc.IndependentBlendEnable);

    const auto renderTargetCount = std::min<int>(
        desc.RenderTargets.size(),
        D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT);

    for (int i = 0; i < renderTargetCount; ++i) {
        ToD3D12Desc(desc.RenderTargets[i], output.RenderTarget[i]);
    }
}
//-----------------------------------------------------------------------
static void ToRasterizerState(
    RasterizerDescription const& desc,
    D3D12_RASTERIZER_DESC & output)
{
    output.CullMode = ToCullMode(desc.CullMode);
    output.FillMode = ToFillMode(desc.FillMode);
    output.FrontCounterClockwise = FALSE;

    output.DepthBias = static_cast<INT>(desc.DepthBias);
    output.DepthBiasClamp = 0.0f;
    output.DepthClipEnable = TRUE;
    output.SlopeScaledDepthBias = desc.SlopeScaledDepthBias;

    output.AntialiasedLineEnable = FALSE;
    output.MultisampleEnable = ToD3D12Boolean(desc.MultisampleEnable);
    output.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
    output.ForcedSampleCount = 0;

    ///@todo FIXME
    /// I cannot find ScissorEnable as a member of D3D12_RASTERIZER_DESC
    /// in DirectX 12 (preliminary version).
    //output.ScissorEnable = ToD3D12Boolean(desc.ScissorTestEnable);

    POMDOG_ASSERT(!output.AntialiasedLineEnable
        || (output.AntialiasedLineEnable && !output.MultisampleEnable));
}
//-----------------------------------------------------------------------
static void ToDepthStencilState(
    DepthStencilDescription const& desc,
    D3D12_DEPTH_STENCIL_DESC & output)
{
    output.DepthEnable = ToD3D12Boolean(desc.DepthBufferEnable);
    output.DepthFunc = ToComparisonFunction(desc.DepthBufferFunction);
    output.DepthWriteMask = (desc.DepthBufferWriteEnable
        ? D3D12_DEPTH_WRITE_MASK_ALL
        : D3D12_DEPTH_WRITE_MASK_ZERO);

    output.StencilEnable = ToD3D12Boolean(desc.StencilEnable);
    output.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    output.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

    output.BackFace.StencilDepthFailOp = ToStencilOperation(desc.CounterClockwiseFace.StencilDepthBufferFail);
    output.BackFace.StencilFailOp = ToStencilOperation(desc.CounterClockwiseFace.StencilFail);
    output.BackFace.StencilPassOp = ToStencilOperation(desc.CounterClockwiseFace.StencilPass);
    output.BackFace.StencilFunc = ToComparisonFunction(desc.CounterClockwiseFace.StencilFunction);

    output.FrontFace.StencilDepthFailOp = ToStencilOperation(desc.ClockwiseFace.StencilDepthBufferFail);
    output.FrontFace.StencilFailOp = ToStencilOperation(desc.ClockwiseFace.StencilFail);
    output.FrontFace.StencilPassOp = ToStencilOperation(desc.ClockwiseFace.StencilPass);
    output.FrontFace.StencilFunc = ToComparisonFunction(desc.ClockwiseFace.StencilFunction);
}
//-----------------------------------------------------------------------
static std::vector<D3D12_INPUT_ELEMENT_DESC> ToInputElements(
    InputLayoutDescription const& desc,
    std::vector<D3D12_SIGNATURE_PARAMETER_DESC> const& signatureParameters)
{
    POMDOG_ASSERT(!desc.InputElements.empty());

    std::vector<D3D12_INPUT_ELEMENT_DESC> elements;
    elements.reserve(desc.InputElements.size());

    for (auto & source: desc.InputElements)
    {
        D3D12_INPUT_ELEMENT_DESC element;
        element.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        //element.AlignedByteOffset = source.ByteOffset;
        element.Format = DXGIFormatHelper::ToDXGIFormat(source.Format);
        element.InputSlot = source.InputSlot;
        element.InputSlotClass = ToInputClassification(source.InputSlotClass);
        element.InstanceDataStepRate = source.InstanceStepRate;
        element.SemanticIndex = 0;
        element.SemanticName = nullptr;

        POMDOG_ASSERT(source.InstanceStepRate == 0
            || source.InputSlotClass == InputClassification::InputPerInstance);
        POMDOG_ASSERT(element.InputSlot <= D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
        POMDOG_ASSERT(element.InstanceDataStepRate >= 0);

        elements.push_back(std::move(element));
    }

    POMDOG_ASSERT(!signatureParameters.empty());
    POMDOG_ASSERT(signatureParameters.size() == desc.InputElements.size());
    auto signature = std::begin(signatureParameters);

    for (auto & element : elements)
    {
        POMDOG_ASSERT(signature != std::end(signatureParameters));
        if (signature == std::end(signatureParameters)) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Invalid signature parameters");
        }

        element.SemanticName = signature->SemanticName;
        element.SemanticIndex = signature->SemanticIndex;
        ++signature;
    }

    return std::move(elements);
}
//-----------------------------------------------------------------------
static void ToRTVFormats(
    std::vector<SurfaceFormat> const& renderTargetViewFormats,
    D3D12_GRAPHICS_PIPELINE_STATE_DESC & pipelineStateDesc)
{
    constexpr auto MaxRenderTargetCount = sizeof(pipelineStateDesc.RTVFormats);
    POMDOG_ASSERT(!renderTargetViewFormats.empty());
    POMDOG_ASSERT(renderTargetViewFormats.size() <= MaxRenderTargetCount);

    pipelineStateDesc.NumRenderTargets
        = std::min(renderTargetViewFormats.size(), MaxRenderTargetCount);

    for (UINT i = 0; i < pipelineStateDesc.NumRenderTargets; ++i) {
        pipelineStateDesc.RTVFormats[i]
            = DXGIFormatHelper::ToDXGIFormat(renderTargetViewFormats[i]);
    }
}
//-----------------------------------------------------------------------
static D3D12_SHADER_BYTECODE ToShaderBytecode(ShaderDirect3D12 const& shader)
{
    POMDOG_ASSERT(!shader.CodeBlob.empty());

    D3D12_SHADER_BYTECODE shaderBytecode;
    shaderBytecode.pShaderBytecode = shader.CodeBlob.data();
    shaderBytecode.BytecodeLength = shader.CodeBlob.size();
    return std::move(shaderBytecode);
}
//-----------------------------------------------------------------------
static void ReflectShaderBytecode(
    D3D12_SHADER_BYTECODE const& shaderBytecode,
    Microsoft::WRL::ComPtr<ID3D12ShaderReflection> & shaderReflector,
    D3D12_SHADER_DESC & shaderDesc)
{
    HRESULT hr = D3DReflect(
        shaderBytecode.pShaderBytecode,
        shaderBytecode.BytecodeLength,
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
//-----------------------------------------------------------------------
static std::vector<D3D12_SIGNATURE_PARAMETER_DESC>
EnumerateSignatureParameters(D3D12_SHADER_BYTECODE const& shaderBytecode)
{
    ComPtr<ID3D12ShaderReflection> shaderReflector;
    D3D12_SHADER_DESC shaderDesc;

    ReflectShaderBytecode(shaderBytecode, shaderReflector, shaderDesc);

    std::vector<D3D12_SIGNATURE_PARAMETER_DESC> signatureParameters;
    signatureParameters.reserve(shaderDesc.InputParameters);

    for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
    {
        D3D12_SIGNATURE_PARAMETER_DESC signatureDesc;
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
static void EnumerateConstantBuffers(
    D3D12_SHADER_BYTECODE const& shaderBytecode,
    std::vector<ConstantBufferBindDesc> & constantBufferBinds,
    std::vector<D3D12_SHADER_INPUT_BIND_DESC> & textureBinds,
    std::vector<D3D12_SHADER_INPUT_BIND_DESC> & samplerBinds)
{
    POMDOG_ASSERT(shaderBytecode.pShaderBytecode != nullptr);

    ComPtr<ID3D12ShaderReflection> shaderReflector;
    D3D12_SHADER_DESC shaderDesc;

    ReflectShaderBytecode(shaderBytecode, shaderReflector, shaderDesc);

    for (UINT i = 0; i < shaderDesc.BoundResources; ++i) {
        D3D12_SHADER_INPUT_BIND_DESC shaderInputBindDesc;
        HRESULT hr = shaderReflector->GetResourceBindingDesc(i, &shaderInputBindDesc);
        if (FAILED(hr)) {
            // FUS RO DAH!
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                "Failed to get D3D12_SHADER_INPUT_BIND_DESC.");
        }

        if (shaderInputBindDesc.Type == D3D_SIT_CBUFFER) {
            ConstantBufferBindDesc desc;
            desc.Name = shaderInputBindDesc.Name;
            desc.BindPoint = shaderInputBindDesc.BindPoint;

            POMDOG_ASSERT(shaderInputBindDesc.BindPoint
                <= D3D12_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT);
            constantBufferBinds.push_back(std::move(desc));
        }

        if (shaderInputBindDesc.Type == D3D_SIT_TEXTURE) {
            // texture (SRV)
            textureBinds.push_back(shaderInputBindDesc);
        }

        if (shaderInputBindDesc.Type == D3D_SIT_SAMPLER) {
            // sampler
            samplerBinds.push_back(shaderInputBindDesc);
        }
    }
}
//-----------------------------------------------------------------------
static std::vector<ConstantBufferBindDesc> CreateConstantBufferBindDescs(
    D3D12_SHADER_BYTECODE const& vertexShaderBytecode,
    D3D12_SHADER_BYTECODE const& pixelShaderBytecode,
    std::vector<D3D12_SHADER_INPUT_BIND_DESC> & textureBinds,
    std::vector<D3D12_SHADER_INPUT_BIND_DESC> & samplerBinds)
{
    using Desc = ConstantBufferBindDesc;

    std::vector<Desc> bindings;
    EnumerateConstantBuffers(vertexShaderBytecode, bindings, textureBinds, samplerBinds);
    EnumerateConstantBuffers(pixelShaderBytecode, bindings, textureBinds, samplerBinds);

    std::sort(std::begin(bindings), std::end(bindings),
        [](Desc const& a, Desc const& b) { return a.Name < b.Name; });

    bindings.erase(std::unique(std::begin(bindings), std::end(bindings),
        [](Desc const& a, Desc const& b) { return a.Name == b.Name; }), std::end(bindings));

    std::sort(std::begin(bindings), std::end(bindings),
        [](Desc const& a, Desc const& b) { return a.BindPoint < b.BindPoint; });

    bindings.shrink_to_fit();

    auto compare = [](D3D12_SHADER_INPUT_BIND_DESC const& a, D3D12_SHADER_INPUT_BIND_DESC const& b) { return a.BindPoint < b.BindPoint; };
    auto equal = [](D3D12_SHADER_INPUT_BIND_DESC const& a, D3D12_SHADER_INPUT_BIND_DESC const& b) { return a.BindPoint == b.BindPoint; };
    std::sort(std::begin(textureBinds), std::end(textureBinds), compare);
    std::sort(std::begin(samplerBinds), std::end(samplerBinds), compare);
    textureBinds.erase(std::unique(std::begin(textureBinds), std::end(textureBinds), equal), std::end(textureBinds));
    samplerBinds.erase(std::unique(std::begin(samplerBinds), std::end(samplerBinds), equal), std::end(samplerBinds));

    return std::move(bindings);
}

} // unnamed namespace
//-----------------------------------------------------------------------
PipelineStateDirect3D12::PipelineStateDirect3D12(ID3D12Device* device,
    PipelineStateDescription const& description)
    : constantBufferCount(0)
    , shaderResourceCount(0)
    , samplerCount(0)
{
    POMDOG_ASSERT(description.VertexShader);
    auto vertexShader = dynamic_cast<ShaderDirect3D12*>(description.VertexShader.get());

    if (vertexShader == nullptr) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "vertexShader is nullptr");
    }

    POMDOG_ASSERT(description.PixelShader);
    auto pixelShader = dynamic_cast<ShaderDirect3D12*>(description.PixelShader.get());

    if (pixelShader == nullptr) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "pixelShader is nullptr");
    }

    auto vertexShaderBytecode = ToShaderBytecode(*vertexShader);
    auto pixelShaderBytecode = ToShaderBytecode(*pixelShader);

    // Use shader reflection
    std::vector<D3D12_SHADER_INPUT_BIND_DESC> textureBinds;
    std::vector<D3D12_SHADER_INPUT_BIND_DESC> samplerBinds;
    constantBufferBinds = CreateConstantBufferBindDescs(
        vertexShaderBytecode,
        pixelShaderBytecode,
        textureBinds,
        samplerBinds);

    // Create root parameters
    std::vector<D3D12_DESCRIPTOR_RANGE> srvDesciptorRanges;
    std::vector<D3D12_DESCRIPTOR_RANGE> samplerDesciptorRanges;
    std::vector<D3D12_ROOT_PARAMETER> rootParameters;
    {
        constantBufferCount = constantBufferBinds.size();
        shaderResourceCount = textureBinds.size();
        samplerCount = samplerBinds.size();

        rootParameters.resize(constantBufferCount + shaderResourceCount + samplerCount);
        auto rootParameter = rootParameters.begin();

        for (UINT i = 0; i < constantBufferCount; ++i) {
            POMDOG_ASSERT(i < constantBufferBinds.size());
            UINT shaderRegisterIndex = constantBufferBinds[i].BindPoint;
            POMDOG_ASSERT(rootParameter != std::end(rootParameters));
            rootParameter->InitAsConstantBufferView(shaderRegisterIndex);
            ++rootParameter;
        }

        if (shaderResourceCount > 0) {
            srvDesciptorRanges.resize(shaderResourceCount);

            for (UINT i = 0; i < shaderResourceCount; ++i) {
                POMDOG_ASSERT(i < textureBinds.size());
                UINT shaderRegisterIndex = textureBinds[i].BindPoint;
                POMDOG_ASSERT(i < srvDesciptorRanges.size());
                auto & range = srvDesciptorRanges[i];
                range.Init(D3D12_DESCRIPTOR_RANGE_SRV, 1, shaderRegisterIndex);
            }

            POMDOG_ASSERT(rootParameter != std::end(rootParameters));
            rootParameter->InitAsDescriptorTable(srvDesciptorRanges.size(), srvDesciptorRanges.data());
            ++rootParameter;
        }

        if (samplerCount > 0) {
            samplerDesciptorRanges.resize(samplerCount);

            for (UINT i = 0; i < samplerCount; ++i) {
                POMDOG_ASSERT(i < samplerBinds.size());
                UINT shaderRegisterIndex = samplerBinds[i].BindPoint;
                POMDOG_ASSERT(i < samplerDesciptorRanges.size());
                auto & range = samplerDesciptorRanges[i];
                range.Init(D3D12_DESCRIPTOR_RANGE_SAMPLER, 1, shaderRegisterIndex);
            }

            POMDOG_ASSERT(rootParameter != std::end(rootParameters));
            rootParameter->InitAsDescriptorTable(samplerDesciptorRanges.size(), samplerDesciptorRanges.data());
            ++rootParameter;
        }
    }

    // Create RootSignature
    D3D12_ROOT_SIGNATURE rootSignatureDesc;
    rootSignatureDesc.pStaticSamplers = nullptr;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    if (!rootParameters.empty()) {
        rootSignatureDesc.pParameters = rootParameters.data();
        rootSignatureDesc.NumParameters = rootParameters.size();
    }
    else {
        rootSignatureDesc.pParameters = nullptr;
        rootSignatureDesc.NumParameters = 0;
    }

    ComPtr<ID3DBlob> rootSignatureBlob;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3D12SerializeRootSignature(
        &rootSignatureDesc,
        D3D_ROOT_SIGNATURE_V1,
        &rootSignatureBlob,
        &errorBlob);

    if (FAILED(hr)) {
        // FUS RO DAH!
        if (errorBlob) {
            POMDOG_THROW_EXCEPTION(std::runtime_error,
                reinterpret_cast<LPCSTR>(errorBlob->GetBufferPointer()));
        }
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to serialize root signature");
    }

    hr = device->CreateRootSignature(
        0,
        rootSignatureBlob->GetBufferPointer(),
        rootSignatureBlob->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create root signature");
    }

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc;
    ZeroMemory(&pipelineStateDesc, sizeof(pipelineStateDesc));

    ToBlendState(description.BlendState, pipelineStateDesc.BlendState);
    ToRasterizerState(description.RasterizerState, pipelineStateDesc.RasterizerState);
    ToDepthStencilState(description.DepthStencilState, pipelineStateDesc.DepthStencilState);
    ToRTVFormats(description.RenderTargetViewFormats, pipelineStateDesc);
    pipelineStateDesc.DSVFormat = ToDSVFormat(description.DepthStencilViewFormat);
    pipelineStateDesc.SampleMask = description.MultiSampleMask;
    pipelineStateDesc.PrimitiveTopologyType = ToPrimitiveTopologyType(description.PrimitiveTopologyType);

    auto signatureParameters = EnumerateSignatureParameters(vertexShaderBytecode);
    auto inputElements = ToInputElements(description.InputLayout, signatureParameters);

    pipelineStateDesc.InputLayout.pInputElementDescs = inputElements.data();
    pipelineStateDesc.InputLayout.NumElements = inputElements.size();

    pipelineStateDesc.VS = vertexShaderBytecode;
    pipelineStateDesc.PS = pixelShaderBytecode;

    pipelineStateDesc.pRootSignature = rootSignature.Get();

    ///@todo MSAA is not implemented
    pipelineStateDesc.SampleDesc.Count = 1;
    pipelineStateDesc.SampleDesc.Quality = 0;

    hr = device->CreateGraphicsPipelineState(
        &pipelineStateDesc, IID_PPV_ARGS(&pipelineState));

    if (FAILED(hr)) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create pipeline state");
    }
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeConstantLayout> PipelineStateDirect3D12::CreateConstantLayout()
{
    std::vector<ConstantBufferView> bindings;
    bindings.reserve(constantBufferBinds.size());

    for (auto & desc : constantBufferBinds) {
        ConstantBufferView binding;
        binding.Name = desc.Name;
        binding.SlotIndex = desc.BindPoint;
        bindings.push_back(std::move(binding));
    }
    return std::make_unique<ConstantLayoutDirect3D12>(std::move(bindings));
}
//-----------------------------------------------------------------------
ID3D12PipelineState* PipelineStateDirect3D12::GetPipelineState() const noexcept
{
    POMDOG_ASSERT(pipelineState);
    return pipelineState.Get();
}
//-----------------------------------------------------------------------
ID3D12RootSignature* PipelineStateDirect3D12::GetRootSignature() const noexcept
{
    POMDOG_ASSERT(rootSignature);
    return rootSignature.Get();
}
//-----------------------------------------------------------------------
std::size_t PipelineStateDirect3D12::GetConstantBufferCount() const noexcept
{
    return constantBufferCount;
}
//-----------------------------------------------------------------------
std::size_t PipelineStateDirect3D12::GetSahderResourceCount() const noexcept
{
    return shaderResourceCount;
}
//-----------------------------------------------------------------------
std::size_t PipelineStateDirect3D12::GetSamplerCount() const noexcept
{
    return samplerCount;
}
//-----------------------------------------------------------------------
} // namespace Direct3D12
} // namespace Detail
} // namespace Pomdog
