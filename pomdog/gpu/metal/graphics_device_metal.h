// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/types.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

#import <Metal/Metal.h>

namespace pomdog::gpu::detail::metal {
class FrameCounter;
} // namespace pomdog::gpu::detail::metal

namespace pomdog::gpu::detail::metal {

class GraphicsDeviceMetal final : public GraphicsDevice {
public:
    ~GraphicsDeviceMetal() override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        const PresentationParameters& presentationParameters,
        std::shared_ptr<const FrameCounter> frameCounter) noexcept;

    /// Gets the type of graphics backend.
    GraphicsBackend
    getBackendKind() const noexcept override;

    /// Gets the presentation parameters.
    PresentationParameters
    getPresentationParameters() const noexcept override;

    /// Creates a graphics command list.
    std::tuple<std::shared_ptr<CommandList>, std::unique_ptr<Error>>
    createCommandList() noexcept override;

    std::tuple<std::shared_ptr<Buffer>, std::unique_ptr<Error>>
    createBuffer(const BufferDesc& desc) noexcept override;

    std::tuple<std::shared_ptr<Buffer>, std::unique_ptr<Error>>
    createBuffer(const BufferDesc& desc, std::span<const u8> initialData) noexcept override;

    /// Creates a vertex buffer.
    std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
    createVertexBuffer(
        const void* vertices,
        u32 vertexCount,
        u32 strideBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a vertex buffer.
    std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
    createVertexBuffer(
        u32 vertexCount,
        u32 strideBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates an index buffer.
    std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
    createIndexBuffer(
        IndexFormat elementSize,
        const void* indices,
        u32 indexCount,
        BufferUsage bufferUsage) noexcept override;

    /// Creates an index buffer.
    std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
    createIndexBuffer(
        IndexFormat elementSize,
        u32 indexCount,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a constant buffer.
    std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
    createConstantBuffer(
        const void* sourceData,
        u32 sizeInBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a constant buffer.
    std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
    createConstantBuffer(
        u32 sizeInBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a pipeline state object.
    std::tuple<std::shared_ptr<PipelineState>, std::unique_ptr<Error>>
    createPipelineState(const PipelineDesc& descriptor) noexcept override;

    /// Creates a shader object.
    std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
    createShader(
        std::span<const u8> shaderBytecode,
        const detail::ShaderCompileOptions& compileOptions) noexcept override;

    /// Creates a 2D render target.
    std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
    createRenderTarget2D(
        i32 width,
        i32 height) noexcept override;

    /// Creates a 2D render target.
    std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
    createRenderTarget2D(
        i32 width,
        i32 height,
        bool generateMipmap,
        PixelFormat format) noexcept override;

    /// Creates a depth stencil buffer.
    std::tuple<std::shared_ptr<DepthStencilBuffer>, std::unique_ptr<Error>>
    createDepthStencilBuffer(
        i32 width,
        i32 height,
        PixelFormat depthStencilFormat) noexcept override;

    /// Creates a sampler state object.
    std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
    createSamplerState(const SamplerDesc& descriptor) noexcept override;

    /// Creates a 2D texture.
    std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
    createTexture2D(
        i32 width,
        i32 height) noexcept override;

    /// Creates a 2D texture.
    std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
    createTexture2D(
        i32 width,
        i32 height,
        bool mipMap,
        PixelFormat format) noexcept override;

    /// Gets the pointer of the native graphics device.
    [[nodiscard]] id<MTLDevice>
    getMTLDevice() noexcept;

    void clientSizeChanged(i32 width, i32 height) noexcept;

private:
    id<MTLDevice> device = nullptr;
    id<MTLLibrary> defaultLibrary = nullptr;
    PresentationParameters presentationParameters;
    std::shared_ptr<const FrameCounter> frameCounter_;
};

} // namespace pomdog::gpu::detail::metal
