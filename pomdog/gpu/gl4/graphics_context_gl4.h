// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/backends/graphics_context.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/gpu/gl4/vertex_buffer_binding_gl4.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/tagged.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <memory>
#include <optional>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu {
class GraphicsDevice;
class Texture;
enum class PrimitiveTopology : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail {
struct GraphicsCapabilities;
} // namespace pomdog::gpu::detail

namespace pomdog::gpu::detail::gl4 {
class OpenGLContext;
class PipelineStateGL4;
class RenderTarget2DGL4;
} // namespace pomdog::gpu::detail::gl4

namespace pomdog::gpu::detail::gl4 {

namespace Tags {
struct FrameBufferTag;
} // namespace Tags

using FrameBufferGL4 = pomdog::detail::Tagged<GLuint, Tags::FrameBufferTag>;
using PrimitiveTopologyGL4 = pomdog::detail::Tagged<GLenum, PrimitiveTopology>;

class GraphicsContextGL4 final : public GraphicsContext {
public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<OpenGLContext>& openGLContext,
        std::weak_ptr<GraphicsDevice>&& graphicsDevice) noexcept;

    ~GraphicsContextGL4() override;

    /// Retrieves the capabilities of a GraphicsContext.
    [[nodiscard]] GraphicsCapabilities getCapabilities() const noexcept;

    void executeCommandLists(
        std::span<std::shared_ptr<CommandListImmediate>> commandLists) override;

    void present() override;

    void draw(
        u32 vertexCount,
        u32 startVertexLocation) override;

    void drawIndexed(
        u32 indexCount,
        u32 startIndexLocation) override;

    void drawInstanced(
        u32 vertexCountPerInstance,
        u32 instanceCount,
        u32 startVertexLocation,
        u32 startInstanceLocation) override;

    void drawIndexedInstanced(
        u32 indexCountPerInstance,
        u32 instanceCount,
        u32 startIndexLocation,
        u32 startInstanceLocation) override;

    void beginRenderPass(const RenderPass& renderPass) override;

    void endRenderPass() override;

    void setViewport(const Viewport& viewport) override;

    void setScissorRect(const Rect2D& scissorRect) override;

    void setBlendFactor(const Vector4& blendFactor) override;

    void setVertexBuffer(
        u32 index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        u32 offset) override;

    void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    void setPipelineState(const std::shared_ptr<PipelineState>& pipelineState) override;

    void setConstantBuffer(
        u32 index,
        const std::shared_ptr<Buffer>& constantBuffer,
        u32 offset,
        u32 sizeInBytes) override;

    void setSampler(u32 index, const std::shared_ptr<SamplerState>& sampler) override;

    void setTexture(u32 index) override;

    void setTexture(u32 index, const std::shared_ptr<gpu::Texture2D>& texture) override;

    void setTexture(u32 index, const std::shared_ptr<RenderTarget2D>& texture) override;

private:
    void applyPipelineState();

    void emulateStartInstanceLocation(u32 startInstanceLocation);

private:
    std::array<VertexBufferBindingGL4, 8> vertexBuffers_;
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    std::vector<std::weak_ptr<Texture>> weakTextures_;
    std::vector<std::weak_ptr<RenderTarget2D>> weakRenderTargets_;
#endif
    std::shared_ptr<OpenGLContext> nativeContext_;
    std::shared_ptr<PipelineStateGL4> pipelineState_;
    std::shared_ptr<IndexBuffer> indexBuffer_;
    std::weak_ptr<GraphicsDevice> graphicsDevice_;
    std::vector<std::optional<GLenum>> textures_;
    std::optional<FrameBufferGL4> frameBuffer_;
    std::array<std::shared_ptr<RenderTarget2DGL4>, 8> renderTargets_;
    PrimitiveTopologyGL4 primitiveTopology_;
    bool needToApplyInputLayout_ = true;
    bool needToApplyPipelineState_ = true;
};

} // namespace pomdog::gpu::detail::gl4
