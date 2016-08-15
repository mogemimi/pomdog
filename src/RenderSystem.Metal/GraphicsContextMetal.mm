// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "GraphicsContextMetal.hpp"
#include "../RenderSystem.Metal/BufferMetal.hpp"
#include "../RenderSystem.Metal/PipelineStateMetal.hpp"
#include "../RenderSystem.Metal/RenderTarget2DMetal.hpp"
#include "../RenderSystem.Metal/SamplerStateMetal.hpp"
#include "../RenderSystem.Metal/Texture2DMetal.hpp"
#include "../RenderSystem/GraphicsCapabilities.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/RenderPass.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Graphics/VertexBufferBinding.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
namespace Detail {
namespace Metal {
namespace {

MTLPrimitiveType ToPrimitiveType(PrimitiveTopology primitiveTopology) noexcept
{
    switch (primitiveTopology) {
    case PrimitiveTopology::TriangleStrip: return MTLPrimitiveTypeTriangleStrip;
    case PrimitiveTopology::TriangleList: return MTLPrimitiveTypeTriangle;
    case PrimitiveTopology::LineList: return MTLPrimitiveTypeLine;
    case PrimitiveTopology::LineStrip: return MTLPrimitiveTypeLineStrip;
    }
    return MTLPrimitiveTypePoint;
}

MTLIndexType ToIndexType(IndexElementSize elementSize) noexcept
{
    switch (elementSize) {
    case IndexElementSize::SixteenBits: return MTLIndexTypeUInt16;
    case IndexElementSize::ThirtyTwoBits: return MTLIndexTypeUInt32;
    }
    return MTLIndexTypeUInt16;
}

MTLClearColor ToClearColor(const Color& color) noexcept
{
    auto vec = color.ToVector4();
    return MTLClearColorMake(vec.X, vec.Y, vec.Z, vec.W);
}

void SetViewport(
    id<MTLRenderCommandEncoder> commandEncoder, const Viewport& viewportIn)
{
    POMDOG_ASSERT(viewportIn.Width > 0);
    POMDOG_ASSERT(viewportIn.Height > 0);
    POMDOG_ASSERT(commandEncoder != nil);

    MTLViewport viewport;
    viewport.originX = viewportIn.TopLeftX;
    viewport.originY = viewportIn.TopLeftY;
    viewport.width = viewportIn.Width;
    viewport.height = viewportIn.Height;
    viewport.znear = viewportIn.MinDepth;
    viewport.zfar = viewportIn.MaxDepth;
    [commandEncoder setViewport:viewport];
}

void SetScissorRectangle(
    id<MTLRenderCommandEncoder> commandEncoder, const Rectangle& rectangle)
{
    POMDOG_ASSERT(rectangle.Width > 0);
    POMDOG_ASSERT(rectangle.Height > 0);
    POMDOG_ASSERT(commandEncoder != nil);

    MTLScissorRect rect;
    rect.x = rectangle.X;
    rect.y = rectangle.Y;
    rect.width = rectangle.Width;
    rect.height = rectangle.Height;
    [commandEncoder setScissorRect:rect];
}

} // unnamed namespace

GraphicsContextMetal::GraphicsContextMetal(
    id<MTLDevice> nativeDevice)
    : commandQueue(nil)
    , commandBuffer(nil)
    , commandEncoder(nil)
    , indexBuffer(nil)
{
    POMDOG_ASSERT(nativeDevice != nil);

    // NOTE: Create a new command queue
    commandQueue = [nativeDevice newCommandQueue];

    // NOTE: Set default values for graphics context
    this->SetBlendFactor(Color::White);
}

GraphicsContextMetal::~GraphicsContextMetal() = default;

GraphicsCapabilities GraphicsContextMetal::GetCapabilities() const
{
    // NOTE: For more information, please see:
    // https://developer.apple.com/library/ios/documentation/Miscellaneous/Conceptual/MetalProgrammingGuide/MetalFeatureSetTables/MetalFeatureSetTables.html
    GraphicsCapabilities caps;
    caps.ConstantBufferSlotCount = 31;
    caps.SamplerSlotCount = 16;
    return caps;
}

void GraphicsContextMetal::Present()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsContextMetal::Draw(std::size_t vertexCount)
{
    POMDOG_ASSERT(commandEncoder != nil);
    POMDOG_ASSERT(vertexCount > 0);

    [commandEncoder drawPrimitives:primitiveType
        vertexStart:0
        vertexCount:vertexCount];
}

void GraphicsContextMetal::DrawIndexed(std::size_t indexCount)
{
    POMDOG_ASSERT(commandEncoder != nil);
    POMDOG_ASSERT(indexCount > 0);

    [commandEncoder drawIndexedPrimitives:primitiveType
        indexCount:indexCount
        indexType:indexType
        indexBuffer:indexBuffer
        indexBufferOffset:0];
}

void GraphicsContextMetal::DrawInstanced(
    std::size_t vertexCount,
    std::size_t instanceCount)
{
    POMDOG_ASSERT(commandEncoder != nil);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(instanceCount > 0);

    [commandEncoder drawPrimitives:primitiveType
        vertexStart:0
        vertexCount:vertexCount
        instanceCount:instanceCount];
}

void GraphicsContextMetal::DrawIndexedInstanced(
    std::size_t indexCount,
    std::size_t instanceCount)
{
    POMDOG_ASSERT(commandEncoder != nil);
    POMDOG_ASSERT(indexCount > 0);
    POMDOG_ASSERT(instanceCount > 0);

    [commandEncoder drawIndexedPrimitives:primitiveType
        indexCount:indexCount
        indexType:indexType
        indexBuffer:indexBuffer
        indexBufferOffset:0
        instanceCount:instanceCount];
}

void GraphicsContextMetal::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
{
    this->primitiveType = ToPrimitiveType(primitiveTopology);
}

void GraphicsContextMetal::SetBlendFactor(const Color& blendFactor)
{
    POMDOG_ASSERT(commandEncoder != nil);
    auto vec = blendFactor.ToVector4();
    [commandEncoder setBlendColorRed:vec.X green:vec.Y blue:vec.Z alpha:vec.W];
}

void GraphicsContextMetal::SetVertexBuffers(const std::vector<VertexBufferBinding>& vertexBuffers)
{
    POMDOG_ASSERT(!vertexBuffers.empty());

    NSUInteger atIndex = 0;
    for (auto & binding: vertexBuffers)
    {
        auto & vertexBuffer = binding.VertexBuffer;

        POMDOG_ASSERT(vertexBuffer);
        POMDOG_ASSERT(vertexBuffer->GetNativeVertexBuffer());

        auto nativeVertexBuffer = static_cast<BufferMetal*>(
            vertexBuffer->GetNativeVertexBuffer());

        POMDOG_ASSERT(nativeVertexBuffer != nullptr);
        POMDOG_ASSERT(nativeVertexBuffer == dynamic_cast<BufferMetal*>(
            vertexBuffer->GetNativeVertexBuffer()));

        POMDOG_ASSERT(nativeVertexBuffer->GetBuffer() != nil);
        [commandEncoder setVertexBuffer:nativeVertexBuffer->GetBuffer()
            offset:binding.VertexOffset
            atIndex:atIndex];

        ++atIndex;
    }
}

void GraphicsContextMetal::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBufferIn)
{
    POMDOG_ASSERT(indexBuffer);

    auto nativeIndexBuffer = static_cast<BufferMetal*>(
        indexBufferIn->NativeIndexBuffer());

    POMDOG_ASSERT(nativeIndexBuffer != nullptr);
    POMDOG_ASSERT(nativeIndexBuffer == dynamic_cast<BufferMetal*>(
        indexBufferIn->NativeIndexBuffer()));

    this->indexType = ToIndexType(indexBufferIn->GetElementSize());
    this->indexBuffer = nativeIndexBuffer->GetBuffer();
}

void GraphicsContextMetal::SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState)
{
    POMDOG_ASSERT(pipelineState != nullptr);

    auto nativePipelineState = static_cast<PipelineStateMetal*>(pipelineState.get());

    POMDOG_ASSERT(nativePipelineState != nullptr);
    POMDOG_ASSERT(nativePipelineState == dynamic_cast<PipelineStateMetal*>(pipelineState.get()));

    POMDOG_ASSERT(commandEncoder != nil);
    nativePipelineState->Apply(commandEncoder);
}

void GraphicsContextMetal::SetConstantBuffer(int index, const std::shared_ptr<NativeBuffer>& constantBuffer)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsContextMetal::SetSampler(int index, const std::shared_ptr<NativeSamplerState>& sampler)
{
    POMDOG_ASSERT(sampler != nullptr);
    POMDOG_ASSERT(index >= 0);

    auto samplerStateMetal = std::static_pointer_cast<SamplerStateMetal>(sampler);

    POMDOG_ASSERT(samplerStateMetal != nullptr);
    POMDOG_ASSERT(samplerStateMetal == std::dynamic_pointer_cast<SamplerStateMetal>(sampler));
    POMDOG_ASSERT(samplerStateMetal->GetSamplerState() != nil);

    POMDOG_ASSERT(commandEncoder != nil);
    [commandEncoder setVertexSamplerState:samplerStateMetal->GetSamplerState() atIndex:index];
    [commandEncoder setFragmentSamplerState:samplerStateMetal->GetSamplerState() atIndex:index];
}

void GraphicsContextMetal::SetTexture(int index)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(commandEncoder != nil);
    [commandEncoder setVertexTexture:nil atIndex:index];
    [commandEncoder setFragmentTexture:nil atIndex:index];
}

void GraphicsContextMetal::SetTexture(int index, const std::shared_ptr<Texture2D>& textureIn)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(textureIn);

    auto textureMetal = static_cast<Texture2DMetal*>(textureIn->GetNativeTexture2D());

    POMDOG_ASSERT(textureMetal != nullptr);
    POMDOG_ASSERT(textureMetal == dynamic_cast<Texture2DMetal*>(textureIn->GetNativeTexture2D()));
    POMDOG_ASSERT(textureMetal->GetTexture() != nil);

    POMDOG_ASSERT(commandEncoder != nil);
    [commandEncoder setVertexTexture:textureMetal->GetTexture() atIndex:index];
    [commandEncoder setFragmentTexture:textureMetal->GetTexture() atIndex:index];
}

void GraphicsContextMetal::SetTexture(int index, const std::shared_ptr<RenderTarget2D>& textureIn)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(textureIn);

    auto renderTargetMetal = static_cast<RenderTarget2DMetal*>(textureIn->GetNativeRenderTarget2D());

    POMDOG_ASSERT(renderTargetMetal != nullptr);
    POMDOG_ASSERT(renderTargetMetal == dynamic_cast<RenderTarget2DMetal*>(textureIn->GetNativeRenderTarget2D()));
    POMDOG_ASSERT(renderTargetMetal->GetTexture() != nil);

    POMDOG_ASSERT(commandEncoder != nil);
    [commandEncoder setVertexTexture:renderTargetMetal->GetTexture() atIndex:index];
    [commandEncoder setFragmentTexture:renderTargetMetal->GetTexture() atIndex:index];
}

void GraphicsContextMetal::SetRenderPass(const RenderPass& renderPass)
{
    POMDOG_ASSERT(!renderPass.RenderTargets.empty());
    POMDOG_ASSERT(renderPass.RenderTargets.size() <= 8);

    MTLRenderPassDescriptor* renderPassDescriptor = [[MTLRenderPassDescriptor alloc] init];
    POMDOG_ASSERT(renderPassDescriptor != nil);

    renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionDontCare;
    renderPassDescriptor.stencilAttachment.loadAction = MTLLoadActionDontCare;

    int renderTargetIndex = 0;
    for (const auto& view: renderPass.RenderTargets) {
        //auto & renderTarget = std::get<0>(view);
        auto & clearColor = std::get<1>(view);

        // TODO: Not implemented
        //if (!renderTarget) {
        //    renderPassDescriptor.colorAttachments[renderTargetIndex].texture = [_view.currentDrawable texture];
        //    renderPassDescriptor.depthAttachment.texture = ;
        //    renderPassDescriptor.stencilAttachment.texture = ;
        //}

        if (clearColor) {
            renderPassDescriptor.colorAttachments[renderTargetIndex].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[renderTargetIndex].clearColor = ToClearColor(*clearColor);
        }
        else {
            // TOOD: Not implemented
            renderPassDescriptor.colorAttachments[renderTargetIndex].loadAction = MTLLoadActionDontCare;
        }
        ++renderTargetIndex;
    }

    if (renderPass.ClearDepth) {
        renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
        renderPassDescriptor.depthAttachment.clearDepth = *renderPass.ClearDepth;
    }
    if (renderPass.ClearStencil) {
        renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
        renderPassDescriptor.stencilAttachment.clearStencil = *renderPass.ClearStencil;
    }

    POMDOG_ASSERT(commandQueue != nil);
    commandBuffer = [commandQueue commandBuffer];

    commandEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    commandEncoder.label = @"PomdogRenderEncoder";

    [commandEncoder setFrontFacingWinding:MTLWindingClockwise];

    if (renderPass.Viewport) {
        SetViewport(commandEncoder, *renderPass.Viewport);
    }
    if (renderPass.ScissorRect) {
        SetScissorRectangle(commandEncoder, *renderPass.ScissorRect);
    }

    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");

    // TODO: set render targets
}

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
