// Copyright mogemimi. Distributed under the MIT license.

#pragma once

namespace pomdog::gpu::detail::gl4 {

/// OpenGLContext is the abstract interface for platform-specific OpenGL context management.
class OpenGLContext {
public:
    virtual ~OpenGLContext();

    /// Makes this OpenGL context current on the calling thread.
    virtual void makeCurrent() = 0;

    /// Releases this context from the calling thread.
    virtual void clearCurrent() = 0;

    /// Swaps the front and back color buffers.
    virtual void swapBuffers() = 0;
};

} // namespace pomdog::gpu::detail::gl4
