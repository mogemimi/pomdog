// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_GRAPHICSDEVICEDIRECT3D11_F96A4B97_2183_4ECE_808D_D15E6FE92833_HPP
#define POMDOG_GRAPHICSDEVICEDIRECT3D11_F96A4B97_2183_4ECE_808D_D15E6FE92833_HPP

#include "PrerequisitesDirect3D11.hpp"
#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include "Pomdog/Platform/Win32/PrerequisitesWin32.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace Direct3D11 {

class GraphicsDeviceDirect3D11 final : public NativeGraphicsDevice {
public:
    GraphicsDeviceDirect3D11();

    ~GraphicsDeviceDirect3D11();

    ///@copydoc NativeGraphicsDevice
    ShaderLanguage GetSupportedLanguage() const override;

    ///@copydoc NativeGraphicsDevice
    std::unique_ptr<Shader>
    CreateShader(ShaderBytecode const& shaderBytecode,
        ShaderCompileOptions const& compileOptions) override;

    ///@copydoc NativeGraphicsDevice
    std::unique_ptr<NativeIndexBuffer>
    CreateIndexBuffer(std::uint32_t sizeInBytes, BufferUsage bufferUsage) override;

    ///@copydoc NativeGraphicsDevice
    std::unique_ptr<NativeIndexBuffer>
    CreateIndexBuffer(void const* indices, std::uint32_t sizeInBytes,
        BufferUsage bufferUsage) override;

    ///@copydoc NativeGraphicsDevice
    std::unique_ptr<NativeVertexBuffer>
    CreateVertexBuffer(std::uint32_t sizeInBytes, BufferUsage bufferUsage) override;

    ///@copydoc NativeGraphicsDevice
    std::unique_ptr<NativeVertexBuffer>
    CreateVertexBuffer(void const* vertices, std::uint32_t sizeInBytes,
        BufferUsage bufferUsage) override;

    ///@copydoc NativeGraphicsDevice
    std::unique_ptr<NativeBlendState>
    CreateBlendState(BlendDescription const& description) override;

    ///@copydoc NativeGraphicsDevice
    std::unique_ptr<NativeDepthStencilState>
    CreateDepthStencilState(DepthStencilDescription const& description) override;

    ///@copydoc NativeGraphicsDevice
    std::unique_ptr<NativeSamplerState>
    CreateSamplerState(SamplerDescription const& description) override;

    ///@copydoc NativeGraphicsDevice
    std::unique_ptr<NativeRasterizerState>
    CreateRasterizerState(RasterizerDescription const& description) override;

    ///@copydoc NativeGraphicsDevice
    std::unique_ptr<NativeEffectPass>
    CreateEffectPass(EffectPassDescription const& description) override;

    ///@copydoc NativeGraphicsDevice
    std::unique_ptr<NativeConstantBuffer>
    CreateConstantBuffer(std::uint32_t byteConstants) override;

    ///@copydoc NativeGraphicsDevice
    std::unique_ptr<NativeEffectReflection>
    CreateEffectReflection(NativeEffectPass & nativeEffectPass) override;

    ///@copydoc NativeGraphicsDevice
    std::unique_ptr<NativeTexture2D>
    CreateTexture2D(std::int32_t width, std::int32_t height,
        std::uint32_t mipmapLevels, SurfaceFormat format) override;

    ///@copydoc NativeGraphicsDevice
    std::unique_ptr<NativeRenderTarget2D>
    CreateRenderTarget2D(std::int32_t width, std::int32_t height,
        std::uint32_t mipmapLevels, SurfaceFormat format, DepthFormat depthStencilFormat) override;

public:
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceContext() const;

    Microsoft::WRL::ComPtr<ID3D11Device> NativeDevice() const;

    Microsoft::WRL::ComPtr<IDXGIFactory1> DXGIFactory() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}// namespace Direct3D11
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_GRAPHICSDEVICEDIRECT3D11_F96A4B97_2183_4ECE_808D_D15E6FE92833_HPP)
