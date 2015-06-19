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
namespace Direct3D11 {

class GraphicsDeviceDirect3D11 final : public NativeGraphicsDevice {
public:
    GraphicsDeviceDirect3D11();

    ~GraphicsDeviceDirect3D11();

    ShaderLanguage GetSupportedLanguage() const override;

    std::unique_ptr<NativeGraphicsCommandList>
    CreateGraphicsCommandList() override;

    std::unique_ptr<Shader>
    CreateShader(ShaderBytecode const& shaderBytecode,
        ShaderCompileOptions const& compileOptions) override;

    std::unique_ptr<NativeBuffer>
    CreateBuffer(std::size_t sizeInBytes,
        BufferUsage bufferUsage, BufferBindMode bindMode) override;

    std::unique_ptr<NativeBuffer>
    CreateBuffer(void const* sourceData,  std::size_t sizeInBytes,
        BufferUsage bufferUsage, BufferBindMode bindMode) override;

    std::unique_ptr<NativeSamplerState>
    CreateSamplerState(SamplerDescription const& description) override;

    std::unique_ptr<NativePipelineState>
    CreatePipelineState(PipelineStateDescription const& description) override;

    std::unique_ptr<NativeEffectReflection>
    CreateEffectReflection(PipelineStateDescription const& description,
        NativePipelineState & pipelineState) override;

    std::unique_ptr<NativeTexture2D>
    CreateTexture2D(std::int32_t width, std::int32_t height,
        std::int32_t mipmapLevels, SurfaceFormat format) override;

    std::unique_ptr<NativeRenderTarget2D>
    CreateRenderTarget2D(std::int32_t width, std::int32_t height,
        std::int32_t mipmapLevels,
        SurfaceFormat format,
        DepthFormat depthStencilFormat,
        std::int32_t multiSampleCount) override;

    Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() const;

    Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetDeviceContext() const;

    Microsoft::WRL::ComPtr<IDXGIFactory1> GetDXGIFactory() const;

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Direct3D11
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_GRAPHICSDEVICEDIRECT3D11_F96A4B97_HPP
