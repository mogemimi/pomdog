// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

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
class PipelineStateGL4;
class RenderTarget2DGL4;

class GraphicsContextGL4 final : public NativeGraphicsContext {
public:
    GraphicsContextGL4() = delete;

    GraphicsContextGL4(
        const std::shared_ptr<OpenGLContext>& openGLContext,
        std::weak_ptr<GameWindow> window);

    ~GraphicsContextGL4();

    GraphicsCapabilities GetCapabilities() const override;

    void Present() override;

    void Draw(std::size_t vertexCount) override;

    void DrawIndexed(std::size_t indexCount) override;

    void DrawInstanced(
        std::size_t vertexCount,
        std::size_t instanceCount) override;

    void DrawIndexedInstanced(
        std::size_t indexCount,
        std::size_t instanceCount) override;

    void SetRenderPass(const RenderPass& renderPass) override;

    void SetPrimitiveTopology(PrimitiveTopology primitiveTopology) override;

    void SetBlendFactor(const Color& blendFactor) override;

    void SetVertexBuffers(const std::vector<VertexBufferBinding>& vertexBuffers) override;

    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    void SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState) override;

    void SetConstantBuffer(int index, const std::shared_ptr<NativeBuffer>& constantBuffer) override;

    void SetSampler(int index, NativeSamplerState* sampler) override;

    void SetTexture(int index) override;

    void SetTexture(int index, Texture2D & texture) override;

    void SetTexture(int index, RenderTarget2D & texture) override;

private:
    void ApplyPipelineState();

private:
    std::vector<VertexBufferBinding> vertexBuffers;
    std::shared_ptr<OpenGLContext> nativeContext;
    std::shared_ptr<PipelineStateGL4> pipelineState;
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
