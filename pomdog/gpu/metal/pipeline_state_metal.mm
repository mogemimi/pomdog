// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/pipeline_state_metal.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/gpu/metal/constants_metal.h"
#include "pomdog/gpu/metal/metal_format_helper.h"
#include "pomdog/gpu/metal/shader_metal.h"
#include "pomdog/gpu/pipeline_descriptor.h"
#include "pomdog/gpu/primitive_topology.h"
#include "pomdog/gpu/surface_format.h"
#include "pomdog/utility/assert.h"
#import <Metal/MTLVertexDescriptor.h>

namespace pomdog::gpu::detail::metal {
namespace {

MTLPrimitiveType ToPrimitiveType(PrimitiveTopology primitiveTopology) noexcept
{
    switch (primitiveTopology) {
    case PrimitiveTopology::TriangleStrip:
        return MTLPrimitiveTypeTriangleStrip;
    case PrimitiveTopology::TriangleList:
        return MTLPrimitiveTypeTriangle;
    case PrimitiveTopology::LineList:
        return MTLPrimitiveTypeLine;
    case PrimitiveTopology::LineStrip:
        return MTLPrimitiveTypeLineStrip;
    }
    POMDOG_UNREACHABLE("Unsupported primitive topology");
}

MTLVertexStepFunction ToVertexStepFunction(InputClassification classification) noexcept
{
    switch (classification) {
    case InputClassification::InputPerVertex:
        return MTLVertexStepFunctionPerVertex;
    case InputClassification::InputPerInstance:
        return MTLVertexStepFunctionPerInstance;
    }
    POMDOG_UNREACHABLE("Unsupported input classsification");
}

MTLVertexFormat ToVertexFormat(InputElementFormat format) noexcept
{
    switch (format) {
    case InputElementFormat::Float:
        return MTLVertexFormatFloat;
    case InputElementFormat::Float2:
        return MTLVertexFormatFloat2;
    case InputElementFormat::Float3:
        return MTLVertexFormatFloat3;
    case InputElementFormat::Float4:
        return MTLVertexFormatFloat4;
    case InputElementFormat::Int4:
        return MTLVertexFormatInt4;
    case InputElementFormat::Byte4:
        return MTLVertexFormatUChar4;
    case InputElementFormat::HalfFloat2:
        return MTLVertexFormatHalf2;
    case InputElementFormat::HalfFloat4:
        return MTLVertexFormatHalf4;
    }
    POMDOG_UNREACHABLE("Unsupported input element format");
}

MTLVertexDescriptor* ToVertexDescriptor(const InputLayoutDescriptor& inputLayout)
{
    MTLVertexDescriptor* vertexDescriptor = [MTLVertexDescriptor new];

    int attributeIndex = 0;
    for (auto& element : inputLayout.InputElements) {
        const auto slotIndex = element.InputSlot + VertexBufferSlotOffset;
        POMDOG_ASSERT(slotIndex < MaxVertexBufferSlotCount);

        auto bufferLayout = vertexDescriptor.layouts[slotIndex];
        bufferLayout.stride = element.ByteOffset + BufferHelper::ToByteSize(element.Format);
        bufferLayout.stepFunction = ToVertexStepFunction(element.InputSlotClass);
        bufferLayout.stepRate = element.InstanceStepRate;
        if (element.InputSlotClass == InputClassification::InputPerVertex) {
            // NOTE: `stepRate` must be one if stepFunction is MTLVertexStepFunctionPerVertex.
            bufferLayout.stepRate = 1;
        }

        auto attribute = vertexDescriptor.attributes[attributeIndex];
        attribute.format = ToVertexFormat(element.Format);
        attribute.offset = element.ByteOffset;
        attribute.bufferIndex = slotIndex;
        ++attributeIndex;
    }

    return vertexDescriptor;
}

MTLBlendOperation ToBlendOperation(BlendOperation blendOperation) noexcept
{
    switch (blendOperation) {
    case BlendOperation::Add:
        return MTLBlendOperationAdd;
    case BlendOperation::Subtract:
        return MTLBlendOperationSubtract;
    case BlendOperation::ReverseSubtract:
        return MTLBlendOperationReverseSubtract;
    case BlendOperation::Min:
        return MTLBlendOperationMin;
    case BlendOperation::Max:
        return MTLBlendOperationMax;
    }
    POMDOG_UNREACHABLE("Unsupported blend operation");
}

MTLBlendFactor ToBlendFactor(BlendFactor blend) noexcept
{
    switch (blend) {
    case BlendFactor::Zero:
        return MTLBlendFactorZero;
    case BlendFactor::One:
        return MTLBlendFactorOne;
    case BlendFactor::SourceColor:
        return MTLBlendFactorSourceColor;
    case BlendFactor::InverseSourceColor:
        return MTLBlendFactorOneMinusSourceColor;
    case BlendFactor::SourceAlpha:
        return MTLBlendFactorSourceAlpha;
    case BlendFactor::InverseSourceAlpha:
        return MTLBlendFactorOneMinusSourceAlpha;
    case BlendFactor::DestinationAlpha:
        return MTLBlendFactorDestinationAlpha;
    case BlendFactor::InverseDestinationAlpha:
        return MTLBlendFactorOneMinusDestinationAlpha;
    case BlendFactor::DestinationColor:
        return MTLBlendFactorDestinationColor;
    case BlendFactor::InverseDestinationColor:
        return MTLBlendFactorOneMinusDestinationColor;
    case BlendFactor::SourceAlphaSaturation:
        return MTLBlendFactorSourceAlphaSaturated;
    case BlendFactor::BlendFactor:
        return MTLBlendFactorBlendColor;
    case BlendFactor::InvereseBlendFactor:
        return MTLBlendFactorOneMinusBlendColor;
    // case BlendFactor::BlendFactorAlpha: return MTLBlendFactorBlendAlpha;
    // case BlendFactor::InvereseBlendFactorAlpha: return MTLBlendFactorOneMinusBlendAlpha;
    case BlendFactor::Source1Color:
        return MTLBlendFactorSource1Color;
    case BlendFactor::InverseSource1Color:
        return MTLBlendFactorOneMinusSource1Color;
    case BlendFactor::Source1Alpha:
        return MTLBlendFactorSource1Alpha;
    case BlendFactor::InverseSource1Alpha:
        return MTLBlendFactorOneMinusSource1Alpha;
    }
    POMDOG_UNREACHABLE("Unsupported blend factor");
}

MTLStencilOperation ToStencilOperation(StencilOperation operation) noexcept
{
    switch (operation) {
    case StencilOperation::Keep:
        return MTLStencilOperationKeep;
    case StencilOperation::Zero:
        return MTLStencilOperationZero;
    case StencilOperation::Replace:
        return MTLStencilOperationReplace;
    case StencilOperation::IncrementSaturation:
        return MTLStencilOperationIncrementClamp;
    case StencilOperation::DecrementSaturation:
        return MTLStencilOperationDecrementClamp;
    case StencilOperation::Invert:
        return MTLStencilOperationInvert;
    case StencilOperation::Increment:
        return MTLStencilOperationIncrementWrap;
    case StencilOperation::Decrement:
        return MTLStencilOperationDecrementWrap;
    }
    POMDOG_UNREACHABLE("Unsupported stencil operation");
}

void ToDepthStencilOperation(
    MTLStencilDescriptor* desc,
    const DepthStencilOperation& operation,
    const DepthStencilDescriptor& descriptor)
{
    desc.stencilCompareFunction = ToComparisonFunction(operation.StencilFunction);
    desc.depthStencilPassOperation = ToStencilOperation(operation.StencilPass);
    desc.stencilFailureOperation = ToStencilOperation(operation.StencilFail);
    desc.depthFailureOperation = ToStencilOperation(operation.StencilDepthBufferFail);

    desc.readMask = descriptor.StencilMask;
    desc.writeMask = descriptor.StencilWriteMask;
}

MTLCullMode ToCullMode(CullMode cullMode) noexcept
{
    switch (cullMode) {
    case CullMode::ClockwiseFace:
        return MTLCullModeFront;
    case CullMode::CounterClockwiseFace:
        return MTLCullModeBack;
    case CullMode::None:
        return MTLCullModeNone;
    }
    POMDOG_UNREACHABLE("Unsupported cull mode");
}

MTLTriangleFillMode ToFillMode(FillMode fillMode) noexcept
{
    switch (fillMode) {
    case FillMode::WireFrame:
        return MTLTriangleFillModeLines;
    case FillMode::Solid:
        return MTLTriangleFillModeFill;
    }
    POMDOG_UNREACHABLE("Unsupported fill mode");
}

} // namespace

PipelineStateMetal::~PipelineStateMetal() = default;

std::unique_ptr<Error>
PipelineStateMetal::Initialize(
    id<MTLDevice> device,
    const PipelineStateDescriptor& descriptor) noexcept
{
    POMDOG_ASSERT(device != nullptr);

    primitiveType = ToPrimitiveType(descriptor.PrimitiveTopology);

    rasterizerState.depthBias = descriptor.RasterizerState.DepthBias;
    rasterizerState.slopeScaledDepthBias = descriptor.RasterizerState.SlopeScaledDepthBias;
    rasterizerState.cullMode = ToCullMode(descriptor.RasterizerState.CullMode);
    rasterizerState.fillMode = ToFillMode(descriptor.RasterizerState.FillMode);

    auto vertexShaderMetal = std::dynamic_pointer_cast<ShaderMetal>(descriptor.VertexShader);
    if (vertexShaderMetal == nullptr) {
        return errors::New("invalid vertex shader");
    }

    auto vertexShader = vertexShaderMetal->GetShader();
    if (vertexShader == nullptr) {
        return errors::New("vertexShader must be != nullptr");
    }

    auto pixelShaderMetal = std::dynamic_pointer_cast<ShaderMetal>(descriptor.PixelShader);
    if (pixelShaderMetal == nullptr) {
        return errors::New("invalid pixel shader");
    }

    auto pixelShader = pixelShaderMetal->GetShader();
    if (pixelShader == nullptr) {
        return errors::New("pixelShader must be != nullptr");
    }

    ///@todo MSAA is not implemented yet
    constexpr int multiSampleCount = 1;

    const auto depthStencilFormat = ToPixelFormat(descriptor.DepthStencilViewFormat);

    MTLRenderPipelineDescriptor* pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineDesc.label = @"Pomdog.RenderPipeline";
    pipelineDesc.vertexFunction = vertexShader;
    pipelineDesc.fragmentFunction = pixelShader;
    pipelineDesc.vertexDescriptor = ToVertexDescriptor(descriptor.InputLayout);
    pipelineDesc.sampleCount = multiSampleCount;
    switch (descriptor.DepthStencilViewFormat) {
    case SurfaceFormat::Depth16:
        [[fallthrough]];
    case SurfaceFormat::Depth32:
        pipelineDesc.depthAttachmentPixelFormat = depthStencilFormat;
        pipelineDesc.stencilAttachmentPixelFormat = MTLPixelFormatInvalid;
        break;
    case SurfaceFormat::Depth24Stencil8:
        [[fallthrough]];
    case SurfaceFormat::Depth32_Float_Stencil8_Uint:
        pipelineDesc.depthAttachmentPixelFormat = depthStencilFormat;
        pipelineDesc.stencilAttachmentPixelFormat = depthStencilFormat;
        break;
    case SurfaceFormat::Invalid:
        [[fallthrough]];
    default:
        pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatInvalid;
        pipelineDesc.stencilAttachmentPixelFormat = MTLPixelFormatInvalid;
        break;
    }

    std::size_t index = 0;
    for (auto& renderTarget : descriptor.BlendState.RenderTargets) {
        if (index >= descriptor.RenderTargetViewFormats.size()) {
            break;
        }

        auto colorAttachment = pipelineDesc.colorAttachments[index];
        colorAttachment.pixelFormat = ToPixelFormat(descriptor.RenderTargetViewFormats[index]);
        colorAttachment.rgbBlendOperation = ToBlendOperation(renderTarget.ColorBlendOperation);
        colorAttachment.alphaBlendOperation = ToBlendOperation(renderTarget.AlphaBlendOperation);
        colorAttachment.sourceRGBBlendFactor = ToBlendFactor(renderTarget.ColorSourceBlend);
        colorAttachment.sourceAlphaBlendFactor = ToBlendFactor(renderTarget.AlphaSourceBlend);
        colorAttachment.destinationRGBBlendFactor = ToBlendFactor(renderTarget.ColorDestinationBlend);
        colorAttachment.destinationAlphaBlendFactor = ToBlendFactor(renderTarget.AlphaDestinationBlend);
        colorAttachment.blendingEnabled = renderTarget.BlendEnable;

        // TODO: Not implemented
        colorAttachment.writeMask = MTLColorWriteMaskAll;

        ++index;
    }

    NSError* error = nullptr;

    MTLRenderPipelineReflection* autoReleasingReflection = nullptr;

    this->pipelineState = [device
        newRenderPipelineStateWithDescriptor:pipelineDesc
                                     options:MTLPipelineOptionArgumentInfo
                                  reflection:&autoReleasingReflection
                                       error:&error];

    reflection = autoReleasingReflection;

    if (this->pipelineState == nullptr) {
        return errors::New(std::string{"newRenderPipelineStateWithDescriptor() failed: "} + [[error domain] UTF8String]);
    }

    MTLDepthStencilDescriptor* depthStencilDesc = [[MTLDepthStencilDescriptor alloc] init];
    depthStencilDesc.label = @"Pomdog.DepthStencilState";
    depthStencilDesc.depthCompareFunction = ToComparisonFunction(
        descriptor.DepthStencilState.DepthBufferEnable
            ? descriptor.DepthStencilState.DepthBufferFunction
            : ComparisonFunction::Always);
    depthStencilDesc.depthWriteEnabled = descriptor.DepthStencilState.DepthBufferWriteEnable ? YES : NO;

    ToDepthStencilOperation(
        depthStencilDesc.frontFaceStencil,
        descriptor.DepthStencilState.ClockwiseFace,
        descriptor.DepthStencilState);

    ToDepthStencilOperation(
        depthStencilDesc.backFaceStencil,
        descriptor.DepthStencilState.CounterClockwiseFace,
        descriptor.DepthStencilState);

    depthStencilState = [device newDepthStencilStateWithDescriptor:depthStencilDesc];
    if (this->depthStencilState == nullptr) {
        return errors::New("newDepthStencilStateWithDescriptor() failed");
    }
    return nullptr;
}

void PipelineStateMetal::Apply(id<MTLRenderCommandEncoder> commandEncoder)
{
    POMDOG_ASSERT(commandEncoder != nullptr);

    [commandEncoder setRenderPipelineState:pipelineState];
    [commandEncoder setDepthStencilState:depthStencilState];
    [commandEncoder setCullMode:rasterizerState.cullMode];
    [commandEncoder setTriangleFillMode:rasterizerState.fillMode];
    [commandEncoder setDepthBias:rasterizerState.depthBias
                      slopeScale:rasterizerState.slopeScaledDepthBias
                           clamp:0.0f];
}

MTLPrimitiveType PipelineStateMetal::GetPrimitiveType() const noexcept
{
    return primitiveType;
}

} // namespace pomdog::gpu::detail::metal
