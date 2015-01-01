//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "RenderTarget2DGL4.hpp"
#include "ErrorChecker.hpp"
#include <Pomdog/Graphics/DepthFormat.hpp>

namespace Pomdog {
namespace Details {
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

		#ifdef DEBUG
		ErrorChecker::CheckError("glBindRenderbuffer", __FILE__, __LINE__);
		#endif

		POMDOG_ASSERT(pixelWidth > 0);
		POMDOG_ASSERT(pixelHeight > 0);
	
		glRenderbufferStorage(GL_RENDERBUFFER, *ToGLDepthStencilFormat(depthStencilFormat), pixelWidth, pixelHeight);

		#ifdef DEBUG
		ErrorChecker::CheckError("glRenderbufferStorage", __FILE__, __LINE__);
		#endif

		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
}
//-----------------------------------------------------------------------
RenderTarget2DGL4::~RenderTarget2DGL4()
{
	if (renderBuffer) {
		glDeleteRenderbuffers(1, renderBuffer->Data());
	}
}
//-----------------------------------------------------------------------
void RenderTarget2DGL4::Apply(std::uint32_t index)
{
	texture.Apply(index);
}
//-----------------------------------------------------------------------
void RenderTarget2DGL4::BindToFramebuffer(GLenum attachmentPoint)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentPoint, GL_TEXTURE_2D,
		texture.NativeHandle().value, 0);

	#ifdef DEBUG
	ErrorChecker::CheckError("glFramebufferTexture2D", __FILE__, __LINE__);
	#endif
}
//-----------------------------------------------------------------------
void RenderTarget2DGL4::UnbindFromFramebuffer(GLenum attachmentPoint)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentPoint, GL_TEXTURE_2D, 0, 0);

	#ifdef DEBUG
	ErrorChecker::CheckError("glFramebufferTexture2D", __FILE__, __LINE__);
	#endif
	
	if (generateMipmap)
	{
		texture.GenerateMipmap();
	}
}
//-----------------------------------------------------------------------
void RenderTarget2DGL4::BindDepthStencilBuffer()
{
	if (renderBuffer)
	{
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer->value);

		#ifdef DEBUG
		ErrorChecker::CheckError("glFramebufferRenderbuffer", __FILE__, __LINE__);
		#endif
	}
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
