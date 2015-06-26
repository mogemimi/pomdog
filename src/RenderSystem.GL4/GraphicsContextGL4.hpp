// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GRAPHICSCONTEXTGL4_1FF90EA8_HPP
#define POMDOG_GRAPHICSCONTEXTGL4_1FF90EA8_HPP

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeGraphicsContext.hpp"
#include "Pomdog/Graphics/VertexBufferBinding.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Utility/Optional.hpp"
#include <vector>
#include <memory>

namespace Pomdog {

class GameWindow;

namespace Detail {
namespace GL4 {

namespace Tags {

struct FrameBufferTag;

} // namespace Tags

using FrameBufferGL4 = Tagged<GLuint, Tags::FrameBufferTag>;
using PrimitiveTopologyGL4 = Tagged<GLenum, PrimitiveTopology>;

class OpenGLContext;
class ConstantLayoutGL4;
class PipelineStateGL4;
class RenderTarget2DGL4;

class GraphicsContextGL4 final: public NativeGraphicsContext {
public:
    GraphicsContextGL4() = delete;

    GraphicsContextGL4(std::shared_ptr<OpenGLContext> const& openGLContext, std::weak_ptr<GameWindow> window);

    ~GraphicsContextGL4();

    GraphicsCapabilities GetCapabilities() const override;

    void Present() override;

    void Clear(ClearOptions options, Color const& color, float depth, std::uint8_t stencil) override;

    void Draw(std::size_t vertexCount) override;

    void DrawIndexed(std::size_t indexCount) override;

    void DrawInstanced(
        std::size_t vertexCount,
        std::size_t instanceCount) override;

    void DrawIndexedInstanced(
        std::size_t indexCount,
        std::size_t instanceCount) override;

    void SetViewport(Viewport const& viewport) override;

    void SetScissorRectangle(Rectangle const& rectangle) override;

    void SetPrimitiveTopology(PrimitiveTopology primitiveTopology) override;

    void SetBlendFactor(Color const& blendFactor) override;

    void SetVertexBuffers(std::vector<VertexBufferBinding> const& vertexBuffers) override;

    void SetIndexBuffer(std::shared_ptr<IndexBuffer> const& indexBuffer) override;

    void SetPipelineState(std::shared_ptr<NativePipelineState> const& pipelineState) override;

    void SetConstantBuffers(std::shared_ptr<NativeConstantLayout> const& constantLayout) override;

    void SetSampler(int index, NativeSamplerState* sampler) override;

    void SetTexture(int index) override;

    void SetTexture(int index, Texture2D & texture) override;

    void SetTexture(int index, RenderTarget2D & texture) override;

    void SetRenderTarget() override;

    void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget2D>> const& renderTargets) override;

private:
    void ApplyPipelineState();

private:
    std::vector<VertexBufferBinding> vertexBuffers;
    std::shared_ptr<OpenGLContext> nativeContext;
    std::shared_ptr<PipelineStateGL4> pipelineState;
    std::shared_ptr<ConstantLayoutGL4> constantLayout;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::weak_ptr<GameWindow> gameWindow;
    std::vector<Optional<GLenum>> textures;
    Optional<FrameBufferGL4> frameBuffer;
    std::vector<std::shared_ptr<RenderTarget2DGL4>> renderTargets;
    PrimitiveTopologyGL4 primitiveTopology;
    bool needToApplyInputLayout;
    bool needToApplyPipelineState;
};

} // namespace GL4
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_GRAPHICSCONTEXTGL4_1FF90EA8_HPP
