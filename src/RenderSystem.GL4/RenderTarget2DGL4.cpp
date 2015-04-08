// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "RenderTarget2DGL4.hpp"
#include "ErrorChecker.hpp"
#include "Pomdog/Graphics/DepthFormat.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {
namespace {

static Optional<GLenum> ToGLDepthStencilFormat(DepthFormat depthFormat)
{
    POMDOG_ASSERT(depthFormat != DepthFormat::None);

    switch (depthFormat) {
    case DepthFormat::Depth16: return GL_DEPTH_COMPONENT16;
    case DepthFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
    case DepthFormat::Depth32: return GL_DEPTH_COMPONENT32;
    case DepthFormat::None:
        break;
    }
    return OptionalType::NullOptional;
}

}// unnamed namespace
//-----------------------------------------------------------------------
RenderTarget2DGL4::RenderTarget2DGL4(std::int32_t pixelWidth, std::int32_t pixelHeight,
    std::uint32_t levelCount, SurfaceFormat format, DepthFormat depthStencilFormat)
    : texture(pixelWidth, pixelHeight, levelCount, format)
    , generateMipmap(levelCount > 1)
{
    if (DepthFormat::None != depthStencilFormat)
    {
        renderBuffer = ([] {
            RenderBuffer2DGL4 nativeBuffer;
            glGenRenderbuffers(1, nativeBuffer.Data());
            return std::move(nativeBuffer);
        })();

        POMDOG_ASSERT(renderBuffer);

        glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer->value);
        POMDOG_CHECK_ERROR_GL4("glBindRenderbuffer");

        POMDOG_ASSERT(pixelWidth > 0);
        POMDOG_ASSERT(pixelHeight > 0);

        glRenderbufferStorage(GL_RENDERBUFFER, *ToGLDepthStencilFormat(depthStencilFormat), pixelWidth, pixelHeight);
        POMDOG_CHECK_ERROR_GL4("glRenderbufferStorage");

        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
}
//-----------------------------------------------------------------------
RenderTarget2DGL4::~RenderTarget2DGL4()
{
    if (renderBuffer) {
        glDeleteRenderbuffers(1, renderBuffer->Data());
        POMDOG_CHECK_ERROR_GL4("glDeleteRenderbuffers");
    }
}
//-----------------------------------------------------------------------
void RenderTarget2DGL4::BindToFramebuffer(GLenum attachmentPoint)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentPoint, GL_TEXTURE_2D,
        texture.GetTextureHandle().value, 0);
    POMDOG_CHECK_ERROR_GL4("glFramebufferTexture2D");
}
//-----------------------------------------------------------------------
void RenderTarget2DGL4::UnbindFromFramebuffer(GLenum attachmentPoint)
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentPoint, GL_TEXTURE_2D, 0, 0);
    POMDOG_CHECK_ERROR_GL4("glFramebufferTexture2D");

    if (generateMipmap) {
        texture.GenerateMipmap();
    }
}
//-----------------------------------------------------------------------
void RenderTarget2DGL4::BindDepthStencilBuffer()
{
    if (renderBuffer) {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->value);
        POMDOG_CHECK_ERROR_GL4("glFramebufferRenderbuffer");
    }
}
//-----------------------------------------------------------------------
Texture2DObjectGL4 const& RenderTarget2DGL4::GetTextureHandle() const
{
    return texture.GetTextureHandle();
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog
