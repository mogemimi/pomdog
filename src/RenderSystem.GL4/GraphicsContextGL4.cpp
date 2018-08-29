// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "GraphicsContextGL4.hpp"
#include "BufferGL4.hpp"
#include "ErrorChecker.hpp"
#include "InputLayoutGL4.hpp"
#include "OpenGLContext.hpp"
#include "PipelineStateGL4.hpp"
#include "RenderTarget2DGL4.hpp"
#include "SamplerStateGL4.hpp"
#include "TypesafeHelperGL4.hpp"
#include "../Basic/Unreachable.hpp"
#include "../RenderSystem/BufferHelper.hpp"
#include "../RenderSystem/GraphicsCapabilities.hpp"
#include "../RenderSystem/GraphicsCommandListImmediate.hpp"
#include "../RenderSystem/NativeRenderTarget2D.hpp"
#include "../RenderSystem/NativeTexture2D.hpp"
#include "../Utility/ScopeGuard.hpp"
#include "Pomdog/Application/GameWindow.hpp"
#include "Pomdog/Basic/Platform.hpp"
#include "Pomdog/Graphics/IndexBuffer.hpp"
#include "Pomdog/Graphics/PrimitiveTopology.hpp"
#include "Pomdog/Graphics/RenderPass.hpp"
#include "Pomdog/Graphics/RenderTarget2D.hpp"
#include "Pomdog/Graphics/Texture2D.hpp"
#include "Pomdog/Graphics/VertexBuffer.hpp"
#include "Pomdog/Graphics/Viewport.hpp"
#include "Pomdog/Logging/Log.hpp"
#include "Pomdog/Math/Color.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/Vector4.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include "Pomdog/Utility/Exception.hpp"
#include "Pomdog/Utility/StringHelper.hpp"
#include <array>
#include <cmath>
#include <limits>
#include <utility>

namespace Pomdog {
namespace Detail {
namespace GL4 {
namespace {

GLenum ToPrimitiveTopology(PrimitiveTopology primitiveTopology) noexcept
{
    switch (primitiveTopology) {
    case PrimitiveTopology::TriangleList: return GL_TRIANGLES;
    case PrimitiveTopology::TriangleStrip: return GL_TRIANGLE_STRIP;
    case PrimitiveTopology::LineList: return GL_LINES;
    case PrimitiveTopology::LineStrip: return GL_LINE_STRIP;
    }
    POMDOG_UNREACHABLE("Unsupported primitive topology");
}

GLenum ToIndexElementType(IndexElementSize indexElementSize) noexcept
{
    static_assert(sizeof(GLushort) == 2, "GLushort is not SixteenBits.");
    static_assert(sizeof(GLuint) == 4, "GLuint is not ThirtyTwoBits.");

    switch (indexElementSize) {
    case IndexElementSize::SixteenBits: return GL_UNSIGNED_SHORT;
    case IndexElementSize::ThirtyTwoBits: return GL_UNSIGNED_INT;
    }
    POMDOG_UNREACHABLE("Unsupported index element size");
}

template <typename T>
GLenum ToTextureUnitIndexGL4(T index) noexcept
{
    static_assert(std::is_integral<T>::value, "T is an integral type.");
    static_assert(GL_TEXTURE0 == (GL_TEXTURE0 + 0), "");
    static_assert(GL_TEXTURE1 == (GL_TEXTURE0 + 1), "");
    static_assert(GL_TEXTURE2 == (GL_TEXTURE0 + 2), "");
    static_assert(GL_TEXTURE3 == (GL_TEXTURE0 + 3), "");
    static_assert(GL_TEXTURE4 == (GL_TEXTURE0 + 4), "");
    static_assert(GL_TEXTURE5 == (GL_TEXTURE0 + 5), "");
    static_assert(GL_TEXTURE6 == (GL_TEXTURE0 + 6), "");
    static_assert(GL_TEXTURE7 == (GL_TEXTURE0 + 7), "");

    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < 128);

    return static_cast<GLenum>(GL_TEXTURE0 + index);
}

template <typename T>
GLenum ToColorAttachment(T index) noexcept
{
    static_assert(std::is_integral<T>::value, "T is an integral type.");
    static_assert(GL_COLOR_ATTACHMENT0 == (GL_COLOR_ATTACHMENT0 + 0), "");
    static_assert(GL_COLOR_ATTACHMENT1 == (GL_COLOR_ATTACHMENT0 + 1), "");
    static_assert(GL_COLOR_ATTACHMENT2 == (GL_COLOR_ATTACHMENT0 + 2), "");
    static_assert(GL_COLOR_ATTACHMENT3 == (GL_COLOR_ATTACHMENT0 + 3), "");
    static_assert(GL_COLOR_ATTACHMENT4 == (GL_COLOR_ATTACHMENT0 + 4), "");
    static_assert(GL_COLOR_ATTACHMENT5 == (GL_COLOR_ATTACHMENT0 + 5), "");
    static_assert(GL_COLOR_ATTACHMENT6 == (GL_COLOR_ATTACHMENT0 + 6), "");
    static_assert(GL_COLOR_ATTACHMENT7 == (GL_COLOR_ATTACHMENT0 + 7), "");

    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < 16);
    return static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + index);
}

std::optional<FrameBufferGL4> CreateFrameBuffer()
{
    auto const prevFrameBuffer = TypesafeHelperGL4::Get<FrameBufferGL4>();
    ScopeGuard scope([&prevFrameBuffer] {
        glBindFramebuffer(GL_FRAMEBUFFER, prevFrameBuffer.value);
    });

    FrameBufferGL4 frameBuffer;
    glGenFramebuffers(1, frameBuffer.Data());
    POMDOG_CHECK_ERROR_GL4("glGenFramebuffers");

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.value);

    // Check framebuffer
    auto const status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (GL_FRAMEBUFFER_UNSUPPORTED == status) {
        return std::nullopt;
    }

    return std::move(frameBuffer);
}

void ApplyTexture2D(int index, const Texture2DObjectGL4& textureObject)
{
#if defined(DEBUG) && !defined(NDEBUG)
    {
        static const auto MaxCombinedTextureImageUnits = ([] {
            GLint units = 0;
            glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &units);
            return units;
        })();
        POMDOG_ASSERT(index < MaxCombinedTextureImageUnits);
    }
#endif

    glActiveTexture(ToTextureUnitIndexGL4(index));
    POMDOG_CHECK_ERROR_GL4("glActiveTexture");

    glBindTexture(GL_TEXTURE_2D, textureObject.value);
    POMDOG_CHECK_ERROR_GL4("glBindTexture");
}

void ApplyConstantBuffer(int slotIndex, const ConstantBufferGL4& buffer)
{
    POMDOG_ASSERT(slotIndex >= 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, slotIndex, buffer.GetBuffer());
    POMDOG_CHECK_ERROR_GL4("glBindBufferBase");
}

void SetViewport(
    const Viewport& viewport,
    const std::weak_ptr<GameWindow>& gameWindow,
    bool useBackBuffer)
{
    POMDOG_ASSERT(viewport.Width > 0);
    POMDOG_ASSERT(viewport.Height > 0);

    GLint viewportY = viewport.TopLeftY;

    if (useBackBuffer) {
        if (auto window = gameWindow.lock()) {
            viewportY = window->GetClientBounds().Height - (viewport.TopLeftY + viewport.Height);
        }
    }

    glViewport(viewport.TopLeftX, viewportY, viewport.Width, viewport.Height);
    POMDOG_CHECK_ERROR_GL4("glViewport");

    static_assert(std::is_same<GLfloat, decltype(viewport.MinDepth)>::value
        && std::is_same<GLfloat, decltype(viewport.MaxDepth)>::value,
        "NOTE: You can use glDepthRange instead of glDepthRangef");

    POMDOG_ASSERT(!std::isinf(viewport.MinDepth));
    POMDOG_ASSERT(!std::isinf(viewport.MaxDepth));
    POMDOG_ASSERT(!std::isnan(viewport.MinDepth));
    POMDOG_ASSERT(!std::isnan(viewport.MaxDepth));

    // NOTE: The MinDepth and MaxDepth must be between 0.0 and 1.0, respectively.
    // Please see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDepthRange.xhtml
    POMDOG_ASSERT((0.0f <= viewport.MinDepth) && (viewport.MinDepth <= 1.0f));
    POMDOG_ASSERT((0.0f <= viewport.MaxDepth) && (viewport.MaxDepth <= 1.0f));

    glDepthRangef(viewport.MinDepth, viewport.MaxDepth);
    POMDOG_CHECK_ERROR_GL4("glDepthRangef");
}

void SetScissorRectangle(
    const Rectangle& rectangle,
    const std::weak_ptr<GameWindow>& gameWindow,
    bool useBackBuffer)
{
    POMDOG_ASSERT(rectangle.Width > 0);
    POMDOG_ASSERT(rectangle.Height > 0);

    GLint lowerLeftCornerY = rectangle.Y;

    if (useBackBuffer) {
        if (auto window = gameWindow.lock()) {
            lowerLeftCornerY = window->GetClientBounds().Height - (rectangle.Y + rectangle.Height);
        }
    }

    glScissor(rectangle.X, lowerLeftCornerY, rectangle.Width, rectangle.Height);
    POMDOG_CHECK_ERROR_GL4("glScissor");
}

void SetRenderTarget(
    const std::optional<FrameBufferGL4>& frameBuffer,
    std::vector<std::shared_ptr<RenderTarget2DGL4>> & renderTargets)
{
    POMDOG_ASSERT(frameBuffer);

    // Bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->value);
    POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");

    // Unbind render targets
    int index = 0;
    for (const auto& renderTarget : renderTargets) {
        POMDOG_ASSERT(renderTarget);
        renderTarget->UnbindFromFramebuffer(ToColorAttachment(index));
        ++index;
    }
    renderTargets.clear();

    // Unbind depth stencil buffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    POMDOG_CHECK_ERROR_GL4("glFramebufferRenderbuffer");

    // Bind default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");
}

void SetRenderTargets(
    const std::optional<FrameBufferGL4>& frameBuffer,
    std::vector<std::shared_ptr<RenderTarget2DGL4>> & renderTargets,
    const std::vector<std::shared_ptr<RenderTarget2D>>& renderTargetViewsIn)
{
    POMDOG_ASSERT(!renderTargetViewsIn.empty());
    POMDOG_ASSERT(frameBuffer);

    // Bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->value);
    POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");

    // Unbind render targets
    {
        int index = 0;
        for (const auto& renderTarget : renderTargets) {
            POMDOG_ASSERT(renderTarget);
            renderTarget->UnbindFromFramebuffer(ToColorAttachment(index));
            ++index;
        }
        renderTargets.clear();
    }

    // Unbind depth stencil buffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    POMDOG_CHECK_ERROR_GL4("glFramebufferRenderbuffer");

    std::vector<GLenum> attachments;
    attachments.reserve(renderTargetViewsIn.size());
    renderTargets.reserve(renderTargetViewsIn.size());

    // Attach textures
    int index = 0;
    for (const auto& renderTarget : renderTargetViewsIn) {
        POMDOG_ASSERT(renderTarget);

        auto const nativeRenderTarget = static_cast<RenderTarget2DGL4*>(renderTarget->GetNativeRenderTarget2D());
        POMDOG_ASSERT(nativeRenderTarget != nullptr);
        POMDOG_ASSERT(nativeRenderTarget == dynamic_cast<RenderTarget2DGL4*>(renderTarget->GetNativeRenderTarget2D()));

        POMDOG_ASSERT(nativeRenderTarget);
        nativeRenderTarget->BindToFramebuffer(ToColorAttachment(index));

        renderTargets.emplace_back(renderTarget, nativeRenderTarget);
        attachments.push_back(ToColorAttachment(index));
        ++index;
    }

    // Attach depth stencil buffer
    {
        POMDOG_ASSERT(renderTargets.front());
        const auto& renderTarget = renderTargets.front();

        POMDOG_ASSERT(renderTarget);
        renderTarget->BindDepthStencilBuffer();
    }

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        // FUS RO DAH!
        POMDOG_THROW_EXCEPTION(std::runtime_error,
            "Failed to make complete framebuffer.");
    }

    POMDOG_ASSERT(!attachments.empty());
    POMDOG_ASSERT(attachments.size() <= static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()));
    glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());
    POMDOG_CHECK_ERROR_GL4("glDrawBuffers");
}

#if defined(DEBUG) && !defined(NDEBUG)
void CheckUnbindingRenderTargetsError(
    const std::vector<std::weak_ptr<RenderTarget2D>>& renderTargets,
    const std::vector<std::weak_ptr<Texture>>& textures)
{
    for (auto & renderTarget: renderTargets) {
        for (auto & texture: textures) {
            if (!renderTarget.expired() && !texture.expired()) {
                POMDOG_ASSERT(renderTarget.lock() != texture.lock());
            }
        }
    }
}
#endif

const GLvoid* ComputeStartIndexLocationPointer(
    IndexElementSize indexElementSize,
    std::size_t startIndexLocation) noexcept
{
    using Detail::BufferHelper::ToIndexElementOffsetBytes;
    auto offsetBytes = startIndexLocation * ToIndexElementOffsetBytes(indexElementSize);
    return reinterpret_cast<const GLvoid*>(offsetBytes);
}

} // unnamed namespace

template<> struct TypesafeHelperGL4::Traits<FrameBufferGL4> {
    constexpr static GLenum BufferBinding = GL_FRAMEBUFFER_BINDING;
};

// MARK: - GraphicsContextGL4

GraphicsContextGL4::GraphicsContextGL4(
    const std::shared_ptr<OpenGLContext>& openGLContextIn,
    std::weak_ptr<GameWindow> windowIn)
    : nativeContext(openGLContextIn)
    , gameWindow(std::move(windowIn))
    , needToApplyInputLayout(true)
    , needToApplyPipelineState(true)
{
    auto version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    Log::Internal(StringHelper::Format("OpenGL Version: %s", version));

    auto capabilities = GetCapabilities();
    if (capabilities.SamplerSlotCount > 0) {
        textures.resize(capabilities.SamplerSlotCount);
    }

    glFrontFace(GL_CW);
    POMDOG_CHECK_ERROR_GL4("glFrontFace");

    frameBuffer = CreateFrameBuffer();
    primitiveTopology = ToPrimitiveTopology(PrimitiveTopology::TriangleList);

    // NOTE: Set default values for graphics context
    this->SetBlendFactor(Color::White);

#if defined(DEBUG) && !defined(NDEBUG)
    auto graphicsCapbilities = this->GetCapabilities();

    POMDOG_ASSERT(graphicsCapbilities.SamplerSlotCount > 0);
    weakTextures.resize(graphicsCapbilities.SamplerSlotCount);
#endif
}

GraphicsContextGL4::~GraphicsContextGL4()
{
    pipelineState.reset();
    vertexBuffers.clear();
    indexBuffer.reset();
    textures.clear();
    renderTargets.clear();

    if (frameBuffer) {
        glDeleteFramebuffers(1, frameBuffer->Data());
        POMDOG_CHECK_ERROR_GL4("glDeleteFramebuffers");
        frameBuffer = std::nullopt;
    }

    nativeContext.reset();
    gameWindow.reset();
}

void GraphicsContextGL4::ExecuteCommandLists(
    const std::vector<std::shared_ptr<GraphicsCommandListImmediate>>& commandLists)
{
    for (auto & commandList : commandLists) {
        POMDOG_ASSERT(commandList);
        commandList->ExecuteImmediate(*this);
    }
}

void GraphicsContextGL4::Present()
{
    nativeContext->SwapBuffers();
    POMDOG_CHECK_ERROR_GL4("SwapBuffers");
}

void GraphicsContextGL4::ApplyPipelineState()
{
    POMDOG_ASSERT(pipelineState);

    if (needToApplyInputLayout) {
        // Bind input-layout to the input-assembler stage:
        auto inputLayout = pipelineState->GetInputLayout();

        POMDOG_ASSERT(inputLayout);
        POMDOG_ASSERT(!vertexBuffers.empty());
        inputLayout->Apply(vertexBuffers);

        needToApplyInputLayout = false;
    }

    if (needToApplyPipelineState) {
        // Use shader program:
        POMDOG_ASSERT(pipelineState);
        pipelineState->ApplyShaders();

        needToApplyPipelineState = false;
    }
}

void GraphicsContextGL4::EmulateStartInstanceLocation(std::size_t startInstanceLocation)
{
    if (startInstanceLocation == 0) {
        // NOTE: nothing to do
        return;
    }

    auto newVertexBuffers = vertexBuffers;
    for (size_t i = 1; i < newVertexBuffers.size(); i++) {
        // NOTE: `i >= 1` is equality to instanced vertex buffer.
        auto& binding = newVertexBuffers[i];
        const auto strideBytes = binding.VertexBuffer->GetStrideBytes();
        POMDOG_ASSERT(strideBytes > 0);
        binding.VertexOffset += (strideBytes * startInstanceLocation);
    }

    auto inputLayout = pipelineState->GetInputLayout();
    POMDOG_ASSERT(inputLayout);
    POMDOG_ASSERT(!newVertexBuffers.empty());

    // NOTE: The following code is a hack.
    inputLayout->Apply(newVertexBuffers);
    needToApplyInputLayout = true;
}

void GraphicsContextGL4::Draw(
    std::size_t vertexCount,
    std::size_t startVertexLocation)
{
#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets, weakTextures);
#endif

    ApplyPipelineState();

    // Draw
    POMDOG_ASSERT(!vertexBuffers.empty());
    POMDOG_ASSERT(vertexBuffers.front().VertexBuffer);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(vertexCount <= vertexBuffers.front().VertexBuffer->GetVertexCount());

    glDrawArrays(
        primitiveTopology.value,
        static_cast<GLint>(startVertexLocation),
        static_cast<GLsizei>(vertexCount));
    POMDOG_CHECK_ERROR_GL4("glDrawArrays");
}

void GraphicsContextGL4::DrawIndexed(
    std::size_t indexCount,
    std::size_t startIndexLocation)
{
#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets, weakTextures);
#endif

    ApplyPipelineState();

    // Bind index buffer
    POMDOG_ASSERT(indexBuffer);
    auto indexBufferGL = dynamic_cast<IndexBufferGL4*>(indexBuffer->NativeIndexBuffer());
    POMDOG_ASSERT(indexBufferGL != nullptr);
    indexBufferGL->BindBuffer();

    // Draw
    POMDOG_ASSERT(indexBuffer);
    POMDOG_ASSERT(indexCount > 0);
    POMDOG_ASSERT(indexCount <= indexBuffer->GetIndexCount());

    const auto indexElementSize = indexBuffer->GetElementSize();

    glDrawElements(
        primitiveTopology.value,
        static_cast<GLsizei>(indexCount),
        ToIndexElementType(indexElementSize),
        ComputeStartIndexLocationPointer(indexElementSize, startIndexLocation));
    POMDOG_CHECK_ERROR_GL4("glDrawElements");
}

void GraphicsContextGL4::DrawInstanced(
    std::size_t vertexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startVertexLocation,
    std::size_t startInstanceLocation)
{
#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets, weakTextures);
#endif

    ApplyPipelineState();

    // Draw
    POMDOG_ASSERT(!vertexBuffers.empty());
    POMDOG_ASSERT(vertexBuffers.front().VertexBuffer);
    POMDOG_ASSERT(vertexCountPerInstance > 0);
    POMDOG_ASSERT(vertexCountPerInstance <= vertexBuffers.front().VertexBuffer->GetVertexCount());
    POMDOG_ASSERT(instanceCount > 0);
    POMDOG_ASSERT(instanceCount <= static_cast<decltype(instanceCount)>(std::numeric_limits<GLsizei>::max()));

#if defined(POMDOG_PLATFORM_MACOSX)
    // NOTE:
    // 'glDrawArraysInstancedBaseInstance' is supported in OpenGL 4.2 and later.
    // But unfortunately, macOS Sierra (latest version of Mac 2016) still uses OpenGL 4.1.
    EmulateStartInstanceLocation(startInstanceLocation);
    glDrawArraysInstanced(
        primitiveTopology.value,
        static_cast<GLint>(startVertexLocation),
        static_cast<GLsizei>(vertexCountPerInstance),
        static_cast<GLsizei>(instanceCount));
    POMDOG_CHECK_ERROR_GL4("glDrawArraysInstanced");
#else
    glDrawArraysInstancedBaseInstance(
        primitiveTopology.value,
        static_cast<GLint>(startVertexLocation),
        static_cast<GLsizei>(vertexCountPerInstance),
        static_cast<GLsizei>(instanceCount),
        static_cast<GLuint>(startInstanceLocation));
    POMDOG_CHECK_ERROR_GL4("glDrawArraysInstancedBaseInstance");
#endif
}

void GraphicsContextGL4::DrawIndexedInstanced(
    std::size_t indexCountPerInstance,
    std::size_t instanceCount,
    std::size_t startIndexLocation,
    std::size_t startInstanceLocation)
{
#if defined(DEBUG) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets, weakTextures);
#endif

    ApplyPipelineState();

    // Bind index buffer
    POMDOG_ASSERT(indexBuffer);
    auto indexBufferGL = dynamic_cast<IndexBufferGL4*>(indexBuffer->NativeIndexBuffer());
    POMDOG_ASSERT(indexBufferGL != nullptr);
    indexBufferGL->BindBuffer();

    // Draw
    POMDOG_ASSERT(indexCountPerInstance > 0);
    POMDOG_ASSERT(indexCountPerInstance <= indexBuffer->GetIndexCount());
    POMDOG_ASSERT(instanceCount > 0);
    POMDOG_ASSERT(instanceCount < static_cast<decltype(instanceCount)>(std::numeric_limits<GLsizei>::max()));

    const auto indexElementSize = indexBuffer->GetElementSize();

#if defined(POMDOG_PLATFORM_MACOSX)
    // NOTE:
    // 'glDrawElementsInstancedBaseInstance' is supported in OpenGL 4.2 and later.
    // But unfortunately, macOS Sierra (latest version of Mac 2016) still uses OpenGL 4.1.
    EmulateStartInstanceLocation(startInstanceLocation);
    glDrawElementsInstanced(
        primitiveTopology.value,
        static_cast<GLsizei>(indexCountPerInstance),
        ToIndexElementType(indexElementSize),
        ComputeStartIndexLocationPointer(indexElementSize, startIndexLocation),
        static_cast<GLsizei>(instanceCount));
    POMDOG_CHECK_ERROR_GL4("glDrawElementsInstanced");
#else
    glDrawElementsInstancedBaseInstance(
        primitiveTopology.value,
        static_cast<GLsizei>(indexCountPerInstance),
        ToIndexElementType(indexElementSize),
        ComputeStartIndexLocationPointer(indexElementSize, startIndexLocation),
        static_cast<GLsizei>(instanceCount),
        static_cast<GLuint>(startInstanceLocation));
    POMDOG_CHECK_ERROR_GL4("glDrawElementsInstancedBaseInstance");
#endif
}

GraphicsCapabilities GraphicsContextGL4::GetCapabilities() const
{
    GraphicsCapabilities capabilities;

    GLint maxTextureUnits = 0;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
    POMDOG_ASSERT(maxTextureUnits > 0);

    GLint maxVertexUniformBlocks = 0;
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &maxVertexUniformBlocks);
    POMDOG_ASSERT(maxVertexUniformBlocks > 0);

    GLint maxFragmentUniformBlocks = 0;
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &maxFragmentUniformBlocks);
    POMDOG_ASSERT(maxFragmentUniformBlocks > 0);

    capabilities.SamplerSlotCount = maxTextureUnits;
    capabilities.ConstantBufferSlotCount = std::min(maxVertexUniformBlocks, maxFragmentUniformBlocks);
    return capabilities;
}

void GraphicsContextGL4::SetPrimitiveTopology(PrimitiveTopology primitiveTopologyIn)
{
    primitiveTopology = ToPrimitiveTopology(primitiveTopologyIn);
}

void GraphicsContextGL4::SetBlendFactor(const Color& blendFactor)
{
    auto colorVector = blendFactor.ToVector4();
    glBlendColor(colorVector.X, colorVector.Y, colorVector.Z, colorVector.W);
    POMDOG_CHECK_ERROR_GL4("glBlendColor");
}

void GraphicsContextGL4::SetVertexBuffers(const std::vector<VertexBufferBinding>& vertexBuffersIn)
{
    POMDOG_ASSERT(!vertexBuffersIn.empty());
    this->vertexBuffers = vertexBuffersIn;
    needToApplyInputLayout = true;
}

void GraphicsContextGL4::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBufferIn)
{
    POMDOG_ASSERT(indexBufferIn);
    indexBuffer = indexBufferIn;
}

void GraphicsContextGL4::SetPipelineState(const std::shared_ptr<NativePipelineState>& pipelineStateIn)
{
    POMDOG_ASSERT(pipelineStateIn);
    if (pipelineState != pipelineStateIn) {
        this->pipelineState = std::dynamic_pointer_cast<PipelineStateGL4>(pipelineStateIn);
        POMDOG_ASSERT(pipelineState);

        needToApplyPipelineState = true;
        needToApplyInputLayout = true;
    }
}

void GraphicsContextGL4::SetConstantBuffer(int index, const std::shared_ptr<NativeBuffer>& constantBufferIn)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(constantBufferIn);

#if defined(DEBUG) && !defined(NDEBUG)
    static const auto capabilities = GetCapabilities();
    POMDOG_ASSERT(index < static_cast<int>(capabilities.ConstantBufferSlotCount));
#endif

    auto constantBuffer = std::dynamic_pointer_cast<ConstantBufferGL4>(constantBufferIn);

    POMDOG_ASSERT(constantBuffer);
    ApplyConstantBuffer(index, *constantBuffer);
}

void GraphicsContextGL4::SetSampler(int index, const std::shared_ptr<NativeSamplerState>& sampler)
{
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(sampler != nullptr);

#if defined(DEBUG) && !defined(NDEBUG)
    static const auto capabilities = GetCapabilities();
    POMDOG_ASSERT(index < static_cast<int>(capabilities.SamplerSlotCount));
#endif

    auto samplerStateGL = std::static_pointer_cast<SamplerStateGL4>(sampler);

    POMDOG_ASSERT(samplerStateGL != nullptr);
    POMDOG_ASSERT(samplerStateGL == std::dynamic_pointer_cast<SamplerStateGL4>(sampler));

    samplerStateGL->Apply(index);
}

void GraphicsContextGL4::SetTexture(int index)
{
    POMDOG_ASSERT(!textures.empty());
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(textures.size()));

#if defined(DEBUG) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures.empty());
    POMDOG_ASSERT(index < static_cast<int>(weakTextures.size()));
    weakTextures[index].reset();
#endif

    if (textures[index]) {
        glActiveTexture(ToTextureUnitIndexGL4(index));
        POMDOG_CHECK_ERROR_GL4("glActiveTexture");

        glBindTexture(*textures[index], 0);
        POMDOG_CHECK_ERROR_GL4("glBindTexture");
    }

    textures[index] = std::nullopt;
}

void GraphicsContextGL4::SetTexture(int index, const std::shared_ptr<Texture2D>& textureIn)
{
    POMDOG_ASSERT(!textures.empty());
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(textures.size()));

#if defined(DEBUG) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures.empty());
    POMDOG_ASSERT(index < static_cast<int>(weakTextures.size()));
    weakTextures[index] = textureIn;
#endif

    constexpr GLenum textureType = GL_TEXTURE_2D;

    if (textures[index] && *textures[index] != textureType) {
        // Unbind texture
        SetTexture(index);
    }

    textures[index] = textureType;

    auto textureGL4 = static_cast<Texture2DGL4*>(textureIn->GetNativeTexture2D());

    POMDOG_ASSERT(textureGL4 != nullptr);
    POMDOG_ASSERT(textureGL4 == dynamic_cast<Texture2DGL4*>(textureIn->GetNativeTexture2D()));

    ApplyTexture2D(index, textureGL4->GetTextureHandle());
}

void GraphicsContextGL4::SetTexture(int index, const std::shared_ptr<RenderTarget2D>& textureIn)
{
    POMDOG_ASSERT(!textures.empty());
    POMDOG_ASSERT(index >= 0);
    POMDOG_ASSERT(index < static_cast<int>(textures.size()));

#if defined(DEBUG) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures.empty());
    POMDOG_ASSERT(index < static_cast<int>(weakTextures.size()));
    weakTextures[index] = textureIn;
#endif

    constexpr GLenum textureType = GL_TEXTURE_2D;

    if (textures[index] && *textures[index] != textureType) {
        // Unbind texture
        SetTexture(index);
    }

    textures[index] = textureType;

    auto renderTargetGL4 = static_cast<RenderTarget2DGL4*>(textureIn->GetNativeRenderTarget2D());

    POMDOG_ASSERT(renderTargetGL4 != nullptr);
    POMDOG_ASSERT(renderTargetGL4 == dynamic_cast<RenderTarget2DGL4*>(textureIn->GetNativeRenderTarget2D()));

    ApplyTexture2D(index, renderTargetGL4->GetTextureHandle());
}

void GraphicsContextGL4::SetRenderPass(const RenderPass& renderPass)
{
    POMDOG_ASSERT(!renderPass.RenderTargets.empty());
    POMDOG_ASSERT(renderPass.RenderTargets.size() <= 16);

    const bool useBackBuffer = ((renderPass.RenderTargets.size() <= 1)
        && (!std::get<0>(renderPass.RenderTargets.front())));

    if (renderPass.Viewport) {
        SetViewport(*renderPass.Viewport, gameWindow, useBackBuffer);
    }
    if (renderPass.ScissorRect) {
        SetScissorRectangle(*renderPass.ScissorRect, gameWindow, useBackBuffer);
    }

    const auto clearDepthStencilMask = [&]() -> GLbitfield {
        GLbitfield mask = 0;
        if (renderPass.ClearDepth) {
            mask |= GL_DEPTH_BUFFER_BIT;
            auto clamped = std::min(std::max(*renderPass.ClearDepth, 0.0f), 1.0f);
            glClearDepthf(clamped);
            POMDOG_CHECK_ERROR_GL4("glClearDepthf");
        }
        if (renderPass.ClearStencil) {
            mask |= GL_STENCIL_BUFFER_BIT;
            glClearStencil(*renderPass.ClearStencil);
            POMDOG_CHECK_ERROR_GL4("glClearStencil");
        }
        return mask;
    }();

    for (const auto& view: renderPass.RenderTargets) {
        auto & renderTarget = std::get<0>(view);
        auto & clearColor = std::get<1>(view);

        GLbitfield mask = clearDepthStencilMask;

        if (clearColor) {
            mask |= GL_COLOR_BUFFER_BIT;
            glClearColor(clearColor->X, clearColor->Y, clearColor->Z, clearColor->W);
            POMDOG_CHECK_ERROR_GL4("glClearColor");
        }
        if (mask != 0) {
            if (!renderTarget) {
                SetRenderTarget(frameBuffer, renderTargets);
            }
            else {
                // FIXME: Optimize
                std::vector<std::shared_ptr<RenderTarget2D>> temporary;
                temporary.push_back(renderTarget);
                SetRenderTargets(frameBuffer, renderTargets, temporary);
            }
            glClear(mask);
            POMDOG_CHECK_ERROR_GL4("glClear");
        }
    }

    if (useBackBuffer) {
        SetRenderTarget(frameBuffer, renderTargets);
        POMDOG_ASSERT(renderTargets.empty());
    }
    else {
        // FIXME: Optimize
        std::vector<std::shared_ptr<RenderTarget2D>> temporary;
        for (auto & tuple : renderPass.RenderTargets) {
            auto & renderTarget = std::get<0>(tuple);
            temporary.push_back(renderTarget);
        }
        SetRenderTargets(frameBuffer, renderTargets, temporary);
        POMDOG_ASSERT(!renderTargets.empty());
    }
}

} // namespace GL4
} // namespace Detail
} // namespace Pomdog
