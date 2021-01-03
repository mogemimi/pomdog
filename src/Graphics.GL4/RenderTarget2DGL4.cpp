// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "RenderTarget2DGL4.hpp"
#include "ErrorChecker.hpp"
#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog::Detail::GL4 {

std::unique_ptr<Error>
RenderTarget2DGL4::Initialize(
    std::int32_t pixelWidthIn,
    std::int32_t pixelHeightIn,
    std::int32_t levelCountIn,
    SurfaceFormat formatIn,
    std::int32_t multiSampleCount) noexcept
{
    if (auto err = texture.Initialize(pixelWidthIn, pixelHeightIn, levelCountIn, formatIn); err != nullptr) {
        return Errors::Wrap(std::move(err), "failed to initialize texture");
    }

    pixelWidth = pixelWidthIn;
    pixelHeight = pixelHeightIn;
    levelCount = levelCountIn;
    format = formatIn;
    generateMipmap = (levelCountIn > 1);
    multiSampleEnabled = (multiSampleCount > 1);

    return nullptr;
}

RenderTarget2DGL4::~RenderTarget2DGL4() = default;

std::int32_t RenderTarget2DGL4::GetWidth() const noexcept
{
    return pixelWidth;
}

std::int32_t RenderTarget2DGL4::GetHeight() const noexcept
{
    return pixelHeight;
}

std::int32_t RenderTarget2DGL4::GetLevelCount() const noexcept
{
    return levelCount;
}

SurfaceFormat RenderTarget2DGL4::GetFormat() const noexcept
{
    return format;
}

Rectangle RenderTarget2DGL4::GetBounds() const noexcept
{
    return Rectangle{0, 0, pixelWidth, pixelHeight};
}

void RenderTarget2DGL4::GetData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const
{
    texture.GetData(result, offsetInBytes, sizeInBytes);
}

void RenderTarget2DGL4::BindToFramebuffer(GLuint frameBuffer, GLenum attachmentPoint)
{
    constexpr GLint attachedMipmapLevel = 0;

#if defined(POMDOG_PLATFORM_WIN32)
    // NOTE: OpenGL >= 4.5
    glNamedFramebufferTexture(
        frameBuffer,
        attachmentPoint,
        texture.GetTextureHandle().value,
        attachedMipmapLevel);
    POMDOG_CHECK_ERROR_GL4("glNamedFramebufferTexture");
#else
    const GLenum textureTarget = (multiSampleEnabled
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
        texture.GetTextureHandle().value,
        attachedMipmapLevel);
    POMDOG_CHECK_ERROR_GL4("glFramebufferTexture2D");

    glBindFramebuffer(GL_FRAMEBUFFER, oldFrameBuffer);
    POMDOG_CHECK_ERROR_GL4("glBindFramebuffer");
#endif
}

void RenderTarget2DGL4::UnbindFromFramebuffer(GLuint frameBuffer, GLenum attachmentPoint)
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
    const GLenum textureTarget = (multiSampleEnabled
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

    if (generateMipmap) {
        texture.GenerateMipmap();
    }
}

Texture2DObjectGL4 RenderTarget2DGL4::GetTextureHandle() const noexcept
{
    return texture.GetTextureHandle();
}

} // namespace Pomdog::Detail::GL4
