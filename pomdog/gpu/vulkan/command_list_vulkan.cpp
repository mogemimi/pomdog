// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/vulkan/command_list_vulkan.h"
#include "pomdog/gpu/backends/graphics_capabilities.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/render_pass.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/gpu/vulkan/buffer_vulkan.h"
#include "pomdog/gpu/vulkan/pipeline_state_vulkan.h"
#include "pomdog/gpu/vulkan/render_target2d_vulkan.h"
#include "pomdog/math/color.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/math/vector4.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/exception.h"

namespace pomdog::gpu::detail::vulkan {
namespace {

[[nodiscard]] VkIndexType ToVkIndexType(IndexFormat elementSize) noexcept
{
    switch (elementSize) {
    case IndexFormat::UInt16:
        return VK_INDEX_TYPE_UINT16;
    case IndexFormat::UInt32:
        return VK_INDEX_TYPE_UINT32;
    }
    return VK_INDEX_TYPE_UINT16;
}

} // namespace

CommandListVulkan::CommandListVulkan()
{
    // TODO: Not implemented
    commandBuffer = nullptr;

    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void CommandListVulkan::Close()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void CommandListVulkan::Reset()
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

std::size_t CommandListVulkan::GetCount() const noexcept
{
    // FIXME: Not implemented yet.
    return 0;
}

void CommandListVulkan::Draw(
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

void CommandListVulkan::DrawIndexed(
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

void CommandListVulkan::DrawInstanced(
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

void CommandListVulkan::DrawIndexedInstanced(
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

void CommandListVulkan::SetRenderPass(RenderPass&& renderPass)
{
    POMDOG_ASSERT(commandBuffer != nullptr);

    if (renderPass.Viewport) {
        const auto& viewportIn = *renderPass.Viewport;

        POMDOG_ASSERT(viewportIn.width > 0);
        POMDOG_ASSERT(viewportIn.height > 0);

        // NOTE: The MinDepth and MaxDepth must be between 0.0 and 1.0, respectively.
        // Please see https://www.khronos.org/registry/vulkan/specs/1.0/man/html/VkViewport.html
        POMDOG_ASSERT((0.0f <= viewportIn.MinDepth) && (viewportIn.MinDepth <= 1.0f));
        POMDOG_ASSERT((0.0f <= viewportIn.MaxDepth) && (viewportIn.MaxDepth <= 1.0f));

        VkViewport viewport;
        viewport.x = viewportIn.topLeftX;
        viewport.y = viewportIn.topLeftY;
        viewport.width = viewportIn.width;
        viewport.height = viewportIn.height;
        viewport.minDepth = viewportIn.minDepth;
        viewport.maxDepth = viewportIn.maxDepth;

        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    }

    if (renderPass.ScissorRect) {
        const auto& rect = *renderPass.ScissorRect;

        POMDOG_ASSERT(rect.width > 0);
        POMDOG_ASSERT(rect.height > 0);

        VkRect2D scissorRect;
        scissorRect.offset.x = rect.x;
        scissorRect.offset.y = rect.y;
        scissorRect.extent.width = rect.width;
        scissorRect.extent.height = rect.height;

        vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
    }

    std::array<VkClearAttachment, 9> clearAttachments;
    std::array<VkClearRect, 9> clearRects;

    const bool useBackBuffer = (std::get<0>(renderPass.RenderTargets.front()) == nullptr);

    const auto setClearColor = [](VkClearAttachment& attachment, std::uint32_t index, const Vector4& clearColor) {
        attachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        attachment.colorAttachment = index;
        attachment.clearValue.color.float32[0] = clearColor.x;
        attachment.clearValue.color.float32[1] = clearColor.y;
        attachment.clearValue.color.float32[2] = clearColor.z;
        attachment.clearValue.color.float32[3] = clearColor.w;
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

void CommandListVulkan::SetViewport(const Viewport& viewportIn)
{
    POMDOG_ASSERT(commandBuffer != nullptr);

    POMDOG_ASSERT(viewportIn.width > 0);
    POMDOG_ASSERT(viewportIn.height > 0);

    VkViewport viewport;
    viewport.x = viewportIn.topLeftX;
    viewport.y = viewportIn.topLeftY;
    viewport.width = viewportIn.width;
    viewport.height = viewportIn.height;
    viewport.minDepth = viewportIn.minDepth;
    viewport.maxDepth = viewportIn.maxDepth;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
}

void CommandListVulkan::SetScissorRect(const Rectangle& rect)
{
    POMDOG_ASSERT(commandBuffer != nullptr);

    POMDOG_ASSERT(rect.width > 0);
    POMDOG_ASSERT(rect.height > 0);

    VkRect2D scissorRect;
    scissorRect.offset.x = rect.x;
    scissorRect.offset.y = rect.y;
    scissorRect.extent.width = rect.width;
    scissorRect.extent.height = rect.height;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
}

void CommandListVulkan::SetBlendFactor(const Vector4& blendFactor)
{
    POMDOG_ASSERT(commandBuffer != nullptr);
    vkCmdSetBlendConstants(commandBuffer, blendFactor.Data());
}

void CommandListVulkan::SetVertexBuffer(
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

void CommandListVulkan::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBufferIn)
{
    POMDOG_ASSERT(commandBuffer != nullptr);
    POMDOG_ASSERT(indexBufferIn != nullptr);

    const auto nativeIndexBuffer = static_cast<BufferVulkan*>(indexBufferIn->GetNativeIndexBuffer());

    POMDOG_ASSERT(nativeIndexBuffer != nullptr);
    POMDOG_ASSERT(nativeIndexBuffer == dynamic_cast<BufferVulkan*>(indexBufferIn->GetNativeIndexBuffer()));

    constexpr VkDeviceSize offset = 0;
    const auto indexBuffer = nativeIndexBuffer->GetBuffer();
    const auto indexType = ToVkIndexType(indexBufferIn->getElementSize());

    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, offset, indexType);
}

void CommandListVulkan::SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void CommandListVulkan::SetConstantBuffer(
    int index,
    const std::shared_ptr<Buffer>& constantBuffer,
    std::size_t offset,
    std::size_t sizeInBytes)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void CommandListVulkan::SetSampler(int index, std::shared_ptr<NativeSamplerState>&& sampler)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void CommandListVulkan::SetTexture(int index)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void CommandListVulkan::SetTexture(int index, const std::shared_ptr<gpu::Texture2D>& texture)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void CommandListVulkan::SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

} // namespace pomdog::gpu::detail::vulkan
