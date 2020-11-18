// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "OpenGLPrerequisites.hpp"
#include "VertexBufferBindingGL4.hpp"
#include "../RenderSystem/NativeGraphicsContext.hpp"
#include "../Utility/Tagged.hpp"
#include <array>
#include <memory>
#include <optional>
#include <vector>

namespace Pomdog {
class GraphicsDevice;
} // namespace Pomdog

namespace Pomdog::Detail::GL4 {

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
        std::weak_ptr<GraphicsDevice>&& graphicsDevice);

    ~GraphicsContextGL4() override;

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

    void SetViewport(const Viewport& viewport) override;

    void SetScissorRect(const Rectangle& scissorRect) override;

    void SetBlendFactor(const Vector4& blendFactor) override;

    void SetVertexBuffer(
        int index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        std::size_t offset) override;

    void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    void SetPipelineState(const std::shared_ptr<PipelineState>& pipelineState) override;

    void SetConstantBuffer(
        int index,
        const std::shared_ptr<NativeBuffer>& constantBuffer,
        std::size_t offset,
        std::size_t sizeInBytes) override;

    void SetSampler(int index, const std::shared_ptr<SamplerState>& sampler) override;

    void SetTexture(int index) override;

    void SetTexture(int index, const std::shared_ptr<Texture2D>& texture) override;

    void SetTexture(int index, const std::shared_ptr<RenderTarget2D>& texture) override;

private:
    void ApplyPipelineState();

    void EmulateStartInstanceLocation(std::size_t startInstanceLocation);

private:
    std::array<VertexBufferBindingGL4, 8> vertexBuffers;
#if defined(DEBUG) && !defined(NDEBUG)
    std::vector<std::weak_ptr<Texture>> weakTextures;
    std::vector<std::weak_ptr<RenderTarget2D>> weakRenderTargets;
#endif
    std::shared_ptr<OpenGLContext> nativeContext;
    std::shared_ptr<PipelineStateGL4> pipelineState;
    std::shared_ptr<IndexBuffer> indexBuffer;
    std::weak_ptr<GraphicsDevice> graphicsDevice;
    std::vector<std::optional<GLenum>> textures;
    std::optional<FrameBufferGL4> frameBuffer;
    std::array<std::shared_ptr<RenderTarget2DGL4>, 8> renderTargets;
    PrimitiveTopologyGL4 primitiveTopology;
    bool needToApplyInputLayout;
    bool needToApplyPipelineState;
};

} // namespace Pomdog::Detail::GL4
