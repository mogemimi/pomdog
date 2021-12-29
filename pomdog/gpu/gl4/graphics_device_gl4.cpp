// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/graphics_device_gl4.h"
#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/gpu/backends/graphics_command_list_immediate.h"
#include "pomdog/gpu/backends/shader_compile_options.h"
#include "pomdog/gpu/backends/texture_helper.h"
#include "pomdog/gpu/buffer_usage.h"
#include "pomdog/gpu/constant_buffer.h"
#include "pomdog/gpu/gl4/buffer_gl4.h"
#include "pomdog/gpu/gl4/depth_stencil_buffer_gl4.h"
#include "pomdog/gpu/gl4/effect_reflection_gl4.h"
#include "pomdog/gpu/gl4/pipeline_state_gl4.h"
#include "pomdog/gpu/gl4/render_target2d_gl4.h"
#include "pomdog/gpu/gl4/sampler_state_gl4.h"
#include "pomdog/gpu/gl4/shader_gl4.h"
#include "pomdog/gpu/gl4/texture2d_gl4.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/shader_language.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/logging/log.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/string_helper.h"

namespace pomdog::gpu::detail::gl4 {

std::unique_ptr<Error>
GraphicsDeviceGL4::Initialize(const PresentationParameters& presentationParametersIn) noexcept
{
    auto version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    Log::Internal(StringHelper::Format("OpenGL Version: %s", version));

    presentationParameters = presentationParametersIn;
    return nullptr;
}

ShaderLanguage GraphicsDeviceGL4::GetSupportedLanguage() const noexcept
{
    return ShaderLanguage::GLSL;
}

PresentationParameters GraphicsDeviceGL4::GetPresentationParameters() const noexcept
{
    return presentationParameters;
}

std::tuple<std::shared_ptr<GraphicsCommandList>, std::unique_ptr<Error>>
GraphicsDeviceGL4::CreateGraphicsCommandList() noexcept
{
    auto commandList = std::make_shared<GraphicsCommandListImmediate>();
    return std::make_tuple(std::move(commandList), nullptr);
}

std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceGL4::CreateVertexBuffer(
    const void* vertices,
    std::size_t vertexCount,
    std::size_t strideBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(vertices != nullptr);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(strideBytes > 0);

    const auto sizeInBytes = vertexCount * strideBytes;

    auto nativeBuffer = std::make_unique<VertexBufferGL4>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->Initialize(vertices, sizeInBytes, bufferUsage); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize VertexBufferGL4"));
    }

    auto vertexBuffer = std::make_shared<VertexBuffer>(std::move(nativeBuffer), vertexCount, strideBytes, bufferUsage);
    return std::make_tuple(std::move(vertexBuffer), nullptr);
}

std::tuple<std::shared_ptr<VertexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceGL4::CreateVertexBuffer(
    std::size_t vertexCount,
    std::size_t strideBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(strideBytes > 0);

    const auto sizeInBytes = vertexCount * strideBytes;

    auto nativeBuffer = std::make_unique<VertexBufferGL4>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->Initialize(sizeInBytes, bufferUsage); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize VertexBufferGL4"));
    }

    auto vertexBuffer = std::make_shared<VertexBuffer>(std::move(nativeBuffer), vertexCount, strideBytes, bufferUsage);
    return std::make_tuple(std::move(vertexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceGL4::CreateIndexBuffer(
    IndexElementSize elementSize,
    const void* indices,
    std::size_t indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(indexCount > 0);

    const auto sizeInBytes = indexCount * detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);

    auto nativeBuffer = std::make_unique<IndexBufferGL4>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->Initialize(indices, sizeInBytes, bufferUsage); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize IndexBufferGL4"));
    }

    auto indexBuffer = std::make_shared<IndexBuffer>(std::move(nativeBuffer), elementSize, indexCount, bufferUsage);
    return std::make_tuple(std::move(indexBuffer), nullptr);
}

std::tuple<std::shared_ptr<IndexBuffer>, std::unique_ptr<Error>>
GraphicsDeviceGL4::CreateIndexBuffer(
    IndexElementSize elementSize,
    std::size_t indexCount,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(indexCount > 0);

    const auto sizeInBytes = indexCount * detail::BufferHelper::ToIndexElementOffsetBytes(elementSize);

    auto nativeBuffer = std::make_unique<IndexBufferGL4>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->Initialize(sizeInBytes, bufferUsage); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize IndexBufferGL4"));
    }

    auto indexBuffer = std::make_shared<IndexBuffer>(std::move(nativeBuffer), elementSize, indexCount, bufferUsage);
    return std::make_tuple(std::move(indexBuffer), nullptr);
}

std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
GraphicsDeviceGL4::CreateConstantBuffer(
    const void* sourceData,
    std::size_t sizeInBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(sizeInBytes > 0);

    auto nativeBuffer = std::make_unique<ConstantBufferGL4>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->Initialize(sourceData, sizeInBytes, bufferUsage); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize ConstantBufferGL4"));
    }

    auto constantBuffer = std::make_shared<ConstantBuffer>(std::move(nativeBuffer), sizeInBytes, bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<ConstantBuffer>, std::unique_ptr<Error>>
GraphicsDeviceGL4::CreateConstantBuffer(
    std::size_t sizeInBytes,
    BufferUsage bufferUsage) noexcept
{
    POMDOG_ASSERT(bufferUsage != BufferUsage::Immutable);
    POMDOG_ASSERT(sizeInBytes > 0);

    auto nativeBuffer = std::make_unique<ConstantBufferGL4>();
    POMDOG_ASSERT(nativeBuffer != nullptr);

    if (auto err = nativeBuffer->Initialize(sizeInBytes, bufferUsage); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize ConstantBufferGL4"));
    }

    auto constantBuffer = std::make_shared<ConstantBuffer>(std::move(nativeBuffer), sizeInBytes, bufferUsage);
    return std::make_tuple(std::move(constantBuffer), nullptr);
}

std::tuple<std::shared_ptr<PipelineState>, std::unique_ptr<Error>>
GraphicsDeviceGL4::CreatePipelineState(const PipelineStateDescriptor& descriptor) noexcept
{
    auto pipelineState = std::make_shared<PipelineStateGL4>();
    POMDOG_ASSERT(pipelineState != nullptr);

    if (auto err = pipelineState->Initialize(descriptor); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize PipelineStateGL4"));
    }
    return std::make_tuple(std::move(pipelineState), nullptr);
}

std::tuple<std::shared_ptr<EffectReflection>, std::unique_ptr<Error>>
GraphicsDeviceGL4::CreateEffectReflection(
    [[maybe_unused]] const PipelineStateDescriptor& descriptor,
    const std::shared_ptr<PipelineState>& pipelineState) noexcept
{
    const auto pipelineStateGL4 = dynamic_cast<PipelineStateGL4*>(pipelineState.get());
    POMDOG_ASSERT(pipelineStateGL4 != nullptr);

    auto effectReflection = std::make_shared<EffectReflectionGL4>();
    POMDOG_ASSERT(effectReflection != nullptr);

    if (auto err = effectReflection->Initialize(pipelineStateGL4->GetShaderProgram()); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize EffectReflectionGL4"));
    }
    return std::make_tuple(std::move(effectReflection), nullptr);
}

std::tuple<std::unique_ptr<Shader>, std::unique_ptr<Error>>
GraphicsDeviceGL4::CreateShader(
    const detail::ShaderBytecode& shaderBytecode,
    const detail::ShaderCompileOptions& compileOptions) noexcept
{
    switch (compileOptions.Profile.PipelineStage) {
    case ShaderPipelineStage::VertexShader: {
        auto shader = std::make_unique<VertexShaderGL4>();
        POMDOG_ASSERT(shader != nullptr);
        if (auto err = shader->Initialize(shaderBytecode); err != nullptr) {
            return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to create VertexShaderGL4"));
        }
        return std::make_tuple(std::move(shader), nullptr);
    }
    case ShaderPipelineStage::PixelShader: {
        auto shader = std::make_unique<PixelShaderGL4>();
        POMDOG_ASSERT(shader != nullptr);
        if (auto err = shader->Initialize(shaderBytecode); err != nullptr) {
            return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to create PixelShaderGL4"));
        }
        return std::make_tuple(std::move(shader), nullptr);
    }
    }
    return std::make_tuple(nullptr, errors::New("unsupported shader stage"));
}

std::tuple<std::shared_ptr<RenderTarget2D>, std::unique_ptr<Error>>
GraphicsDeviceGL4::CreateRenderTarget2D(
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
GraphicsDeviceGL4::CreateRenderTarget2D(
    std::int32_t width,
    std::int32_t height,
    bool generateMipmap,
    SurfaceFormat format) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    const auto levelCount = generateMipmap
        ? detail::TextureHelper::ComputeMipmapLevelCount(width, height)
        : 1;

    // TODO: MSAA is not implemented yet.
    constexpr int multiSampleCount = 1;

    auto renderTarget = std::make_shared<RenderTarget2DGL4>();
    POMDOG_ASSERT(renderTarget != nullptr);

    if (auto err = renderTarget->Initialize(
            width,
            height,
            levelCount,
            format,
            multiSampleCount);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize RenderTarget2DGL4"));
    }
    return std::make_tuple(std::move(renderTarget), nullptr);
}

std::tuple<std::shared_ptr<DepthStencilBuffer>, std::unique_ptr<Error>>
GraphicsDeviceGL4::CreateDepthStencilBuffer(
    std::int32_t width,
    std::int32_t height,
    SurfaceFormat depthStencilFormat) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    // TODO: MSAA is not implemented yet.
    constexpr int multiSampleCount = 1;

    auto depthStencilBuffer = std::make_shared<DepthStencilBufferGL4>();
    POMDOG_ASSERT(depthStencilBuffer != nullptr);

    if (auto err = depthStencilBuffer->Initialize(
            width,
            height,
            depthStencilFormat,
            multiSampleCount);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize DepthStencilBufferGL4"));
    }
    return std::make_tuple(std::move(depthStencilBuffer), nullptr);
}

std::tuple<std::shared_ptr<SamplerState>, std::unique_ptr<Error>>
GraphicsDeviceGL4::CreateSamplerState(const SamplerDescriptor& descriptor) noexcept
{
    auto samplerState = std::make_shared<SamplerStateGL4>();
    POMDOG_ASSERT(samplerState != nullptr);

    if (auto err = samplerState->Initialize(descriptor); err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize SamplerStateGL4"));
    }
    return std::make_tuple(std::move(samplerState), nullptr);
}

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
GraphicsDeviceGL4::CreateTexture2D(
    std::int32_t width,
    std::int32_t height) noexcept
{
    return CreateTexture2D(
        width,
        height,
        false,
        SurfaceFormat::R8G8B8A8_UNorm);
}

std::tuple<std::shared_ptr<gpu::Texture2D>, std::unique_ptr<Error>>
GraphicsDeviceGL4::CreateTexture2D(
    std::int32_t width,
    std::int32_t height,
    bool mipMap,
    SurfaceFormat format) noexcept
{
    POMDOG_ASSERT(width > 0);
    POMDOG_ASSERT(height > 0);

    const auto levelCount = mipMap
        ? detail::TextureHelper::ComputeMipmapLevelCount(width, height)
        : 1;

    auto texture = std::make_shared<Texture2DGL4>();
    POMDOG_ASSERT(texture != nullptr);

    if (auto err = texture->Initialize(
            width,
            height,
            levelCount,
            format);
        err != nullptr) {
        return std::make_tuple(nullptr, errors::Wrap(std::move(err), "failed to initialize Texture2DGL4"));
    }
    return std::make_tuple(std::move(texture), nullptr);
}

void GraphicsDeviceGL4::ClientSizeChanged(int width, int height)
{
    presentationParameters.BackBufferWidth = width;
    presentationParameters.BackBufferHeight = height;
}

} // namespace pomdog::gpu::detail::gl4
