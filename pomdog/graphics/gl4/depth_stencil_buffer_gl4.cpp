// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/graphics/gl4/depth_stencil_buffer_gl4.hpp"
#include "pomdog/graphics/gl4/error_checker.hpp"
#include "pomdog/graphics/surface_format.hpp"
#include "pomdog/math/rectangle.hpp"
#include "pomdog/utility/assert.hpp"

namespace pomdog::detail::gl4 {
namespace {

[[nodiscard]] std::optional<GLenum>
ToDepthStencilFormat(SurfaceFormat depthFormat) noexcept
{
    switch (depthFormat) {
    case SurfaceFormat::Depth16:
        return GL_DEPTH_COMPONENT16;
    case SurfaceFormat::Depth24Stencil8:
        return GL_DEPTH24_STENCIL8;
    case SurfaceFormat::Depth32:
        return GL_DEPTH_COMPONENT32;
    case SurfaceFormat::Depth32_Float_Stencil8_Uint:
        return GL_DEPTH32F_STENCIL8;
    default:
        break;
    }
    return std::nullopt;
}

} // namespace

std::unique_ptr<Error>
DepthStencilBufferGL4::Initialize(
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    SurfaceFormat depthStencilFormatIn,
    std::int32_t multiSampleCount) noexcept
{
    pixelWidth = pixelWidthIn;
    pixelHeight = pixelHeightIn;
    depthStencilFormat = depthStencilFormatIn;
    multiSampleEnabled = (multiSampleCount > 1);

    const auto nativeFormat = ToDepthStencilFormat(depthStencilFormat);

    if (nativeFormat == std::nullopt) {
        return errors::New("this format not supported as a depth stencil format");
    }

    POMDOG_ASSERT(renderBuffer == 0);

    glGenRenderbuffers(1, &renderBuffer);
    if (auto err = gl4::GetLastError(); err != nullptr) {
        return errors::Wrap(std::move(err), "glGenRenderbuffers() failed");
    }

    GLint oldRenderBuffer = 0;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRenderBuffer);

    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
    if (auto err = gl4::GetLastError(); err != nullptr) {
        return errors::Wrap(std::move(err), "glBindRenderbuffer() failed");
    }

    POMDOG_ASSERT(pixelWidth > 0);
    POMDOG_ASSERT(pixelHeight > 0);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        *nativeFormat,
        pixelWidth,
        pixelHeight);
    if (auto err = gl4::GetLastError(); err != nullptr) {
        glBindRenderbuffer(GL_RENDERBUFFER, oldRenderBuffer);
        POMDOG_CHECK_ERROR_GL4("glBindRenderbuffer");
        return errors::Wrap(std::move(err), "glRenderbufferStorage() failed");
    }

    glBindRenderbuffer(GL_RENDERBUFFER, oldRenderBuffer);
    if (auto err = gl4::GetLastError(); err != nullptr) {
        return errors::Wrap(std::move(err), "glBindRenderbuffer(GL_RENDERBUFFER, 0) failed");
    }

    return nullptr;
}

DepthStencilBufferGL4::~DepthStencilBufferGL4()
{
    if (renderBuffer != 0) {
        glDeleteRenderbuffers(1, &renderBuffer);
        POMDOG_CHECK_ERROR_GL4("glDeleteRenderbuffers");
        renderBuffer = 0;
    }
}

std::int32_t DepthStencilBufferGL4::GetWidth() const noexcept
{
    return pixelWidth;
}

std::int32_t DepthStencilBufferGL4::GetHeight() const noexcept
{
    return pixelHeight;
}

SurfaceFormat DepthStencilBufferGL4::GetFormat() const noexcept
{
    return depthStencilFormat;
}

Rectangle DepthStencilBufferGL4::GetBounds() const noexcept
{
    return Rectangle{0, 0, pixelWidth, pixelHeight};
}

void DepthStencilBufferGL4::BindToFramebuffer(GLuint frameBuffer) noexcept
{
    POMDOG_ASSERT(renderBuffer != 0);

    // NOTE: If stencilRenderBuffer is zero, glFramebufferRenderbuffer() will
    //       detach the stencil buffer from frame buffer.
    const auto stencilRenderBuffer = hasStencil ? renderBuffer : 0;

#if defined(POMDOG_PLATFORM_WIN32)
    // NOTE: OpenGL >= 4.5
    glNamedFramebufferRenderbuffer(
        frameBuffer,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        renderBuffer);
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
        renderBuffer);
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

} // namespace pomdog::detail::gl4
