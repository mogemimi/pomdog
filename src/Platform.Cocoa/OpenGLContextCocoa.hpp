// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "../RenderSystem.GL4/OpenGLContext.hpp"
#include "Pomdog/Graphics/detail/ForwardDeclarations.hpp"
#import <Cocoa/Cocoa.h>

@class NSOpenGLContext, NSOpenGLPixelFormat;

namespace Pomdog {
namespace Detail {
namespace Cocoa {

struct CocoaOpenGLHelper {
    static NSOpenGLPixelFormat* CreatePixelFormat(
        PresentationParameters const& presentationParameters);
};

class OpenGLContextCocoa final: public GL4::OpenGLContext {
public:
    OpenGLContextCocoa() = delete;

    explicit OpenGLContextCocoa(NSOpenGLPixelFormat* pixelFormat);

    ~OpenGLContextCocoa();

    void MakeCurrent() override;

    void ClearCurrent() override;

    void SwapBuffers() override;

    void Lock();

    void Unlock();

    void SetView(NSView* view);

    void SetView();

    NSOpenGLContext* NativeOpenGLContext();

private:
    __strong NSOpenGLContext* openGLContext;
};

} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog
