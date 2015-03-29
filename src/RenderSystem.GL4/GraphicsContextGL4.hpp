// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GRAPHICSCONTEXTGL4_1FF90EA8_HPP
#define POMDOG_GRAPHICSCONTEXTGL4_1FF90EA8_HPP

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeGraphicsContext.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <vector>
#include <memory>

namespace Pomdog {

class GameWindow;

namespace Detail {
namespace RenderSystem {
namespace GL4 {

namespace Tags {

struct FrameBufferTag;

}// namespace Tags

using FrameBufferGL4 = Tagged<GLuint, Tags::FrameBufferTag>;

class OpenGLContext;
class ConstantLayoutGL4;
class EffectPassGL4;
class RenderTarget2DGL4;

class GraphicsContextGL4 final: public NativeGraphicsContext {
public:
    GraphicsContextGL4() = delete;

    GraphicsContextGL4(std::shared_ptr<OpenGLContext> const& openGLContext, std::weak_ptr<GameWindow> window);

    ~GraphicsContextGL4();

    void Clear(Color const& color) override;

    void Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil) override;

    void Present() override;

    void Draw(PrimitiveTopology primitiveTopology, std::size_t vertexCount) override;

    void DrawIndexed(PrimitiveTopology primitiveTopology,
        std::shared_ptr<IndexBuffer> const& indexBuffer, std::size_t indexCount) override;

    void DrawInstanced(PrimitiveTopology primitiveTopology,
        std::size_t vertexCount, std::size_t instanceCount) override;

    void DrawIndexedInstanced(PrimitiveTopology primitiveTopology,
        std::shared_ptr<IndexBuffer> const& indexBuffer, std::size_t indexCount, std::size_t instanceCount) override;

    GraphicsCapabilities GetCapabilities() const override;

    void SetViewport(Viewport const& viewport) override;

    Rectangle GetScissorRectangle() const override;

    void SetScissorRectangle(Rectangle const& rectangle) override;

    void SetBlendFactor(Color const& blendFactor) override;

    void SetVertexBuffers(std::vector<std::shared_ptr<VertexBuffer>> const& vertexBuffers) override;

    void SetTexture(int index) override;

    void SetTexture(int index, Texture2D & texture) override;

    void SetTexture(int index, RenderTarget2D & texture) override;

    void SetRenderTarget() override;

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets) override;

    void SetEffectPass(std::shared_ptr<NativeEffectPass> const& nativeEffectPass) override;

    void SetConstantBuffers(std::shared_ptr<NativeConstantLayout> const& nativeConstantLayout) override;

private:
    void ApplyPipelineState();

private:
    std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers;
    std::shared_ptr<OpenGLContext> nativeContext;
    std::shared_ptr<EffectPassGL4> effectPass;
    std::shared_ptr<ConstantLayoutGL4> constantLayout;
    std::weak_ptr<GameWindow> gameWindow;
    std::vector<Optional<GLenum>> textures;
    Optional<FrameBufferGL4> frameBuffer;
    std::vector<std::shared_ptr<RenderTarget2DGL4>> renderTargets;
    bool needToApplyInputLayout;
    bool needToApplyPipelineState;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // POMDOG_GRAPHICSCONTEXTGL4_1FF90EA8_HPP
