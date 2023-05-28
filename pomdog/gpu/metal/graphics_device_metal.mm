// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/graphics_device_metal.h"
#include "pomdog/gpu/backends/buffer_bind_mode.h"
#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/gpu/backends/command_list_immediate.h"
#include "pomdog/gpu/backends/shader_bytecode.h"
#include "pomdog/gpu/backends/shader_compile_options.h"
#include "pomdog/gpu/backends/texture_helper.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/metal/buffer_metal.h"
#include "pomdog/gpu/metal/depth_stencil_buffer_metal.h"
#include "pomdog/gpu/metal/pipeline_state_metal.h"
#include "pomdog/gpu/metal/render_target2d_metal.h"
#include "pomdog/gpu/metal/sampler_state_metal.h"
#include "pomdog/gpu/metal/shader_metal.h"
#include "pomdog/gpu/metal/texture2d_metal.h"
#include "pomdog/gpu/shader_language.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#import <Metal/Metal.h>

namespace pomdog::gpu::detail::metal {

std::unique_ptr<Error>
GraphicsDeviceMetal::Initialize(
    const PresentationParameters& presentationParametersIn,
    std::shared_ptr<const FrameCounter> frameCounter) noexcept
{
    presentationParameters = presentationParametersIn;

    frameCounter_ = std::move(frameCounter);
    if (frameCounter_ == nullptr) {
        return errors::New("frameCounter_ must be != nullptr");
    }

    device = MTLCreateSystemDefaultDevice();
    if (device == nullptr) {
        return errors::New("MTLCreateSystemDefaultDevice() failed");
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

std::tuple<std::shared_ptr<CommandList>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateCommandList() noexcept
{
    auto commandList = std::make_shared<CommandListImmediate>();
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
    POMDOG_ASSERT(frameCounter_ != nullptr);

    const auto sizeInBytes = vertexCount * strideBytes;

    auto nativeBuffer = std::make_unique<BufferMetal>();
    if (auto err = nativeBuffer->Initialize(
            frameCounter_,
            device,
            vertices,
            sizeInBytes,
            bufferUsage,
            BufferBindMode::VertexBuffer);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize BufferMetal"));
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
    POMDOG_ASSERT(frameCounter_ != nullptr);

    const auto sizeInBytes = vertexCount * strideBytes;

    auto nativeBuffer = std::make_unique<BufferMetal>();
    if (auto err = nativeBuffer->Initialize(
            frameCounter_,
            device,
            sizeInBytes,
            bufferUsage,
            BufferBindMode::VertexBuffer);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize BufferMetal"));
    }
    POMDOG_ASSERT(nativeBuffer != nullptr);

    auto vertexBuffer = std::make_shared<VertexBuffer>(std::move(nativeBuffer), vertexCount, strideBytes, bufferUsage);
    return std::make_tuple(std::move(vertexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateIndexBuffer(
    IndexFormat elementSize,
    const void* indices,
    std::size_t indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(indexCount > 0);
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(frameCounter_ != nullptr);

    const auto sizeInBytes = indexCount * detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);

    auto nativeBuffer = std::make_unique<BufferMetal>();
    if (auto err = nativeBuffer->Initialize(
            frameCounter_,
            device,
            indices,
            sizeInBytes,
            bufferUsage,
            BufferBindMode::IndexBuffer);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize BufferMetal"));
    }
    POMDOG_ASSERT(nativeBuffer != nullptr);

    auto indexBuffer = std::make_shared<IndexBuffer>(std::move(nativeBuffer), elementSize, indexCount, bufferUsage);
    return std::make_tuple(std::move(indexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateIndexBuffer(
    IndexFormat elementSize,
    std::size_t indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(indexCount > 0);
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(frameCounter_ != nullptr);

    const auto sizeInBytes = indexCount * detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);

    auto nativeBuffer = std::make_unique<BufferMetal>();
    if (auto err = nativeBuffer->Initialize(
            frameCounter_,
            device,
            sizeInBytes,
            bufferUsage,
            BufferBindMode::IndexBuffer);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize BufferMetal"));
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
    POMDOG_ASSERT(frameCounter_ != nullptr);

    auto nativeBuffer = std::make_unique<BufferMetal>();
    if (auto err = nativeBuffer->Initialize(
            frameCounter_,
            device,
            sourceData,
            sizeInBytes,
            bufferUsage,
            BufferBindMode::ConstantBuffer);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize BufferMetal"));
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
    POMDOG_ASSERT(frameCounter_ != nullptr);

    auto nativeBuffer = std::make_unique<BufferMetal>();
    if (auto err = nativeBuffer->Initialize(
            frameCounter_,
            device,
            sizeInBytes,
            bufferUsage,
            BufferBindMode::ConstantBuffer);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize BufferMetal"));
    }
    POMDOG_ASSERT(nativeBuffer != nullptr);

    auto constantBuffer = std::make_shared<ConstantBuffer>(std::move(nativeBuffer), sizeInBytes, bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<PipelineState>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreatePipelineState(const PipelineDescriptor& descriptor) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    auto pipelineState = std::make_shared<PipelineStateMetal>();
    if (auto err = pipelineState->Initialize(device, descriptor); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize PipelineStateMetal"));
    }
    return std::make_tuple(std::move(pipelineState), nullptr);
}

std::tuple<std::shared_ptr<EffectReflection>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateEffectReflection(
    const PipelineDescriptor& descriptor,
    const std::shared_ptr<PipelineState>& pipelineState) noexcept
{
    return std::make_tuple(nullptr, errors::New("not implemented yet"));
}

std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateShader(
    const detail::ShaderBytecode& shaderBytecode,
    const detail::ShaderCompileOptions& compileOptions) noexcept
{
    POMDOG_ASSERT(device != nullptr);

    if ((shaderBytecode.Code == nullptr) &&
        (shaderBytecode.ByteLength == 0) &&
        !compileOptions.EntryPoint.empty()) {

        auto shader = std::make_unique<ShaderMetal>();
        if (auto err = shader->Initialize(device, defaultLibrary, compileOptions); err != nullptr) {
            return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize ShaderMetal"));
        }

        return std::make_tuple(std::move(shader), nullptr);
    }

    auto shader = std::make_unique<ShaderMetal>();
    if (auto err = shader->Initialize(device, shaderBytecode, compileOptions); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize ShaderMetal"));
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
        PixelFormat::R8G8B8A8_UNorm);
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateRenderTarget2D(
    std::int32_t width,
    std::int32_t height,
    bool generateMipmap,
    PixelFormat format) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    POMDOG_ASSERT(device != nullptr);

    const auto levelCount = generateMipmap
                                ? detail::TextureHelper::ComputeMipmapLevelCount(width, height)
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
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize RenderTarget2DMetal"));
    }
    return std::make_tuple(std::move(renderTarget), nullptr);
}

std::tuple<std::shared_ptr<DepthStencilBuffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateDepthStencilBuffer(
    std::int32_t width,
    std::int32_t height,
    PixelFormat depthStencilFormat) noexcept
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
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize DepthStencilBufferMetal"));
    }
    return std::make_tuple(std::move(depthStencilBuffer), nullptr);
}

std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateSamplerState(const SamplerDescriptor& descriptor) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    auto samplerState = std::make_shared<SamplerStateMetal>();
    if (auto err = samplerState->Initialize(device, descriptor); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize SamplerStateMetal"));
    }
    return std::make_tuple(std::move(samplerState), nullptr);
}

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateTexture2D(
    std::int32_t width,
    std::int32_t height) noexcept
{
    return CreateTexture2D(
        width,
        height,
        false,
        PixelFormat::R8G8B8A8_UNorm);
}

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
GraphicsDeviceMetal::CreateTexture2D(
    std::int32_t width,
    std::int32_t height,
    bool mipMap,
    PixelFormat format) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    POMDOG_ASSERT(device != nullptr);

    const auto levelCount = mipMap
                                ? detail::TextureHelper::ComputeMipmapLevelCount(width, height)
                                : 1;

    auto texture = std::make_shared<Texture2DMetal>();
    if (auto err = texture->Initialize(
            device,
            width,
            height,
            levelCount,
            format);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize Texture2DMetal"));
    }

    return std::make_tuple(std::move(texture), nullptr);
}

id<MTLDevice> GraphicsDeviceMetal::GetMTLDevice() noexcept
{
    POMDOG_ASSERT(device != nullptr);
    return device;
}

void GraphicsDeviceMetal::clientSizeChanged(int width, int height) noexcept
{
    presentationParameters.backBufferWidth = width;
    presentationParameters.backBufferHeight = height;
}

} // namespace pomdog::gpu::detail::metal
