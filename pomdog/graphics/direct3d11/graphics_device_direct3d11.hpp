// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/direct3d11/prerequisites_direct3d11.hpp"
#include "pomdog/graphics/graphics_device.hpp"
#include "pomdog/graphics/presentation_parameters.hpp"
#include "pomdog/platform/win32/prerequisites_win32.hpp"
#include "pomdog/utility/errors.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::Direct3D11 {

class AdapterManager final {
private:
    std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapters;
    Microsoft::WRL::ComPtr<IDXGIAdapter1> activeAdapter;

public:
    [[nodiscard]] std::unique_ptr<Error>
    EnumAdapters() noexcept;

    void Clear();

    IDXGIAdapter1* ActiveAdapter() const;

    [[nodiscard]] std::tuple<Microsoft::WRL::ComPtr<IDXGIFactory1>, std::unique_ptr<Error>>
    GetFactory() noexcept;
};

class GraphicsDeviceDirect3D11 final : public GraphicsDevice {
public:
    [[nodiscard]] std::unique_ptr<Error>
    Initialize(const PresentationParameters& presentationParameters) noexcept;

    ~GraphicsDeviceDirect3D11();

    /// Gets the currently supported shader language.
    ShaderLanguage GetSupportedLanguage() const noexcept override;

    /// Gets the presentation parameters.
    PresentationParameters GetPresentationParameters() const noexcept override;

    /// Creates a graphics command list.
    std::tuple<std::shared_ptr<GraphicsCommandList>, std::unique_ptr<Error>>
    CreateGraphicsCommandList() noexcept override;

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
        IndexElementSize elementSize,
        const void* indices,
        std::size_t indexCount,
        BufferUsage bufferUsage) noexcept override;

    /// Creates an index buffer.
    std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
    CreateIndexBuffer(
        IndexElementSize elementSize,
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
    CreatePipelineState(const PipelineStateDescription& description) noexcept override;

    /// Creates an effect reflection.
    std::tuple<std::shared_ptr<EffectReflection>, std::unique_ptr<Error>>
    CreateEffectReflection(
        const PipelineStateDescription& description,
        const std::shared_ptr<PipelineState>& pipelineState) noexcept override;

    /// Creates a shader object.
    std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
    CreateShader(
        const Detail::ShaderBytecode& shaderBytecode,
        const Detail::ShaderCompileOptions& compileOptions) noexcept override;

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
        SurfaceFormat format) noexcept override;

    /// Creates a depth stencil buffer.
    std::tuple<std::shared_ptr<DepthStencilBuffer>, std::unique_ptr<Error>>
    CreateDepthStencilBuffer(
        std::int32_t width,
        std::int32_t height,
        SurfaceFormat depthStencilFormat) noexcept override;

    /// Creates a sampler state object.
    std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
    CreateSamplerState(const SamplerDescription& description) noexcept override;

    /// Creates a 2D texture.
    std::tuple<std::shared_ptr<Texture2D>, std::unique_ptr<Error>>
    CreateTexture2D(
        std::int32_t width,
        std::int32_t height) noexcept override;

    /// Creates a 2D texture.
    std::tuple<std::shared_ptr<Texture2D>, std::unique_ptr<Error>>
    CreateTexture2D(
        std::int32_t width,
        std::int32_t height,
        bool mipMap,
        SurfaceFormat format) noexcept override;

    /// Gets the pointer of the native graphics device.
    [[nodiscard]] Microsoft::WRL::ComPtr<ID3D11Device3>
    GetDevice() const noexcept;

    /// Gets the pointer of the IDXGIFactory1 object.
    [[nodiscard]] std::tuple<Microsoft::WRL::ComPtr<IDXGIFactory1>, std::unique_ptr<Error>>
    GetDXGIFactory() noexcept;

    void ClientSizeChanged(int width, int height);

private:
    AdapterManager adapters;
    Microsoft::WRL::ComPtr<ID3D11Device3> device;
    Microsoft::WRL::ComPtr<ID3D11InfoQueue> infoQueue;
    D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
    PresentationParameters presentationParameters;
};

} // namespace Pomdog::Detail::Direct3D11
