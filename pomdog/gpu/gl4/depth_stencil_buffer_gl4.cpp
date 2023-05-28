// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/depth_stencil_buffer_gl4.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail::gl4 {
namespace {

[[nodiscard]] std::optional<GLenum>
toDepthStencilFormat(PixelFormat depthFormat) noexcept
{
    switch (depthFormat) {
    case PixelFormat::Depth16:
        return GL_DEPTH_COMPONENT16;
    case PixelFormat::Depth24Stencil8:
        return GL_DEPTH24_STENCIL8;
    case PixelFormat::Depth32:
        return GL_DEPTH_COMPONENT32;
    case PixelFormat::Depth32_Float_Stencil8_Uint:
        return GL_DEPTH32F_STENCIL8;
    default:
        break;
    }
    return std::nullopt;
}

} // namespace

std::unique_ptr<Error>
DepthStencilBufferGL4::initialize(
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    PixelFormat depthStencilFormatIn,
    std::int32_t multiSampleCount) noexcept
{
    pixelWidth_ = pixelWidthIn;
    pixelHeight_ = pixelHeightIn;
    depthStencilFormat_ = depthStencilFormatIn;
    multiSampleEnabled_ = (multiSampleCount > 1);

    const auto nativeFormat = toDepthStencilFormat(depthStencilFormat_);

    if (nativeFormat == std::nullopt) {
        return errors::make("this format not supported as a depth stencil format");
    }

    POMDOG_ASSERT(renderBuffer_ == 0);

    glGenRenderbuffers(1, &renderBuffer_);
    if (auto err = gl4::GetLastError(); err != nullptr) {
        return errors::wrap(std::move(err), "glGenRenderbuffers() failed");
    }

    GLint oldRenderBuffer = 0;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRenderBuffer);

    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer_);
    if (auto err = gl4::GetLastError(); err != nullptr) {
        return errors::wrap(std::move(err), "glBindRenderbuffer() failed");
    }

    POMDOG_ASSERT(pixelWidth_ > 0);
    POMDOG_ASSERT(pixelHeight_ > 0);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        *nativeFormat,
        pixelWidth_,
        pixelHeight_);
    if (auto err = gl4::GetLastError(); err != nullptr) {
        glBindRenderbuffer(GL_RENDERBUFFER, oldRenderBuffer);
        POMDOG_CHECK_ERROR_GL4("glBindRenderbuffer");
        return errors::wrap(std::move(err), "glRenderbufferStorage() failed");
    }

    glBindRenderbuffer(GL_RENDERBUFFER, oldRenderBuffer);
    if (auto err = gl4::GetLastError(); err != nullptr) {
        return errors::wrap(std::move(err), "glBindRenderbuffer(GL_RENDERBUFFER, 0) failed");
    }

    return nullptr;
}

DepthStencilBufferGL4::~DepthStencilBufferGL4()
{
    if (renderBuffer_ != 0) {
        glDeleteRenderbuffers(1, &renderBuffer_);
        POMDOG_CHECK_ERROR_GL4("glDeleteRenderbuffers");
        renderBuffer_ = 0;
    }
}

std::int32_t DepthStencilBufferGL4::getWidth() const noexcept
{
    return pixelWidth_;
}

std::int32_t DepthStencilBufferGL4::getHeight() const noexcept
{
    return pixelHeight_;
}

PixelFormat DepthStencilBufferGL4::getFormat() const noexcept
{
    return depthStencilFormat_;
}

Rectangle DepthStencilBufferGL4::getBounds() const noexcept
{
    return Rectangle{0, 0, pixelWidth_, pixelHeight_};
}

void DepthStencilBufferGL4::bindToFramebuffer(GLuint frameBuffer) noexcept
{
    POMDOG_ASSERT(renderBuffer_ != 0);

    // NOTE: If stencilRenderBuffer is zero, glFramebufferRenderbuffer() will
    //       detach the stencil buffer from frame buffer.
    const auto stencilRenderBuffer = hasStencil_ ? renderBuffer_ : 0;

#if defined(POMDOG_PLATFORM_WIN32)
    // NOTE: OpenGL >= 4.5
    glNamedFramebufferRenderbuffer(
        frameBuffer,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        renderBuffer_);
    POMDOG_CHECK_ERROR_GL4("glNamedFramebufferRenderbuffer(GL_DEPTH_ATTACHMENT)");

    glNamedFramebufferRenderbuffer(
        frameBuffer,
        GL_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        stencilRenderBuffer);
    POMDOG_CHECK_ERROR_GL4("glNamedFramebufferRenderbuffer(GL_STENCIL_ATTACHMENT)");
#else
    GLint oldFrameBuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFrameBuffer);
    POMDOG_CHECK_ERROR_GL4("glGetIntegerv(GL_FRAMEBUFFER_BINDING)");

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        renderBuffer_);
    POMDOG_CHECK_ERROR_GL4("glFramebufferRenderbuffer(GL_DEPTH_ATTACHMENT)");

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        stencilRenderBuffer);
    POMDOG_CHECK_ERROR_GL4("glFramebufferRenderbuffer(GL_STENCIL_ATTACHMENT)");

    glBindFramebuffer(GL_FRAMEBUFFER, oldFrameBuffer);
    POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");
#endif
}

} // namespace pomdog::gpu::detail::gl4
