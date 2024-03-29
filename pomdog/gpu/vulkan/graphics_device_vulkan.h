// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/graphics_device.h"
#include <memory>

namespace pomdog::gpu::detail::vulkan {

class GraphicsDeviceVulkan final : public GraphicsDevice {
public:
    GraphicsDeviceVulkan();

    ~GraphicsDeviceVulkan();

    /// Gets the currently supported shader language.
    ShaderLanguage getSupportedLanguage() const noexcept override;

    /// Gets the presentation parameters.
    PresentationParameters getPresentationParameters() const noexcept override;

    /// Creates a graphics command list.
    std::tuple<std::shared_ptr<CommandList>, std::unique_ptr<Error>>
    createCommandList() noexcept override;

    /// Creates a vertex buffer.
    std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
    createVertexBuffer(
        const void* vertices,
        std::size_t vertexCount,
        std::size_t strideBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a vertex buffer.
    std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
    createVertexBuffer(
        std::size_t vertexCount,
        std::size_t strideBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates an index buffer.
    std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
    createIndexBuffer(
        IndexFormat elementSize,
        const void* indices,
        std::size_t indexCount,
        BufferUsage bufferUsage) noexcept override;

    /// Creates an index buffer.
    std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
    createIndexBuffer(
        IndexFormat elementSize,
        std::size_t indexCount,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a constant buffer.
    std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
    createConstantBuffer(
        const void* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a constant buffer.
    std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
    createConstantBuffer(
        std::size_t sizeInBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a pipeline state object.
    std::tuple<std::shared_ptr<PipelineState>, std::unique_ptr<Error>>
    createPipelineState(const PipelineDescriptor& descriptor) noexcept override;

    /// Creates an effect reflection.
    std::tuple<std::shared_ptr<EffectReflection>, std::unique_ptr<Error>>
    createEffectReflection(
        const PipelineDescriptor& descriptor,
        const std::shared_ptr<PipelineState>& pipelineState) noexcept override;

    /// Creates a shader object.
    std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
    createShader(
        const detail::ShaderBytecode& shaderBytecode,
        const detail::ShaderCompileOptions& compileOptions) noexcept override;

    /// Creates a 2D render target.
    std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
    createRenderTarget2D(
        std::int32_t width,
        std::int32_t height) noexcept override;

    /// Creates a 2D render target.
    std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
    createRenderTarget2D(
        std::int32_t width,
        std::int32_t height,
        bool generateMipmap,
        PixelFormat format,
        PixelFormat depthStencilFormat) noexcept override;

    /// Creates a sampler state object.
    std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
    createSamplerState(const SamplerDescriptor& descriptor) noexcept override;

    /// Creates a 2D texture.
    std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
    createTexture2D(
        std::int32_t width,
        std::int32_t height) noexcept override;

    /// Creates a 2D texture.
    std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
    createTexture2D(
        std::int32_t width,
        std::int32_t height,
        bool mipMap,
        PixelFormat format) noexcept override;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace pomdog::gpu::detail::vulkan
