// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/gpu/gl4/render_target2d_gl4.h"
#include "pomdog/gpu/gl4/error_checker.h"
#include "pomdog/math/rectangle.h"
#include "pomdog/utility/assert.h"

namespace pomdog::gpu::detail::gl4 {

std::unique_ptr<Error>
RenderTarget2DGL4::initialize(
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    std::int32_t levelCountIn,
    PixelFormat formatIn,
    std::int32_t multiSampleCount) noexcept
{
    if (auto err = texture_.initialize(pixelWidthIn, pixelHeightIn, levelCountIn, formatIn); err != nullptr) {
        return errors::wrap(std::move(err), "failed to initialize texture");
    }

    pixelWidth_ = pixelWidthIn;
    pixelHeight_ = pixelHeightIn;
    levelCount_ = levelCountIn;
    format_ = formatIn;
    generateMipmap_ = (levelCountIn > 1);
    multiSampleEnabled_ = (multiSampleCount > 1);

    return nullptr;
}

RenderTarget2DGL4::~RenderTarget2DGL4() = default;

std::int32_t RenderTarget2DGL4::getWidth() const noexcept
{
    return pixelWidth_;
}

std::int32_t RenderTarget2DGL4::getHeight() const noexcept
{
    return pixelHeight_;
}

std::int32_t RenderTarget2DGL4::getLevelCount() const noexcept
{
    return levelCount_;
}

PixelFormat RenderTarget2DGL4::getFormat() const noexcept
{
    return format_;
}

Rectangle RenderTarget2DGL4::getBounds() const noexcept
{
    return Rectangle{0, 0, pixelWidth_, pixelHeight_};
}

void RenderTarget2DGL4::getData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const
{
    texture_.getData(result, offsetInBytes, sizeInBytes);
}

void RenderTarget2DGL4::bindToFramebuffer(GLuint frameBuffer, GLenum attachmentPoint)
{
    constexpr GLint attachedMipmapLevel = 0;

#if defined(POMDOG_PLATFORM_WIN32)
    // NOTE: OpenGL >= 4.5
    glNamedFramebufferTexture(
        frameBuffer,
        attachmentPoint,
        texture_.getTextureHandle().value,
        attachedMipmapLevel);
    POMDOG_CHECK_ERROR_GL4("glNamedFramebufferTexture");
#else
    const GLenum textureTarget = (multiSampleEnabled_
                                      ? GL_TEXTURE_2D_MULTISAMPLE
                                      : GL_TEXTURE_2D);

    GLint oldFrameBuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFrameBuffer);
    POMDOG_CHECK_ERROR_GL4("glGetIntegerv(GL_FRAMEBUFFER_BINDING)");

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        attachmentPoint,
        textureTarget,
        texture_.getTextureHandle().value,
        attachedMipmapLevel);
    POMDOG_CHECK_ERROR_GL4("glFramebufferTexture2D");

    glBindFramebuffer(GL_FRAMEBUFFER, oldFrameBuffer);
    POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");
#endif
}

void RenderTarget2DGL4::unbindFromFramebuffer(GLuint frameBuffer, GLenum attachmentPoint)
{
    constexpr GLint attachedMipmapLevel = 0;

#if defined(POMDOG_PLATFORM_WIN32)
    // NOTE: OpenGL >= 4.5
    glNamedFramebufferTexture(
        frameBuffer,
        attachmentPoint,
        0,
        attachedMipmapLevel);
    POMDOG_CHECK_ERROR_GL4("glNamedFramebufferTexture");
#else
    const GLenum textureTarget = (multiSampleEnabled_
                                      ? GL_TEXTURE_2D_MULTISAMPLE
                                      : GL_TEXTURE_2D);

    GLint oldFrameBuffer = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldFrameBuffer);
    POMDOG_CHECK_ERROR_GL4("glGetIntegerv(GL_FRAMEBUFFER_BINDING)");

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");

    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        attachmentPoint,
        textureTarget,
        0,
        attachedMipmapLevel);
    POMDOG_CHECK_ERROR_GL4("glFramebufferTexture2D");

    glBindFramebuffer(GL_FRAMEBUFFER, oldFrameBuffer);
    POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");
#endif

    if (generateMipmap_) {
        texture_.generateMipmap();
    }
}

Texture2DObjectGL4 RenderTarget2DGL4::getTextureHandle() const noexcept
{
    return texture_.getTextureHandle();
}

} // namespace pomdog::gpu::detail::gl4
