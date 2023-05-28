// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/gl4/opengl_context.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Cocoa/Cocoa.h>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

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
    initialize(const gpu::PresentationParameters& presentationParameters) noexcept;

    void makeCurrent() override;

    void clearCurrent() override;

    void swapBuffers() override;

    void lock() noexcept;

    void unlock() noexcept;

    void setView(NSView* view) noexcept;

    void setView() noexcept;

    [[nodiscard]] NSOpenGLContext*
    getNativeOpenGLContext() noexcept;

private:
    __strong NSOpenGLContext* openGLContext_ = nil;
};

} // namespace pomdog::detail::cocoa
