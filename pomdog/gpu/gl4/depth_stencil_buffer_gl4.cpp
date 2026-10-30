// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/depth_stencil_buffer_gl4.h"
#include "pomdog/basic/platform.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/math/rect2d.h"
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
#if defined(POMDOG_PLATFORM_EMSCRIPTEN)
        return GL_DEPTH_COMPONENT32F;
#else
        return GL_DEPTH_COMPONENT32;
#endif
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
    i32 pixelWidthIn,
    i32 pixelHeightIn,
    PixelFormat depthStencilFormatIn,
    i32 multiSampleCount) noexcept
{
    pixelWidth_ = pixelWidthIn;
    pixelHeight_ = pixelHeightIn;
    depthStencilFormat_ = depthStencilFormatIn;
    multiSampleEnabled_ = (multiSampleCount > 1);
    hasStencil_ = [depthStencilFormatIn]() -> bool {
        switch (depthStencilFormatIn) {
        case PixelFormat::Depth24Stencil8:
        case PixelFormat::Depth32_Float_Stencil8_Uint:
            return true;
        default:
            break;
        }
        return false;
    }();

    const auto nativeFormat = toDepthStencilFormat(depthStencilFormat_);

    if (nativeFormat == std::nullopt) {
        return errors::make("this format not supported as a depth stencil format");
    }

    POMDOG_ASSERT(renderBuffer_ == 0);

    glGenRenderbuffers(1, &renderBuffer_);
    if (auto err = gl4::getLastError(); err != nullptr) {
        return errors::wrap(std::move(err), "glGenRenderbuffers() failed");
    }

    GLint oldRenderBuffer = 0;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRenderBuffer);

    glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer_);
    if (auto err = gl4::getLastError(); err != nullptr) {
        return errors::wrap(std::move(err), "glBindRenderbuffer() failed");
    }

    POMDOG_ASSERT(pixelWidth_ > 0);
    POMDOG_ASSERT(pixelHeight_ > 0);

    glRenderbufferStorage(
        GL_RENDERBUFFER,
        *nativeFormat,
        pixelWidth_,
        pixelHeight_);
    if (auto err = gl4::getLastError(); err != nullptr) {
        glBindRenderbuffer(GL_RENDERBUFFER, oldRenderBuffer);
        return errors::wrap(std::move(err), "glRenderbufferStorage() failed");
    }

    glBindRenderbuffer(GL_RENDERBUFFER, oldRenderBuffer);
    if (auto err = gl4::getLastError(); err != nullptr) {
        return errors::wrap(std::move(err), "glBindRenderbuffer(GL_RENDERBUFFER, 0) failed");
    }

    return nullptr;
}

DepthStencilBufferGL4::~DepthStencilBufferGL4()
{
    if (renderBuffer_ != 0) {
        glDeleteRenderbuffers(1, &renderBuffer_);
        renderBuffer_ = 0;
    }
}

i32 DepthStencilBufferGL4::getWidth() const noexcept
{
    return pixelWidth_;
}

i32 DepthStencilBufferGL4::getHeight() const noexcept
{
    return pixelHeight_;
}

PixelFormat
DepthStencilBufferGL4::getFormat() const noexcept
{
    return depthStencilFormat_;
}

Rect2D
DepthStencilBufferGL4::getBounds() const noexcept
{
    return Rect2D{0, 0, pixelWidth_, pixelHeight_};
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

    glNamedFramebufferRenderbuffer(
        frameBuffer,
        GL_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        stencilRenderBuffer);
#else
    GLint oldFrameBuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFrameBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        renderBuffer_);

    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        stencilRenderBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, oldFrameBuffer);
#endif
}

} // namespace pomdog::gpu::detail::gl4
