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

    void Clear(ClearOptions options, const Color& color, float depth, std::uint8_t stencil);

    void Draw(std::size_t vertexCount);

    void DrawIndexed(std::size_t indexCount);

    void DrawInstanced(
        std::size_t vertexCount,
        std::size_t instanceCount);

    void DrawIndexedInstanced(
        std::size_t indexCount,
        std::size_t instanceCount);

    void SetViewport(const Viewport& viewport);

    void SetScissorRectangle(const Rectangle& rectangle);

    void SetPrimitiveTopology(PrimitiveTopology primitiveTopology);

    void SetBlendFactor(const Color& blendFactor);

    void SetVertexBuffers(const std::vector<VertexBufferBinding>& vertexBuffers);

    void SetVertexBuffers(std::vector<VertexBufferBinding> && vertexBuffers);

    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

    void SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState);

    void SetConstantBuffer(int index, const std::shared_ptr<NativeBuffer>& constantBuffer);

    void SetSampler(int index, std::shared_ptr<NativeSamplerState> && sampler);

    void SetTexture(int index);

    void SetTexture(int index, const std::shared_ptr<Texture2D>& texture);

    void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture);

    void SetRenderTarget();

    void SetRenderTargets(const std::vector<std::shared_ptr<RenderTarget2D>>& renderTargets);

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets);

private:
};

} // namespace Vulkan
} // namespace Detail
} // namespace Pomdog
