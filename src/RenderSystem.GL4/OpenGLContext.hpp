//
//  Copyright (C) 2013 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#ifndef POMDOG_GL4_OPENGLCONTEXT_HPP
#define POMDOG_GL4_OPENGLCONTEXT_HPP

#if (_MSC_VER > 1000)
#	pragma once
#endif

#include <Pomdog/Utility/Noncopyable.hpp>

namespace Pomdog {
namespace Details {
namespace GL4 {

class OpenGLContext: Noncopyable
{
public:
	virtual ~OpenGLContext() = default;

	virtual void BindCurrentContext() = 0;

	virtual void UnbindCurrentContext() = 0;

	virtual void SwapBuffers() = 0;
};

}// namespace GL4
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_GL4_OPENGLCONTEXT_HPP)
