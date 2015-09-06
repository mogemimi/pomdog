// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include <memory>

namespace Pomdog {
namespace Detail {
namespace Metal {

class GraphicsDeviceMetal final : public NativeGraphicsDevice {
public:
    GraphicsDeviceMetal();

    ~GraphicsDeviceMetal();

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

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
