// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#include "GraphicsDeviceMetal.hpp"
#include "BufferMetal.hpp"
#include "PipelineStateMetal.hpp"
#include "RenderTarget2DMetal.hpp"
#include "SamplerStateMetal.hpp"
#include "ShaderMetal.hpp"
#include "Texture2DMetal.hpp"
#include "../RenderSystem/BufferBindMode.hpp"
#include "../RenderSystem/BufferHelper.hpp"
#include "../RenderSystem/GraphicsCommandListImmediate.hpp"
#include "../RenderSystem/ShaderBytecode.hpp"
#include "../RenderSystem/ShaderCompileOptions.hpp"
#include "../RenderSystem/TextureHelper.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Errors.hpp"
#import <Metal/Metal.h>

namespace Pomdog::Detail::Metal {

GraphicsDeviceMetal::GraphicsDeviceMetal(const PresentationParameters& presentationParametersIn)
{
    presentationParameters = presentationParametersIn;

    device = MTLCreateSystemDefaultDevice();
    POMDOG_ASSERT(device != nullptr);

    // NOTE: Load all the shader files with a metal file extension in the project
    defaultLibrary = [device newDefaultLibrary];
}

GraphicsDeviceMetal::~GraphicsDeviceMetal() = default;

ShaderLanguage GraphicsDeviceMetal::GetSupportedLanguage() const noexcept
{
    return ShaderLanguage::Metal;
}

PresentationParameters GraphicsDeviceMetal::GetPresentationParameters() const noexcept
{
    return presentationParameters;
}

std::tuple<std::shared_ptr<GraphicsCommandList>, std::shared_ptr<Error>>
GraphicsDeviceMetal::CreateGraphicsCommandList() noexcept
{
    auto commandList = std::make_shared<GraphicsCommandListImmediate>();
    return std::make_tuple(std::move(commandList), nullptr);
}

std::tuple<std::shared_ptr<VertexBuffer>, std::shared_ptr<Error>>
GraphicsDeviceMetal::CreateVertexBuffer(
    const void* vertices,
    std::size_t vertexCount,
    std::size_t strideBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(vertices != nullptr);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(strideBytes > 0);
    POMDOG_ASSERT(device != nullptr);

    const auto sizeInBytes = vertexCount * strideBytes;

    auto nativeBuffer = std::make_unique<BufferMetal>(
        device, vertices, sizeInBytes, bufferUsage, BufferBindMode::VertexBuffer);
    POMDOG_ASSERT(nativeBuffer != nullptr);

    auto vertexBuffer = std::make_shared<VertexBuffer>(std::move(nativeBuffer), vertexCount, strideBytes, bufferUsage);
    return std::make_tuple(std::move(vertexBuffer), nullptr);
}

std::tuple<std::shared_ptr<VertexBuffer>, std::shared_ptr<Error>>
GraphicsDeviceMetal::CreateVertexBuffer(
    std::size_t vertexCount,
    std::size_t strideBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(strideBytes > 0);
    POMDOG_ASSERT(device != nullptr);

    const auto sizeInBytes = vertexCount * strideBytes;

    auto nativeBuffer = std::make_unique<BufferMetal>(
        device, sizeInBytes, bufferUsage, BufferBindMode::VertexBuffer);
    POMDOG_ASSERT(nativeBuffer != nullptr);

    auto vertexBuffer = std::make_shared<VertexBuffer>(std::move(nativeBuffer), vertexCount, strideBytes, bufferUsage);
    return std::make_tuple(std::move(vertexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::shared_ptr<Error>>
GraphicsDeviceMetal::CreateIndexBuffer(
    IndexElementSize elementSize,
    const void* indices,
    std::size_t indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(indexCount > 0);
    POMDOG_ASSERT(device != nullptr);

    const auto sizeInBytes = indexCount * Detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);

    auto nativeBuffer = std::make_unique<BufferMetal>(
        device, indices, sizeInBytes, bufferUsage, BufferBindMode::IndexBuffer);
    POMDOG_ASSERT(nativeBuffer != nullptr);

    auto indexBuffer = std::make_shared<IndexBuffer>(std::move(nativeBuffer), elementSize, indexCount, bufferUsage);
    return std::make_tuple(std::move(indexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::shared_ptr<Error>>
GraphicsDeviceMetal::CreateIndexBuffer(
    IndexElementSize elementSize,
    std::size_t indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(indexCount > 0);
    POMDOG_ASSERT(device != nullptr);

    const auto sizeInBytes = indexCount * Detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);

    auto nativeBuffer = std::make_unique<BufferMetal>(
        device, sizeInBytes, bufferUsage, BufferBindMode::IndexBuffer);
    POMDOG_ASSERT(nativeBuffer != nullptr);

    auto indexBuffer = std::make_shared<IndexBuffer>(std::move(nativeBuffer), elementSize, indexCount, bufferUsage);
    return std::make_tuple(std::move(indexBuffer), nullptr);
}

std::tuple<std::shared_ptr<ConstantBuffer>, std::shared_ptr<Error>>
GraphicsDeviceMetal::CreateConstantBuffer(
    const void* sourceData,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(sizeInBytes > 0);
    POMDOG_ASSERT(device != nullptr);

    auto nativeBuffer = std::make_unique<BufferMetal>(
        device, sourceData, sizeInBytes, bufferUsage, BufferBindMode::ConstantBuffer);
    POMDOG_ASSERT(nativeBuffer != nullptr);

    auto constantBuffer = std::make_shared<ConstantBuffer>(std::move(nativeBuffer), sizeInBytes, bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<ConstantBuffer>, std::shared_ptr<Error>>
GraphicsDeviceMetal::CreateConstantBuffer(
    std::size_t sizeInBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(sizeInBytes > 0);
    POMDOG_ASSERT(device != nullptr);

    auto nativeBuffer = std::make_unique<BufferMetal>(
        device, sizeInBytes, bufferUsage, BufferBindMode::ConstantBuffer);
    POMDOG_ASSERT(nativeBuffer != nullptr);

    auto constantBuffer = std::make_shared<ConstantBuffer>(std::move(nativeBuffer), sizeInBytes, bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<PipelineState>, std::shared_ptr<Error>>
GraphicsDeviceMetal::CreatePipelineState(const PipelineStateDescription& description) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    auto pipelineState = std::make_shared<PipelineStateMetal>(device, description);
    return std::make_tuple(std::move(pipelineState), nullptr);
}

std::tuple<std::shared_ptr<EffectReflection>, std::shared_ptr<Error>>
GraphicsDeviceMetal::CreateEffectReflection(
    const PipelineStateDescription& description,
    const std::shared_ptr<PipelineState>& pipelineState) noexcept
{
    return std::make_tuple(nullptr, Errors::New("not implemented yet"));
}

std::tuple<std::unique_ptr<Shader>, std::shared_ptr<Error>>
GraphicsDeviceMetal::CreateShader(
    const Detail::ShaderBytecode& shaderBytecode,
    const Detail::ShaderCompileOptions& compileOptions) noexcept
{
    POMDOG_ASSERT(device != nullptr);

    if ((shaderBytecode.Code == nullptr) &&
        (shaderBytecode.ByteLength == 0) &&
        !compileOptions.EntryPoint.empty()) {

        auto shader = std::make_unique<ShaderMetal>(device, defaultLibrary, compileOptions);
        return std::make_tuple(std::move(shader), nullptr);
    }

    auto shader = std::make_unique<ShaderMetal>(device, shaderBytecode, compileOptions);
    return std::make_tuple(std::move(shader), nullptr);
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::shared_ptr<Error>>
GraphicsDeviceMetal::CreateRenderTarget2D(
    std::int32_t width,
    std::int32_t height) noexcept
{
    return CreateRenderTarget2D(
        width,
        height,
        false,
        SurfaceFormat::R8G8B8A8_UNorm,
        DepthFormat::None);
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::shared_ptr<Error>>
GraphicsDeviceMetal::CreateRenderTarget2D(
    std::int32_t width,
    std::int32_t height,
    bool generateMipmap,
    SurfaceFormat format,
    DepthFormat depthStencilFormat) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    POMDOG_ASSERT(device != nullptr);

    const auto levelCount = generateMipmap
                                ? Detail::TextureHelper::ComputeMipmapLevelCount(width, height)
                                : 1;

    // TODO: MSAA is not implemented yet.
    constexpr int multiSampleCount = 1;

    auto renderTarget = std::make_shared<RenderTarget2DMetal>(
        device,
        width,
        height,
        levelCount,
        format,
        depthStencilFormat,
        multiSampleCount);

    return std::make_tuple(std::move(renderTarget), nullptr);
}

std::tuple<std::shared_ptr<SamplerState>, std::shared_ptr<Error>>
GraphicsDeviceMetal::CreateSamplerState(const SamplerDescription& description) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    auto samplerState = std::make_shared<SamplerStateMetal>(device, description);
    return std::make_tuple(std::move(samplerState), nullptr);
}

std::tuple<std::shared_ptr<Texture2D>, std::shared_ptr<Error>>
GraphicsDeviceMetal::CreateTexture2D(
    std::int32_t width,
    std::int32_t height) noexcept
{
    return CreateTexture2D(
        width,
        height,
        false,
        SurfaceFormat::R8G8B8A8_UNorm);
}

std::tuple<std::shared_ptr<Texture2D>, std::shared_ptr<Error>>
GraphicsDeviceMetal::CreateTexture2D(
    std::int32_t width,
    std::int32_t height,
    bool mipMap,
    SurfaceFormat format) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    POMDOG_ASSERT(device != nullptr);

    const auto levelCount = mipMap
                                ? Detail::TextureHelper::ComputeMipmapLevelCount(width, height)
                                : 1;

    auto texture = std::make_shared<Texture2DMetal>(
        device,
        width,
        height,
        levelCount,
        format);

    return std::make_tuple(std::move(texture), nullptr);
}

id<MTLDevice> GraphicsDeviceMetal::GetMTLDevice() noexcept
{
    POMDOG_ASSERT(device != nullptr);
    return device;
}

void GraphicsDeviceMetal::ClientSizeChanged(int width, int height) noexcept
{
    presentationParameters.BackBufferWidth = width;
    presentationParameters.BackBufferHeight = height;
}

} // namespace Pomdog::Detail::Metal
