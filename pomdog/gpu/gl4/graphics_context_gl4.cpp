// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/graphics_context_gl4.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/platform.h"
#include "pomdog/basic/unreachable.h"
#include "pomdog/gpu/backends/buffer_helper.h"
#include "pomdog/gpu/backends/command_list_immediate.h"
#include "pomdog/gpu/backends/graphics_capabilities.h"
#include "pomdog/gpu/gl4/buffer_gl4.h"
#include "pomdog/gpu/gl4/depth_stencil_buffer_gl4.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/gl4/input_layout_gl4.h"
#include "pomdog/gpu/gl4/opengl_context.h"
#include "pomdog/gpu/gl4/pipeline_state_gl4.h"
#include "pomdog/gpu/gl4/render_target2d_gl4.h"
#include "pomdog/gpu/gl4/sampler_state_gl4.h"
#include "pomdog/gpu/gl4/typesafe_helper_gl4.h"
#include "pomdog/gpu/graphics_device.h"
#include "pomdog/gpu/index_buffer.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/gpu/render_pass.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/gpu/texture2d.h"
#include "pomdog/gpu/vertex_buffer.h"
#include "pomdog/gpu/viewport.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/math/vector4.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/scope_guard.h"
#include "pomdog/utility/static_down_cast.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <utility>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

using pomdog::detail::ScopeGuard;
using pomdog::detail::StaticDownCast;

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] GLenum ToIndexElementType(IndexFormat indexElementSize) noexcept
{
    static_assert(sizeof(GLushort) == 2, "GLushort is not SixteenBits.");
    static_assert(sizeof(GLuint) == 4, "GLuint is not ThirtyTwoBits.");

    switch (indexElementSize) {
    case IndexFormat::UInt16:
        return GL_UNSIGNED_SHORT;
    case IndexFormat::UInt32:
        return GL_UNSIGNED_INT;
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

#if defined(__GNUC__) && !defined(__clang__)
    return frameBuffer;
#else
    return std::move(frameBuffer);
#endif
}

void ApplyTexture2D(int index, const Texture2DObjectGL4& textureObject)
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
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

void SetViewport(
    const Viewport& viewport,
    const std::weak_ptr<GraphicsDevice>& graphicsDevice,
    bool useBackBuffer)
{
    POMDOG_ASSERT(viewport.width > 0);
    POMDOG_ASSERT(viewport.height > 0);

    GLint viewportY = viewport.topLeftY;

    if (useBackBuffer) {
        if (auto device = graphicsDevice.lock(); device != nullptr) {
            const auto presentationParameters = device->getPresentationParameters();
            viewportY = presentationParameters.backBufferHeight - (viewport.topLeftY + viewport.height);
        }
    }

    glViewport(viewport.topLeftX, viewportY, viewport.width, viewport.height);
    POMDOG_CHECK_ERROR_GL4("glViewport");

    static_assert(std::is_same<GLfloat, decltype(viewport.minDepth)>::value && std::is_same<GLfloat, decltype(viewport.maxDepth)>::value,
        "NOTE: You can use glDepthRange instead of glDepthRangef");

    POMDOG_ASSERT(!std::isinf(viewport.minDepth));
    POMDOG_ASSERT(!std::isinf(viewport.maxDepth));
    POMDOG_ASSERT(!std::isnan(viewport.minDepth));
    POMDOG_ASSERT(!std::isnan(viewport.maxDepth));

    // NOTE: The MinDepth and MaxDepth must be between 0.0 and 1.0, respectively.
    // Please see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glDepthRange.xhtml
    POMDOG_ASSERT((0.0f <= viewport.minDepth) && (viewport.minDepth <= 1.0f));
    POMDOG_ASSERT((0.0f <= viewport.maxDepth) && (viewport.maxDepth <= 1.0f));

    glDepthRangef(viewport.minDepth, viewport.maxDepth);
    POMDOG_CHECK_ERROR_GL4("glDepthRangef");
}

void SetScissorRectangle(
    const Rectangle& rectangle,
    const std::weak_ptr<GraphicsDevice>& graphicsDevice,
    bool useBackBuffer)
{
    POMDOG_ASSERT(rectangle.width >= 0);
    POMDOG_ASSERT(rectangle.height >= 0);

    GLint lowerLeftCornerY = rectangle.y;

    if (useBackBuffer) {
        // FIXME: Use glClipControl(GL_UPPER_LEFT) instead when OpenGL version is >= 4.5
        if (auto device = graphicsDevice.lock(); device != nullptr) {
            const auto presentationParameters = device->getPresentationParameters();
            lowerLeftCornerY = presentationParameters.backBufferHeight - (rectangle.y + rectangle.height);
        }
    }

    // NOTE: To enable the scissor test, set `RasterizerStateGL4::scissorTestEnable` to true.
    glScissor(rectangle.x, lowerLeftCornerY, rectangle.width, rectangle.height);
    POMDOG_CHECK_ERROR_GL4("glScissor");
}

void UnbindDepthStencilBufferFromFrameBuffer(const FrameBufferGL4& frameBuffer) noexcept
{
#if defined(POMDOG_PLATFORM_WIN32)
    // NOTE: OpenGL >= 4.5
    glNamedFramebufferRenderbuffer(
        frameBuffer.value,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        0);
    POMDOG_CHECK_ERROR_GL4("glNamedFramebufferRenderbuffer(GL_DEPTH_ATTACHMENT)");

    glNamedFramebufferRenderbuffer(
        frameBuffer.value,
        GL_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        0);
    POMDOG_CHECK_ERROR_GL4("glNamedFramebufferRenderbuffer(GL_STENCIL_ATTACHMENT)");
#else
    GLint oldFrameBuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFrameBuffer);
    POMDOG_CHECK_ERROR_GL4("glGetIntegerv(GL_FRAMEBUFFER_BINDING)");

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.value);
    POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        0);
    POMDOG_CHECK_ERROR_GL4("glFramebufferRenderbuffer(GL_DEPTH_ATTACHMENT)");

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        0);
    POMDOG_CHECK_ERROR_GL4("glFramebufferRenderbuffer(GL_STENCIL_ATTACHMENT)");

    glBindFramebuffer(GL_FRAMEBUFFER, oldFrameBuffer);
    POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");
#endif
}

void UnbindRenderTargetsFromFrameBuffer(
    FrameBufferGL4 frameBuffer,
    std::array<std::shared_ptr<RenderTarget2DGL4>, 8>& renderTargets)
{
    // NOTE: Unbind render targets
    int index = 0;
    for (auto& renderTarget : renderTargets) {
        if (renderTarget != nullptr) {
            renderTarget->unbindFromFramebuffer(frameBuffer.value, ToColorAttachment(index));
        }
        renderTarget = nullptr;
        ++index;
    }
}

void ValidateFrameBuffer(FrameBufferGL4 frameBuffer, GLenum* colorAttachments, GLsizei colorAttachmentCount)
{
    POMDOG_ASSERT(colorAttachments != nullptr);
    POMDOG_ASSERT(colorAttachmentCount > 0);

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    {
        GLint oldFrameBuffer = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFrameBuffer);
        POMDOG_CHECK_ERROR_GL4("glGetIntegerv(GL_FRAMEBUFFER_BINDING)");

        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.value);
        POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");

        // NOTE: Check framebuffer status
        POMDOG_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
        POMDOG_CHECK_ERROR_GL4("glCheckFramebufferStatus");

        glBindFramebuffer(GL_FRAMEBUFFER, oldFrameBuffer);
        POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");
    }
#endif

    {
#if 0 && defined(POMDOG_PLATFORM_WIN32)
        // NOTE: OpenGL >= 4.5
        // NOTE: Specifies a list of color buffers to be drawn into.
        glNamedFramebufferDrawBuffers(frameBuffer.value, colorAttachmentCount, colorAttachments);
        POMDOG_CHECK_ERROR_GL4("glNamedFramebufferDrawBuffers");
#else
        GLint oldFrameBuffer = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFrameBuffer);
        POMDOG_CHECK_ERROR_GL4("glGetIntegerv(GL_FRAMEBUFFER_BINDING)");

        // NOTE: Bind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.value);
        POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");

        // NOTE: Specifies a list of color buffers to be drawn into.
        glDrawBuffers(colorAttachmentCount, colorAttachments);
        POMDOG_CHECK_ERROR_GL4("glDrawBuffers");

        glBindFramebuffer(GL_FRAMEBUFFER, oldFrameBuffer);
        POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");
#endif
    }
}

void SetDepthStencilBuffer(
    FrameBufferGL4 frameBuffer,
    const std::shared_ptr<DepthStencilBuffer>& depthStencilBufferIn)
{
    // NOTE: Attach depth stencil buffer
    if (depthStencilBufferIn == nullptr) {
        return;
    }

    const auto depthStencilBuffer = StaticDownCast<DepthStencilBufferGL4>(depthStencilBufferIn.get());
    POMDOG_ASSERT(depthStencilBuffer != nullptr);
    depthStencilBuffer->bindToFramebuffer(frameBuffer.value);
}

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
void CheckUnbindingRenderTargetsError(
    const std::vector<std::weak_ptr<RenderTarget2D>>& renderTargets,
    const std::vector<std::weak_ptr<Texture>>& textures)
{
    for (auto& renderTarget : renderTargets) {
        for (auto& texture : textures) {
            if (!renderTarget.expired() && !texture.expired()) {
                POMDOG_ASSERT(renderTarget.lock() != texture.lock());
            }
        }
    }
}
#endif

const GLvoid* ComputeStartIndexLocationPointer(
    IndexFormat indexElementSize,
    std::size_t startIndexLocation) noexcept
{
    using detail::BufferHelper::ToIndexElementOffsetBytes;
    auto offsetBytes = startIndexLocation * ToIndexElementOffsetBytes(indexElementSize);
    return reinterpret_cast<const GLvoid*>(offsetBytes);
}

} // namespace

template <>
struct TypesafeHelperGL4::Traits<FrameBufferGL4> {
    constexpr static GLenum BufferBinding = GL_FRAMEBUFFER_BINDING;
};

std::unique_ptr<Error>
GraphicsContextGL4::initialize(
    const std::shared_ptr<OpenGLContext>& openGLContextIn,
    std::weak_ptr<GraphicsDevice>&& graphicsDeviceIn) noexcept
{
    nativeContext_ = openGLContextIn;
    graphicsDevice_ = std::move(graphicsDeviceIn);
    needToApplyInputLayout_ = true;
    needToApplyPipelineState_ = true;

    auto capabilities = GetCapabilities();
    if (capabilities.SamplerSlotCount > 0) {
        textures_.resize(capabilities.SamplerSlotCount);
    }

    glFrontFace(GL_CW);
    POMDOG_CHECK_ERROR_GL4("glFrontFace");

    frameBuffer_ = CreateFrameBuffer();
    primitiveTopology_ = GL_TRIANGLES;

    // NOTE: Set default values for graphics context
    setBlendFactor(Vector4{1.0f, 1.0f, 1.0f, 1.0f});

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    auto graphicsCapbilities = this->GetCapabilities();

    POMDOG_ASSERT(graphicsCapbilities.SamplerSlotCount > 0);
    weakTextures_.resize(graphicsCapbilities.SamplerSlotCount);
#endif

    return nullptr;
}

GraphicsContextGL4::~GraphicsContextGL4()
{
    pipelineState_.reset();
    for (auto& v : vertexBuffers_) {
        v.VertexBuffer.reset();
    }
    indexBuffer_.reset();
    textures_.clear();
    for (auto& r : renderTargets_) {
        r.reset();
    }

    if (frameBuffer_) {
        glDeleteFramebuffers(1, frameBuffer_->Data());
        POMDOG_CHECK_ERROR_GL4("glDeleteFramebuffers");
        frameBuffer_ = std::nullopt;
    }

    nativeContext_.reset();
    graphicsDevice_.reset();
}

void GraphicsContextGL4::executeCommandLists(
    std::span<std::shared_ptr<CommandListImmediate>> commandLists)
{
    pipelineState_ = nullptr;
    needToApplyInputLayout_ = true;
    needToApplyPipelineState_ = true;

    for (auto& commandList : commandLists) {
        POMDOG_ASSERT(commandList);
        commandList->executeImmediate(*this);
    }
}

void GraphicsContextGL4::present()
{
    nativeContext_->swapBuffers();
    POMDOG_CHECK_ERROR_GL4("swapBuffers");
}

void GraphicsContextGL4::applyPipelineState()
{
    POMDOG_ASSERT(pipelineState_);

    if (needToApplyInputLayout_) {
        // Bind input-layout to the input-assembler stage:
        auto inputLayout = pipelineState_->GetInputLayout();

        POMDOG_ASSERT(inputLayout != nullptr);
        inputLayout->Apply(vertexBuffers_);

        needToApplyInputLayout_ = false;
    }

    if (needToApplyPipelineState_) {
        // Use shader program:
        POMDOG_ASSERT(pipelineState_);
        pipelineState_->ApplyShaders();

        needToApplyPipelineState_ = false;
    }
}

void GraphicsContextGL4::emulateStartInstanceLocation(std::size_t startInstanceLocation)
{
    if (startInstanceLocation == 0) {
        // NOTE: nothing to do
        return;
    }

    auto newVertexBuffers = vertexBuffers_;
    for (std::size_t i = 1; i < newVertexBuffers.size(); i++) {
        // NOTE: `i >= 1` is equality to instanced vertex buffer.
        auto& binding = newVertexBuffers[i];
        if (binding.VertexBuffer == nullptr) {
            continue;
        }
        const auto strideBytes = binding.VertexBuffer->getStrideBytes();
        POMDOG_ASSERT(strideBytes > 0);
        binding.VertexOffset += (strideBytes * startInstanceLocation);
    }

    auto inputLayout = pipelineState_->GetInputLayout();
    POMDOG_ASSERT(inputLayout != nullptr);
    POMDOG_ASSERT(!newVertexBuffers.empty());

    // NOTE: The following code is a hack.
    inputLayout->Apply(newVertexBuffers);
    needToApplyInputLayout_ = true;
}

void GraphicsContextGL4::draw(
    std::uint32_t vertexCount,
    std::uint32_t startVertexLocation)
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets_, weakTextures_);
#endif

    applyPipelineState();

    // Draw
    POMDOG_ASSERT(!vertexBuffers_.empty());
    POMDOG_ASSERT(vertexBuffers_.front().VertexBuffer);
    POMDOG_ASSERT(vertexCount > 0);
    POMDOG_ASSERT(vertexCount <= vertexBuffers_.front().VertexBuffer->getVertexCount());

    glDrawArrays(
        primitiveTopology_.value,
        static_cast<GLint>(startVertexLocation),
        static_cast<GLsizei>(vertexCount));
    POMDOG_CHECK_ERROR_GL4("glDrawArrays");
}

void GraphicsContextGL4::drawIndexed(
    std::uint32_t indexCount,
    std::uint32_t startIndexLocation)
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets_, weakTextures_);
#endif

    applyPipelineState();

    // Bind index buffer
    POMDOG_ASSERT(indexBuffer_);
    const auto indexBufferGL = StaticDownCast<IndexBufferGL4>(indexBuffer_->getBuffer());
    POMDOG_ASSERT(indexBufferGL != nullptr);
    indexBufferGL->bindBuffer();

    // Draw
    POMDOG_ASSERT(indexBuffer_);
    POMDOG_ASSERT(indexCount > 0);
    POMDOG_ASSERT(indexCount <= indexBuffer_->getIndexCount());

    const auto indexElementSize = indexBuffer_->getElementSize();

    glDrawElements(
        primitiveTopology_.value,
        static_cast<GLsizei>(indexCount),
        ToIndexElementType(indexElementSize),
        ComputeStartIndexLocationPointer(indexElementSize, startIndexLocation));
    POMDOG_CHECK_ERROR_GL4("glDrawElements");
}

void GraphicsContextGL4::drawInstanced(
    std::uint32_t vertexCountPerInstance,
    std::uint32_t instanceCount,
    std::uint32_t startVertexLocation,
    std::uint32_t startInstanceLocation)
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets_, weakTextures_);
#endif

    applyPipelineState();

    // Draw
    POMDOG_ASSERT(!vertexBuffers_.empty());
    POMDOG_ASSERT(vertexBuffers_.front().VertexBuffer);
    POMDOG_ASSERT(vertexCountPerInstance > 0);
    POMDOG_ASSERT(vertexCountPerInstance <= vertexBuffers_.front().VertexBuffer->getVertexCount());
    POMDOG_ASSERT(instanceCount > 0);
    POMDOG_ASSERT(instanceCount <= static_cast<decltype(instanceCount)>(std::numeric_limits<GLsizei>::max()));

#if defined(POMDOG_PLATFORM_MACOSX)
    // NOTE:
    // 'glDrawArraysInstancedBaseInstance' is supported in OpenGL 4.2 and later.
    // But unfortunately, macOS Sierra (latest version of Mac 2016) still uses OpenGL 4.1.
    emulateStartInstanceLocation(startInstanceLocation);
    glDrawArraysInstanced(
        primitiveTopology_.value,
        static_cast<GLint>(startVertexLocation),
        static_cast<GLsizei>(vertexCountPerInstance),
        static_cast<GLsizei>(instanceCount));
    POMDOG_CHECK_ERROR_GL4("glDrawArraysInstanced");
#else
    glDrawArraysInstancedBaseInstance(
        primitiveTopology_.value,
        static_cast<GLint>(startVertexLocation),
        static_cast<GLsizei>(vertexCountPerInstance),
        static_cast<GLsizei>(instanceCount),
        static_cast<GLuint>(startInstanceLocation));
    POMDOG_CHECK_ERROR_GL4("glDrawArraysInstancedBaseInstance");
#endif
}

void GraphicsContextGL4::drawIndexedInstanced(
    std::uint32_t indexCountPerInstance,
    std::uint32_t instanceCount,
    std::uint32_t startIndexLocation,
    std::uint32_t startInstanceLocation)
{
#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    CheckUnbindingRenderTargetsError(weakRenderTargets_, weakTextures_);
#endif

    applyPipelineState();

    // Bind index buffer
    POMDOG_ASSERT(indexBuffer_ != nullptr);
    const auto indexBufferGL = StaticDownCast<IndexBufferGL4>(indexBuffer_->getBuffer());
    POMDOG_ASSERT(indexBufferGL != nullptr);
    indexBufferGL->bindBuffer();

    // Draw
    POMDOG_ASSERT(indexCountPerInstance > 0);
    POMDOG_ASSERT(indexCountPerInstance <= indexBuffer_->getIndexCount());
    POMDOG_ASSERT(instanceCount > 0);
    POMDOG_ASSERT(instanceCount < static_cast<decltype(instanceCount)>(std::numeric_limits<GLsizei>::max()));

    const auto indexElementSize = indexBuffer_->getElementSize();

#if defined(POMDOG_PLATFORM_MACOSX)
    // NOTE:
    // 'glDrawElementsInstancedBaseInstance' is supported in OpenGL 4.2 and later.
    // But unfortunately, macOS Sierra (latest version of Mac 2016) still uses OpenGL 4.1.
    emulateStartInstanceLocation(startInstanceLocation);
    glDrawElementsInstanced(
        primitiveTopology_.value,
        static_cast<GLsizei>(indexCountPerInstance),
        ToIndexElementType(indexElementSize),
        ComputeStartIndexLocationPointer(indexElementSize, startIndexLocation),
        static_cast<GLsizei>(instanceCount));
    POMDOG_CHECK_ERROR_GL4("glDrawElementsInstanced");
#else
    glDrawElementsInstancedBaseInstance(
        primitiveTopology_.value,
        static_cast<GLsizei>(indexCountPerInstance),
        ToIndexElementType(indexElementSize),
        ComputeStartIndexLocationPointer(indexElementSize, startIndexLocation),
        static_cast<GLsizei>(instanceCount),
        static_cast<GLuint>(startInstanceLocation));
    POMDOG_CHECK_ERROR_GL4("glDrawElementsInstancedBaseInstance");
#endif
}

GraphicsCapabilities GraphicsContextGL4::GetCapabilities() const noexcept
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

void GraphicsContextGL4::setViewport(const Viewport& viewport)
{
    POMDOG_ASSERT(!renderTargets_.empty());
    POMDOG_ASSERT(renderTargets_.size() == 8);

    const bool useBackBuffer = (renderTargets_.front() == nullptr);
    gl4::SetViewport(viewport, graphicsDevice_, useBackBuffer);
}

void GraphicsContextGL4::setScissorRect(const Rectangle& scissorRect)
{
    POMDOG_ASSERT(!renderTargets_.empty());
    POMDOG_ASSERT(renderTargets_.size() == 8);

    const bool useBackBuffer = (renderTargets_.front() == nullptr);
    SetScissorRectangle(scissorRect, graphicsDevice_, useBackBuffer);
}

void GraphicsContextGL4::setBlendFactor(const Vector4& blendFactor)
{
    glBlendColor(blendFactor.x, blendFactor.y, blendFactor.z, blendFactor.w);
    POMDOG_CHECK_ERROR_GL4("glBlendColor");
}

void GraphicsContextGL4::setVertexBuffer(
    std::uint32_t index,
    const std::shared_ptr<VertexBuffer>& vertexBuffer,
    std::uint32_t offset)
{
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(vertexBuffers_.size()));
    POMDOG_ASSERT(vertexBuffer != nullptr);
    vertexBuffers_[index].VertexBuffer = vertexBuffer;
    vertexBuffers_[index].VertexOffset = offset;
    needToApplyInputLayout_ = true;
}

void GraphicsContextGL4::setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBufferIn)
{
    POMDOG_ASSERT(indexBufferIn);
    indexBuffer_ = indexBufferIn;
}

void GraphicsContextGL4::setPipelineState(const std::shared_ptr<PipelineState>& pipelineStateIn)
{
    POMDOG_ASSERT(pipelineStateIn);
    if (pipelineState_ != pipelineStateIn) {
        pipelineState_ = StaticDownCast<PipelineStateGL4>(pipelineStateIn);
        POMDOG_ASSERT(pipelineState_ != nullptr);

        primitiveTopology_ = pipelineState_->GetPrimitiveTopology();
        needToApplyPipelineState_ = true;
        needToApplyInputLayout_ = true;
    }
}

void GraphicsContextGL4::setConstantBuffer(
    std::uint32_t index,
    const std::shared_ptr<Buffer>& constantBufferIn,
    std::uint32_t offset,
    std::uint32_t sizeInBytes)
{
    POMDOG_ASSERT(constantBufferIn);
    POMDOG_ASSERT(sizeInBytes > 0);
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");
    static_assert(std::is_unsigned_v<decltype(offset)>, "offset >= 0");

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    static const auto capabilities = GetCapabilities();
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(capabilities.ConstantBufferSlotCount));
#endif

    const auto constantBuffer = StaticDownCast<ConstantBufferGL4>(constantBufferIn.get());
    POMDOG_ASSERT(constantBuffer != nullptr);

    glBindBufferRange(
        GL_UNIFORM_BUFFER,
        index,
        constantBuffer->getBuffer(),
        static_cast<GLintptr>(offset),
        static_cast<GLsizeiptr>(sizeInBytes));
    POMDOG_CHECK_ERROR_GL4("glBindBufferRange");
}

void GraphicsContextGL4::setSampler(std::uint32_t index, const std::shared_ptr<SamplerState>& sampler)
{
    POMDOG_ASSERT(sampler != nullptr);
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    static const auto capabilities = GetCapabilities();
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(capabilities.SamplerSlotCount));
#endif

    const auto samplerStateGL = StaticDownCast<SamplerStateGL4>(sampler);
    POMDOG_ASSERT(samplerStateGL != nullptr);

    samplerStateGL->Apply(index);
}

void GraphicsContextGL4::setTexture(std::uint32_t index)
{
    POMDOG_ASSERT(!textures_.empty());
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(textures_.size()));
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures_.empty());
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(weakTextures_.size()));
    weakTextures_[index].reset();
#endif

    if (textures_[index]) {
        glActiveTexture(ToTextureUnitIndexGL4(index));
        POMDOG_CHECK_ERROR_GL4("glActiveTexture");

        glBindTexture(*textures_[index], 0);
        POMDOG_CHECK_ERROR_GL4("glBindTexture");
    }

    textures_[index] = std::nullopt;
}

void GraphicsContextGL4::setTexture(std::uint32_t index, const std::shared_ptr<gpu::Texture2D>& textureIn)
{
    POMDOG_ASSERT(!textures_.empty());
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(textures_.size()));
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures_.empty());
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(weakTextures_.size()));
    weakTextures_[index] = textureIn;
#endif

    constexpr GLenum textureType = GL_TEXTURE_2D;

    if (textures_[index] && *textures_[index] != textureType) {
        // Unbind texture
        setTexture(index);
    }

    textures_[index] = textureType;

    const auto textureGL4 = StaticDownCast<Texture2DGL4>(textureIn.get());
    POMDOG_ASSERT(textureGL4 != nullptr);

    ApplyTexture2D(index, textureGL4->getTextureHandle());
}

void GraphicsContextGL4::setTexture(std::uint32_t index, const std::shared_ptr<RenderTarget2D>& textureIn)
{
    POMDOG_ASSERT(!textures_.empty());
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(textures_.size()));
    static_assert(std::is_unsigned_v<decltype(index)>, "index must be >= 0");

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    POMDOG_ASSERT(!weakTextures_.empty());
    POMDOG_ASSERT(index < static_cast<std::uint32_t>(weakTextures_.size()));
    weakTextures_[index] = textureIn;
#endif

    constexpr GLenum textureType = GL_TEXTURE_2D;

    if (textures_[index] && *textures_[index] != textureType) {
        // Unbind texture
        setTexture(index);
    }

    textures_[index] = textureType;

    const auto renderTargetGL4 = StaticDownCast<RenderTarget2DGL4>(textureIn.get());
    POMDOG_ASSERT(renderTargetGL4 != nullptr);

    ApplyTexture2D(index, renderTargetGL4->getTextureHandle());
}

void GraphicsContextGL4::beginRenderPass(const RenderPass& renderPass)
{
    POMDOG_ASSERT(!renderPass.renderTargets.empty());
    POMDOG_ASSERT(renderPass.renderTargets.size() == 8);

    POMDOG_ASSERT(renderTargets_.size() >= 1);
    POMDOG_ASSERT(renderTargets_.front() == nullptr);

    const bool useBackBuffer = (std::get<0>(renderPass.renderTargets.front()) == nullptr);

    if (useBackBuffer) {
        // NOTE: Bind default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");
    }
    else {
        // NOTE: Bind framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer_->value);
        POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");
    }

    if (renderPass.viewport) {
        gl4::SetViewport(*renderPass.viewport, graphicsDevice_, useBackBuffer);
    }
    if (renderPass.scissorRect) {
        SetScissorRectangle(*renderPass.scissorRect, graphicsDevice_, useBackBuffer);
    }

    {
        if (!useBackBuffer) {
            // NOTE: Set depth stencil buffer.
            SetDepthStencilBuffer(*frameBuffer_, renderPass.depthStencilBuffer);
        }

        GLbitfield clearMask = 0;
        if (renderPass.clearDepth) {
            // NOTE: glDepthMask() must be enabled to clear the depth buffer.
            glDepthMask(GL_TRUE);
            POMDOG_CHECK_ERROR_GL4("glDepthMask");

            clearMask |= GL_DEPTH_BUFFER_BIT;
            auto clamped = std::min(std::max(*renderPass.clearDepth, 0.0f), 1.0f);
            glClearDepthf(clamped);
            POMDOG_CHECK_ERROR_GL4("glClearDepthf");
        }
        if (renderPass.clearStencil) {
            // NOTE: glStencilMask() must be enabled to clear the stencil buffer.
            glStencilMask(GL_TRUE);
            POMDOG_CHECK_ERROR_GL4("glStencilMask");

            clearMask |= GL_STENCIL_BUFFER_BIT;
            glClearStencil(*renderPass.clearStencil);
            POMDOG_CHECK_ERROR_GL4("glClearStencil");
        }

        if ((clearMask != 0) && !useBackBuffer) {
            auto& view = renderPass.renderTargets[0];
            auto& renderTarget = std::get<0>(view);
            POMDOG_ASSERT(renderTarget != nullptr);

            const auto renderTargetGL4 = StaticDownCast<RenderTarget2DGL4>(renderTarget.get());
            POMDOG_ASSERT(renderTargetGL4 != nullptr);

            renderTargetGL4->bindToFramebuffer(frameBuffer_->value, ToColorAttachment(0));

            std::array<GLenum, 1> attachments = {ToColorAttachment(0)};
            ValidateFrameBuffer(*frameBuffer_, attachments.data(), static_cast<GLsizei>(attachments.size()));
        }

        if (clearMask != 0) {
            // NOTE: Clear depth stencil buffer.
            glClear(clearMask);
            POMDOG_CHECK_ERROR_GL4("glClear");
        }
    }

    if (useBackBuffer) {
        auto& view = renderPass.renderTargets[0];
        auto& clearColor = std::get<1>(view);

        if (clearColor != std::nullopt) {
            glClearColor(clearColor->x, clearColor->y, clearColor->z, clearColor->w);
            POMDOG_CHECK_ERROR_GL4("glClearColor");

            constexpr GLbitfield clearMask = GL_COLOR_BUFFER_BIT;
            glClear(clearMask);
            POMDOG_CHECK_ERROR_GL4("glClear");
        }
    }
    else {
        for (const auto& view : renderPass.renderTargets) {
            auto& renderTarget = std::get<0>(view);
            auto& clearColor = std::get<1>(view);

            if (renderTarget == nullptr) {
                break;
            }
            if (clearColor == std::nullopt) {
                continue;
            }

            const auto renderTargetGL4 = StaticDownCast<RenderTarget2DGL4>(renderTarget.get());
            POMDOG_ASSERT(renderTargetGL4 != nullptr);

            renderTargetGL4->bindToFramebuffer(frameBuffer_->value, ToColorAttachment(0));

            std::array<GLenum, 1> attachments = {ToColorAttachment(0)};
            ValidateFrameBuffer(*frameBuffer_, attachments.data(), static_cast<GLsizei>(attachments.size()));

            glClearColor(clearColor->x, clearColor->y, clearColor->z, clearColor->w);
            POMDOG_CHECK_ERROR_GL4("glClearColor");

            constexpr GLbitfield clearMask = GL_COLOR_BUFFER_BIT;
            glClear(clearMask);
            POMDOG_CHECK_ERROR_GL4("glClear");
        }
    }

#if defined(POMDOG_DEBUG_BUILD) && !defined(NDEBUG)
    if (useBackBuffer) {
        POMDOG_ASSERT(renderTargets_.size() >= 1);
        POMDOG_ASSERT(renderTargets_.front() == nullptr);
    }
#endif
    if (!useBackBuffer) {
        POMDOG_ASSERT(frameBuffer_ != std::nullopt);
        POMDOG_ASSERT(renderTargets_.size() == 8);

        // NOTE: Assign nullptr to the renderTargets.
        std::fill(std::begin(renderTargets_), std::end(renderTargets_), nullptr);

        std::array<GLenum, 8> attachments;
        POMDOG_ASSERT(attachments.size() == renderTargets_.size());

        // NOTE: Bind color attachments to frame buffer.
        int index = 0;
        for (auto& tuple : renderPass.renderTargets) {
            auto& renderTarget = std::get<0>(tuple);
            if (renderTarget == nullptr) {
                break;
            }
            const auto renderTargetGL4 = StaticDownCast<RenderTarget2DGL4>(renderTarget);
            POMDOG_ASSERT(renderTargetGL4 != nullptr);

            renderTargetGL4->bindToFramebuffer(frameBuffer_->value, ToColorAttachment(index));

            renderTargets_[index] = std::move(renderTargetGL4);
            attachments[index] = ToColorAttachment(index);
            ++index;
        }

        POMDOG_ASSERT(!renderTargets_.empty());
        POMDOG_ASSERT(renderTargets_.front() != nullptr);

        const auto renderTargetCount = index;

        POMDOG_ASSERT(!attachments.empty());
        POMDOG_ASSERT(renderTargetCount > 0);
        ValidateFrameBuffer(*frameBuffer_, attachments.data(), static_cast<GLsizei>(renderTargetCount));
    }
}

void GraphicsContextGL4::endRenderPass()
{
    if (frameBuffer_ != std::nullopt) {
        UnbindRenderTargetsFromFrameBuffer(*frameBuffer_, renderTargets_);
        UnbindDepthStencilBufferFromFrameBuffer(*frameBuffer_);
        POMDOG_ASSERT(renderTargets_.size() >= 1);
        POMDOG_ASSERT(renderTargets_.front() == nullptr);
    }
}

} // namespace pomdog::gpu::detail::gl4
