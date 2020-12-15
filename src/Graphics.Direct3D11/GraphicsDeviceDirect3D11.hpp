// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "PrerequisitesDirect3D11.hpp"
#include "Pomdog/Graphics/GraphicsDevice.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include "Pomdog/Utility/Errors.hpp"
#include <wrl/client.h>
#include <memory>
#include <vector>

namespace Pomdog::Detail::Direct3D11 {

class AdapterManager final {
private:
    std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapters;
    Microsoft::WRL::ComPtr<IDXGIAdapter1> activeAdapter;

public:
    [[nodiscard]] std::shared_ptr<Error>
    EnumAdapters() noexcept;

    void Clear();

    IDXGIAdapter1* ActiveAdapter() const;

    [[nodiscard]] std::tuple<Microsoft::WRL::ComPtr<IDXGIFactory1>, std::shared_ptr<Error>>
    GetFactory() noexcept;
};

class GraphicsDeviceDirect3D11 final : public GraphicsDevice {
public:
    [[nodiscard]] std::shared_ptr<Error>
    Initialize(const PresentationParameters& presentationParameters) noexcept;

    ~GraphicsDeviceDirect3D11();

    /// Gets the currently supported shader language.
    ShaderLanguage GetSupportedLanguage() const noexcept override;

    /// Gets the presentation parameters.
    PresentationParameters GetPresentationParameters() const noexcept override;

    /// Creates a graphics command list.
    std::tuple<std::shared_ptr<GraphicsCommandList>, std::shared_ptr<Error>>
    CreateGraphicsCommandList() noexcept override;

    /// Creates a vertex buffer.
    std::tuple<std::shared_ptr<VertexBuffer>, std::shared_ptr<Error>>
    CreateVertexBuffer(
        const void* vertices,
        std::size_t vertexCount,
        std::size_t strideBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a vertex buffer.
    std::tuple<std::shared_ptr<VertexBuffer>, std::shared_ptr<Error>>
    CreateVertexBuffer(
        std::size_t vertexCount,
        std::size_t strideBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates an index buffer.
    std::tuple<std::shared_ptr<IndexBuffer>, std::shared_ptr<Error>>
    CreateIndexBuffer(
        IndexElementSize elementSize,
        const void* indices,
        std::size_t indexCount,
        BufferUsage bufferUsage) noexcept override;

    /// Creates an index buffer.
    std::tuple<std::shared_ptr<IndexBuffer>, std::shared_ptr<Error>>
    CreateIndexBuffer(
        IndexElementSize elementSize,
        std::size_t indexCount,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a constant buffer.
    std::tuple<std::shared_ptr<ConstantBuffer>, std::shared_ptr<Error>>
    CreateConstantBuffer(
        const void* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a constant buffer.
    std::tuple<std::shared_ptr<ConstantBuffer>, std::shared_ptr<Error>>
    CreateConstantBuffer(
        std::size_t sizeInBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a pipeline state object.
    std::tuple<std::shared_ptr<PipelineState>, std::shared_ptr<Error>>
    CreatePipelineState(const PipelineStateDescription& description) noexcept override;

    /// Creates an effect reflection.
    std::tuple<std::shared_ptr<EffectReflection>, std::shared_ptr<Error>>
    CreateEffectReflection(
        const PipelineStateDescription& description,
        const std::shared_ptr<PipelineState>& pipelineState) noexcept override;

    /// Creates a shader object.
    std::tuple<std::unique_ptr<Shader>, std::shared_ptr<Error>>
    CreateShader(
        const Detail::ShaderBytecode& shaderBytecode,
        const Detail::ShaderCompileOptions& compileOptions) noexcept override;

    /// Creates a 2D render target.
    std::tuple<std::shared_ptr<RenderTarget2D>, std::shared_ptr<Error>>
    CreateRenderTarget2D(
        std::int32_t width,
        std::int32_t height) noexcept override;

    /// Creates a 2D render target.
    std::tuple<std::shared_ptr<RenderTarget2D>, std::shared_ptr<Error>>
    CreateRenderTarget2D(
        std::int32_t width,
        std::int32_t height,
        bool generateMipmap,
        SurfaceFormat format,
        SurfaceFormat depthStencilFormat) noexcept override;

    /// Creates a sampler state object.
    std::tuple<std::shared_ptr<SamplerState>, std::shared_ptr<Error>>
    CreateSamplerState(const SamplerDescription& description) noexcept override;

    /// Creates a 2D texture.
    std::tuple<std::shared_ptr<Texture2D>, std::shared_ptr<Error>>
    CreateTexture2D(
        std::int32_t width,
        std::int32_t height) noexcept override;

    /// Creates a 2D texture.
    std::tuple<std::shared_ptr<Texture2D>, std::shared_ptr<Error>>
    CreateTexture2D(
        std::int32_t width,
        std::int32_t height,
        bool mipMap,
        SurfaceFormat format) noexcept override;

    /// Gets the pointer of the native graphics device.
    [[nodiscard]] Microsoft::WRL::ComPtr<ID3D11Device3>
    GetDevice() const noexcept;

    /// Gets the pointer of the IDXGIFactory1 object.
    [[nodiscard]] std::tuple<Microsoft::WRL::ComPtr<IDXGIFactory1>, std::shared_ptr<Error>>
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
