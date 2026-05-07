// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <span>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::gpu {
class Buffer;
class CommandList;
class ConstantBuffer;
class DepthStencilBuffer;
class IndexBuffer;
class PipelineState;
class RenderTarget2D;
class SamplerState;
class Shader;
class Texture;
class Texture2D;
class VertexBuffer;
struct BufferDesc;
struct PipelineDesc;
struct PresentationParameters;
struct SamplerDesc;
struct TextureDesc;
enum class BufferUsage : u8;
enum class GraphicsBackend : u8;
enum class IndexFormat : u8;
enum class PixelFormat : u8;
} // namespace pomdog::gpu

namespace pomdog::gpu::detail {
struct ShaderCompileOptions;
} // namespace pomdog::gpu::detail

namespace pomdog::gpu {

/// GraphicsDevice is the primary interface for creating GPU resources and command lists.
class POMDOG_EXPORT GraphicsDevice {
public:
    GraphicsDevice() noexcept;
    GraphicsDevice(const GraphicsDevice&) = delete;
    GraphicsDevice& operator=(const GraphicsDevice&) = delete;

    virtual ~GraphicsDevice();

    /// Gets the type of graphics backend.
    [[nodiscard]] virtual GraphicsBackend
    getBackendKind() const noexcept = 0;

    /// Gets the presentation parameters.
    [[nodiscard]] virtual PresentationParameters
    getPresentationParameters() const noexcept = 0;

    /// Creates a buffer resource without initial data.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<Buffer>, std::unique_ptr<Error>>
    createBuffer(const BufferDesc& desc) noexcept = 0;

    /// Creates a buffer resource with initial data.
    ///
    /// The `initialData` span size must be >= `BufferDesc::sizeInBytes`.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<Buffer>, std::unique_ptr<Error>>
    createBuffer(const BufferDesc& desc, std::span<const u8> initialData) noexcept = 0;

    /// Creates a graphics command list.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<CommandList>, std::unique_ptr<Error>>
    createCommandList() noexcept = 0;

    /// Creates a vertex buffer.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
    createVertexBuffer(
        const void* vertices,
        u32 vertexCount,
        u32 strideBytes,
        BufferUsage bufferUsage) noexcept = 0;

    /// Creates a vertex buffer.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
    createVertexBuffer(
        u32 vertexCount,
        u32 strideBytes,
        BufferUsage bufferUsage) noexcept = 0;

    /// Creates an index buffer.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
    createIndexBuffer(
        IndexFormat elementSize,
        const void* indices,
        u32 indexCount,
        BufferUsage bufferUsage) noexcept = 0;

    /// Creates an index buffer.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
    createIndexBuffer(
        IndexFormat elementSize,
        u32 indexCount,
        BufferUsage bufferUsage) noexcept = 0;

    /// Creates a constant buffer.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
    createConstantBuffer(
        const void* sourceData,
        u32 sizeInBytes,
        BufferUsage bufferUsage) noexcept = 0;

    /// Creates a constant buffer.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
    createConstantBuffer(
        u32 sizeInBytes,
        BufferUsage bufferUsage) noexcept = 0;

    /// Creates a pipeline state object.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<PipelineState>, std::unique_ptr<Error>>
    createPipelineState(const PipelineDesc& descriptor) noexcept = 0;

    /// Creates a shader object.
    [[nodiscard]] virtual std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
    createShader(
        std::span<const u8> shaderBytecode,
        const detail::ShaderCompileOptions& compileOptions) noexcept = 0;

    /// Creates a 2D render target.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
    createRenderTarget2D(
        i32 width,
        i32 height) noexcept = 0;

    /// Creates a 2D render target.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
    createRenderTarget2D(
        i32 width,
        i32 height,
        bool generateMipmap,
        PixelFormat format) noexcept = 0;

    /// Creates a depth stencil buffer.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<DepthStencilBuffer>, std::unique_ptr<Error>>
    createDepthStencilBuffer(
        i32 width,
        i32 height,
        PixelFormat depthStencilFormat) noexcept = 0;

    /// Creates a sampler state object.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
    createSamplerState(const SamplerDesc& descriptor) noexcept = 0;

    /// Creates a 2D texture.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<Texture2D>, std::unique_ptr<Error>>
    createTexture2D(
        i32 width,
        i32 height) noexcept = 0;

    /// Creates a 2D texture.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<Texture2D>, std::unique_ptr<Error>>
    createTexture2D(
        i32 width,
        i32 height,
        bool mipMap,
        PixelFormat format) noexcept = 0;

    /// Creates a texture resource from a descriptor.
    ///
    /// The texture type is determined by TextureDesc::usage:
    /// - Sampled: a 2D texture suitable for shader sampling.
    /// - RenderTarget (with or without Sampled): a render target.
    /// - DepthStencil: a depth-stencil buffer.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<Texture>, std::unique_ptr<Error>>
    createTexture(const TextureDesc& desc) noexcept = 0;

    /// Returns true if textures rendered to a RenderTarget2D need a Y-flip
    /// when sampled (e.g. OpenGL stores framebuffer data bottom-to-top).
    [[nodiscard]] virtual bool
    isRenderTargetTextureFlipped() const noexcept;
};

} // namespace pomdog::gpu
