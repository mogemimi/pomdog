// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <vector>

namespace Pomdog {
namespace Detail {

class ShaderBytecode;
struct ShaderCompileOptions;
enum class BufferBindMode;

class NativeGraphicsDevice {
public:
    NativeGraphicsDevice() = default;
    NativeGraphicsDevice(const NativeGraphicsDevice&) = delete;
    NativeGraphicsDevice & operator=(const NativeGraphicsDevice&) = delete;

    virtual ~NativeGraphicsDevice() = default;

    virtual ShaderLanguage GetSupportedLanguage() const noexcept = 0;

    virtual PresentationParameters GetPresentationParameters() const noexcept = 0;

    virtual std::unique_ptr<NativeGraphicsCommandList>
    CreateGraphicsCommandList() = 0;

    virtual std::unique_ptr<Shader>
    CreateShader(
        const ShaderBytecode& shaderBytecode,
        const ShaderCompileOptions& compileOptions) = 0;

    virtual std::unique_ptr<NativeBuffer>
    CreateBuffer(
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode) = 0;

    virtual std::unique_ptr<NativeBuffer>
    CreateBuffer(
        const void* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode) = 0;

    virtual std::unique_ptr<NativeSamplerState>
    CreateSamplerState(const SamplerDescription& description) = 0;

    virtual std::unique_ptr<NativePipelineState>
    CreatePipelineState(const PipelineStateDescription& description) = 0;

    virtual std::unique_ptr<NativeEffectReflection>
    CreateEffectReflection(
        const PipelineStateDescription& description,
        NativePipelineState & pipelineState) = 0;

    virtual std::unique_ptr<NativeTexture2D>
    CreateTexture2D(
        std::int32_t width,
        std::int32_t height,
        std::int32_t mipmapLevels,
        SurfaceFormat format) = 0;

    virtual std::unique_ptr<NativeRenderTarget2D>
    CreateRenderTarget2D(
        std::int32_t width,
        std::int32_t height,
        std::int32_t mipmapLevels,
        SurfaceFormat format,
        DepthFormat depthStencilFormat,
        std::int32_t multiSampleCount) = 0;
};

} // namespace Detail
} // namespace Pomdog
