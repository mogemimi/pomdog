// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/gl4/opengl_context.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#import <Cocoa/Cocoa.h>
#include <memory>
#include <tuple>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

@class NSOpenGLContext;
@class NSView;

namespace pomdog {
class Error;
} // namespace pomdog

namespace pomdog::gpu {
struct PresentationParameters;
} // namespace pomdog::gpu

namespace pomdog::detail::cocoa {

class OpenGLContextCocoa : public gpu::detail::gl4::OpenGLContext {
public:
    OpenGLContextCocoa();

    ~OpenGLContextCocoa() override;

    [[nodiscard]] static std::tuple<std::shared_ptr<OpenGLContextCocoa>, std::unique_ptr<Error>>
    create(const gpu::PresentationParameters& presentationParameters) noexcept;

    virtual void
    lock() noexcept = 0;

    virtual void
    unlock() noexcept = 0;

    virtual void
    setView(NSView* view) noexcept = 0;

    virtual void
    setView() noexcept = 0;

    [[nodiscard]] virtual NSOpenGLContext*
    getNativeOpenGLContext() noexcept = 0;
};

} // namespace pomdog::detail::cocoa
