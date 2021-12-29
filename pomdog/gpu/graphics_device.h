// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/gpu/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog {

class POMDOG_EXPORT GraphicsDevice {
public:
    GraphicsDevice() noexcept;
    GraphicsDevice(const GraphicsDevice&) = delete;
    GraphicsDevice& operator=(const GraphicsDevice&) = delete;

    virtual ~GraphicsDevice();

    /// Gets the currently supported shader language.
    [[nodiscard]] virtual ShaderLanguage
    GetSupportedLanguage() const noexcept = 0;

    /// Gets the presentation parameters.
    [[nodiscard]] virtual PresentationParameters
    GetPresentationParameters() const noexcept = 0;

    /// Creates a graphics command list.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<GraphicsCommandList>, std::unique_ptr<Error>>
    CreateGraphicsCommandList() noexcept = 0;

    /// Creates a vertex buffer.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
    CreateVertexBuffer(
        const void* vertices,
        std::size_t vertexCount,
        std::size_t strideBytes,
        BufferUsage bufferUsage) noexcept = 0;

    /// Creates a vertex buffer.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
    CreateVertexBuffer(
        std::size_t vertexCount,
        std::size_t strideBytes,
        BufferUsage bufferUsage) noexcept = 0;

    /// Creates an index buffer.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
    CreateIndexBuffer(
        IndexElementSize elementSize,
        const void* indices,
        std::size_t indexCount,
        BufferUsage bufferUsage) noexcept = 0;

    /// Creates an index buffer.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
    CreateIndexBuffer(
        IndexElementSize elementSize,
        std::size_t indexCount,
        BufferUsage bufferUsage) noexcept = 0;

    /// Creates a constant buffer.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
    CreateConstantBuffer(
        const void* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage) noexcept = 0;

    /// Creates a constant buffer.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
    CreateConstantBuffer(
        std::size_t sizeInBytes,
        BufferUsage bufferUsage) noexcept = 0;

    /// Creates a pipeline state object.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<PipelineState>, std::unique_ptr<Error>>
    CreatePipelineState(const PipelineStateDescriptor& descriptor) noexcept = 0;

    /// Creates an effect reflection.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<EffectReflection>, std::unique_ptr<Error>>
    CreateEffectReflection(
        const PipelineStateDescriptor& descriptor,
        const std::shared_ptr<PipelineState>& pipelineState) noexcept = 0;

    /// Creates a shader object.
    [[nodiscard]] virtual std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
    CreateShader(
        const detail::ShaderBytecode& shaderBytecode,
        const detail::ShaderCompileOptions& compileOptions) noexcept = 0;

    /// Creates a 2D render target.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
    CreateRenderTarget2D(
        std::int32_t width,
        std::int32_t height) noexcept = 0;

    /// Creates a 2D render target.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
    CreateRenderTarget2D(
        std::int32_t width,
        std::int32_t height,
        bool generateMipmap,
        SurfaceFormat format) noexcept = 0;

    /// Creates a depth stencil buffer.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<DepthStencilBuffer>, std::unique_ptr<Error>>
    CreateDepthStencilBuffer(
        std::int32_t width,
        std::int32_t height,
        SurfaceFormat depthStencilFormat) noexcept = 0;

    /// Creates a sampler state object.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
    CreateSamplerState(const SamplerDescriptor& descriptor) noexcept = 0;

    /// Creates a 2D texture.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<Texture2D>, std::unique_ptr<Error>>
    CreateTexture2D(
        std::int32_t width,
        std::int32_t height) noexcept = 0;

    /// Creates a 2D texture.
    [[nodiscard]] virtual std::tuple<std::shared_ptr<Texture2D>, std::unique_ptr<Error>>
    CreateTexture2D(
        std::int32_t width,
        std::int32_t height,
        bool mipMap,
        SurfaceFormat format) noexcept = 0;
};

} // namespace pomdog
