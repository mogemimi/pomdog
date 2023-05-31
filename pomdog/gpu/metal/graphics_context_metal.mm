// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/graphics_context_metal.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/backends/command_list_immediate.h"
#include "pomdog/gpu/backends/graphics_capabilities.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/metal/buffer_metal.h"
#include "pomdog/gpu/metal/constants_metal.h"
#include "pomdog/gpu/metal/depth_stencil_buffer_metal.h"
#include "pomdog/gpu/metal/pipeline_state_metal.h"
#include "pomdog/gpu/metal/render_target2d_metal.h"
#include "pomdog/gpu/metal/sampler_state_metal.h"
#include "pomdog/gpu/metal/texture2d_metal.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/render_pass.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/math/vector4.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/static_down_cast.h"

using pomdog::detail::StaticDownCast;

namespace pomdog::gpu::detail::metal {
namespace {

[[nodiscard]] MTLIndexType ToIndexType(IndexFormat elementSize) noexcept
{
    switch (elementSize) {
    case IndexFormat::UInt16:
        return MTLIndexTypeUInt16;
    case IndexFormat::UInt32:
        return MTLIndexTypeUInt32;
    }
    POMDOG_UNREACHABLE("Unsupported index element size");
}

std::size_t ToIndexByteSize(MTLIndexType elementSize) noexcept
{
    switch (elementSize) {
    case MTLIndexTypeUInt16:
        return 2;
    case MTLIndexTypeUInt32:
        return 4;
    }
    POMDOG_UNREACHABLE("Unsupported index element size");
}

MTLClearColor ToClearColor(const Vector4& color) noexcept
{
    return MTLClearColorMake(
        static_cast<double>(color.x),
        static_cast<double>(color.y),
        static_cast<double>(color.z),
        static_cast<double>(color.w));
}

void SetViewport(
    id<MTLRenderCommandEncoder> commandEncoder, const Viewport& viewportIn)
{
    POMDOG_ASSERT(viewportIn.width > 0);
    POMDOG_ASSERT(viewportIn.height > 0);
    POMDOG_ASSERT(commandEncoder != nullptr);

    // NOTE: The MinDepth and MaxDepth must be between 0.0 and 1.0, respectively.
    // Please see https://developer.apple.com/documentation/metal/mtlrendercommandencoder/1515527-setviewport
    POMDOG_ASSERT((0.0f <= viewportIn.minDepth) && (viewportIn.minDepth <= 1.0f));
    POMDOG_ASSERT((0.0f <= viewportIn.maxDepth) && (viewportIn.maxDepth <= 1.0f));

    MTLViewport viewport;
    viewport.originX = viewportIn.topLeftX;
    viewport.originY = viewportIn.topLeftY;
    viewport.width = viewportIn.width;
    viewport.height = viewportIn.height;
    viewport.znear = static_cast<double>(viewportIn.minDepth);
    viewport.zfar = static_cast<double>(viewportIn.maxDepth);
    [commandEncoder setViewport:viewport];
}

void SetScissorRectangle(
    id<MTLRenderCommandEncoder> commandEncoder, const Rectangle& rectangle)
{
    POMDOG_ASSERT(rectangle.width >= 0);
    POMDOG_ASSERT(rectangle.height >= 0);
    POMDOG_ASSERT(commandEncoder != nullptr);

    MTLScissorRect rect;
    rect.x = rectangle.x;
    rect.y = rectangle.y;
    rect.width = rectangle.width;
    rect.height = rectangle.height;
    [commandEncoder setScissorRect:rect];
}

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
void CheckUnbindingRenderTargetsError(
    const std::vector<std::weak_ptr<RenderTarget2D>>& renderTargets,
    const std::vector<std::weak_ptr<Texture>>& textures)
{
    for (auto& renderTarget : renderTargets) {
        for (auto& texture : textures) {
            if (!renderTarget.expired() && !texture.expired()) {
                POMDOG_ASSERT(renderTarget.lock() != texture.lock());
            }
        }
    }
}
#endif

} // namespace

GraphicsContextMetal::GraphicsContextMetal(id<MTLDevice> nativeDevice)
{
    POMDOG_ASSERT(nativeDevice != nullptr);

    // NOTE: The max number of command buffers in flight.
    //       Pomdog's metal renderer uses triple buffering.
    constexpr NSUInteger inflightBufferCount = 3;

    // NOTE: Create semaphore for Metal
    inflightSemaphore_ = dispatch_semaphore_create(inflightBufferCount);

    // NOTE: Create a new command queue
    commandQueue_ = [nativeDevice newCommandQueue];

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    const auto graphicsCapbilities = getCapabilities();

    POMDOG_ASSERT(graphicsCapbilities.SamplerSlotCount > 0);
    weakTextures_.resize(graphicsCapbilities.SamplerSlotCount);
#endif
}

GraphicsContextMetal::~GraphicsContextMetal() = default;

GraphicsCapabilities GraphicsContextMetal::getCapabilities() const noexcept
{
    // NOTE: For more information, please see:
    // https://developer.apple.com/library/ios/documentation/Miscellaneous/Conceptual/MetalProgrammingGuide/MetalFeatureSetTables/MetalFeatureSetTables.html
    GraphicsCapabilities caps;
    caps.ConstantBufferSlotCount = 31;
    caps.SamplerSlotCount = 16;
    return caps;
}

void GraphicsContextMetal::dispatchSemaphoreWait()
{
    if (!isDrawing_) {
        // NOTE: Skip waiting
        return;
    }

    dispatch_semaphore_wait(inflightSemaphore_, DISPATCH_TIME_FOREVER);
    isDrawing_ = false;
}

void GraphicsContextMetal::setMTKView(MTKView* view)
{
    POMDOG_ASSERT(view != nullptr);
    targetView_ = view;
}

void GraphicsContextMetal::executeCommandLists(
    std::span<std::shared_ptr<CommandListImmediate>> commandLists)
{
    POMDOG_ASSERT(commandQueue_ != nullptr);
    POMDOG_ASSERT(targetView_ != nullptr);
    POMDOG_ASSERT(commandBuffer_ == nullptr);
    POMDOG_ASSERT(commandEncoder_ == nullptr);

    // NOTE: Create a new command buffer for each renderpass to the current drawable
    commandBuffer_ = [commandQueue_ commandBuffer];
    commandBuffer_.label = @"PomdogCommand";

    // Call the view's completion handler which is required by the view
    // since it will signal its semaphore and set up the next buffer
    __block dispatch_semaphore_t blockSema = inflightSemaphore_;
    [commandBuffer_ addCompletedHandler:^(id<MTLCommandBuffer> buffer) {
        dispatch_semaphore_signal(blockSema);
    }];
    isDrawing_ = true;

    POMDOG_ASSERT(commandBuffer_ != nullptr);

    // NOTE: Skip rendering when the graphics device is lost.
    const bool skipRender = (targetView_.currentDrawable.texture.pixelFormat == MTLPixelFormatInvalid);

    if (!skipRender) {
        for (auto& commandList : commandLists) {
            POMDOG_ASSERT(commandList);
            commandList->executeImmediate(*this);
        }
    }

    POMDOG_ASSERT(commandEncoder_ == nullptr);
    POMDOG_ASSERT(commandBuffer_ != nullptr);

    if (auto drawable = targetView_.currentDrawable; drawable != nullptr) {
        // Schedule a present once the framebuffer is complete using the current drawable
        [commandBuffer_ presentDrawable:drawable];
    }

    // Finalize rendering here & push the command buffer to the GPU
    [commandBuffer_ commit];
    commandBuffer_ = nullptr;
}

void GraphicsContextMetal::present()
{
    // NOTE: commandBuffer::commit() has already been called at GraphicsContextMetal::ExecuteCommandLists().
}

void GraphicsContextMetal::draw(
    std::uint32_t vertexCount,
    std::uint32_t startVertexLocation)
{
    POMDOG_ASSERT(commandEncoder_ != nullptr);
    POMDOG_ASSERT(vertexCount > 0);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets_, weakTextures_);
#endif

    [commandEncoder_ drawPrimitives:primitiveType_
                        vertexStart:startVertexLocation
                        vertexCount:vertexCount];
}

void GraphicsContextMetal::drawIndexed(
    std::uint32_t indexCount,
    std::uint32_t startIndexLocation)
{
    POMDOG_ASSERT(commandEncoder_ != nullptr);
    POMDOG_ASSERT(indexCount > 0);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets_, weakTextures_);
#endif
    const auto indexBufferOffset = startIndexLocation * ToIndexByteSize(indexType_);

    [commandEncoder_ drawIndexedPrimitives:primitiveType_
                                indexCount:indexCount
                                 indexType:indexType_
                               indexBuffer:indexBuffer_
                         indexBufferOffset:indexBufferOffset];
}

void GraphicsContextMetal::drawInstanced(
    std::uint32_t vertexCountPerInstance,
    std::uint32_t instanceCount,
    std::uint32_t startVertexLocation,
    std::uint32_t startInstanceLocation)
{
    POMDOG_ASSERT(commandEncoder_ != nullptr);
    POMDOG_ASSERT(vertexCountPerInstance > 0);
    POMDOG_ASSERT(instanceCount > 0);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets_, weakTextures_);
#endif

    [commandEncoder_ drawPrimitives:primitiveType_
                        vertexStart:startVertexLocation
                        vertexCount:vertexCountPerInstance
                      instanceCount:instanceCount
                       baseInstance:startInstanceLocation];
}

void GraphicsContextMetal::drawIndexedInstanced(
    std::uint32_t indexCountPerInstance,
    std::uint32_t instanceCount,
    std::uint32_t startIndexLocation,
    std::uint32_t startInstanceLocation)
{
    POMDOG_ASSERT(commandEncoder_ != nullptr);
    POMDOG_ASSERT(indexCountPerInstance > 0);
    POMDOG_ASSERT(instanceCount > 0);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets_, weakTextures_);
#endif
    const auto indexBufferOffset = startIndexLocation * ToIndexByteSize(indexType_);

    [commandEncoder_ drawIndexedPrimitives:primitiveType_
                                indexCount:indexCountPerInstance
                                 indexType:indexType_
                               indexBuffer:indexBuffer_
                         indexBufferOffset:indexBufferOffset
                             instanceCount:instanceCount
                                baseVertex:0
                              baseInstance:startInstanceLocation];
}

void GraphicsContextMetal::setViewport(const Viewport& viewport)
{
    POMDOG_ASSERT(commandEncoder_ != nullptr);
    metal::SetViewport(commandEncoder_, viewport);
}

void GraphicsContextMetal::setScissorRect(const Rectangle& scissorRect)
{
    POMDOG_ASSERT(commandEncoder_ != nullptr);
    SetScissorRectangle(commandEncoder_, scissorRect);
}

void GraphicsContextMetal::setBlendFactor(const Vector4& blendFactor)
{
    POMDOG_ASSERT(commandEncoder_ != nullptr);
    [commandEncoder_ setBlendColorRed:blendFactor.x green:blendFactor.y blue:blendFactor.z alpha:blendFactor.w];
}

void GraphicsContextMetal::setVertexBuffer(
    std::uint32_t index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer,
    std::uint32_t offset)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(vertexBuffer != nullptr);
    POMDOG_ASSERT(vertexBuffer->getBuffer() != nullptr);
    POMDOG_ASSERT((offset % 256) == 0);

    const auto nativeVertexBuffer = StaticDownCast<BufferMetal>(vertexBuffer->getBuffer());
    POMDOG_ASSERT(nativeVertexBuffer != nullptr);
    POMDOG_ASSERT(nativeVertexBuffer->getBuffer() != nullptr);

    const auto slotIndex = index + VertexBufferSlotOffset;
    POMDOG_ASSERT(slotIndex < MaxVertexBufferSlotCount);

    [commandEncoder_ setVertexBuffer:nativeVertexBuffer->getBuffer()
                              offset:offset
                             atIndex:slotIndex];
}

void GraphicsContextMetal::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBufferIn)
{
    POMDOG_ASSERT(indexBufferIn != nullptr);

    const auto nativeIndexBuffer = StaticDownCast<BufferMetal>(indexBufferIn->getBuffer());
    POMDOG_ASSERT(nativeIndexBuffer != nullptr);

    indexType_ = ToIndexType(indexBufferIn->getElementSize());
    indexBuffer_ = nativeIndexBuffer->getBuffer();
}

void GraphicsContextMetal::setPipelineState(const std::shared_ptr<PipelineState>& pipelineState)
{
    POMDOG_ASSERT(pipelineState != nullptr);

    const auto nativePipelineState = StaticDownCast<PipelineStateMetal>(pipelineState.get());
    POMDOG_ASSERT(nativePipelineState != nullptr);

    primitiveType_ = nativePipelineState->GetPrimitiveType();

    POMDOG_ASSERT(commandEncoder_ != nullptr);
    nativePipelineState->Apply(commandEncoder_);
}

void GraphicsContextMetal::setConstantBuffer(
    std::uint32_t index,
    const std::shared_ptr<Buffer>& constantBufferIn,
    std::uint32_t offset,
    [[maybe_unused]] std::uint32_t sizeInBytes)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(constantBufferIn);
    POMDOG_ASSERT(offset >= 0);
    POMDOG_ASSERT(sizeInBytes > 0);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    static const auto capabilities = getCapabilities();
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(capabilities.ConstantBufferSlotCount));
#endif

    const auto constantBuffer = StaticDownCast<BufferMetal>(constantBufferIn.get());
    POMDOG_ASSERT(constantBuffer != nullptr);

    POMDOG_ASSERT(constantBuffer->getBuffer() != nullptr);
    [commandEncoder_ setVertexBuffer:constantBuffer->getBuffer()
                              offset:offset
                             atIndex:index];
    [commandEncoder_ setFragmentBuffer:constantBuffer->getBuffer()
                                offset:offset
                               atIndex:index];
}

void GraphicsContextMetal::setSampler(std::uint32_t index, const std::shared_ptr<SamplerState>& sampler)
{
    POMDOG_ASSERT(sampler != nullptr);
    POMDOG_ASSERT(index >= 0);

    const auto samplerStateMetal = StaticDownCast<SamplerStateMetal>(sampler.get());
    POMDOG_ASSERT(samplerStateMetal != nullptr);
    POMDOG_ASSERT(samplerStateMetal->GetSamplerState() != nullptr);

    POMDOG_ASSERT(commandEncoder_ != nullptr);
    [commandEncoder_ setVertexSamplerState:samplerStateMetal->GetSamplerState() atIndex:index];
    [commandEncoder_ setFragmentSamplerState:samplerStateMetal->GetSamplerState() atIndex:index];
}

void GraphicsContextMetal::setTexture(std::uint32_t index)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(commandEncoder_ != nullptr);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures_.empty());
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(weakTextures_.size()));
    weakTextures_[index].reset();
#endif

    [commandEncoder_ setVertexTexture:nullptr
                              atIndex:index];
    [commandEncoder_ setFragmentTexture:nullptr atIndex:index];
}

void GraphicsContextMetal::setTexture(std::uint32_t index, const std::shared_ptr<gpu::Texture2D>& textureIn)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(textureIn);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures_.empty());
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(weakTextures_.size()));
    weakTextures_[index] = textureIn;
#endif

    const auto textureMetal = StaticDownCast<Texture2DMetal>(textureIn.get());
    POMDOG_ASSERT(textureMetal != nullptr);
    POMDOG_ASSERT(textureMetal->getTexture() != nullptr);

    POMDOG_ASSERT(commandEncoder_ != nullptr);
    [commandEncoder_ setVertexTexture:textureMetal->getTexture() atIndex:index];
    [commandEncoder_ setFragmentTexture:textureMetal->getTexture() atIndex:index];
}

void GraphicsContextMetal::setTexture(std::uint32_t index, const std::shared_ptr<RenderTarget2D>& textureIn)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(textureIn);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures_.empty());
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(weakTextures_.size()));
    weakTextures_[index] = textureIn;
#endif

    const auto renderTargetMetal = StaticDownCast<RenderTarget2DMetal>(textureIn.get());
    POMDOG_ASSERT(renderTargetMetal != nullptr);
    POMDOG_ASSERT(renderTargetMetal->getTexture() != nullptr);

    POMDOG_ASSERT(commandEncoder_ != nullptr);
    [commandEncoder_ setVertexTexture:renderTargetMetal->getTexture() atIndex:index];
    [commandEncoder_ setFragmentTexture:renderTargetMetal->getTexture() atIndex:index];
}

void GraphicsContextMetal::beginRenderPass(const RenderPass& renderPass)
{
    POMDOG_ASSERT(!renderPass.renderTargets.empty());
    POMDOG_ASSERT(renderPass.renderTargets.size() <= 8);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    weakRenderTargets_.clear();
    for (auto& renderTarget : renderPass.renderTargets) {
        weakRenderTargets_.push_back(std::get<0>(renderTarget));
    }
#endif

    MTLRenderPassDescriptor* renderPassDescriptor = [[MTLRenderPassDescriptor alloc] init];
    POMDOG_ASSERT(renderPassDescriptor != nullptr);

    const bool useBackBuffer = (std::get<0>(renderPass.renderTargets.front()) == nullptr);

    const auto setClearColor = [&](int index, const std::optional<Vector4>& clearColor) {
        if (clearColor) {
            renderPassDescriptor.colorAttachments[index].loadAction = MTLLoadActionClear;
            renderPassDescriptor.colorAttachments[index].clearColor = ToClearColor(*clearColor);
        }
        else {
            renderPassDescriptor.colorAttachments[index].loadAction = MTLLoadActionDontCare;
        }
        renderPassDescriptor.colorAttachments[index].storeAction = MTLStoreActionStore;
    };

    if (useBackBuffer) {
        auto& renderTargetView = renderPass.renderTargets.front();
        auto& clearColor = std::get<1>(renderTargetView);
        POMDOG_ASSERT(std::get<0>(renderTargetView) == nullptr);

        constexpr int renderTargetIndex = 0;
        renderPassDescriptor.colorAttachments[renderTargetIndex].texture = targetView_.currentDrawable.texture;
        setClearColor(renderTargetIndex, clearColor);
    }
    else {
        int renderTargetIndex = 0;
        for (const auto& renderTargetView : renderPass.renderTargets) {
            auto& renderTarget = std::get<0>(renderTargetView);
            auto& clearColor = std::get<1>(renderTargetView);

            if (renderTarget == nullptr) {
                break;
            }
            const auto renderTargetMetal = StaticDownCast<RenderTarget2DMetal>(renderTarget.get());
            POMDOG_ASSERT(renderTargetMetal != nullptr);

            renderPassDescriptor.colorAttachments[renderTargetIndex].texture = renderTargetMetal->getTexture();

            setClearColor(renderTargetIndex, clearColor);
            ++renderTargetIndex;
        }
    }

    if (renderPass.depthStencilBuffer == nullptr) {
        renderPassDescriptor.depthAttachment.texture = targetView_.currentRenderPassDescriptor.depthAttachment.texture;
        renderPassDescriptor.stencilAttachment.texture = targetView_.currentRenderPassDescriptor.stencilAttachment.texture;
    }
    else {
        const auto depthStencilBuffer = StaticDownCast<DepthStencilBufferMetal>(renderPass.depthStencilBuffer.get());
        POMDOG_ASSERT(depthStencilBuffer != nullptr);

        renderPassDescriptor.depthAttachment.texture = depthStencilBuffer->getTexture();

        const bool isStencilRenderable = [&]() -> bool {
            switch ([depthStencilBuffer->getTexture() pixelFormat]) {
            case MTLPixelFormatStencil8:
            case MTLPixelFormatX24_Stencil8:
            case MTLPixelFormatX32_Stencil8:
            case MTLPixelFormatDepth24Unorm_Stencil8:
            case MTLPixelFormatDepth32Float_Stencil8:
                return true;
            default:
                break;
            }
            return false;
        }();

        if (isStencilRenderable) {
            renderPassDescriptor.stencilAttachment.texture = depthStencilBuffer->getTexture();
        }
        else {
            renderPassDescriptor.stencilAttachment.texture = nullptr;
        }
    }

    if (renderPassDescriptor.depthAttachment.texture != nullptr) {
        renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionLoad;
        renderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
    }
    if (renderPassDescriptor.stencilAttachment.texture != nullptr) {
        renderPassDescriptor.stencilAttachment.loadAction = MTLLoadActionLoad;
        renderPassDescriptor.stencilAttachment.storeAction = MTLStoreActionStore;
    }

    if (renderPass.clearDepth) {
        renderPassDescriptor.depthAttachment.loadAction = MTLLoadActionClear;
        renderPassDescriptor.depthAttachment.clearDepth = static_cast<double>(*renderPass.clearDepth);
    }
    if (renderPass.clearStencil) {
        renderPassDescriptor.stencilAttachment.loadAction = MTLLoadActionClear;
        renderPassDescriptor.stencilAttachment.clearStencil = *renderPass.clearStencil;
    }

    POMDOG_ASSERT(commandBuffer_ != nullptr);
    POMDOG_ASSERT(commandEncoder_ == nullptr);

    // NOTE: Create a render command encoder so we can render into something
    commandEncoder_ = [commandBuffer_ renderCommandEncoderWithDescriptor:renderPassDescriptor];
    commandEncoder_.label = @"PomdogRenderEncoder";
    [commandEncoder_ pushDebugGroup:@"PomdogDraw"];

    [commandEncoder_ setFrontFacingWinding:MTLWindingClockwise];

    if (renderPass.viewport) {
        metal::SetViewport(commandEncoder_, *renderPass.viewport);
    }
    if (renderPass.scissorRect) {
        SetScissorRectangle(commandEncoder_, *renderPass.scissorRect);
    }
}

void GraphicsContextMetal::endRenderPass()
{
    POMDOG_ASSERT(commandEncoder_ != nullptr);

    indexBuffer_ = nullptr;

    if (commandEncoder_ != nullptr) {
        // NOTE: We're done encoding commands
        [commandEncoder_ popDebugGroup];
        [commandEncoder_ endEncoding];
        commandEncoder_ = nullptr;
    }
}

} // namespace pomdog::gpu::detail::metal
