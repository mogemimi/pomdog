// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GraphicsDeviceGL4.hpp"
#include "BufferGL4.hpp"
#include "EffectReflectionGL4.hpp"
#include "PipelineStateGL4.hpp"
#include "RenderTarget2DGL4.hpp"
#include "SamplerStateGL4.hpp"
#include "ShaderGL4.hpp"
#include "Texture2DGL4.hpp"
#include "../RenderSystem/BufferBindMode.hpp"
#include "../RenderSystem/GraphicsCommandListImmediate.hpp"
#include "../RenderSystem/ShaderCompileOptions.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"

namespace Pomdog {
namespace Detail {
namespace GL4 {

GraphicsDeviceGL4::GraphicsDeviceGL4(const PresentationParameters& presentationParametersIn)
    : presentationParameters(presentationParametersIn)
{
}

ShaderLanguage GraphicsDeviceGL4::GetSupportedLanguage() const noexcept
{
    return ShaderLanguage::GLSL;
}

PresentationParameters GraphicsDeviceGL4::GetPresentationParameters() const noexcept
{
    return presentationParameters;
}

std::unique_ptr<NativeGraphicsCommandList>
GraphicsDeviceGL4::CreateGraphicsCommandList()
{
    return std::make_unique<GraphicsCommandListImmediate>();
}

std::unique_ptr<Shader>
GraphicsDeviceGL4::CreateShader(
    const ShaderBytecode& shaderBytecode,
    const ShaderCompileOptions& compileOptions)
{
    switch (compileOptions.Profile.PipelineStage) {
    case ShaderPipelineStage::VertexShader: {
        return std::make_unique<VertexShaderGL4>(shaderBytecode);
    }
    case ShaderPipelineStage::PixelShader: {
        return std::make_unique<PixelShaderGL4>(shaderBytecode);
    }
    }

    POMDOG_THROW_EXCEPTION(std::domain_error, "Failed to create shader");
}

std::unique_ptr<NativeBuffer>
GraphicsDeviceGL4::CreateBuffer(
    std::size_t sizeInBytes, BufferUsage bufferUsage, BufferBindMode bindMode)
{
    switch (bindMode) {
    case BufferBindMode::ConstantBuffer:
        return std::make_unique<ConstantBufferGL4>(sizeInBytes, bufferUsage);
        break;
    case BufferBindMode::IndexBuffer:
        return std::make_unique<IndexBufferGL4>(sizeInBytes, bufferUsage);
        break;
    case BufferBindMode::VertexBuffer:
        break;
    }
    return std::make_unique<VertexBufferGL4>(sizeInBytes, bufferUsage);
}

std::unique_ptr<NativeBuffer>
GraphicsDeviceGL4::CreateBuffer(
    const void* sourceData,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage,
    BufferBindMode bindMode)
{
    switch (bindMode) {
    case BufferBindMode::ConstantBuffer:
        return std::make_unique<ConstantBufferGL4>(
            sourceData, sizeInBytes, bufferUsage);
        break;
    case BufferBindMode::IndexBuffer:
        return std::make_unique<IndexBufferGL4>(
            sourceData, sizeInBytes, bufferUsage);
        break;
    case BufferBindMode::VertexBuffer:
        break;
    }

    return std::make_unique<VertexBufferGL4>(
        sourceData, sizeInBytes, bufferUsage);
}

std::unique_ptr<NativeSamplerState>
GraphicsDeviceGL4::CreateSamplerState(const SamplerDescription& description)
{
    return std::make_unique<SamplerStateGL4>(description);
}

std::unique_ptr<NativePipelineState>
GraphicsDeviceGL4::CreatePipelineState(const PipelineStateDescription& description)
{
    return std::make_unique<PipelineStateGL4>(description);
}

std::unique_ptr<NativeEffectReflection>
GraphicsDeviceGL4::CreateEffectReflection(
    const PipelineStateDescription&,
    NativePipelineState & pipelineState)
{
    auto const pipelineStateGL4 = dynamic_cast<PipelineStateGL4*>(&pipelineState);
    POMDOG_ASSERT(pipelineStateGL4 != nullptr);

    return std::make_unique<EffectReflectionGL4>(
        pipelineStateGL4->GetShaderProgram());
}

std::unique_ptr<NativeTexture2D>
GraphicsDeviceGL4::CreateTexture2D(
    std::int32_t width,
    std::int32_t height,
    std::int32_t mipmapLevels,
    SurfaceFormat format)
{
    return std::make_unique<Texture2DGL4>(width, height, mipmapLevels, format);
}

std::unique_ptr<NativeRenderTarget2D>
GraphicsDeviceGL4::CreateRenderTarget2D(
    std::int32_t width,
    std::int32_t height,
    std::int32_t mipmapLevels,
    SurfaceFormat format,
    DepthFormat depthStencilFormat,
    std::int32_t multiSampleCount)
{
    return std::make_unique<RenderTarget2DGL4>(width, height, mipmapLevels,
        format, depthStencilFormat, multiSampleCount);
}

void GraphicsDeviceGL4::ClientSizeChanged(int width, int height)
{
    presentationParameters.BackBufferWidth = width;
    presentationParameters.BackBufferHeight = height;
}

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
