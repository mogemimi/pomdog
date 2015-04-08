// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GRAPHICSDEVICEDIRECT3D11_F96A4B97_HPP
#define POMDOG_GRAPHICSDEVICEDIRECT3D11_F96A4B97_HPP

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include <wrl/client.h>
#include <memory>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {

class GraphicsDeviceDirect3D11 final : public NativeGraphicsDevice {
public:
    GraphicsDeviceDirect3D11();

    ~GraphicsDeviceDirect3D11();

    ShaderLanguage GetSupportedLanguage() const override;

    std::unique_ptr<Shader>
    CreateShader(ShaderBytecode const& shaderBytecode,
        ShaderCompileOptions const& compileOptions) override;

    std::unique_ptr<NativeBuffer>
    CreateConstantBuffer(std::size_t sizeInBytes) override;

    std::unique_ptr<NativeBuffer>
    CreateIndexBuffer(std::size_t sizeInBytes, BufferUsage bufferUsage) override;

    std::unique_ptr<NativeBuffer>
    CreateIndexBuffer(void const* indices, std::size_t sizeInBytes,
        BufferUsage bufferUsage) override;

    std::unique_ptr<NativeBuffer>
    CreateVertexBuffer(std::size_t sizeInBytes, BufferUsage bufferUsage) override;

    std::unique_ptr<NativeBuffer>
    CreateVertexBuffer(void const* vertices, std::size_t sizeInBytes,
        BufferUsage bufferUsage) override;

    std::unique_ptr<NativeSamplerState>
    CreateSamplerState(SamplerDescription const& description) override;

    std::unique_ptr<NativePipelineState>
    CreatePipelineState(EffectPassDescription const& description) override;

    std::unique_ptr<NativeEffectReflection>
    CreateEffectReflection(NativePipelineState & pipelineState) override;

    std::unique_ptr<NativeTexture2D>
    CreateTexture2D(std::int32_t width, std::int32_t height,
        std::uint32_t mipmapLevels, SurfaceFormat format) override;

    std::unique_ptr<NativeRenderTarget2D>
    CreateRenderTarget2D(std::int32_t width, std::int32_t height,
        std::uint32_t mipmapLevels, SurfaceFormat format,
        DepthFormat depthStencilFormat) override;

public:
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContext() const;

    Microsoft::WRL::ComPtr<ID3D11Device> NativeDevice() const;

    Microsoft::WRL::ComPtr<IDXGIFactory1> DXGIFactory() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Direct3D11
} // namespace RenderSystem
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_GRAPHICSDEVICEDIRECT3D11_F96A4B97_HPP
