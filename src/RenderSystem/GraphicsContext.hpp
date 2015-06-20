// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GRAPHICSCONTEXT_F873DD34_HPP
#define POMDOG_GRAPHICSCONTEXT_F873DD34_HPP

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
    GraphicsContext(GraphicsContext const&) = delete;
    GraphicsContext & operator=(GraphicsContext const&) = delete;

    GraphicsContext(
        std::unique_ptr<Detail::NativeGraphicsContext> && nativeContext,
        PresentationParameters const& presentationParameters);

    ~GraphicsContext();

    void Clear(Color const& color);

    void Clear(ClearOptions options, Color const& color, float depth, std::uint8_t stencil);

    void Present();

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

    void SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers);

    void SetIndexBuffer(std::shared_ptr<IndexBuffer> const& indexBuffer);

    void SetPipelineState(std::shared_ptr<Detail::NativePipelineState> const& pipelineState);

    void SetConstantBuffers(std::shared_ptr<Detail::NativeConstantLayout> const& constantBuffers);

    void SetSampler(int index, std::shared_ptr<Detail::NativeSamplerState> const& sampler);

    void SetTexture(int index);

    void SetTexture(int index, std::shared_ptr<Texture2D> const& texture);

    void SetTexture(int index, std::shared_ptr<RenderTarget2D> const& texture);

    void SetRenderTarget();

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets);

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets);

    Detail::NativeGraphicsContext* GetNativeGraphicsContext();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_GRAPHICSCONTEXT_F873DD34_HPP
