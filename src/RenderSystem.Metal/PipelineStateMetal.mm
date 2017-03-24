// Copyright (c) 2013-2017 mogemimi. Distributed under the MIT license.

#include "PipelineStateMetal.hpp"
#include "MetalFormatHelper.hpp"
#include "../RenderSystem/BufferHelper.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"
#include "Pomdog/Graphics/PipelineStateDescription.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include "ShaderMetal.hpp"
#import <Metal/MTLVertexDescriptor.h>

namespace Pomdog {
namespace Detail {
namespace Metal {
namespace {

MTLVertexStepFunction ToVertexStepFunction(InputClassification classification) noexcept
{
    switch (classification) {
    case InputClassification::InputPerVertex:
        return MTLVertexStepFunctionPerVertex;
    case InputClassification::InputPerInstance:
        return MTLVertexStepFunctionPerInstance;
    }
}

MTLVertexFormat ToVertexFormat(InputElementFormat format) noexcept
{
    switch (format) {
    case InputElementFormat::Float: return MTLVertexFormatFloat;
    case InputElementFormat::Float2: return MTLVertexFormatFloat2;
    case InputElementFormat::Float3: return MTLVertexFormatFloat3;
    case InputElementFormat::Float4: return MTLVertexFormatFloat4;
    case InputElementFormat::Int4: return MTLVertexFormatInt4;
    case InputElementFormat::Byte4: return MTLVertexFormatUChar4;
    case InputElementFormat::HalfFloat2: return MTLVertexFormatHalf2;
    case InputElementFormat::HalfFloat4: return MTLVertexFormatHalf4;
    }
}

MTLVertexDescriptor* ToVertexDescriptor(const InputLayoutDescription& inputLayout)
{
    MTLVertexDescriptor* vertexDescriptor = [MTLVertexDescriptor new];

    int attributeIndex = 0;
    for (auto & element : inputLayout.InputElements) {
        auto bufferLayout = vertexDescriptor.layouts[element.InputSlot];
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
        attribute.bufferIndex = element.InputSlot;
        ++attributeIndex;
    }

    return vertexDescriptor;
}

MTLBlendOperation ToBlendOperation(BlendOperation blendOperation) noexcept
{
    switch (blendOperation) {
    case BlendOperation::Add: return MTLBlendOperationAdd;
    case BlendOperation::Subtract: return MTLBlendOperationSubtract;
    case BlendOperation::ReverseSubtract: return MTLBlendOperationReverseSubtract;
    case BlendOperation::Min: return MTLBlendOperationMin;
    case BlendOperation::Max: return MTLBlendOperationMax;
    }
}

MTLBlendFactor ToBlendFactor(Blend blend)
{
    switch (blend) {
    case Blend::Zero: return MTLBlendFactorZero;
    case Blend::One: return MTLBlendFactorOne;
    case Blend::SourceColor: return MTLBlendFactorSourceColor;
    case Blend::InverseSourceColor: return MTLBlendFactorOneMinusSourceColor;
    case Blend::SourceAlpha: return MTLBlendFactorSourceAlpha;
    case Blend::InverseSourceAlpha: return MTLBlendFactorOneMinusSourceAlpha;
    case Blend::DestinationAlpha: return MTLBlendFactorDestinationAlpha;
    case Blend::InverseDestinationAlpha: return MTLBlendFactorOneMinusDestinationAlpha;
    case Blend::DestinationColor: return MTLBlendFactorDestinationColor;
    case Blend::InverseDestinationColor: return MTLBlendFactorOneMinusDestinationColor;
    case Blend::SourceAlphaSaturation: return MTLBlendFactorSourceAlphaSaturated;
    case Blend::Source1Color: return MTLBlendFactorBlendColor;
    case Blend::InverseSource1Color: return MTLBlendFactorOneMinusBlendColor;
    case Blend::Source1Alpha: return MTLBlendFactorBlendAlpha;
    case Blend::InverseSource1Alpha: return MTLBlendFactorOneMinusBlendAlpha;
    case Blend::BlendFactor:
        break;
    case Blend::InvereseBlendFactor:
        break;
    }

    POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid blend factor");
    return MTLBlendFactorOne;
}

MTLStencilOperation ToStencilOperation(StencilOperation operation) noexcept
{
    switch (operation) {
    case StencilOperation::Keep: return MTLStencilOperationKeep;
    case StencilOperation::Zero: return MTLStencilOperationZero;
    case StencilOperation::Replace: return MTLStencilOperationReplace;
    case StencilOperation::IncrementSaturation: return MTLStencilOperationIncrementClamp;
    case StencilOperation::DecrementSaturation: return MTLStencilOperationDecrementClamp;
    case StencilOperation::Invert: return MTLStencilOperationInvert;
    case StencilOperation::Increment: return MTLStencilOperationIncrementWrap;
    case StencilOperation::Decrement: return MTLStencilOperationDecrementWrap;
    }
    return MTLStencilOperationKeep;
}

MTLCompareFunction ToComparisonFunction(ComparisonFunction compareFunction) noexcept
{
    switch (compareFunction) {
    case ComparisonFunction::Never: return MTLCompareFunctionNever;
    case ComparisonFunction::Less:  return MTLCompareFunctionLess;
    case ComparisonFunction::Equal: return MTLCompareFunctionEqual;
    case ComparisonFunction::LessEqual: return MTLCompareFunctionLessEqual;
    case ComparisonFunction::Greater:   return MTLCompareFunctionGreater;
    case ComparisonFunction::NotEqual:  return MTLCompareFunctionNotEqual;
    case ComparisonFunction::GreaterEqual: return MTLCompareFunctionGreaterEqual;
    case ComparisonFunction::Always: return MTLCompareFunctionAlways;
    }
    return MTLCompareFunctionLessEqual;
}

void ToDepthStencilOperation(
    MTLStencilDescriptor* desc,
    const DepthStencilOperation& operation,
    const DepthStencilDescription& description)
{
    desc.stencilCompareFunction = ToComparisonFunction(operation.StencilFunction);
    desc.depthStencilPassOperation = ToStencilOperation(operation.StencilPass);
    desc.stencilFailureOperation = ToStencilOperation(operation.StencilFail);
    desc.depthFailureOperation = ToStencilOperation(operation.StencilDepthBufferFail);

    desc.readMask = description.StencilMask;
    desc.writeMask = description.StencilWriteMask;
}

MTLCullMode ToCullMode(CullMode cullMode) noexcept
{
    switch (cullMode) {
    case CullMode::ClockwiseFace: return MTLCullModeFront;
    case CullMode::CounterClockwiseFace: return MTLCullModeBack;
    case CullMode::None: return MTLCullModeNone;
    }
    return MTLCullModeBack;
}

MTLTriangleFillMode ToFillMode(FillMode fillMode) noexcept
{
    switch (fillMode) {
    case FillMode::WireFrame: return MTLTriangleFillModeLines;
    case FillMode::Solid: return MTLTriangleFillModeFill;
    }
    return MTLTriangleFillModeFill;
}

MTLPixelFormat ToDepthPixelFormat(DepthFormat depthFormat) noexcept
{
    // Not supported:
    POMDOG_ASSERT(depthFormat != DepthFormat::Depth16);

    switch (depthFormat) {
    case DepthFormat::Depth16: return MTLPixelFormatDepth32Float;
    case DepthFormat::Depth32: return MTLPixelFormatDepth32Float;
#if defined(MAC_OS_X_VERSION_10_11) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_11)
    case DepthFormat::Depth24Stencil8: return MTLPixelFormatDepth24Unorm_Stencil8;
#else
    case DepthFormat::Depth24Stencil8: return MTLPixelFormatDepth32Float_Stencil8;
#endif
    case DepthFormat::Depth32_Float_Stencil8_Uint: return MTLPixelFormatDepth32Float_Stencil8;
    case DepthFormat::None: return MTLPixelFormatInvalid;
    }
    return MTLPixelFormatDepth32Float;
}

} // unnamed namespace

PipelineStateMetal::PipelineStateMetal(
    id<MTLDevice> device,
    const PipelineStateDescription& description)
    : pipelineState(nil)
    , depthStencilState(nil)
    , reflection(nil)
{
    POMDOG_ASSERT(device != nil);

    rasterizerState.depthBias = description.RasterizerState.DepthBias;
    rasterizerState.slopeScaledDepthBias = description.RasterizerState.SlopeScaledDepthBias;
    rasterizerState.cullMode = ToCullMode(description.RasterizerState.CullMode);
    rasterizerState.fillMode = ToFillMode(description.RasterizerState.FillMode);

    auto vertexShaderMetal = std::dynamic_pointer_cast<ShaderMetal>(description.VertexShader);
    if (!vertexShaderMetal) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid vertex shader.");
    }

    auto vertexShader = vertexShaderMetal->GetShader();
    if (!vertexShader) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "The vertex shader is null");
    }

    auto pixelShaderMetal = std::dynamic_pointer_cast<ShaderMetal>(description.PixelShader);
    if (!pixelShaderMetal) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "Invalid pixel shader.");
    }

    auto pixelShader = pixelShaderMetal->GetShader();
    if (!pixelShader) {
        POMDOG_THROW_EXCEPTION(std::runtime_error, "The pixel shader is null");
    }

    ///@todo MSAA is not implemented yet
    constexpr int multiSampleCount = 1;

    const auto depthStencilFormat = ToDepthPixelFormat(description.DepthStencilViewFormat);

    MTLRenderPipelineDescriptor* descriptor = [[MTLRenderPipelineDescriptor alloc] init];
    descriptor.label = @"Pomdog.RenderPipeline";
    descriptor.vertexFunction = vertexShader;
    descriptor.fragmentFunction = pixelShader;
    descriptor.vertexDescriptor = ToVertexDescriptor(description.InputLayout);
    descriptor.sampleCount = multiSampleCount;
    descriptor.depthAttachmentPixelFormat = depthStencilFormat;
    descriptor.stencilAttachmentPixelFormat = depthStencilFormat;

    std::size_t index = 0;
    for (auto & renderTarget : description.BlendState.RenderTargets) {
        if (index >= description.RenderTargetViewFormats.size()) {
            break;
        }

        auto colorAttachment = descriptor.colorAttachments[index];
        colorAttachment.rgbBlendOperation = ToBlendOperation(renderTarget.ColorBlendOperation);
        colorAttachment.alphaBlendOperation = ToBlendOperation(renderTarget.AlphaBlendOperation);
        colorAttachment.sourceRGBBlendFactor = ToBlendFactor(renderTarget.ColorSourceBlend);
        colorAttachment.sourceAlphaBlendFactor = ToBlendFactor(renderTarget.AlphaSourceBlend);
        colorAttachment.destinationRGBBlendFactor = ToBlendFactor(renderTarget.ColorDestinationBlend);
        colorAttachment.destinationAlphaBlendFactor = ToBlendFactor(renderTarget.AlphaDestinationBlend);
        colorAttachment.blendingEnabled = renderTarget.BlendEnable;

        auto pixelFormat = MetalFormatHelper::ToMTLPixelFormat(description.RenderTargetViewFormats[index]);
        POMDOG_ASSERT(pixelFormat);
        colorAttachment.pixelFormat = *pixelFormat;

        // TODO: Not implemented
        colorAttachment.writeMask = MTLColorWriteMaskAll;

        ++index;
    }

    NSError* error = nil;

    MTLRenderPipelineReflection* autoReleasingReflection = nil;

    this->pipelineState = [device
        newRenderPipelineStateWithDescriptor:descriptor
        options:MTLPipelineOptionArgumentInfo
        reflection:&autoReleasingReflection
        error:&error];

    reflection = autoReleasingReflection;

    if (!this->pipelineState) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            StringHelper::Format(
                "Failed to create pipeline state, error %s",
                [[error domain] UTF8String]));
    }

    MTLDepthStencilDescriptor* depthStencilDesc = [[MTLDepthStencilDescriptor alloc] init];
    depthStencilDesc.label = @"Pomdog.DepthStencilState";
    depthStencilDesc.depthCompareFunction = ToComparisonFunction(description.DepthStencilState.DepthBufferFunction);
    depthStencilDesc.depthWriteEnabled = description.DepthStencilState.DepthBufferWriteEnable ? YES : NO;

    ToDepthStencilOperation(
        depthStencilDesc.frontFaceStencil,
        description.DepthStencilState.ClockwiseFace,
        description.DepthStencilState);

    ToDepthStencilOperation(
        depthStencilDesc.backFaceStencil,
        description.DepthStencilState.CounterClockwiseFace,
        description.DepthStencilState);

    depthStencilState = [device newDepthStencilStateWithDescriptor:depthStencilDesc];
    if (!this->depthStencilState) {
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to create depth stencil state for Metal");
    }
}

void PipelineStateMetal::Apply(id<MTLRenderCommandEncoder> commandEncoder)
{
    POMDOG_ASSERT(commandEncoder != nil);

    [commandEncoder setRenderPipelineState:pipelineState];
    [commandEncoder setDepthStencilState:depthStencilState];
    [commandEncoder setCullMode:rasterizerState.cullMode];
    [commandEncoder setTriangleFillMode:rasterizerState.fillMode];
    [commandEncoder setDepthBias:rasterizerState.depthBias
        slopeScale:rasterizerState.slopeScaledDepthBias
        clamp:0.0f];
}

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
