// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GRAPHICSCONTEXT_F873DD34_HPP
#define POMDOG_GRAPHICSCONTEXT_F873DD34_HPP

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>

namespace Pomdog {

class POMDOG_EXPORT GraphicsContext {
public:
    GraphicsContext() = delete;
    GraphicsContext(GraphicsContext const&) = delete;
    GraphicsContext & operator=(GraphicsContext const&) = delete;

    GraphicsContext(std::unique_ptr<Detail::NativeGraphicsContext> && nativeContext,
        PresentationParameters const& presentationParameters,
        std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    ~GraphicsContext();

    void Clear(Color const& color);

    void Clear(ClearOptions options, Color const& color, float depth, std::uint8_t stencil);

    void Present();

    void Draw(std::size_t vertexCount);

    void DrawIndexed(
        std::shared_ptr<IndexBuffer> const& indexBuffer,
        std::size_t indexCount);

    void DrawInstanced(
        std::size_t vertexCount,
        std::size_t instanceCount);

    void DrawIndexedInstanced(
        std::shared_ptr<IndexBuffer> const& indexBuffer,
        std::size_t indexCount,
        std::size_t instanceCount);

    Viewport GetViewport() const;

    void SetViewport(Viewport const& viewport);

    Rectangle GetScissorRectangle() const;

    void SetScissorRectangle(Rectangle const& rectangle);

    void SetPrimitiveTopology(PrimitiveTopology primitiveTopology);

    void SetBlendFactor(Color const& blendFactor);

    void SetSamplerState(int index, std::shared_ptr<SamplerState> const& samplerState);

    void SetVertexBuffer(std::shared_ptr<VertexBuffer> const& vertexBuffer);

    void SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers);

    void SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> && vertexBuffers);

    std::shared_ptr<Texture> GetTexture(int index) const;

    void SetTexture(int index);

    void SetTexture(int index, std::shared_ptr<Texture2D> const& texture);

    void SetTexture(int index, std::shared_ptr<RenderTarget2D> const& texture);

    std::vector<std::shared_ptr<RenderTarget2D>> GetRenderTargets() const;

    void SetRenderTarget();

    void SetRenderTarget(std::shared_ptr<RenderTarget2D> const& renderTarget);

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets);

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> && renderTargets);

    void SetPipelineState(std::shared_ptr<PipelineState> const& pipelineState);

    void SetConstantBuffers(std::shared_ptr<ConstantBufferBinding> const& constantBuffers);

public:
    Detail::NativeGraphicsContext* NativeGraphicsContext();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Pomdog

#endif // POMDOG_GRAPHICSCONTEXT_F873DD34_HPP
