//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "GraphicsContextGL4.hpp"
#include <array>
#include <utility>
#include <Pomdog/Math/Color.hpp>
#include <Pomdog/Math/Rectangle.hpp>
#include <Pomdog/Utility/Assert.hpp>
#include <Pomdog/Graphics/ClearOptions.hpp>
#include <Pomdog/Graphics/Viewport.hpp>
#include <Pomdog/Application/GameWindow.hpp>
#include "OpenGLContext.hpp"
#include "../RenderSystem/GraphicsCapabilities.hpp"

#include <Pomdog/Logging/Log.hpp>
#include <Pomdog/Logging/LogStream.hpp>

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {
//-----------------------------------------------------------------------
GraphicsContextGL4::GraphicsContextGL4(std::shared_ptr<OpenGLContext> openGLContext, std::weak_ptr<GameWindow> window)
	: nativeContext(std::move(openGLContext))
	, gameWindow(std::move(window))
{
	auto version = reinterpret_cast<char const*>(glGetString(GL_VERSION));
	Log::Stream(LoggingLevel::Internal) << "OpenGL Version: " << version << "\n";
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::Clear(Color const& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::Clear(ClearOptions options, Color const& color, float depth, std::int32_t stencil)
{
	glClearColor(color.r, color.g, color.b, color.a);
	
	GLbitfield mask = 0;
	
	if ((options | ClearOptions::DepthBuffer) == options) {
		mask |= GL_DEPTH_BUFFER_BIT;
		glClearDepth(static_cast<GLclampd>(depth));
	}
	if ((options | ClearOptions::Stencil) == options) {
		mask |= GL_STENCIL_BUFFER_BIT;
		glClearStencil(stencil);
	}
	if ((options | ClearOptions::RenderTarget) == options) {
		mask |= GL_COLOR_BUFFER_BIT;
		glClearColor(color.r, color.g, color.b, color.a);
	}
	
	glClear(mask);
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::Present()
{
	glFlush();

	nativeContext->SwapBuffers();
}
//-----------------------------------------------------------------------
GraphicsCapabilities GraphicsContextGL4::GetCapabilities() const
{
	GraphicsCapabilities capabilities;

	GLint maxTextureUnits = 0;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	POMDOG_ASSERT(maxTextureUnits > 0);
	
	capabilities.SamplerSlotCount = maxTextureUnits;
	return capabilities;
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetViewport(Viewport const& viewport)
{
	POMDOG_ASSERT(viewport.GetWidth() > 0);
	POMDOG_ASSERT(viewport.GetHeight() > 0);

	GLint viewportY = viewport.TopLeftY();
	if (auto window = gameWindow.lock()) {
		viewportY = window->GetClientBounds().height - (viewport.TopLeftY() + viewport.GetHeight());
	}

	// Notes: glViewport(x, y, width, height)
	// "Specify the lower left corner of the viewport rectangle, in pixels."
	glViewport(
		static_cast<GLint>(viewport.TopLeftX()),
		static_cast<GLint>(viewportY),
		static_cast<GLsizei>(viewport.GetWidth()), 
		static_cast<GLsizei>(viewport.GetHeight())
	);
}
//-----------------------------------------------------------------------
Rectangle GraphicsContextGL4::GetScissorRectangle() const
{
	std::array<GLint, 4> scissorBox;
	glGetIntegerv(GL_SCISSOR_BOX, scissorBox.data());
	return Rectangle{ scissorBox[0], scissorBox[1], scissorBox[2], scissorBox[3] };
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::SetScissorRectangle(Rectangle const& rectangle)
{
	glScissor(rectangle.x, rectangle.y, rectangle.width, rectangle.height);
}
//-----------------------------------------------------------------------
}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog
