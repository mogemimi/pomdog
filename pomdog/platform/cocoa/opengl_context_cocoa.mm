// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/cocoa/opengl_context_cocoa.hpp"
#include "pomdog/graphics/gl4/opengl_prerequisites.hpp"
#include "pomdog/graphics/presentation_parameters.hpp"
#include "pomdog/utility/assert.hpp"
#include "pomdog/utility/errors.hpp"
#include <vector>

namespace pomdog::detail::cocoa {
namespace {

[[nodiscard]] NSOpenGLPixelFormat*
CreatePixelFormat(const PresentationParameters& presentationParameters) noexcept
{
    std::vector<NSOpenGLPixelFormatAttribute> attributes = {
        // NOTE: OpenGL >= 4.1
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion4_1Core,

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
    case SurfaceFormat::B8G8R8A8_UNorm:
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
    case SurfaceFormat::A8_UNorm:
        attributes.push_back(NSOpenGLPFAAlphaSize);
        attributes.push_back(8);
        break;
    case SurfaceFormat::R8_UNorm:
        attributes.push_back(NSOpenGLPFAColorSize);
        attributes.push_back(8);
        break;
    case SurfaceFormat::R8G8_UNorm:
        attributes.push_back(NSOpenGLPFAColorSize);
        attributes.push_back(16);
        break;
    default:
        attributes.push_back(NSOpenGLPFAColorSize);
        attributes.push_back(24);
        attributes.push_back(NSOpenGLPFAAlphaSize);
        attributes.push_back(8);
        break;
    }

    switch (presentationParameters.DepthStencilFormat) {
    case SurfaceFormat::Depth16:
        attributes.push_back(NSOpenGLPFADepthSize);
        attributes.push_back(16);
        break;
    case SurfaceFormat::Depth24Stencil8:
        attributes.push_back(NSOpenGLPFADepthSize);
        attributes.push_back(24);
        attributes.push_back(NSOpenGLPFAStencilSize);
        attributes.push_back(8);
        break;
    case SurfaceFormat::Depth32:
        attributes.push_back(NSOpenGLPFADepthSize);
        attributes.push_back(32);
        break;
    case SurfaceFormat::Depth32_Float_Stencil8_Uint:
        attributes.push_back(NSOpenGLPFADepthSize);
        attributes.push_back(32);
        attributes.push_back(NSOpenGLPFAStencilSize);
        attributes.push_back(8);
        break;
    case SurfaceFormat::Invalid:
        [[fallthrough]];
    default:
        break;
    }

    attributes.push_back(0);
    return [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes.data()];
}

} // namespace

OpenGLContextCocoa::OpenGLContextCocoa() noexcept = default;

std::unique_ptr<Error>
OpenGLContextCocoa::Initialize(const PresentationParameters& presentationParameters) noexcept
{
    // NOTE: Create a pixel format for OpenGL context.
    auto pixelFormat = detail::cocoa::CreatePixelFormat(presentationParameters);
    if (pixelFormat == nil) {
        return errors::New("failed to create NSOpenGLPixelFormat.");
    }

    // NOTE: Create a OpenGL context with the pixel format.
    openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
    if (openGLContext == nil) {
        return errors::New("failed to create NSOpenGLContext.");
    }

    // NOTE: Make the context current.
    [openGLContext makeCurrentContext];

    constexpr GLint swapInterval = 1;
    [openGLContext setValues:&swapInterval forParameter:NSOpenGLContextParameterSwapInterval];

    return nullptr;
}

OpenGLContextCocoa::~OpenGLContextCocoa() noexcept
{
    openGLContext = nil;
}

void OpenGLContextCocoa::MakeCurrent()
{
    POMDOG_ASSERT(openGLContext != nil);
    [openGLContext makeCurrentContext];
}

void OpenGLContextCocoa::ClearCurrent()
{
    POMDOG_ASSERT(openGLContext != nil);
    [NSOpenGLContext clearCurrentContext];
}

void OpenGLContextCocoa::SwapBuffers()
{
    POMDOG_ASSERT(openGLContext != nil);
    [openGLContext flushBuffer];
}

void OpenGLContextCocoa::Lock() noexcept
{
    POMDOG_ASSERT(openGLContext != nil);
    if (auto p = [openGLContext CGLContextObj]; p != nullptr) {
        CGLLockContext(p);
    }
}

void OpenGLContextCocoa::Unlock() noexcept
{
    POMDOG_ASSERT(openGLContext != nil);
    if (auto p = [openGLContext CGLContextObj]; p != nullptr) {
        CGLUnlockContext(p);
    }
}

void OpenGLContextCocoa::SetView(NSView* view) noexcept
{
    POMDOG_ASSERT(openGLContext != nil);
    POMDOG_ASSERT(view != nil);

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
#endif
    // FIXME
    [openGLContext setView:view];
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}

void OpenGLContextCocoa::SetView() noexcept
{
    POMDOG_ASSERT(openGLContext != nil);

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
#endif
    // FIXME
    [openGLContext setView:nil];
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}

NSOpenGLContext* OpenGLContextCocoa::GetNativeOpenGLContext() noexcept
{
    return openGLContext;
}

} // namespace pomdog::detail::cocoa
