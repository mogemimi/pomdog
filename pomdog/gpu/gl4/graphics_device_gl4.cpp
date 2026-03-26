// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/graphics_device_gl4.h"
#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/gpu/backends/command_list_immediate.h"
#include "pomdog/gpu/backends/shader_compile_options.h"
#include "pomdog/gpu/backends/texture_helper.h"
#include "pomdog/gpu/buffer_bind_flags.h"
#include "pomdog/gpu/buffer_desc.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/gl4/buffer_gl4.h"
#include "pomdog/gpu/gl4/depth_stencil_buffer_gl4.h"
#include "pomdog/gpu/gl4/pipeline_state_gl4.h"
#include "pomdog/gpu/gl4/render_target2d_gl4.h"
#include "pomdog/gpu/gl4/sampler_state_gl4.h"
#include "pomdog/gpu/gl4/shader_gl4.h"
#include "pomdog/gpu/gl4/texture2d_gl4.h"
#include "pomdog/gpu/graphics_backend.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/logging/log.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_format.h"

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] MemoryUsage
toMemoryUsage(BufferUsage bufferUsage) noexcept
{
    return (bufferUsage == BufferUsage::Immutable) ? MemoryUsage::GpuOnly : MemoryUsage::CpuToGpu;
}

} // namespace

std::unique_ptr<Error>
GraphicsDeviceGL4::initialize(const PresentationParameters& presentationParametersIn) noexcept
{
    auto version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    Log::Internal(pomdog::format("OpenGL Version: {}", version));

    presentationParameters_ = presentationParametersIn;
    return nullptr;
}

GraphicsBackend
GraphicsDeviceGL4::getBackendKind() const noexcept
{
    return GraphicsBackend::OpenGL4;
}

PresentationParameters
GraphicsDeviceGL4::getPresentationParameters() const noexcept
{
    return presentationParameters_;
}

std::tuple<std::shared_ptr<CommandList>, std::unique_ptr<Error>>
GraphicsDeviceGL4::createCommandList() noexcept
{
    auto commandList = std::make_shared<CommandListImmediate>();
    return std::make_tuple(std::move(commandList), nullptr);
}

std::tuple<std::shared_ptr<Buffer>, std::unique_ptr<Error>>
GraphicsDeviceGL4::createBuffer(const BufferDesc& desc) noexcept
{
    return createBuffer(desc, std::span<const u8>{});
}

std::tuple<std::shared_ptr<Buffer>, std::unique_ptr<Error>>
GraphicsDeviceGL4::createBuffer(const BufferDesc& desc, std::span<const u8> initialData) noexcept
{
    POMDOG_ASSERT(desc.sizeInBytes > 0);

    // Choose GL4 buffer type based on primary bind flag
    if (hasFlag(desc.bindFlags, BufferBindFlags::ConstantBuffer)) {
        auto nativeBuffer = std::make_shared<ConstantBufferGL4>();
        if (auto err = nativeBuffer->initialize(desc, initialData); err != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize ConstantBufferGL4"));
        }
        return std::make_tuple(std::move(nativeBuffer), nullptr);
    }
    else if (hasFlag(desc.bindFlags, BufferBindFlags::IndexBuffer)) {
        auto nativeBuffer = std::make_shared<IndexBufferGL4>();
        if (auto err = nativeBuffer->initialize(desc, initialData); err != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize IndexBufferGL4"));
        }
        return std::make_tuple(std::move(nativeBuffer), nullptr);
    }
    else {
        auto nativeBuffer = std::make_shared<VertexBufferGL4>();
        if (auto err = nativeBuffer->initialize(desc, initialData); err != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize VertexBufferGL4"));
        }
        return std::make_tuple(std::move(nativeBuffer), nullptr);
    }
}

std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceGL4::createVertexBuffer(
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
GraphicsDeviceGL4::createVertexBuffer(
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
GraphicsDeviceGL4::createIndexBuffer(
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
GraphicsDeviceGL4::createIndexBuffer(
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
GraphicsDeviceGL4::createConstantBuffer(
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
GraphicsDeviceGL4::createConstantBuffer(
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
GraphicsDeviceGL4::createPipelineState(const PipelineDesc& descriptor) noexcept
{
    auto pipelineState = std::make_shared<PipelineStateGL4>();
    POMDOG_ASSERT(pipelineState != nullptr);

    if (auto err = pipelineState->initialize(descriptor); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize PipelineStateGL4"));
    }
    return std::make_tuple(std::move(pipelineState), nullptr);
}

std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
GraphicsDeviceGL4::createShader(
    std::span<const u8> shaderBytecode,
    const detail::ShaderCompileOptions& compileOptions) noexcept
{
    switch (compileOptions.profile.pipelineStage) {
    case ShaderPipelineStage::VertexShader: {
        auto shader = std::make_unique<VertexShaderGL4>();
        POMDOG_ASSERT(shader != nullptr);
        if (auto err = shader->initialize(shaderBytecode, compileOptions.reflectionBlob); err != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create VertexShaderGL4"));
        }
        return std::make_tuple(std::move(shader), nullptr);
    }
    case ShaderPipelineStage::PixelShader: {
        auto shader = std::make_unique<PixelShaderGL4>();
        POMDOG_ASSERT(shader != nullptr);
        if (auto err = shader->initialize(shaderBytecode, compileOptions.reflectionBlob); err != nullptr) {
            return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to create PixelShaderGL4"));
        }
        return std::make_tuple(std::move(shader), nullptr);
    }
    }
    return std::make_tuple(nullptr, errors::make("unsupported shader stage"));
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
GraphicsDeviceGL4::createRenderTarget2D(
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
GraphicsDeviceGL4::createRenderTarget2D(
    i32 width,
    i32 height,
    bool generateMipmap,
    PixelFormat format) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    const auto levelCount = generateMipmap
                                ? detail::TextureHelper::computeMipmapLevelCount(width, height)
                                : 1;

    // TODO: MSAA is not implemented yet.
    constexpr int multiSampleCount = 1;

    auto renderTarget = std::make_shared<RenderTarget2DGL4>();
    POMDOG_ASSERT(renderTarget != nullptr);

    if (auto err = renderTarget->initialize(
            width,
            height,
            levelCount,
            format,
            multiSampleCount);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize RenderTarget2DGL4"));
    }
    return std::make_tuple(std::move(renderTarget), nullptr);
}

std::tuple<std::shared_ptr<DepthStencilBuffer>, std::unique_ptr<Error>>
GraphicsDeviceGL4::createDepthStencilBuffer(
    i32 width,
    i32 height,
    PixelFormat depthStencilFormat) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    // TODO: MSAA is not implemented yet.
    constexpr int multiSampleCount = 1;

    auto depthStencilBuffer = std::make_shared<DepthStencilBufferGL4>();
    POMDOG_ASSERT(depthStencilBuffer != nullptr);

    if (auto err = depthStencilBuffer->initialize(
            width,
            height,
            depthStencilFormat,
            multiSampleCount);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize DepthStencilBufferGL4"));
    }
    return std::make_tuple(std::move(depthStencilBuffer), nullptr);
}

std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
GraphicsDeviceGL4::createSamplerState(const SamplerDesc& descriptor) noexcept
{
    auto samplerState = std::make_shared<SamplerStateGL4>();
    POMDOG_ASSERT(samplerState != nullptr);

    if (auto err = samplerState->initialize(descriptor); err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize SamplerStateGL4"));
    }
    return std::make_tuple(std::move(samplerState), nullptr);
}

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
GraphicsDeviceGL4::createTexture2D(
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
GraphicsDeviceGL4::createTexture2D(
    i32 width,
    i32 height,
    bool mipMap,
    PixelFormat format) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    const auto levelCount = mipMap
                                ? detail::TextureHelper::computeMipmapLevelCount(width, height)
                                : 1;

    auto texture = std::make_shared<Texture2DGL4>();
    POMDOG_ASSERT(texture != nullptr);

    if (auto err = texture->initialize(
            width,
            height,
            levelCount,
            format);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::wrap(std::move(err), "failed to initialize Texture2DGL4"));
    }
    return std::make_tuple(std::move(texture), nullptr);
}

void GraphicsDeviceGL4::clientSizeChanged(i32 width, i32 height)
{
    presentationParameters_.backBufferWidth = width;
    presentationParameters_.backBufferHeight = height;
}

} // namespace pomdog::gpu::detail::gl4
