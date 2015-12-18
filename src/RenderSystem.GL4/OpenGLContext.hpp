// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#ifndef POMDOG_OPENGLCONTEXT_62674DFD_HPP
#define POMDOG_OPENGLCONTEXT_62674DFD_HPP

#include "../Utility/Noncopyable.hpp"

namespace Pomdog {
namespace Detail {
namespace GL4 {

class OpenGLContext: Noncopyable {
public:
    virtual ~OpenGLContext() = default;

    virtual void MakeCurrent() = 0;

    virtual void ClearCurrent() = 0;

    virtual void SwapBuffers() = 0;
};

} // namespace GL4
} // namespace Detail
} // namespace Pomdog

#endif // POMDOG_OPENGLCONTEXT_62674DFD_HPP
