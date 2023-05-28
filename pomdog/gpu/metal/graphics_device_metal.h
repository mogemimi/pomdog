// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/errors.h"
#include <memory>

#import <Metal/Metal.h>

namespace pomdog::gpu::detail::metal {
class FrameCounter;
} // namespace pomdog::gpu::detail::metal

namespace pomdog::gpu::detail::metal {

class GraphicsDeviceMetal final : public GraphicsDevice {
public:
    ~GraphicsDeviceMetal() override;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        const PresentationParameters& presentationParameters,
        std::shared_ptr<const FrameCounter> frameCounter) noexcept;

    /// Gets the currently supported shader language.
    ShaderLanguage GetSupportedLanguage() const noexcept override;

    /// Gets the presentation parameters.
    PresentationParameters GetPresentationParameters() const noexcept override;

    /// Creates a graphics command list.
    std::tuple<std::shared_ptr<CommandList>, std::unique_ptr<Error>>
    CreateCommandList() noexcept override;

    /// Creates a vertex buffer.
    std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
    CreateVertexBuffer(
        const void* vertices,
        std::size_t vertexCount,
        std::size_t strideBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a vertex buffer.
    std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
    CreateVertexBuffer(
        std::size_t vertexCount,
        std::size_t strideBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates an index buffer.
    std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
    CreateIndexBuffer(
        IndexFormat elementSize,
        const void* indices,
        std::size_t indexCount,
        BufferUsage bufferUsage) noexcept override;

    /// Creates an index buffer.
    std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
    CreateIndexBuffer(
        IndexFormat elementSize,
        std::size_t indexCount,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a constant buffer.
    std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
    CreateConstantBuffer(
        const void* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a constant buffer.
    std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
    CreateConstantBuffer(
        std::size_t sizeInBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a pipeline state object.
    std::tuple<std::shared_ptr<PipelineState>, std::unique_ptr<Error>>
    CreatePipelineState(const PipelineDescriptor& descriptor) noexcept override;

    /// Creates an effect reflection.
    std::tuple<std::shared_ptr<EffectReflection>, std::unique_ptr<Error>>
    CreateEffectReflection(
        const PipelineDescriptor& descriptor,
        const std::shared_ptr<PipelineState>& pipelineState) noexcept override;

    /// Creates a shader object.
    std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
    CreateShader(
        const detail::ShaderBytecode& shaderBytecode,
        const detail::ShaderCompileOptions& compileOptions) noexcept override;

    /// Creates a 2D render target.
    std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
    CreateRenderTarget2D(
        std::int32_t width,
        std::int32_t height) noexcept override;

    /// Creates a 2D render target.
    std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
    CreateRenderTarget2D(
        std::int32_t width,
        std::int32_t height,
        bool generateMipmap,
        PixelFormat format) noexcept override;

    /// Creates a depth stencil buffer.
    std::tuple<std::shared_ptr<DepthStencilBuffer>, std::unique_ptr<Error>>
    CreateDepthStencilBuffer(
        std::int32_t width,
        std::int32_t height,
        PixelFormat depthStencilFormat) noexcept override;

    /// Creates a sampler state object.
    std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
    CreateSamplerState(const SamplerDescriptor& descriptor) noexcept override;

    /// Creates a 2D texture.
    std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
    CreateTexture2D(
        std::int32_t width,
        std::int32_t height) noexcept override;

    /// Creates a 2D texture.
    std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
    CreateTexture2D(
        std::int32_t width,
        std::int32_t height,
        bool mipMap,
        PixelFormat format) noexcept override;

    /// Gets the pointer of the native graphics device.
    id<MTLDevice> GetMTLDevice() noexcept;

    void clientSizeChanged(int width, int height) noexcept;

private:
    id<MTLDevice> device = nullptr;
    id<MTLLibrary> defaultLibrary = nullptr;
    PresentationParameters presentationParameters;
    std::shared_ptr<const FrameCounter> frameCounter_;
};

} // namespace pomdog::gpu::detail::metal
