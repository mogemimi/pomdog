// Copyright (c) 2013-2020 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem.GL4/OpenGLContext.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#import <Cocoa/Cocoa.h>
#include <memory>

@class NSOpenGLContext;

namespace Pomdog {
class Error;
} // namespace Pomdog

namespace Pomdog::Detail::Cocoa {

class OpenGLContextCocoa final : public GL4::OpenGLContext {
public:
    OpenGLContextCocoa() noexcept;
    OpenGLContextCocoa(const OpenGLContextCocoa&) = delete;
    OpenGLContextCocoa& operator=(const OpenGLContextCocoa&) = delete;

    ~OpenGLContextCocoa() noexcept;

    [[nodiscard]] std::shared_ptr<Error>
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

} // namespace Pomdog::Detail::Cocoa
