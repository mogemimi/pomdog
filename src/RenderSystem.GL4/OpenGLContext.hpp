//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#ifndef POMDOG_SRC_GL4_OPENGLCONTEXT_62674DFD_AE30_4350_9D3D_B6BCAFFFA941_HPP
#define POMDOG_SRC_GL4_OPENGLCONTEXT_62674DFD_AE30_4350_9D3D_B6BCAFFFA941_HPP

#if (_MSC_VER > 1000)
#pragma once
#endif

#include "../Utility/Noncopyable.hpp"

namespace Pomdog {
namespace Details {
namespace RenderSystem {
namespace GL4 {

class OpenGLContext: Noncopyable {
public:
	virtual ~OpenGLContext() = default;

	virtual void MakeCurrentContext() = 0;

	virtual void ClearCurrentContext() = 0;

	virtual void SwapBuffers() = 0;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Details
}// namespace Pomdog

#endif // !defined(POMDOG_SRC_GL4_OPENGLCONTEXT_62674DFD_AE30_4350_9D3D_B6BCAFFFA941_HPP)
