// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem/NativeGraphicsDevice.hpp"
#include <memory>

#import <Metal/Metal.h>

namespace Pomdog {
namespace Detail {
namespace Metal {

class GraphicsDeviceMetal final : public NativeGraphicsDevice {
public:
    explicit GraphicsDeviceMetal(const PresentationParameters& presentationParameters);

    ~GraphicsDeviceMetal();

    ShaderLanguage GetSupportedLanguage() const noexcept override;

    PresentationParameters GetPresentationParameters() const noexcept override;

    std::unique_ptr<NativeGraphicsCommandList>
    CreateGraphicsCommandList() override;

    std::unique_ptr<Shader>
    CreateShader(
        const ShaderBytecode& shaderBytecode,
        const ShaderCompileOptions& compileOptions) override;

    std::unique_ptr<NativeBuffer>
    CreateBuffer(
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode) override;

    std::unique_ptr<NativeBuffer>
    CreateBuffer(
        const void* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode) override;

    std::unique_ptr<NativeSamplerState>
    CreateSamplerState(const SamplerDescription& description) override;

    std::unique_ptr<NativePipelineState>
    CreatePipelineState(const PipelineStateDescription& description) override;

    std::unique_ptr<NativeEffectReflection>
    CreateEffectReflection(
        const PipelineStateDescription& description,
        NativePipelineState & pipelineState) override;

    std::unique_ptr<NativeTexture2D>
    CreateTexture2D(
        std::int32_t width,
        std::int32_t height,
        std::int32_t mipmapLevels,
        SurfaceFormat format) override;

    std::unique_ptr<NativeRenderTarget2D>
    CreateRenderTarget2D(
        std::int32_t width,
        std::int32_t height,
        std::int32_t mipmapLevels,
        SurfaceFormat format,
        DepthFormat depthStencilFormat,
        std::int32_t multiSampleCount) override;

    id<MTLDevice> GetMTLDevice();

    void ClientSizeChanged(int width, int height);

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
