// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "GraphicsDeviceMetal.hpp"
#include "BufferMetal.hpp"
#include "DepthStencilBufferMetal.hpp"
#include "PipelineStateMetal.hpp"
#include "RenderTarget2DMetal.hpp"
#include "SamplerStateMetal.hpp"
#include "ShaderMetal.hpp"
#include "Texture2DMetal.hpp"
#include "../Graphics.Backends/BufferBindMode.hpp"
#include "../Graphics.Backends/BufferHelper.hpp"
#include "../Graphics.Backends/GraphicsCommandListImmediate.hpp"
#include "../Graphics.Backends/ShaderBytecode.hpp"
#include "../Graphics.Backends/ShaderCompileOptions.hpp"
#include "../Graphics.Backends/TextureHelper.hpp"
#include "Pomdog/Graphics/ConstantBuffer.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/ShaderLanguage.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Errors.hpp"
#import <Metal/Metal.h>

namespace Pomdog::Detail::Metal {

std::unique_ptr<Error>
GraphicsDeviceMetal::Initialize(const PresentationParameters& presentationParametersIn) noexcept
{
    presentationParameters = presentationParametersIn;

    device = MTLCreateSystemDefaultDevice();
    if (device == nullptr) {
        return Errors::New("MTLCreateSystemDefaultDevice() failed");
    }

    // NOTE: Load all the shader files with a metal file extension in the project
    defaultLibrary = [device newDefaultLibrary];

    return nullptr;
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

std::tuple<std::shared_ptr<GraphicsCommandList>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateGraphicsCommandList() noexcept
{
    auto commandList = std::make_shared<GraphicsCommandListImmediate>();
    return std::make_tuple(std::move(commandList), nullptr);
}

std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
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

    auto nativeBuffer = std::make_unique<BufferMetal>();
    if (auto err = nativeBuffer->Initialize(
            device,
            vertices,
            sizeInBytes,
            bufferUsage,
            BufferBindMode::VertexBuffer);
        err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize BufferMetal"));
    }

    POMDOG_ASSERT(nativeBuffer != nullptr);

    auto vertexBuffer = std::make_shared<VertexBuffer>(std::move(nativeBuffer), vertexCount, strideBytes, bufferUsage);
    return std::make_tuple(std::move(vertexBuffer), nullptr);
}

std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
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

    auto nativeBuffer = std::make_unique<BufferMetal>();
    if (auto err = nativeBuffer->Initialize(
            device,
            sizeInBytes,
            bufferUsage,
            BufferBindMode::VertexBuffer);
        err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize BufferMetal"));
    }
    POMDOG_ASSERT(nativeBuffer != nullptr);

    auto vertexBuffer = std::make_shared<VertexBuffer>(std::move(nativeBuffer), vertexCount, strideBytes, bufferUsage);
    return std::make_tuple(std::move(vertexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateIndexBuffer(
    IndexElementSize elementSize,
    const void* indices,
    std::size_t indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(indexCount > 0);
    POMDOG_ASSERT(device != nullptr);

    const auto sizeInBytes = indexCount * Detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);

    auto nativeBuffer = std::make_unique<BufferMetal>();
    if (auto err = nativeBuffer->Initialize(
            device,
            indices,
            sizeInBytes,
            bufferUsage,
            BufferBindMode::IndexBuffer);
        err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize BufferMetal"));
    }
    POMDOG_ASSERT(nativeBuffer != nullptr);

    auto indexBuffer = std::make_shared<IndexBuffer>(std::move(nativeBuffer), elementSize, indexCount, bufferUsage);
    return std::make_tuple(std::move(indexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateIndexBuffer(
    IndexElementSize elementSize,
    std::size_t indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(indexCount > 0);
    POMDOG_ASSERT(device != nullptr);

    const auto sizeInBytes = indexCount * Detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);

    auto nativeBuffer = std::make_unique<BufferMetal>();
    if (auto err = nativeBuffer->Initialize(
            device,
            sizeInBytes,
            bufferUsage,
            BufferBindMode::IndexBuffer);
        err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize BufferMetal"));
    }
    POMDOG_ASSERT(nativeBuffer != nullptr);

    auto indexBuffer = std::make_shared<IndexBuffer>(std::move(nativeBuffer), elementSize, indexCount, bufferUsage);
    return std::make_tuple(std::move(indexBuffer), nullptr);
}

std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateConstantBuffer(
    const void* sourceData,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(sizeInBytes > 0);
    POMDOG_ASSERT(device != nullptr);

    auto nativeBuffer = std::make_unique<BufferMetal>();
    if (auto err = nativeBuffer->Initialize(
            device,
            sourceData,
            sizeInBytes,
            bufferUsage,
            BufferBindMode::ConstantBuffer);
        err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize BufferMetal"));
    }
    POMDOG_ASSERT(nativeBuffer != nullptr);

    auto constantBuffer = std::make_shared<ConstantBuffer>(std::move(nativeBuffer), sizeInBytes, bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateConstantBuffer(
    std::size_t sizeInBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(sizeInBytes > 0);
    POMDOG_ASSERT(device != nullptr);

    auto nativeBuffer = std::make_unique<BufferMetal>();
    if (auto err = nativeBuffer->Initialize(
            device,
            sizeInBytes,
            bufferUsage,
            BufferBindMode::ConstantBuffer);
        err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize BufferMetal"));
    }
    POMDOG_ASSERT(nativeBuffer != nullptr);

    auto constantBuffer = std::make_shared<ConstantBuffer>(std::move(nativeBuffer), sizeInBytes, bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<PipelineState>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreatePipelineState(const PipelineStateDescription& description) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    auto pipelineState = std::make_shared<PipelineStateMetal>();
    if (auto err = pipelineState->Initialize(device, description); err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize PipelineStateMetal"));
    }
    return std::make_tuple(std::move(pipelineState), nullptr);
}

std::tuple<std::shared_ptr<EffectReflection>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateEffectReflection(
    const PipelineStateDescription& description,
    const std::shared_ptr<PipelineState>& pipelineState) noexcept
{
    return std::make_tuple(nullptr, Errors::New("not implemented yet"));
}

std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateShader(
    const Detail::ShaderBytecode& shaderBytecode,
    const Detail::ShaderCompileOptions& compileOptions) noexcept
{
    POMDOG_ASSERT(device != nullptr);

    if ((shaderBytecode.Code == nullptr) &&
        (shaderBytecode.ByteLength == 0) &&
        !compileOptions.EntryPoint.empty()) {

        auto shader = std::make_unique<ShaderMetal>();
        if (auto err = shader->Initialize(device, defaultLibrary, compileOptions); err != nullptr) {
            return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize ShaderMetal"));
        }

        return std::make_tuple(std::move(shader), nullptr);
    }

    auto shader = std::make_unique<ShaderMetal>();
    if (auto err = shader->Initialize(device, shaderBytecode, compileOptions); err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize ShaderMetal"));
    }

    return std::make_tuple(std::move(shader), nullptr);
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateRenderTarget2D(
    std::int32_t width,
    std::int32_t height) noexcept
{
    return CreateRenderTarget2D(
        width,
        height,
        false,
        SurfaceFormat::R8G8B8A8_UNorm);
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateRenderTarget2D(
    std::int32_t width,
    std::int32_t height,
    bool generateMipmap,
    SurfaceFormat format) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    POMDOG_ASSERT(device != nullptr);

    const auto levelCount = generateMipmap
                                ? Detail::TextureHelper::ComputeMipmapLevelCount(width, height)
                                : 1;

    // TODO: MSAA is not implemented yet.
    constexpr int multiSampleCount = 1;

    auto renderTarget = std::make_shared<RenderTarget2DMetal>();
    if (auto err = renderTarget->Initialize(
            device,
            width,
            height,
            levelCount,
            format,
            multiSampleCount);
        err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize RenderTarget2DMetal"));
    }
    return std::make_tuple(std::move(renderTarget), nullptr);
}

std::tuple<std::shared_ptr<DepthStencilBuffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateDepthStencilBuffer(
    std::int32_t width,
    std::int32_t height,
    SurfaceFormat depthStencilFormat) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    POMDOG_ASSERT(device != nullptr);

    // TODO: MSAA is not implemented yet.
    constexpr int multiSampleCount = 1;

    auto depthStencilBuffer = std::make_shared<DepthStencilBufferMetal>();
    if (auto err = depthStencilBuffer->Initialize(
            device,
            width,
            height,
            depthStencilFormat,
            multiSampleCount);
        err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize DepthStencilBufferMetal"));
    }
    return std::make_tuple(std::move(depthStencilBuffer), nullptr);
}

std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateSamplerState(const SamplerDescription& description) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    auto samplerState = std::make_shared<SamplerStateMetal>();
    if (auto err = samplerState->Initialize(device, description); err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize SamplerStateMetal"));
    }
    return std::make_tuple(std::move(samplerState), nullptr);
}

std::tuple<std::shared_ptr<Texture2D>, std::unique_ptr<Error>>
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

std::tuple<std::shared_ptr<Texture2D>, std::unique_ptr<Error>>
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

    auto texture = std::make_shared<Texture2DMetal>();
    if (auto err = texture->Initialize(
            device,
            width,
            height,
            levelCount,
            format);
        err != nullptr) {
        return std::make_tuple(nullptr, Errors::Wrap(std::move(err), "failed to initialize Texture2DMetal"));
    }

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
