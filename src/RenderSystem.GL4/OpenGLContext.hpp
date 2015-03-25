// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_OPENGLCONTEXT_62674DFD_AE30_4350_9D3D_B6BCAFFFA941_HPP
#define POMDOG_OPENGLCONTEXT_62674DFD_AE30_4350_9D3D_B6BCAFFFA941_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "../Utility/Noncopyable.hpp"

namespace Pomdog {
namespace Detail {
namespace RenderSystem {
namespace GL4 {

class OpenGLContext: Noncopyable {
public:
    virtual ~OpenGLContext() = default;

    virtual void MakeCurrent() = 0;

    virtual void ClearCurrent() = 0;

    virtual void SwapBuffers() = 0;
};

}// namespace GL4
}// namespace RenderSystem
}// namespace Detail
}// namespace Pomdog

#endif // !defined(POMDOG_OPENGLCONTEXT_62674DFD_AE30_4350_9D3D_B6BCAFFFA941_HPP)
