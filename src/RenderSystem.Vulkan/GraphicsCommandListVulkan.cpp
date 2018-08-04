// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GraphicsCommandListVulkan.hpp"
#include "../RenderSystem.Vulkan/BufferVulkan.hpp"
#include "../RenderSystem.Vulkan/PipelineStateVulkan.hpp"
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
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
namespace Detail {
namespace Vulkan {
namespace {

VkPrimitiveTopology ToVkPrimitiveTopology(
    PrimitiveTopology primitiveTopology) noexcept
{
    switch (primitiveTopology) {
    case PrimitiveTopology::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    case PrimitiveTopology::TriangleList: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case PrimitiveTopology::LineList: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case PrimitiveTopology::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    }
    return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

VkIndexType ToVkIndexType(IndexElementSize elementSize) noexcept
{
    switch (elementSize) {
    case IndexElementSize::SixteenBits: return VK_INDEX_TYPE_UINT16;
    case IndexElementSize::ThirtyTwoBits: return VK_INDEX_TYPE_UINT32;
    }
    return VK_INDEX_TYPE_UINT16;
}

} // unnamed namespace

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

void GraphicsCommandListVulkan::SetRenderPass(RenderPass && renderPass)
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
        const auto& rectangle = *renderPass.ScissorRect;

        POMDOG_ASSERT(rectangle.Width > 0);
        POMDOG_ASSERT(rectangle.Height > 0);

        VkRect2D scissorRect;
        scissorRect.offset.x = rectangle.X;
        scissorRect.offset.y = rectangle.Y;
        scissorRect.extent.width = rectangle.Width;
        scissorRect.extent.height = rectangle.Height;

        vkCmdSetScissor(commandBuffer, 0, 1, &scissorRect);
    }

    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandListVulkan::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandListVulkan::SetBlendFactor(const Color& blendFactor)
{
    POMDOG_ASSERT(commandBuffer != nullptr);
    const auto colorVector = blendFactor.ToVector4();
    vkCmdSetBlendConstants(commandBuffer, colorVector.Data());
}

void GraphicsCommandListVulkan::SetVertexBuffers(const std::vector<VertexBufferBinding>& vertexBuffers)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandListVulkan::SetVertexBuffers(std::vector<VertexBufferBinding> && vertexBuffers)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandListVulkan::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBufferIn)
{
    POMDOG_ASSERT(commandBuffer != nullptr);
    POMDOG_ASSERT(indexBufferIn != nullptr);

    const auto nativeIndexBuffer = static_cast<BufferVulkan*>(indexBufferIn->NativeIndexBuffer());

    POMDOG_ASSERT(nativeIndexBuffer != nullptr);
    POMDOG_ASSERT(nativeIndexBuffer == dynamic_cast<BufferVulkan*>(indexBufferIn->NativeIndexBuffer()));

    constexpr VkDeviceSize offset = 0;
    const auto indexBuffer = nativeIndexBuffer->GetBuffer();
    const auto indexType = ToVkIndexType(indexBufferIn->GetElementSize());

    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, offset, indexType);

    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandListVulkan::SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandListVulkan::SetConstantBuffer(int index, const std::shared_ptr<NativeBuffer>& constantBuffer)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

void GraphicsCommandListVulkan::SetSampler(int index, std::shared_ptr<NativeSamplerState> && sampler)
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

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
