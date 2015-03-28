// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GRAPHICSCONTEXT_F873DD34_HPP
#define POMDOG_GRAPHICSCONTEXT_F873DD34_HPP

#include "detail/ForwardDeclarations.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>

namespace Pomdog {

class PresentationParameters;

namespace Detail {
namespace RenderSystem {

class NativeGraphicsContext;

}// namespace RenderSystem
}// namespace Detail

class POMDOG_EXPORT GraphicsContext {
public:
    GraphicsContext() = delete;
    GraphicsContext(GraphicsContext const&) = delete;
    GraphicsContext & operator=(GraphicsContext const&) = delete;

    GraphicsContext(std::unique_ptr<Detail::RenderSystem::NativeGraphicsContext> nativeContext,
        PresentationParameters const& presentationParameters,
        std::shared_ptr<GraphicsDevice> const& graphicsDevice);

    ~GraphicsContext();

    void Clear(Color const& color);

    void Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil);

    void Present();

    void Draw(PrimitiveTopology primitiveTopology, std::size_t vertexCount);

    void DrawIndexed(PrimitiveTopology primitiveTopology,
        std::shared_ptr<IndexBuffer> const& indexBuffer, std::size_t indexCount);

    void DrawInstanced(PrimitiveTopology primitiveTopology,
        std::size_t vertexCount, std::size_t instanceCount);

    void DrawIndexedInstanced(PrimitiveTopology primitiveTopology,
        std::shared_ptr<IndexBuffer> const& indexBuffer, std::size_t indexCount, std::size_t instanceCount);

    Pomdog::Viewport const& Viewport() const;

    void Viewport(Pomdog::Viewport const& viewport);

    Pomdog::Rectangle ScissorRectangle() const;

    void ScissorRectangle(Pomdog::Rectangle const& rectangle);

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

    void SetEffectPass(std::shared_ptr<EffectPass> const& effectPass);

    void SetConstantBuffers(std::shared_ptr<ConstantBufferBinding> const& constantBuffers);

public:
    Detail::RenderSystem::NativeGraphicsContext* NativeGraphicsContext();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}// namespace Pomdog

#endif // POMDOG_GRAPHICSCONTEXT_F873DD34_HPP
