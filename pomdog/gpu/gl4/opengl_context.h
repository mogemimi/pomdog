// Copyright mogemimi. Distributed under the MIT license.

#pragma once

namespace pomdog::gpu::detail::gl4 {

class OpenGLContext {
public:
    virtual ~OpenGLContext();

    virtual void makeCurrent() = 0;

    virtual void clearCurrent() = 0;

    virtual void swapBuffers() = 0;
};

} // namespace pomdog::gpu::detail::gl4
