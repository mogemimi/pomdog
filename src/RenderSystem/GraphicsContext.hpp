// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>

namespace Pomdog {
namespace Detail {

class NativeGraphicsContext;

class GraphicsContext {
public:
    GraphicsContext() = delete;
    GraphicsContext(const GraphicsContext&) = delete;
    GraphicsContext & operator=(const GraphicsContext&) = delete;

    GraphicsContext(
        std::unique_ptr<Detail::NativeGraphicsContext> && nativeContext,
        const PresentationParameters& presentationParameters);

    ~GraphicsContext();

    void Clear(const Color& color);

    void Clear(ClearOptions options, const Color& color, float depth, std::uint8_t stencil);

    void Present();

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

    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

    void SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState);

    void SetConstantBuffer(int index, const std::shared_ptr<NativeBuffer>& constantBuffer);

    void SetSampler(int index, const std::shared_ptr<NativeSamplerState>& sampler);

    void SetTexture(int index);

    void SetTexture(int index, const std::shared_ptr<Texture2D>& texture);

    void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture);

    void SetRenderTarget();

    void SetRenderTargets(const std::vector<std::shared_ptr<RenderTarget2D>>& renderTargets);

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets);

    Detail::NativeGraphicsContext* GetNativeGraphicsContext();

private:
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<std::shared_ptr<RenderTarget2D>> renderTargets;
    std::unique_ptr<Detail::NativeGraphicsContext> nativeContext;
    DepthFormat depthStencilFormat;
};

} // namespace Detail
} // namespace Pomdog
