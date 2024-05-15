// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/platform/cocoa/opengl_context_cocoa.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/gpu/presentation_parameters.h"
#include "pomdog/utility/assert.h"
#include "pomdog/utility/errors.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::detail::cocoa {
namespace {

[[nodiscard]] NSOpenGLPixelFormat*
CreatePixelFormat(const gpu::PresentationParameters& presentationParameters) noexcept
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

    if (presentationParameters.multiSampleCount > 1) {
        attributes.push_back(NSOpenGLPFAMultisample);
        attributes.push_back(NSOpenGLPFASampleBuffers);
        attributes.push_back(1);
        attributes.push_back(NSOpenGLPFASamples);
        attributes.push_back(presentationParameters.multiSampleCount);
    }

    switch (presentationParameters.backBufferFormat) {
    case PixelFormat::R8G8B8A8_UNorm:
    case PixelFormat::B8G8R8A8_UNorm:
        attributes.push_back(NSOpenGLPFAColorSize);
        attributes.push_back(24);
        attributes.push_back(NSOpenGLPFAAlphaSize);
        attributes.push_back(8);
        break;
    case PixelFormat::R16G16B16A16_Float:
        attributes.push_back(NSOpenGLPFAColorSize);
        attributes.push_back(48);
        attributes.push_back(NSOpenGLPFAAlphaSize);
        attributes.push_back(16);
        break;
    case PixelFormat::R32G32B32A32_Float:
        attributes.push_back(NSOpenGLPFAColorSize);
        attributes.push_back(96);
        attributes.push_back(NSOpenGLPFAAlphaSize);
        attributes.push_back(32);
        break;
    case PixelFormat::A8_UNorm:
        attributes.push_back(NSOpenGLPFAAlphaSize);
        attributes.push_back(8);
        break;
    case PixelFormat::R8_UNorm:
        attributes.push_back(NSOpenGLPFAColorSize);
        attributes.push_back(8);
        break;
    case PixelFormat::R8G8_UNorm:
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

    switch (presentationParameters.depthStencilFormat) {
    case PixelFormat::Depth16:
        attributes.push_back(NSOpenGLPFADepthSize);
        attributes.push_back(16);
        break;
    case PixelFormat::Depth24Stencil8:
        attributes.push_back(NSOpenGLPFADepthSize);
        attributes.push_back(24);
        attributes.push_back(NSOpenGLPFAStencilSize);
        attributes.push_back(8);
        break;
    case PixelFormat::Depth32:
        attributes.push_back(NSOpenGLPFADepthSize);
        attributes.push_back(32);
        break;
    case PixelFormat::Depth32_Float_Stencil8_Uint:
        attributes.push_back(NSOpenGLPFADepthSize);
        attributes.push_back(32);
        attributes.push_back(NSOpenGLPFAStencilSize);
        attributes.push_back(8);
        break;
    case PixelFormat::Invalid:
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
OpenGLContextCocoa::initialize(const gpu::PresentationParameters& presentationParameters) noexcept
{
    // NOTE: Create a pixel format for OpenGL context.
    auto pixelFormat = detail::cocoa::CreatePixelFormat(presentationParameters);
    if (pixelFormat == nil) {
        return errors::make("failed to create NSOpenGLPixelFormat.");
    }

    // NOTE: Create a OpenGL context with the pixel format.
    openGLContext_ = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
    if (openGLContext_ == nil) {
        return errors::make("failed to create NSOpenGLContext.");
    }

    // NOTE: Make the context current.
    [openGLContext_ makeCurrentContext];

    constexpr GLint swapInterval = 1;
    [openGLContext_ setValues:&swapInterval forParameter:NSOpenGLContextParameterSwapInterval];

    return nullptr;
}

OpenGLContextCocoa::~OpenGLContextCocoa() noexcept
{
    openGLContext_ = nil;
}

void OpenGLContextCocoa::makeCurrent()
{
    POMDOG_ASSERT(openGLContext_ != nil);
    [openGLContext_ makeCurrentContext];
}

void OpenGLContextCocoa::clearCurrent()
{
    POMDOG_ASSERT(openGLContext_ != nil);
    [NSOpenGLContext clearCurrentContext];
}

void OpenGLContextCocoa::swapBuffers()
{
    POMDOG_ASSERT(openGLContext_ != nil);
    [openGLContext_ flushBuffer];
}

void OpenGLContextCocoa::lock() noexcept
{
    POMDOG_ASSERT(openGLContext_ != nil);
    if (auto p = [openGLContext_ CGLContextObj]; p != nullptr) {
        CGLLockContext(static_cast<CGLContextObj _Nonnull>(p));
    }
}

void OpenGLContextCocoa::unlock() noexcept
{
    POMDOG_ASSERT(openGLContext_ != nil);
    if (auto p = [openGLContext_ CGLContextObj]; p != nullptr) {
        CGLUnlockContext(static_cast<CGLContextObj _Nonnull>(p));
    }
}

void OpenGLContextCocoa::setView(NSView* view) noexcept
{
    POMDOG_ASSERT(openGLContext_ != nil);
    POMDOG_ASSERT(view != nil);

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
#endif
    // FIXME
    [openGLContext_ setView:view];
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}

void OpenGLContextCocoa::setView() noexcept
{
    POMDOG_ASSERT(openGLContext_ != nil);

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated"
#endif
    // FIXME
    [openGLContext_ setView:nil];
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
}

NSOpenGLContext* OpenGLContextCocoa::getNativeOpenGLContext() noexcept
{
    return openGLContext_;
}

} // namespace pomdog::detail::cocoa
