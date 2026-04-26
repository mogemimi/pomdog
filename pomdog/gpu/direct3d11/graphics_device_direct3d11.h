// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/win32/windows_headers.h"
#include "pomdog/gpu/direct3d11/prerequisites_direct3d11.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <wrl/client.h>
#include <memory>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::direct3d11 {

class AdapterManager final {
private:
    std::vector<Microsoft::WRL::ComPtr<IDXGIAdapter1>> adapters_;
    Microsoft::WRL::ComPtr<IDXGIAdapter1> activeAdapter_;

public:
    [[nodiscard]] std::unique_ptr<Error>
    enumAdapters() noexcept;

    void clear();

    [[nodiscard]] IDXGIAdapter1*
    getActiveAdapter() const;

    [[nodiscard]] std::tuple<Microsoft::WRL::ComPtr<IDXGIFactory1>, std::unique_ptr<Error>>
    getFactory() noexcept;
};

class GraphicsDeviceDirect3D11 final : public GraphicsDevice {
private:
    AdapterManager adapters_;
    Microsoft::WRL::ComPtr<ID3D11Device3> device_;
    Microsoft::WRL::ComPtr<ID3D11InfoQueue> infoQueue_;
    D3D_DRIVER_TYPE driverType_ = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL featureLevel_ = D3D_FEATURE_LEVEL_11_1;
    PresentationParameters presentationParameters_;

public:
    [[nodiscard]] std::unique_ptr<Error>
    initialize(const PresentationParameters& presentationParameters) noexcept;

    ~GraphicsDeviceDirect3D11();

    /// Gets the type of graphics backend.
    [[nodiscard]] GraphicsBackend
    getBackendKind() const noexcept override;

    /// Gets the presentation parameters.
    [[nodiscard]] PresentationParameters
    getPresentationParameters() const noexcept override;

    /// Creates a graphics command list.
    [[nodiscard]] std::tuple<std::shared_ptr<CommandList>, std::unique_ptr<Error>>
    createCommandList() noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<Buffer>, std::unique_ptr<Error>>
    createBuffer(const BufferDesc& desc) noexcept override;

    [[nodiscard]] std::tuple<std::shared_ptr<Buffer>, std::unique_ptr<Error>>
    createBuffer(const BufferDesc& desc, std::span<const u8> initialData) noexcept override;

    /// Creates a vertex buffer.
    [[nodiscard]] std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
    createVertexBuffer(
        const void* vertices,
        u32 vertexCount,
        u32 strideBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a vertex buffer.
    [[nodiscard]] std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
    createVertexBuffer(
        u32 vertexCount,
        u32 strideBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates an index buffer.
    [[nodiscard]] std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
    createIndexBuffer(
        IndexFormat elementSize,
        const void* indices,
        u32 indexCount,
        BufferUsage bufferUsage) noexcept override;

    /// Creates an index buffer.
    [[nodiscard]] std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
    createIndexBuffer(
        IndexFormat elementSize,
        u32 indexCount,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a constant buffer.
    [[nodiscard]] std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
    createConstantBuffer(
        const void* sourceData,
        u32 sizeInBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a constant buffer.
    [[nodiscard]] std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
    createConstantBuffer(
        u32 sizeInBytes,
        BufferUsage bufferUsage) noexcept override;

    /// Creates a pipeline state object.
    [[nodiscard]] std::tuple<std::shared_ptr<PipelineState>, std::unique_ptr<Error>>
    createPipelineState(const PipelineDesc& descriptor) noexcept override;

    /// Creates a shader object.
    [[nodiscard]] std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
    createShader(
        std::span<const u8> shaderBytecode,
        const detail::ShaderCompileOptions& compileOptions) noexcept override;

    /// Creates a 2D render target.
    [[nodiscard]] std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
    createRenderTarget2D(
        i32 width,
        i32 height) noexcept override;

    /// Creates a 2D render target.
    [[nodiscard]] std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
    createRenderTarget2D(
        i32 width,
        i32 height,
        bool generateMipmap,
        PixelFormat format) noexcept override;

    /// Creates a depth stencil buffer.
    [[nodiscard]] std::tuple<std::shared_ptr<DepthStencilBuffer>, std::unique_ptr<Error>>
    createDepthStencilBuffer(
        i32 width,
        i32 height,
        PixelFormat depthStencilFormat) noexcept override;

    /// Creates a sampler state object.
    [[nodiscard]] std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
    createSamplerState(const SamplerDesc& descriptor) noexcept override;

    /// Creates a 2D texture.
    [[nodiscard]] std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
    createTexture2D(
        i32 width,
        i32 height) noexcept override;

    /// Creates a 2D texture.
    [[nodiscard]] std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
    createTexture2D(
        i32 width,
        i32 height,
        bool mipMap,
        PixelFormat format) noexcept override;

    /// Creates a texture resource from a descriptor.
    [[nodiscard]] std::tuple<std::shared_ptr<gpu::Texture>, std::unique_ptr<Error>>
    createTexture(const TextureDesc& desc) noexcept override;

    /// Gets the pointer of the native graphics device.
    [[nodiscard]] Microsoft::WRL::ComPtr<ID3D11Device3>
    getDevice() const noexcept;

    /// Gets the pointer of the IDXGIFactory1 object.
    [[nodiscard]] std::tuple<Microsoft::WRL::ComPtr<IDXGIFactory1>, std::unique_ptr<Error>>
    getDXGIFactory() noexcept;

    void clientSizeChanged(i32 width, i32 height);
};

} // namespace pomdog::gpu::detail::direct3d11
