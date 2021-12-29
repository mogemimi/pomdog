// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/gl4/opengl_context.h"
#import <Cocoa/Cocoa.h>
#include <memory>

@class NSOpenGLContext;

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::detail::cocoa {

class OpenGLContextCocoa final : public gpu::detail::gl4::OpenGLContext {
public:
    OpenGLContextCocoa() noexcept;
    OpenGLContextCocoa(const OpenGLContextCocoa&) = delete;
    OpenGLContextCocoa& operator=(const OpenGLContextCocoa&) = delete;

    ~OpenGLContextCocoa() noexcept override;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(const PresentationParameters& presentationParameters) noexcept;

    void MakeCurrent() override;

    void ClearCurrent() override;

    void SwapBuffers() override;

    void Lock() noexcept;

    void Unlock() noexcept;

    void SetView(NSView* view) noexcept;

    void SetView() noexcept;

    [[nodiscard]] NSOpenGLContext*
    GetNativeOpenGLContext() noexcept;

private:
    __strong NSOpenGLContext* openGLContext = nil;
};

} // namespace pomdog::detail::cocoa
