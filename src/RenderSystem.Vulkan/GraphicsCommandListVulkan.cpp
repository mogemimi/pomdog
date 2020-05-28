// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "GraphicsCommandListVulkan.hpp"
#include "../RenderSystem.Vulkan/BufferVulkan.hpp"
#include "../RenderSystem.Vulkan/PipelineStateVulkan.hpp"
#include "../RenderSystem.Vulkan/RenderTarget2DVulkan.hpp"
#include "../RenderSystem/GraphicsCapabilities.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/RenderPass.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog::Detail::Vulkan {
namespace {

VkIndexType ToVkIndexType(IndexElementSize elementSize) noexcept
{
    switch (elementSize) {
    case IndexElementSize::SixteenBits:
        return VK_INDEX_TYPE_UINT16;
    case IndexElementSize::ThirtyTwoBits:
        return VK_INDEX_TYPE_UINT32;
    }
    return VK_INDEX_TYPE_UINT16;
}

} // namespace

GraphicsCommandListVulkan::GraphicsCommandListVulkan()
{
    // TODO: Not implemented
    commandBuffer = nullptr;

    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandListVulkan::Close()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandListVulkan::Reset()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

std::size_t GraphicsCommandListVulkan::GetCount() const noexcept
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandListVulkan::Draw(
    std::size_t vertexCount,
    std::size_t startVertexLocation)
{
    POMDOG_ASSERT(commandBuffer != nullptr);

    vkCmdDraw(
        commandBuffer,
        static_cast<uint32_t>(vertexCount),
        0,
        static_cast<uint32_t>(startVertexLocation),
        0);
}

void GraphicsCommandListVulkan::DrawIndexed(
    std::size_t indexCount,
    std::size_t startIndexLocation)
{
    POMDOG_ASSERT(commandBuffer != nullptr);

    vkCmdDrawIndexed(
        commandBuffer,
        static_cast<uint32_t>(indexCount),
        0,
        static_cast<uint32_t>(startIndexLocation),
        0,
        0);
}

void GraphicsCommandListVulkan::DrawInstanced(
    std::size_t vertexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startVertexLocation,
    std::size_t startInstanceLocation)
{
    POMDOG_ASSERT(commandBuffer != nullptr);

    vkCmdDraw(
        commandBuffer,
        static_cast<uint32_t>(vertexCountPerInstance),
        static_cast<uint32_t>(instanceCount),
        static_cast<uint32_t>(startVertexLocation),
        static_cast<uint32_t>(startInstanceLocation));
}

void GraphicsCommandListVulkan::DrawIndexedInstanced(
    std::size_t indexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startIndexLocation,
    std::size_t startInstanceLocation)
{
    POMDOG_ASSERT(commandBuffer != nullptr);

    vkCmdDrawIndexed(
        commandBuffer,
        static_cast<uint32_t>(indexCountPerInstance),
        static_cast<uint32_t>(instanceCount),
        static_cast<uint32_t>(startIndexLocation),
        0,
        static_cast<uint32_t>(startInstanceLocation));
}

void GraphicsCommandListVulkan::SetRenderPass(RenderPass&& renderPass)
{
    POMDOG_ASSERT(commandBuffer != nullptr);

    if (renderPass.Viewport) {
        const auto& viewportIn = *renderPass.Viewport;

        POMDOG_ASSERT(viewportIn.Width > 0);
        POMDOG_ASSERT(viewportIn.Height > 0);

        // NOTE: The MinDepth and MaxDepth must be between 0.0 and 1.0, respectively.
        // Please see https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkViewport.html
        POMDOG_ASSERT((0.0f <= viewportIn.MinDepth) && (viewportIn.MinDepth <= 1.0f));
        POMDOG_ASSERT((0.0f <= viewportIn.MaxDepth) && (viewportIn.MaxDepth <= 1.0f));

        VkViewport viewport;
        viewport.x = viewportIn.TopLeftX;
        viewport.y = viewportIn.TopLeftY;
        viewport.width = viewportIn.Width;
        viewport.height = viewportIn.Height;
        viewport.minDepth = viewportIn.MinDepth;
        viewport.maxDepth = viewportIn.MaxDepth;

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    }

    if (renderPass.ScissorRect) {
        const auto& rect = *renderPass.ScissorRect;

        POMDOG_ASSERT(rect.Width > 0);
        POMDOG_ASSERT(rect.Height > 0);

        VkRect2D scissorRect;
        scissorRect.offset.x = rect.X;
        scissorRect.offset.y = rect.Y;
        scissorRect.extent.width = rect.Width;
        scissorRect.extent.height = rect.Height;

        vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
    }

    std::array<VkClearAttachment, 9> clearAttachments;
    std::array<VkClearRect, 9> clearRects;

    const bool useBackBuffer = (std::get<0>(renderPass.RenderTargets.front()) == nullptr);

    const auto setClearColor = [](VkClearAttachment& attachment, std::uint32_t index, const Vector4& clearColor) {
        attachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        attachment.colorAttachment = index;
        attachment.clearValue.color.float32[0] = clearColor.X;
        attachment.clearValue.color.float32[1] = clearColor.Y;
        attachment.clearValue.color.float32[2] = clearColor.Z;
        attachment.clearValue.color.float32[3] = clearColor.W;
    };

    // FIXME: Not implemented yet.
    POMDOG_ASSERT(false);
    const std::uint32_t renderTargetWidth = 1;
    const std::uint32_t renderTargetHeight = 1;

    std::uint32_t attachmentCount = 0;

    if (useBackBuffer) {
        auto& renderTargetView = renderPass.RenderTargets.front();
        auto& clearColor = std::get<1>(renderTargetView);
        POMDOG_ASSERT(std::get<0>(renderTargetView) == nullptr);

        constexpr std::uint32_t renderTargetIndex = 0;
        if (clearColor) {
            auto& clearAttachment = clearAttachments[renderTargetIndex];
            setClearColor(clearAttachment, renderTargetIndex, *clearColor);

            auto& rect = clearRects[renderTargetIndex];
            rect.rect.offset.x = 0;
            rect.rect.offset.y = 0;
            rect.rect.extent.width = renderTargetWidth;
            rect.rect.extent.height = renderTargetHeight;
            rect.baseArrayLayer = 0;
            rect.layerCount = 1;
        }

        attachmentCount = 1;
    }
    else {
        std::uint32_t renderTargetIndex = 0;
        for (const auto& renderTargetView : renderPass.RenderTargets) {
            auto& renderTarget = std::get<0>(renderTargetView);
            auto& clearColor = std::get<1>(renderTargetView);

            if (renderTarget == nullptr) {
                break;
            }
            auto nativeRenderTarget = static_cast<RenderTarget2DVulkan*>(renderTarget->GetNativeRenderTarget2D());
            POMDOG_ASSERT(nativeRenderTarget == dynamic_cast<RenderTarget2DVulkan*>(renderTarget->GetNativeRenderTarget2D()));
            POMDOG_ASSERT(nativeRenderTarget != nullptr);

            if (clearColor) {
                auto& clearAttachment = clearAttachments[renderTargetIndex];
                setClearColor(clearAttachment, renderTargetIndex, *clearColor);

                auto& rect = clearRects[renderTargetIndex];
                rect.rect.offset.x = 0;
                rect.rect.offset.y = 0;
                rect.rect.extent.width = renderTargetWidth;
                rect.rect.extent.height = renderTargetHeight;
                rect.baseArrayLayer = 0;
                rect.layerCount = 1;
            }

            ++renderTargetIndex;
        }

        attachmentCount = renderTargetIndex;
    }

    {
        auto& renderTargetView = renderPass.RenderTargets.front();
        auto& renderTarget = std::get<0>(renderTargetView);

        const auto depthStencilBufferIndex = attachmentCount;
        POMDOG_ASSERT(depthStencilBufferIndex < clearAttachments.size());
        POMDOG_ASSERT(depthStencilBufferIndex < clearRects.size());

        // FIXME: Not implemented yet.
        POMDOG_ASSERT(false);
        const bool useDepthStencilView = true;

        if (useDepthStencilView) {
            if (!renderTarget) {
                // NOTE: Use back buffer
                auto& rect = clearRects[depthStencilBufferIndex];
                rect.rect.offset.x = 0;
                rect.rect.offset.y = 0;
                rect.rect.extent.width = renderTargetWidth;
                rect.rect.extent.height = renderTargetHeight;
                rect.baseArrayLayer = 0;
                rect.layerCount = 1;
            }
            else {
                // NOTE: Use render target texture
                auto& rect = clearRects[depthStencilBufferIndex];
                rect.rect.offset.x = 0;
                rect.rect.offset.y = 0;
                rect.rect.extent.width = renderTargetWidth;
                rect.rect.extent.height = renderTargetHeight;
                rect.baseArrayLayer = 0;
                rect.layerCount = 1;
            }

            auto& attachment = clearAttachments[depthStencilBufferIndex];
            attachment.aspectMask = 0;
            attachment.colorAttachment = depthStencilBufferIndex;

            if (renderPass.ClearDepth) {
                attachment.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
                attachment.clearValue.depthStencil.depth = *renderPass.ClearDepth;
            }
            if (renderPass.ClearStencil) {
                attachment.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
                attachment.clearValue.depthStencil.stencil = *renderPass.ClearStencil;
            }

            attachmentCount += 1;
        }
    }

    vkCmdClearAttachments(
        commandBuffer,
        attachmentCount,
        clearAttachments.data(),
        attachmentCount,
        clearRects.data());
}

void GraphicsCommandListVulkan::SetViewport(const Viewport& viewportIn)
{
    POMDOG_ASSERT(commandBuffer != nullptr);

    POMDOG_ASSERT(viewportIn.Width > 0);
    POMDOG_ASSERT(viewportIn.Height > 0);

    VkViewport viewport;
    viewport.x = viewportIn.TopLeftX;
    viewport.y = viewportIn.TopLeftY;
    viewport.width = viewportIn.Width;
    viewport.height = viewportIn.Height;
    viewport.minDepth = viewportIn.MinDepth;
    viewport.maxDepth = viewportIn.MaxDepth;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
}

void GraphicsCommandListVulkan::SetScissorRect(const Rectangle& rect)
{
    POMDOG_ASSERT(commandBuffer != nullptr);

    POMDOG_ASSERT(rect.Width > 0);
    POMDOG_ASSERT(rect.Height > 0);

    VkRect2D scissorRect;
    scissorRect.offset.x = rect.X;
    scissorRect.offset.y = rect.Y;
    scissorRect.extent.width = rect.Width;
    scissorRect.extent.height = rect.Height;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
}

void GraphicsCommandListVulkan::SetBlendFactor(const Vector4& blendFactor)
{
    POMDOG_ASSERT(commandBuffer != nullptr);
    vkCmdSetBlendConstants(commandBuffer, blendFactor.Data());
}

void GraphicsCommandListVulkan::SetVertexBuffer(
    int index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer,
    std::size_t offset)
{
    POMDOG_ASSERT(commandBuffer != nullptr);
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(vertexBuffer != nullptr);
    POMDOG_ASSERT(vertexBuffer->GetNativeVertexBuffer() != nullptr);
    POMDOG_ASSERT((offset % 256) == 0);

    auto nativeVertexBuffer = static_cast<BufferVulkan*>(
        vertexBuffer->GetNativeVertexBuffer());

    POMDOG_ASSERT(nativeVertexBuffer != nullptr);
    POMDOG_ASSERT(nativeVertexBuffer == dynamic_cast<BufferVulkan*>(vertexBuffer->GetNativeVertexBuffer()));
    POMDOG_ASSERT(nativeVertexBuffer->GetBuffer() != nullptr);

    const auto slotIndex = static_cast<std::uint32_t>(index);
    std::array<VkBuffer, 1> vertexBuffers = {nativeVertexBuffer->GetBuffer()};
    std::array<VkDeviceSize, 1> offsets = {offset};

    vkCmdBindVertexBuffers(commandBuffer, slotIndex, 1, vertexBuffers.data(), offsets.data());
}

void GraphicsCommandListVulkan::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBufferIn)
{
    POMDOG_ASSERT(commandBuffer != nullptr);
    POMDOG_ASSERT(indexBufferIn != nullptr);

    const auto nativeIndexBuffer = static_cast<BufferVulkan*>(indexBufferIn->GetNativeIndexBuffer());

    POMDOG_ASSERT(nativeIndexBuffer != nullptr);
    POMDOG_ASSERT(nativeIndexBuffer == dynamic_cast<BufferVulkan*>(indexBufferIn->GetNativeIndexBuffer()));

    constexpr VkDeviceSize offset = 0;
    const auto indexBuffer = nativeIndexBuffer->GetBuffer();
    const auto indexType = ToVkIndexType(indexBufferIn->GetElementSize());

    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, offset, indexType);
}

void GraphicsCommandListVulkan::SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandListVulkan::SetConstantBuffer(
    int index,
    const std::shared_ptr<NativeBuffer>& constantBuffer,
    std::size_t offset,
    std::size_t sizeInBytes)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandListVulkan::SetSampler(int index, std::shared_ptr<NativeSamplerState>&& sampler)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandListVulkan::SetTexture(int index)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandListVulkan::SetTexture(int index, const std::shared_ptr<Texture2D>& texture)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandListVulkan::SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

} // namespace Pomdog::Detail::Vulkan
