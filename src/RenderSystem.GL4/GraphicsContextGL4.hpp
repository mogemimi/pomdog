// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "../RenderSystem/NativeGraphicsContext.hpp"
#include "../Utility/Tagged.hpp"
#include "Pomdog/Graphics/VertexBufferBinding.hpp"
#include <memory>
#include <optional>
#include <vector>

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

    void ExecuteCommandLists(
        const std::vector<std::shared_ptr<GraphicsCommandListImmediate>>& commandLists) override;

    void Present() override;

    void Draw(
        std::size_t vertexCount,
        std::size_t startVertexLocation) override;

    void DrawIndexed(
        std::size_t indexCount,
        std::size_t startIndexLocation) override;

    void DrawInstanced(
        std::size_t vertexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startVertexLocation,
        std::size_t startInstanceLocation) override;

    void DrawIndexedInstanced(
        std::size_t indexCountPerInstance,
        std::size_t instanceCount,
        std::size_t startIndexLocation,
        std::size_t startInstanceLocation) override;

    void SetRenderPass(const RenderPass& renderPass) override;

    void SetPrimitiveTopology(PrimitiveTopology primitiveTopology) override;

    void SetBlendFactor(const Color& blendFactor) override;

    void SetVertexBuffers(const std::vector<VertexBufferBinding>& vertexBuffers) override;

    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    void SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineState) override;

    void SetConstantBuffer(int index, const std::shared_ptr<NativeBuffer>& constantBuffer) override;

    void SetSampler(int index, const std::shared_ptr<NativeSamplerState>& sampler) override;

    void SetTexture(int index) override;

    void SetTexture(int index, const std::shared_ptr<Texture2D>& texture) override;

    void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture) override;

private:
    void ApplyPipelineState();

    void EmulateStartInstanceLocation(std::size_t startInstanceLocation);

private:
    std::vector<VertexBufferBinding> vertexBuffers;
#if defined(DEBUG) && !defined(NDEBUG)
    std::vector<std::weak_ptr<Texture>> weakTextures;
    std::vector<std::weak_ptr<RenderTarget2D>> weakRenderTargets;
#endif
    std::shared_ptr<OpenGLContext> nativeContext;
    std::shared_ptr<PipelineStateGL4> pipelineState;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::weak_ptr<GameWindow> gameWindow;
    std::vector<std::optional<GLenum>> textures;
    std::optional<FrameBufferGL4> frameBuffer;
    std::vector<std::shared_ptr<RenderTarget2DGL4>> renderTargets;
    PrimitiveTopologyGL4 primitiveTopology;
    bool needToApplyInputLayout;
    bool needToApplyPipelineState;
};

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
