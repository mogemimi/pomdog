// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "GraphicsDeviceMetal.hpp"
#include "BufferMetal.hpp"
#include "PipelineStateMetal.hpp"
#include "RenderTarget2DMetal.hpp"
#include "SamplerStateMetal.hpp"
#include "ShaderMetal.hpp"
#include "Texture2DMetal.hpp"
#include "../RenderSystem/GraphicsCommandListImmediate.hpp"
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

public:
    Impl();
};
//-----------------------------------------------------------------------
GraphicsDeviceMetal::Impl::Impl()
    : device(nil)
{
    device = MTLCreateSystemDefaultDevice();
}
//-----------------------------------------------------------------------
GraphicsDeviceMetal::GraphicsDeviceMetal()
    : impl(std::make_unique<Impl>())
{
}
//-----------------------------------------------------------------------
GraphicsDeviceMetal::~GraphicsDeviceMetal() = default;
//-----------------------------------------------------------------------
ShaderLanguage GraphicsDeviceMetal::GetSupportedLanguage() const
{
    return ShaderLanguage::Metal;
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeGraphicsCommandList>
GraphicsDeviceMetal::CreateGraphicsCommandList()
{
    return std::make_unique<GraphicsCommandListImmediate>();
}
//-----------------------------------------------------------------------
std::unique_ptr<Shader>
GraphicsDeviceMetal::CreateShader(
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nil);
    return std::make_unique<ShaderMetal>(
        impl->device, shaderBytecode, compileOptions);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeBuffer>
GraphicsDeviceMetal::CreateBuffer(
    std::size_t sizeInBytes, BufferUsage bufferUsage, BufferBindMode)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nil);
    return std::make_unique<BufferMetal>(
        impl->device, sizeInBytes, bufferUsage);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeBuffer>
GraphicsDeviceMetal::CreateBuffer(
    const void* sourceData,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    BufferBindMode)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nil);
    return std::make_unique<BufferMetal>(
        impl->device, sourceData, sizeInBytes, bufferUsage);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeSamplerState>
GraphicsDeviceMetal::CreateSamplerState(const SamplerDescription& description)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nil);
    return std::make_unique<SamplerStateMetal>(impl->device, description);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativePipelineState>
GraphicsDeviceMetal::CreatePipelineState(const PipelineStateDescription& description)
{
    POMDOG_ASSERT(impl);
    POMDOG_ASSERT(impl->device != nil);
    return std::make_unique<PipelineStateMetal>(impl->device, description);
}
//-----------------------------------------------------------------------
std::unique_ptr<NativeEffectReflection>
GraphicsDeviceMetal::CreateEffectReflection(
    const PipelineStateDescription& description,
    NativePipelineState & pipelineState)
{
    POMDOG_THROW_EXCEPTION(std::runtime_error, "Not implemented");
}
//-----------------------------------------------------------------------
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
//-----------------------------------------------------------------------
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
//-----------------------------------------------------------------------
} // namespace Metal
} // namespace Detail
} // namespace Pomdog
