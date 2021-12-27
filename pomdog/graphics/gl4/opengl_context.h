// Copyright mogemimi. Distributed under the MIT license.

#pragma once

namespace pomdog::detail::gl4 {

class OpenGLContext {
public:
    virtual ~OpenGLContext();

    virtual void MakeCurrent() = 0;

    virtual void ClearCurrent() = 0;

    virtual void SwapBuffers() = 0;
};

} // namespace pomdog::detail::gl4
