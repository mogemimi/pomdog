// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "../Utility/Noncopyable.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include <memory>
#include <vector>

namespace Pomdog {
namespace Detail {

class NativeBuffer;
class NativeEffectReflection;
class NativePipelineState;
class NativeRenderTarget2D;
class NativeSamplerState;
class NativeTexture2D;
class ShaderBytecode;
struct ShaderCompileOptions;

enum class BufferBindMode {
    ConstantBuffer,
    IndexBuffer,
    VertexBuffer,
};

class NativeGraphicsDevice : Noncopyable {
public:
    virtual ~NativeGraphicsDevice() = default;

    virtual ShaderLanguage GetSupportedLanguage() const = 0;

    virtual std::unique_ptr<NativeGraphicsCommandList>
    CreateGraphicsCommandList() = 0;

    virtual std::unique_ptr<Shader>
    CreateShader(ShaderBytecode const& shaderBytecode,
        ShaderCompileOptions const& compileOptions) = 0;

    virtual std::unique_ptr<NativeBuffer>
    CreateBuffer(
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode) = 0;

    virtual std::unique_ptr<NativeBuffer>
    CreateBuffer(
        void const* sourceData,
        std::size_t sizeInBytes,
        BufferUsage bufferUsage,
        BufferBindMode bindMode) = 0;

    virtual std::unique_ptr<NativeSamplerState>
    CreateSamplerState(SamplerDescription const& description) = 0;

    virtual std::unique_ptr<NativePipelineState>
    CreatePipelineState(PipelineStateDescription const& description) = 0;

    virtual std::unique_ptr<NativeEffectReflection>
    CreateEffectReflection(
        PipelineStateDescription const& description,
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
