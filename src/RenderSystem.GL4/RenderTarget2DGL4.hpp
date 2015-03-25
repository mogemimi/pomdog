// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RENDERTARGET2DGL4_0C6A48A1_E6BF_4896_9EB9_9F9A4082ABDF_HPP
#define POMDOG_RENDERTARGET2DGL4_0C6A48A1_E6BF_4896_9EB9_9F9A4082ABDF_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "OpenGLPrerequisites.hpp"
#include "Texture2DGL4.hpp"
#include "../RenderSystem/NativeRenderTarget2D.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#include "Pomdog/Utility/detail/Tagged.hpp"
#include "Pomdog/Utility/Optional.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {

using RenderBuffer2DGL4 = Tagged<GLuint, RenderTarget2D>;

class RenderTarget2DGL4 final: public NativeRenderTarget2D {
public:
	RenderTarget2DGL4(std::int32_t pixelWidth, std::int32_t pixelHeight,
		std::uint32_t levelCount, SurfaceFormat format, DepthFormat depthStencilFormat);

	~RenderTarget2DGL4();

	void BindToFramebuffer(GLenum attachmentPoint);
	void UnbindFromFramebuffer(GLenum attachmentPoint);

	void BindDepthStencilBuffer();

	Texture2DObjectGL4 const& GetTextureHandle() const;

private:
	Texture2DGL4 texture;
	Optional<RenderBuffer2DGL4> renderBuffer;
	bool generateMipmap;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_RENDERTARGET2DGL4_0C6A48A1_E6BF_4896_9EB9_9F9A4082ABDF_HPP)
