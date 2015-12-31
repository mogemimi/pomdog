// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "OpenGLContextCocoa.hpp"
#include "../RenderSystem.GL4/OpenGLPrerequisites.hpp"
#include "Pomdog/Graphics/PresentationParameters.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <vector>

namespace Pomdog {
namespace Detail {
namespace Cocoa {
//-----------------------------------------------------------------------
NSOpenGLPixelFormat* CocoaOpenGLHelper::CreatePixelFormat(
    PresentationParameters const& presentationParameters)
{
    std::vector<NSOpenGLPixelFormatAttribute> attributes =
    {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFANoRecovery,
        NSOpenGLPFAAllowOfflineRenderers,
    };

    if (presentationParameters.MultiSampleCount > 1) {
        attributes.push_back(NSOpenGLPFAMultisample);
        attributes.push_back(NSOpenGLPFASampleBuffers);
        attributes.push_back(1);
        attributes.push_back(NSOpenGLPFASamples);
        attributes.push_back(presentationParameters.MultiSampleCount);
    }

    switch (presentationParameters.BackBufferFormat) {
    case SurfaceFormat::R8G8B8A8_UNorm:
        attributes.push_back(NSOpenGLPFAColorSize);
        attributes.push_back(24);
        attributes.push_back(NSOpenGLPFAAlphaSize);
        attributes.push_back(8);
        break;
    case SurfaceFormat::R16G16B16A16_Float:
        attributes.push_back(NSOpenGLPFAColorSize);
        attributes.push_back(48);
        attributes.push_back(NSOpenGLPFAAlphaSize);
        attributes.push_back(16);
        break;
    case SurfaceFormat::R32G32B32A32_Float:
        attributes.push_back(NSOpenGLPFAColorSize);
        attributes.push_back(96);
        attributes.push_back(NSOpenGLPFAAlphaSize);
        attributes.push_back(32);
        break;
    default:
        attributes.push_back(NSOpenGLPFAColorSize);
        attributes.push_back(24);
        attributes.push_back(NSOpenGLPFAAlphaSize);
        attributes.push_back(8);
        break;
    }

    switch (presentationParameters.DepthStencilFormat) {
    case DepthFormat::Depth16:
        attributes.push_back(NSOpenGLPFADepthSize);
        attributes.push_back(16);
        break;
    case DepthFormat::Depth24Stencil8:
        attributes.push_back(NSOpenGLPFADepthSize);
        attributes.push_back(24);
        attributes.push_back(NSOpenGLPFAStencilSize);
        attributes.push_back(8);
        break;
    case DepthFormat::Depth32:
        attributes.push_back(NSOpenGLPFADepthSize);
        attributes.push_back(32);
        break;
    case DepthFormat::None:
        break;
    }

    attributes.push_back(0);
    return [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes.data()];
}
//-----------------------------------------------------------------------
OpenGLContextCocoa::OpenGLContextCocoa(NSOpenGLPixelFormat* pixelFormat)
    : openGLContext(nil)
{
    POMDOG_ASSERT(pixelFormat != nil);

    openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext: nil];
    [openGLContext makeCurrentContext];

    constexpr GLint swapInterval = 1;
    [openGLContext setValues:&swapInterval forParameter:NSOpenGLCPSwapInterval];
}
//-----------------------------------------------------------------------
OpenGLContextCocoa::~OpenGLContextCocoa()
{
    openGLContext = nil;
}
//-----------------------------------------------------------------------
void OpenGLContextCocoa::MakeCurrent()
{
    POMDOG_ASSERT(openGLContext != nil);
    [openGLContext makeCurrentContext];
}
//-----------------------------------------------------------------------
void OpenGLContextCocoa::ClearCurrent()
{
    POMDOG_ASSERT(openGLContext != nil);
    [NSOpenGLContext clearCurrentContext];
}
//-----------------------------------------------------------------------
void OpenGLContextCocoa::SwapBuffers()
{
    POMDOG_ASSERT(openGLContext != nil);
    [openGLContext flushBuffer];
}
//-----------------------------------------------------------------------
void OpenGLContextCocoa::Lock()
{
    POMDOG_ASSERT(openGLContext != nil);
    CGLLockContext([openGLContext CGLContextObj]);
}
//-----------------------------------------------------------------------
void OpenGLContextCocoa::Unlock()
{
    POMDOG_ASSERT(openGLContext != nil);
    CGLUnlockContext([openGLContext CGLContextObj]);
}
//-----------------------------------------------------------------------
void OpenGLContextCocoa::SetView(NSView* view)
{
    POMDOG_ASSERT(openGLContext != nil);
    POMDOG_ASSERT(view != nil);
    [openGLContext setView:view];
}
//-----------------------------------------------------------------------
void OpenGLContextCocoa::SetView()
{
    POMDOG_ASSERT(openGLContext != nil);
    [openGLContext setView:nil];
}
//-----------------------------------------------------------------------
NSOpenGLContext* OpenGLContextCocoa::NativeOpenGLContext()
{
    return openGLContext;
}
//-----------------------------------------------------------------------
} // namespace Cocoa
} // namespace Detail
} // namespace Pomdog
