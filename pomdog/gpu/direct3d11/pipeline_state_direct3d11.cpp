// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/direct3d11/pipeline_state_direct3d11.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/backends/shader_bytecode.h"
#include "pomdog/gpu/direct3d/prerequisites_direct3d.h"
#include "pomdog/gpu/direct3d11/format_helper.h"
#include "pomdog/gpu/direct3d11/graphics_device_direct3d11.h"
#include "pomdog/gpu/direct3d11/shader_direct3d11.h"
#include "pomdog/gpu/dxgi/dxgi_format_helper.h"
#include "pomdog/gpu/input_layout_descriptor.h"
#include "pomdog/gpu/pipeline_descriptor.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {
namespace {

using Microsoft::WRL::ComPtr;

[[nodiscard]] D3D11_PRIMITIVE_TOPOLOGY
toPrimitiveTopology(PrimitiveTopology primitiveTopology) noexcept
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

[[nodiscard]] D3D11_BLEND_OP
toBlendOperation(BlendOperation blendOperation) noexcept
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

[[nodiscard]] D3D11_BLEND
toBlend(BlendFactor blend) noexcept
{
    switch (blend) {
    case BlendFactor::Zero:
        return D3D11_BLEND_ZERO;
    case BlendFactor::One:
        return D3D11_BLEND_ONE;
    case BlendFactor::SourceColor:
        return D3D11_BLEND_SRC_COLOR;
    case BlendFactor::InverseSourceColor:
        return D3D11_BLEND_INV_SRC_COLOR;
    case BlendFactor::SourceAlpha:
        return D3D11_BLEND_SRC_ALPHA;
    case BlendFactor::InverseSourceAlpha:
        return D3D11_BLEND_INV_SRC_ALPHA;
    case BlendFactor::DestinationAlpha:
        return D3D11_BLEND_DEST_ALPHA;
    case BlendFactor::InverseDestinationAlpha:
        return D3D11_BLEND_INV_DEST_ALPHA;
    case BlendFactor::DestinationColor:
        return D3D11_BLEND_DEST_COLOR;
    case BlendFactor::InverseDestinationColor:
        return D3D11_BLEND_INV_DEST_COLOR;
    case BlendFactor::SourceAlphaSaturation:
        return D3D11_BLEND_SRC_ALPHA_SAT;
    case BlendFactor::BlendFactor:
        return D3D11_BLEND_BLEND_FACTOR;
    case BlendFactor::InvereseBlendFactor:
        return D3D11_BLEND_INV_BLEND_FACTOR;
    case BlendFactor::Source1Color:
        return D3D11_BLEND_SRC1_COLOR;
    case BlendFactor::InverseSource1Color:
        return D3D11_BLEND_INV_SRC1_COLOR;
    case BlendFactor::Source1Alpha:
        return D3D11_BLEND_SRC1_ALPHA;
    case BlendFactor::InverseSource1Alpha:
        return D3D11_BLEND_INV_SRC1_ALPHA;
    }
    return D3D11_BLEND_ONE;
}

[[nodiscard]] D3D11_STENCIL_OP
toStencilOperation(StencilOperation operation) noexcept
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

[[nodiscard]] D3D11_CULL_MODE
toCullMode(CullMode cullMode) noexcept
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

[[nodiscard]] D3D11_FILL_MODE
toFillMode(FillMode fillMode) noexcept
{
    switch (fillMode) {
    case FillMode::Solid:
        return D3D11_FILL_SOLID;
    case FillMode::Wireframe:
        return D3D11_FILL_WIREFRAME;
    }
    return D3D11_FILL_SOLID;
}

[[nodiscard]] D3D11_INPUT_CLASSIFICATION
toD3D11InputClassification(InputClassification slotClass) noexcept
{
    switch (slotClass) {
    case InputClassification::InputPerVertex:
        return D3D11_INPUT_PER_VERTEX_DATA;
    case InputClassification::InputPerInstance:
        return D3D11_INPUT_PER_INSTANCE_DATA;
    }
    return D3D11_INPUT_PER_VERTEX_DATA;
}

[[nodiscard]] constexpr BOOL
toD3D11Boolean(bool is) noexcept
{
    return is ? TRUE : FALSE;
}

void toD3D11Desc(
    const RenderTargetBlendDescriptor& desc,
    D3D11_RENDER_TARGET_BLEND_DESC& result) noexcept
{
    result.BlendEnable = toD3D11Boolean(desc.blendEnable);
    result.BlendOp = toBlendOperation(desc.colorBlendOperation);
    result.BlendOpAlpha = toBlendOperation(desc.alphaBlendOperation);
    result.DestBlend = toBlend(desc.colorDestinationBlend);
    result.DestBlendAlpha = toBlend(desc.alphaDestinationBlend);
    result.SrcBlend = toBlend(desc.colorSourceBlend);
    result.SrcBlendAlpha = toBlend(desc.alphaSourceBlend);
    result.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
}

[[nodiscard]] std::tuple<ComPtr<ID3D11BlendState>, std::unique_ptr<Error>>
createBlendState(
    ID3D11Device* nativeDevice,
    const BlendDescriptor& descriptor) noexcept
{
    D3D11_BLEND_DESC blendDesc;
    ::ZeroMemory(&blendDesc, sizeof(blendDesc));
    blendDesc.AlphaToCoverageEnable = toD3D11Boolean(descriptor.alphaToCoverageEnable);
    blendDesc.IndependentBlendEnable = toD3D11Boolean(descriptor.independentBlendEnable);

    const auto renderTargetCount = std::min<int>(
        static_cast<int>(descriptor.renderTargets.size()),
        D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);

    for (int i = 0; i < renderTargetCount; ++i) {
        toD3D11Desc(descriptor.renderTargets[i], blendDesc.RenderTarget[i]);
    }

    POMDOG_ASSERT(nativeDevice != nullptr);

    ComPtr<ID3D11BlendState> blendState;
    HRESULT hr = nativeDevice->CreateBlendState(&blendDesc, &blendState);

    if (FAILED(hr)) {
        return std::make_tuple(nullptr, errors::make("failed to create ID3D11BlendState"));
    }

    return std::make_tuple(std::move(blendState), nullptr);
}

[[nodiscard]] std::tuple<ComPtr<ID3D11DepthStencilState>, std::unique_ptr<Error>>
createDepthStencilState(
    ID3D11Device* nativeDevice,
    const DepthStencilDescriptor& descriptor) noexcept
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    depthStencilDesc.DepthEnable = toD3D11Boolean(descriptor.depthBufferEnable);
    depthStencilDesc.DepthFunc = toComparisonFunction(descriptor.depthBufferFunction);
    depthStencilDesc.DepthWriteMask = (descriptor.depthBufferWriteEnable
                                           ? D3D11_DEPTH_WRITE_MASK_ALL
                                           : D3D11_DEPTH_WRITE_MASK_ZERO);

    depthStencilDesc.StencilEnable = toD3D11Boolean(descriptor.stencilEnable);
    depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    depthStencilDesc.BackFace.StencilDepthFailOp = toStencilOperation(descriptor.counterClockwiseFace.stencilDepthBufferFail);
    depthStencilDesc.BackFace.StencilFailOp = toStencilOperation(descriptor.counterClockwiseFace.stencilFail);
    depthStencilDesc.BackFace.StencilPassOp = toStencilOperation(descriptor.counterClockwiseFace.stencilPass);
    depthStencilDesc.BackFace.StencilFunc = toComparisonFunction(descriptor.counterClockwiseFace.stencilFunction);

    depthStencilDesc.FrontFace.StencilDepthFailOp = toStencilOperation(descriptor.clockwiseFace.stencilDepthBufferFail);
    depthStencilDesc.FrontFace.StencilFailOp = toStencilOperation(descriptor.clockwiseFace.stencilFail);
    depthStencilDesc.FrontFace.StencilPassOp = toStencilOperation(descriptor.clockwiseFace.stencilPass);
    depthStencilDesc.FrontFace.StencilFunc = toComparisonFunction(descriptor.clockwiseFace.stencilFunction);

    POMDOG_ASSERT(nativeDevice != nullptr);

    ComPtr<ID3D11DepthStencilState> depthStencilState;
    HRESULT hr = nativeDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

    if (FAILED(hr)) {
        return std::make_tuple(nullptr, errors::make("failed to create ID3D11DepthStencilState"));
    }

    return std::make_tuple(std::move(depthStencilState), nullptr);
}

[[nodiscard]] std::tuple<ComPtr<ID3D11RasterizerState>, std::unique_ptr<Error>>
createRasterizerState(
    ID3D11Device* nativeDevice,
    const RasterizerDescriptor& descriptor) noexcept
{
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

    rasterizerDesc.CullMode = toCullMode(descriptor.cullMode);
    rasterizerDesc.FillMode = toFillMode(descriptor.fillMode);
    rasterizerDesc.FrontCounterClockwise = FALSE;

    rasterizerDesc.DepthBias = static_cast<INT>(descriptor.depthBias);
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.SlopeScaledDepthBias = descriptor.slopeScaledDepthBias;
    rasterizerDesc.AntialiasedLineEnable = FALSE;
    rasterizerDesc.MultisampleEnable = toD3D11Boolean(descriptor.multisampleEnable);

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
        return std::make_tuple(nullptr, errors::make("failed to create ID3D11RasterizerState"));
    }

    return std::make_tuple(std::move(rasterizerState), nullptr);
}

[[nodiscard]] std::unique_ptr<Error>
reflectShaderBytecode(
    const ShaderBytecode& shaderBytecode,
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection>& shaderReflector,
    D3D11_SHADER_DESC& shaderDesc) noexcept
{
    if (auto hr = D3DReflect(
            shaderBytecode.code,
            shaderBytecode.byteLength,
            IID_PPV_ARGS(&shaderReflector));
        FAILED(hr)) {
        return errors::make("D3DReflect() failed");
    }

    POMDOG_ASSERT(shaderReflector != nullptr);

    if (auto hr = shaderReflector->GetDesc(&shaderDesc); FAILED(hr)) {
        return errors::make("failed to get shader descriptor");
    }

    return nullptr;
}

[[nodiscard]] std::tuple<std::vector<D3D11_INPUT_ELEMENT_DESC>, std::unique_ptr<Error>>
buildInputElements(
    const std::vector<D3D11_SIGNATURE_PARAMETER_DESC>& signatureParameters,
    const InputLayoutDescriptor& descriptor) noexcept
{
    POMDOG_ASSERT(!signatureParameters.empty());
    POMDOG_ASSERT(!descriptor.inputElements.empty());
    POMDOG_ASSERT(signatureParameters.size() == descriptor.inputElements.size());

    std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements;
    inputElements.reserve(descriptor.inputElements.size());

    auto signature = std::begin(signatureParameters);

    for (auto& sourceElement : descriptor.inputElements) {
        POMDOG_ASSERT(signature != std::end(signatureParameters));

        if (signature == std::end(signatureParameters)) {
            return std::make_tuple(std::move(inputElements), errors::make("invalid input elements"));
        }

        POMDOG_ASSERT(sourceElement.instanceStepRate == 0 ||
                      sourceElement.inputSlotClass == InputClassification::InputPerInstance);

        D3D11_INPUT_ELEMENT_DESC elementDesc;
        elementDesc.SemanticName = signature->SemanticName;
        elementDesc.SemanticIndex = signature->SemanticIndex;
        elementDesc.Format = dxgi::toDXGIFormat(sourceElement.format);
        elementDesc.InputSlot = sourceElement.inputSlot;
        elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        elementDesc.InputSlotClass = toD3D11InputClassification(sourceElement.inputSlotClass);
        elementDesc.InstanceDataStepRate = sourceElement.instanceStepRate;

        POMDOG_ASSERT(elementDesc.InputSlot <= D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
        static_assert(std::is_unsigned_v<decltype(elementDesc.InstanceDataStepRate)>, "elementDesc.InstanceDataStepRate >= 0");

        inputElements.push_back(std::move(elementDesc));

        ++signature;
    }

    return std::make_tuple(std::move(inputElements), nullptr);
}

[[nodiscard]] std::vector<D3D11_SIGNATURE_PARAMETER_DESC>
enumerateSignatureParameters(
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
createInputLayout(
    ID3D11Device* device,
    const ShaderBytecode& vertexShaderBytecode,
    const InputLayoutDescriptor& descriptor) noexcept
{
    POMDOG_ASSERT(device);
    POMDOG_ASSERT(vertexShaderBytecode.code);

    D3D11_SHADER_DESC shaderDesc;
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> shaderReflector;

    if (auto err = reflectShaderBytecode(
            vertexShaderBytecode,
            shaderReflector,
            shaderDesc);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "reflectShaderBytecode() failed"));
    }

    auto signatureParameters = enumerateSignatureParameters(
        shaderReflector.Get(), shaderDesc);

    auto [inputElements, buildErr] = buildInputElements(signatureParameters, descriptor);
    if (buildErr != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(buildErr), "buildInputElements() failed"));
    }

    Microsoft::WRL::ComPtr<ID3D11InputLayout> nativeInputLayout;
    HRESULT hr = device->CreateInputLayout(
        inputElements.data(),
        static_cast<UINT>(inputElements.size()),
        vertexShaderBytecode.code,
        vertexShaderBytecode.byteLength,
        &nativeInputLayout);

    if (FAILED(hr)) {
        return std::make_tuple(nullptr, errors::make("failed to create ID3D11InputLayout"));
    }

    return std::make_tuple(std::move(nativeInputLayout), nullptr);
}

} // namespace

std::unique_ptr<Error>
PipelineStateDirect3D11::initialize(
    ID3D11Device* device,
    const PipelineDescriptor& descriptor) noexcept
{
    POMDOG_ASSERT(device);

    sampleMask_ = descriptor.multiSampleMask;

    if (auto [result, err] = createBlendState(device, descriptor.blendState); err != nullptr) {
        return errors::wrap(std::move(err), "createBlendState() failed");
    }
    else {
        blendState_ = std::move(result);
    }

    if (auto [result, err] = createDepthStencilState(device, descriptor.depthStencilState); err != nullptr) {
        return errors::wrap(std::move(err), "createDepthStencilState() failed");
    }
    else {
        depthStencilState_ = std::move(result);
    }

    if (auto [result, err] = createRasterizerState(device, descriptor.rasterizerState); err != nullptr) {
        return errors::wrap(std::move(err), "createRasterizerState() failed");
    }
    else {
        rasterizerState_ = std::move(result);
    }

    auto vertexShaderD3D = std::dynamic_pointer_cast<VertexShaderDirect3D11>(descriptor.vertexShader);
    if (vertexShaderD3D == nullptr) {
        return errors::make("invalid vertex shader");
    }

    vertexShader_ = vertexShaderD3D->getShader();
    if (vertexShader_ == nullptr) {
        return errors::make("vertexShader must be != nullptr");
    }

    auto pixelShaderD3D = std::dynamic_pointer_cast<PixelShaderDirect3D11>(descriptor.pixelShader);
    if (pixelShaderD3D == nullptr) {
        return errors::make("invalid pixel shader");
    }

    pixelShader_ = pixelShaderD3D->getShader();
    if (pixelShader_ == nullptr) {
        return errors::make("pixelShader must be != nullptr");
    }

    if (auto [result, err] = createInputLayout(
            device,
            vertexShaderD3D->getShaderBytecode(),
            descriptor.inputLayout);
        err != nullptr) {
        return errors::wrap(std::move(err), "createInputLayout() failed");
    }
    else {
        inputLayout_ = std::move(result);
    }

    primitiveTopology_ = toPrimitiveTopology(descriptor.primitiveTopology);

    return nullptr;
}

void PipelineStateDirect3D11::apply(
    ID3D11DeviceContext* deviceContext,
    FLOAT const blendFactor[4])
{
    POMDOG_ASSERT(deviceContext);
    POMDOG_ASSERT(inputLayout_);
    POMDOG_ASSERT(vertexShader_);
    POMDOG_ASSERT(pixelShader_);
    POMDOG_ASSERT(blendState_);
    POMDOG_ASSERT(rasterizerState_);
    POMDOG_ASSERT(depthStencilState_);
    POMDOG_ASSERT(primitiveTopology_ != D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED);

    deviceContext->IASetInputLayout(inputLayout_.Get());
    deviceContext->IASetPrimitiveTopology(primitiveTopology_);
    deviceContext->VSSetShader(vertexShader_.Get(), nullptr, 0);
    deviceContext->PSSetShader(pixelShader_.Get(), nullptr, 0);
    deviceContext->OMSetBlendState(blendState_.Get(), blendFactor, sampleMask_);
    deviceContext->OMSetDepthStencilState(depthStencilState_.Get(), 0);
    deviceContext->RSSetState(rasterizerState_.Get());
}

} // namespace pomdog::gpu::detail::direct3d11
