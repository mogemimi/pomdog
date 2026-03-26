// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/metal/graphics_device_metal.h"
#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/gpu/backends/command_list_immediate.h"
#include "pomdog/gpu/backends/shader_compile_options.h"
#include "pomdog/gpu/backends/texture_helper.h"
#include "pomdog/gpu/buffer_bind_flags.h"
#include "pomdog/gpu/buffer_desc.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/graphics_backend.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/metal/buffer_metal.h"
#include "pomdog/gpu/metal/depth_stencil_buffer_metal.h"
#include "pomdog/gpu/metal/pipeline_state_metal.h"
#include "pomdog/gpu/metal/render_target2d_metal.h"
#include "pomdog/gpu/metal/sampler_state_metal.h"
#include "pomdog/gpu/metal/shader_metal.h"
#include "pomdog/gpu/metal/texture2d_metal.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/texture_desc.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Metal/Metal.h>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::metal {
namespace {

[[nodiscard]] MemoryUsage
toMemoryUsage(BufferUsage bufferUsage) noexcept
{
    return (bufferUsage == BufferUsage::Immutable) ? MemoryUsage::GpuOnly : MemoryUsage::CpuToGpu;
}

} // namespace

std::unique_ptr<Error>
GraphicsDeviceMetal::initialize(
    const PresentationParameters& presentationParametersIn,
    std::shared_ptr<const FrameCounter> frameCounter) noexcept
{
    presentationParameters = presentationParametersIn;

    frameCounter_ = std::move(frameCounter);
    if (frameCounter_ == nullptr) {
        return errors::make("frameCounter_ must be != nullptr");
    }

    device = MTLCreateSystemDefaultDevice();
    if (device == nullptr) {
        return errors::make("MTLCreateSystemDefaultDevice() failed");
    }

    // NOTE: Load all the shader files with a metal file extension in the project
    defaultLibrary = [device newDefaultLibrary];

    return nullptr;
}

GraphicsDeviceMetal::~GraphicsDeviceMetal() = default;

GraphicsBackend
GraphicsDeviceMetal::getBackendKind() const noexcept
{
    return GraphicsBackend::Metal;
}

PresentationParameters
GraphicsDeviceMetal::getPresentationParameters() const noexcept
{
    return presentationParameters;
}

std::tuple<std::shared_ptr<CommandList>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createCommandList() noexcept
{
    auto commandList = std::make_shared<CommandListImmediate>();
    return std::make_tuple(std::move(commandList), nullptr);
}

std::tuple<std::shared_ptr<Buffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createBuffer(const BufferDesc& desc) noexcept
{
    return createBuffer(desc, std::span<const u8>{});
}

std::tuple<std::shared_ptr<Buffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createBuffer(const BufferDesc& desc, std::span<const u8> initialData) noexcept
{
    POMDOG_ASSERT(desc.sizeInBytes > 0);
    POMDOG_ASSERT(device != nullptr);
    POMDOG_ASSERT(frameCounter_ != nullptr);

    auto nativeBuffer = std::make_shared<BufferMetal>();
    if (auto err = nativeBuffer->initialize(
            frameCounter_,
            device,
            desc,
            initialData);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize BufferMetal"));
    }
    POMDOG_ASSERT(nativeBuffer != nullptr);

    return std::make_tuple(std::move(nativeBuffer), nullptr);
}

std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createVertexBuffer(
    const void* vertices,
    u32 vertexCount,
    u32 strideBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(vertices != nullptr);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(strideBytes > 0);

    const auto sizeInBytes = vertexCount * strideBytes;

    BufferDesc desc;
    desc.sizeInBytes = sizeInBytes;
    desc.memoryUsage = toMemoryUsage(bufferUsage);
    desc.bindFlags = BufferBindFlags::VertexBuffer;

    auto [buffer, err] = createBuffer(
        desc, std::span<const u8>(static_cast<const u8*>(vertices), sizeInBytes));
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create vertex buffer"));
    }

    auto vertexBuffer = std::make_shared<VertexBuffer>(
        std::move(buffer), vertexCount, strideBytes, bufferUsage);
    return std::make_tuple(std::move(vertexBuffer), nullptr);
}

std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createVertexBuffer(
    u32 vertexCount,
    u32 strideBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(strideBytes > 0);

    const auto sizeInBytes = vertexCount * strideBytes;

    BufferDesc desc;
    desc.sizeInBytes = sizeInBytes;
    desc.memoryUsage = toMemoryUsage(bufferUsage);
    desc.bindFlags = BufferBindFlags::VertexBuffer;

    auto [buffer, err] = createBuffer(desc);
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create vertex buffer"));
    }

    auto vertexBuffer = std::make_shared<VertexBuffer>(
        std::move(buffer), vertexCount, strideBytes, bufferUsage);
    return std::make_tuple(std::move(vertexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createIndexBuffer(
    IndexFormat elementSize,
    const void* indices,
    u32 indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(indexCount > 0);

    const auto sizeInBytes = indexCount * detail::BufferHelper::toIndexElementOffsetBytes(elementSize);

    BufferDesc desc;
    desc.sizeInBytes = sizeInBytes;
    desc.memoryUsage = toMemoryUsage(bufferUsage);
    desc.bindFlags = BufferBindFlags::IndexBuffer;

    auto [buffer, err] = createBuffer(
        desc, std::span<const u8>(static_cast<const u8*>(indices), sizeInBytes));
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create index buffer"));
    }

    auto indexBuffer = std::make_shared<IndexBuffer>(
        std::move(buffer), elementSize, indexCount, bufferUsage);
    return std::make_tuple(std::move(indexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createIndexBuffer(
    IndexFormat elementSize,
    u32 indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(indexCount > 0);

    const auto sizeInBytes = indexCount * detail::BufferHelper::toIndexElementOffsetBytes(elementSize);

    BufferDesc desc;
    desc.sizeInBytes = sizeInBytes;
    desc.memoryUsage = toMemoryUsage(bufferUsage);
    desc.bindFlags = BufferBindFlags::IndexBuffer;

    auto [buffer, err] = createBuffer(desc);
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create index buffer"));
    }

    auto indexBuffer = std::make_shared<IndexBuffer>(
        std::move(buffer), elementSize, indexCount, bufferUsage);
    return std::make_tuple(std::move(indexBuffer), nullptr);
}

std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createConstantBuffer(
    const void* sourceData,
    u32 sizeInBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(sizeInBytes > 0);

    BufferDesc desc;
    desc.sizeInBytes = sizeInBytes;
    desc.memoryUsage = toMemoryUsage(bufferUsage);
    desc.bindFlags = BufferBindFlags::ConstantBuffer;

    auto [buffer, err] = createBuffer(
        desc, std::span<const u8>(static_cast<const u8*>(sourceData), sizeInBytes));
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create constant buffer"));
    }

    auto constantBuffer = std::make_shared<ConstantBuffer>(
        std::move(buffer), sizeInBytes, bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createConstantBuffer(
    u32 sizeInBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(sizeInBytes > 0);

    BufferDesc desc;
    desc.sizeInBytes = sizeInBytes;
    desc.memoryUsage = toMemoryUsage(bufferUsage);
    desc.bindFlags = BufferBindFlags::ConstantBuffer;

    auto [buffer, err] = createBuffer(desc);
    if (err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create constant buffer"));
    }

    auto constantBuffer = std::make_shared<ConstantBuffer>(
        std::move(buffer), sizeInBytes, bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<PipelineState>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createPipelineState(const PipelineDesc& descriptor) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    auto pipelineState = std::make_shared<PipelineStateMetal>();
    if (auto err = pipelineState->initialize(device, descriptor); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize PipelineStateMetal"));
    }
    return std::make_tuple(std::move(pipelineState), nullptr);
}

std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createShader(
    std::span<const u8> shaderBytecode,
    const detail::ShaderCompileOptions& compileOptions) noexcept
{
    POMDOG_ASSERT(device != nullptr);

    if (shaderBytecode.empty() &&
        !compileOptions.entryPoint.empty()) {

        auto shader = std::make_unique<ShaderMetal>();
        if (auto err = shader->initialize(device, defaultLibrary, compileOptions); err != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize ShaderMetal"));
        }

        return std::make_tuple(std::move(shader), nullptr);
    }

    auto shader = std::make_unique<ShaderMetal>();
    if (auto err = shader->initialize(device, shaderBytecode, compileOptions); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize ShaderMetal"));
    }

    return std::make_tuple(std::move(shader), nullptr);
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createRenderTarget2D(
    i32 width,
    i32 height) noexcept
{
    return createRenderTarget2D(
        width,
        height,
        false,
        PixelFormat::R8G8B8A8_UNorm);
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createRenderTarget2D(
    i32 width,
    i32 height,
    bool generateMipmap,
    PixelFormat format) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    POMDOG_ASSERT(device != nullptr);

    const auto levelCount = generateMipmap
                                ? detail::TextureHelper::computeMipmapLevelCount(width, height)
                                : 1;

    // TODO: MSAA is not implemented yet.
    constexpr int multiSampleCount = 1;

    auto renderTarget = std::make_shared<RenderTarget2DMetal>();
    if (auto err = renderTarget->initialize(
            device,
            width,
            height,
            levelCount,
            format,
            multiSampleCount);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize RenderTarget2DMetal"));
    }
    return std::make_tuple(std::move(renderTarget), nullptr);
}

std::tuple<std::shared_ptr<DepthStencilBuffer>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createDepthStencilBuffer(
    i32 width,
    i32 height,
    PixelFormat depthStencilFormat) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    POMDOG_ASSERT(device != nullptr);

    // TODO: MSAA is not implemented yet.
    constexpr int multiSampleCount = 1;

    auto depthStencilBuffer = std::make_shared<DepthStencilBufferMetal>();
    if (auto err = depthStencilBuffer->initialize(
            device,
            width,
            height,
            depthStencilFormat,
            multiSampleCount);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize DepthStencilBufferMetal"));
    }
    return std::make_tuple(std::move(depthStencilBuffer), nullptr);
}

std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createSamplerState(const SamplerDesc& descriptor) noexcept
{
    POMDOG_ASSERT(device != nullptr);
    auto samplerState = std::make_shared<SamplerStateMetal>();
    if (auto err = samplerState->initialize(device, descriptor); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize SamplerStateMetal"));
    }
    return std::make_tuple(std::move(samplerState), nullptr);
}

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createTexture2D(
    i32 width,
    i32 height) noexcept
{
    return createTexture2D(
        width,
        height,
        false,
        PixelFormat::R8G8B8A8_UNorm);
}

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createTexture2D(
    i32 width,
    i32 height,
    bool mipMap,
    PixelFormat format) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);
    POMDOG_ASSERT(device != nullptr);

    const auto levelCount = mipMap
                                ? detail::TextureHelper::computeMipmapLevelCount(width, height)
                                : 1;

    auto texture = std::make_shared<Texture2DMetal>();
    if (auto err = texture->initialize(
            device,
            width,
            height,
            levelCount,
            format);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize Texture2DMetal"));
    }

    return std::make_tuple(std::move(texture), nullptr);
}

std::tuple<std::shared_ptr<gpu::Texture>, std::unique_ptr<Error>>
GraphicsDeviceMetal::createTexture(const TextureDesc& desc) noexcept
{
    POMDOG_ASSERT(desc.width > 0);
    POMDOG_ASSERT(desc.height > 0);
    POMDOG_ASSERT(device != nullptr);

    const auto hasRenderTarget = (static_cast<u8>(desc.usage) & static_cast<u8>(TextureUsage::RenderTarget)) != 0;
    const auto hasDepthStencil = (static_cast<u8>(desc.usage) & static_cast<u8>(TextureUsage::DepthStencil)) != 0;

    if (hasDepthStencil) {
        // TODO: MSAA is not implemented yet.
        constexpr int multiSampleCount = 1;

        auto depthStencil = std::make_shared<DepthStencilBufferMetal>();
        if (auto err = depthStencil->initialize(
                device,
                desc.width,
                desc.height,
                desc.format,
                multiSampleCount);
            err != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize DepthStencilBufferMetal"));
        }
        return std::make_tuple(std::move(depthStencil), nullptr);
    }

    if (hasRenderTarget) {
        // TODO: MSAA is not implemented yet.
        constexpr int multiSampleCount = 1;

        auto renderTarget = std::make_shared<RenderTarget2DMetal>();
        if (auto err = renderTarget->initialize(
                device,
                desc.width,
                desc.height,
                desc.mipLevels,
                desc.format,
                multiSampleCount);
            err != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize RenderTarget2DMetal"));
        }
        return std::make_tuple(std::move(renderTarget), nullptr);
    }

    auto texture = std::make_shared<Texture2DMetal>();
    if (auto err = texture->initialize(
            device,
            desc.width,
            desc.height,
            desc.mipLevels,
            desc.format);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize Texture2DMetal"));
    }
    return std::make_tuple(std::move(texture), nullptr);
}

[[nodiscard]] id<MTLDevice>
GraphicsDeviceMetal::getMTLDevice() noexcept
{
    POMDOG_ASSERT(device != nullptr);
    return device;
}

void GraphicsDeviceMetal::clientSizeChanged(i32 width, i32 height) noexcept
{
    presentationParameters.backBufferWidth = width;
    presentationParameters.backBufferHeight = height;
}

} // namespace pomdog::gpu::detail::metal
