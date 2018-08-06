// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GraphicsDeviceMetal.hpp"
#include "BufferMetal.hpp"
#include "PipelineStateMetal.hpp"
#include "RenderTarget2DMetal.hpp"
#include "SamplerStateMetal.hpp"
#include "ShaderMetal.hpp"
#include "Texture2DMetal.hpp"
#include "../RenderSystem/BufferBindMode.hpp"
#include "../RenderSystem/GraphicsCommandListImmediate.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "../RenderSystem/ShaderCompileOptions.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#import <Metal/Metal.h>

namespace Pomdog {
namespace Detail {
namespace Metal {

class GraphicsDeviceMetal::Impl final {
public:
    id<MTLDevice> device;
    id<MTLLibrary> defaultLibrary;
    PresentationParameters presentationParameters;

public:
    explicit Impl(const PresentationParameters& presentationParameters);
};

GraphicsDeviceMetal::Impl::Impl(const PresentationParameters& presentationParametersIn)
    : device(nil)
    , defaultLibrary(nil)
    , presentationParameters(presentationParametersIn)
{
    device = MTLCreateSystemDefaultDevice();

    POMDOG_ASSERT(device != nil);

    // Load all the shader files with a metal file extension in the project
    defaultLibrary = [device newDefaultLibrary];
}

GraphicsDeviceMetal::GraphicsDeviceMetal(const PresentationParameters& presentationParameters)
    : impl(std::make_unique<Impl>(presentationParameters))
{
}

GraphicsDeviceMetal::~GraphicsDeviceMetal() = default;

ShaderLanguage GraphicsDeviceMetal::GetSupportedLanguage() const noexcept
{
    return ShaderLanguage::Metal;
}

PresentationParameters GraphicsDeviceMetal::GetPresentationParameters() const noexcept
{
    POMDOG_ASSERT(impl);
    return impl->presentationParameters;
}

std::unique_ptr<NativeGraphicsCommandList>
GraphicsDeviceMetal::CreateGraphicsCommandList()
{
    return std::make_unique<GraphicsCommandListImmediate>();
}

std::unique_ptr<Shader>
GraphicsDeviceMetal::CreateShader(
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nil);

    if (shaderBytecode.Code == nullptr
        && shaderBytecode.ByteLength == 0
        && !compileOptions.EntryPoint.empty()) {
        return std::make_unique<ShaderMetal>(
            impl->device, impl->defaultLibrary, compileOptions);
    }

    return std::make_unique<ShaderMetal>(
        impl->device, shaderBytecode, compileOptions);
}

std::unique_ptr<NativeBuffer>
GraphicsDeviceMetal::CreateBuffer(
    std::size_t sizeInBytes, BufferUsage bufferUsage, BufferBindMode bindMode)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nil);
    return std::make_unique<BufferMetal>(
        impl->device, sizeInBytes, bufferUsage, bindMode);
}

std::unique_ptr<NativeBuffer>
GraphicsDeviceMetal::CreateBuffer(
    const void* sourceData,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    BufferBindMode bindMode)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nil);
    return std::make_unique<BufferMetal>(
        impl->device, sourceData, sizeInBytes, bufferUsage, bindMode);
}

std::unique_ptr<NativeSamplerState>
GraphicsDeviceMetal::CreateSamplerState(const SamplerDescription& description)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nil);
    return std::make_unique<SamplerStateMetal>(impl->device, description);
}

std::unique_ptr<NativePipelineState>
GraphicsDeviceMetal::CreatePipelineState(const PipelineStateDescription& description)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nil);
    return std::make_unique<PipelineStateMetal>(impl->device, description);
}

std::unique_ptr<NativeEffectReflection>
GraphicsDeviceMetal::CreateEffectReflection(
    const PipelineStateDescription& description,
    NativePipelineState & pipelineState)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}

std::unique_ptr<NativeTexture2D>
GraphicsDeviceMetal::CreateTexture2D(
    std::int32_t width,
    std::int32_t height,
    std::int32_t mipmapLevels,
    SurfaceFormat format)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nil);
    return std::make_unique<Texture2DMetal>(
        impl->device, width, height, mipmapLevels, format);
}

std::unique_ptr<NativeRenderTarget2D>
GraphicsDeviceMetal::CreateRenderTarget2D(
    std::int32_t width,
    std::int32_t height,
    std::int32_t mipmapLevels,
    SurfaceFormat format,
    DepthFormat depthStencilFormat,
    std::int32_t multiSampleCount)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nil);
    return std::make_unique<RenderTarget2DMetal>(
        impl->device, width, height, mipmapLevels,
        format, depthStencilFormat, multiSampleCount);
}

id<MTLDevice> GraphicsDeviceMetal::GetMTLDevice()
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nil);
    return impl->device;
}

void GraphicsDeviceMetal::ClientSizeChanged(int width, int height)
{
    POMDOG_ASSERT(impl);
    impl->presentationParameters.BackBufferWidth = width;
    impl->presentationParameters.BackBufferHeight = height;
}

} // namespace Metal
} // namespace Detail
} // namespace Pomdog
