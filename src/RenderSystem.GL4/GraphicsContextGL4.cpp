//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include "GraphicsContextGL4.hpp"
#include <Pomdog/Math/Color.hpp>
#include <utility>

namespace Pomdog {
namespace Details {
namespace GL4 {

//-----------------------------------------------------------------------
GraphicsContextGL4::GraphicsContextGL4(std::shared_ptr<GLContext> openGLContext)
	: nativeContext(openGLContext)
{
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::Clear(Color const& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
//-----------------------------------------------------------------------
void GraphicsContextGL4::Present()
{
	glFlush();

	nativeContext->SwapBuffers();
}

}// namespace GL4
}// namespace Details
}// namespace Pomdog
