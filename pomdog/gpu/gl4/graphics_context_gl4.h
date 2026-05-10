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

/// FrameBufferGL4 is a type-safe wrapper around a GL framebuffer object name.
using FrameBufferGL4 = pomdog::detail::Tagged<GLuint, Tags::FrameBufferTag>;

/// PrimitiveTopologyGL4 is a type-safe wrapper around the GL primitive topology enum.
using PrimitiveTopologyGL4 = pomdog::detail::Tagged<GLenum, PrimitiveTopology>;

/// GraphicsContextGL4 is the OpenGL 4 / WebGL 2 implementation of GraphicsContext.
class GraphicsContextGL4 final : public GraphicsContext {
public:
    /// Initializes the context with an OpenGL context and a reference to the graphics device.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const std::shared_ptr<OpenGLContext>& openGLContext,
        std::weak_ptr<GraphicsDevice>&& graphicsDevice) noexcept;

    ~GraphicsContextGL4() override;

    /// Retrieves the capabilities of a GraphicsContext.
    [[nodiscard]] GraphicsCapabilities getCapabilities() const noexcept;

    /// Executes all recorded command lists against the OpenGL context.
    void executeCommandLists(
        std::span<std::shared_ptr<CommandListImmediate>> commandLists) override;

    /// Swaps the front and back buffers to present the rendered frame.
    void present() override;

    /// Draws non-indexed primitives.
    void draw(
        u32 vertexCount,
        u32 startVertexLocation) override;

    /// Draws indexed primitives.
    void drawIndexed(
        u32 indexCount,
        u32 startIndexLocation) override;

    /// Draws instanced non-indexed primitives.
    void drawInstanced(
        u32 vertexCountPerInstance,
        u32 instanceCount,
        u32 startVertexLocation,
        u32 startInstanceLocation) override;

    /// Draws instanced indexed primitives.
    void drawIndexedInstanced(
        u32 indexCountPerInstance,
        u32 instanceCount,
        u32 startIndexLocation,
        u32 startInstanceLocation) override;

    /// Begins a render pass with the specified targets and clear values.
    void beginRenderPass(const RenderPass& renderPass) override;

    /// Ends the current render pass.
    void endRenderPass() override;

    /// Sets the viewport for the rasterizer stage.
    void setViewport(const Viewport& viewport) override;

    /// Sets the scissor rectangle for the rasterizer stage.
    void setScissorRect(const Rect2D& scissorRect) override;

    /// Sets the constant blend factor used by blend operations that reference it.
    void setBlendFactor(const Vector4& blendFactor) override;

    /// Binds a vertex buffer to the given input slot.
    void setVertexBuffer(
        u32 index,
        const std::shared_ptr<VertexBuffer>& vertexBuffer,
        u32 offset) override;

    /// Binds an index buffer.
    void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer) override;

    /// Binds a pipeline state object.
    void setPipelineState(const std::shared_ptr<PipelineState>& pipelineState) override;

    /// Binds a constant buffer to the given slot.
    void setConstantBuffer(
        u32 index,
        const std::shared_ptr<Buffer>& constantBuffer,
        u32 offset,
        u32 sizeInBytes) override;

    /// Binds a sampler state to the given texture unit.
    void setSampler(u32 index, const std::shared_ptr<SamplerState>& sampler) override;

    /// Unbinds the texture at the given slot.
    void setTexture(u32 index) override;

    /// Binds a texture to the given slot.
    void setTexture(u32 index, const std::shared_ptr<gpu::Texture>& texture) override;

private:
    void applyPipelineState();

    void emulateStartInstanceLocation(u32 startInstanceLocation);

private:
    std::array<VertexBufferBindingGL4, 8> vertexBuffers_;
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    std::vector<std::weak_ptr<Texture>> weakTextures_;
    std::vector<std::weak_ptr<Texture>> weakRenderTargets_;
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
