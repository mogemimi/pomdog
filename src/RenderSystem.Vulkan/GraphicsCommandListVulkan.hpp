// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeGraphicsCommandList.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <vulkan/vulkan.h>

namespace Pomdog {
namespace Detail {
namespace Vulkan {

class GraphicsCommandListVulkan final : public NativeGraphicsCommandList {
public:
    void Close();

    void Reset();

    std::size_t GetCount() const noexcept;

    void Clear(ClearOptions options, Color const& color, float depth, std::uint8_t stencil);

    void Draw(std::size_t vertexCount);

    void DrawIndexed(std::size_t indexCount);

    void DrawInstanced(
        std::size_t vertexCount,
        std::size_t instanceCount);

    void DrawIndexedInstanced(
        std::size_t indexCount,
        std::size_t instanceCount);

    void SetViewport(Viewport const& viewport);

    void SetScissorRectangle(Rectangle const& rectangle);

    void SetPrimitiveTopology(PrimitiveTopology primitiveTopology);

    void SetBlendFactor(Color const& blendFactor);

    void SetVertexBuffers(std::vector<VertexBufferBinding> const& vertexBuffers);

    void SetVertexBuffers(std::vector<VertexBufferBinding> && vertexBuffers);

    void SetIndexBuffer(std::shared_ptr<IndexBuffer> const& indexBuffer);

    void SetPipelineState(std::shared_ptr<NativePipelineState> const& pipelineState);

    void SetConstantBuffer(int index, std::shared_ptr<NativeBuffer> const& constantBuffer);

    void SetSampler(int index, std::shared_ptr<NativeSamplerState> && sampler);

    void SetTexture(int index);

    void SetTexture(int index, std::shared_ptr<Texture2D> const& texture);

    void SetTexture(int index, std::shared_ptr<RenderTarget2D> const& texture);

    void SetRenderTarget();

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets);

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets);

private:
};

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
