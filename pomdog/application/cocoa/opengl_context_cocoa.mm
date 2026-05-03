// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/application/cocoa/opengl_context_cocoa.h"
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
createPixelFormat(const gpu::PresentationParameters& presentationParameters) noexcept
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
    case gpu::PixelFormat::R8G8B8A8_UNorm:
    case gpu::PixelFormat::B8G8R8A8_UNorm:
        attributes.push_back(NSOpenGLPFAColorSize);
        attributes.push_back(24);
        attributes.push_back(NSOpenGLPFAAlphaSize);
        attributes.push_back(8);
        break;
    case gpu::PixelFormat::R16G16B16A16_Float:
        attributes.push_back(NSOpenGLPFAColorSize);
        attributes.push_back(48);
        attributes.push_back(NSOpenGLPFAAlphaSize);
        attributes.push_back(16);
        break;
    case gpu::PixelFormat::R32G32B32A32_Float:
        attributes.push_back(NSOpenGLPFAColorSize);
        attributes.push_back(96);
        attributes.push_back(NSOpenGLPFAAlphaSize);
        attributes.push_back(32);
        break;
    case gpu::PixelFormat::A8_UNorm:
        attributes.push_back(NSOpenGLPFAAlphaSize);
        attributes.push_back(8);
        break;
    case gpu::PixelFormat::R8_UNorm:
        attributes.push_back(NSOpenGLPFAColorSize);
        attributes.push_back(8);
        break;
    case gpu::PixelFormat::R8G8_UNorm:
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
    case gpu::PixelFormat::Depth16:
        attributes.push_back(NSOpenGLPFADepthSize);
        attributes.push_back(16);
        break;
    case gpu::PixelFormat::Depth24Stencil8:
        attributes.push_back(NSOpenGLPFADepthSize);
        attributes.push_back(24);
        attributes.push_back(NSOpenGLPFAStencilSize);
        attributes.push_back(8);
        break;
    case gpu::PixelFormat::Depth32:
        attributes.push_back(NSOpenGLPFADepthSize);
        attributes.push_back(32);
        break;
    case gpu::PixelFormat::Depth32_Float_Stencil8_Uint:
        attributes.push_back(NSOpenGLPFADepthSize);
        attributes.push_back(32);
        attributes.push_back(NSOpenGLPFAStencilSize);
        attributes.push_back(8);
        break;
    case gpu::PixelFormat::Invalid:
        [[fallthrough]];
    default:
        break;
    }

    attributes.push_back(0);
    return [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes.data()];
}

class OpenGLContextCocoaImpl final : public OpenGLContextCocoa {
private:
    __strong NSOpenGLContext* openGLContext_ = nil;

public:
    ~OpenGLContextCocoaImpl() noexcept override
    {
        openGLContext_ = nil;
    }

    [[nodiscard]] std::unique_ptr<Error>
    initialize(const gpu::PresentationParameters& presentationParameters) noexcept
    {
        // NOTE: Create a pixel format for OpenGL context.
        auto pixelFormat = createPixelFormat(presentationParameters);
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

    void makeCurrent() override
    {
        POMDOG_ASSERT(openGLContext_ != nil);
        [openGLContext_ makeCurrentContext];
    }

    void clearCurrent() override
    {
        POMDOG_ASSERT(openGLContext_ != nil);
        [NSOpenGLContext clearCurrentContext];
    }

    void swapBuffers() override
    {
        POMDOG_ASSERT(openGLContext_ != nil);
        [openGLContext_ flushBuffer];
    }

    void lock() noexcept override
    {
        POMDOG_ASSERT(openGLContext_ != nil);
        if (auto p = [openGLContext_ CGLContextObj]; p != nullptr) {
            CGLLockContext(static_cast<CGLContextObj _Nonnull>(p));
        }
    }

    void unlock() noexcept override
    {
        POMDOG_ASSERT(openGLContext_ != nil);
        if (auto p = [openGLContext_ CGLContextObj]; p != nullptr) {
            CGLUnlockContext(static_cast<CGLContextObj _Nonnull>(p));
        }
    }

    void setView(NSView* view) noexcept override
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

    void setView() noexcept override
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

    NSOpenGLContext*
    getNativeOpenGLContext() noexcept override
    {
        return openGLContext_;
    }

    [[nodiscard]] i32
    getSwapInterval() const noexcept override
    {
        POMDOG_ASSERT(openGLContext_ != nil);
        GLint interval = 0;
        [openGLContext_ getValues:&interval forParameter:NSOpenGLContextParameterSwapInterval];
        return static_cast<i32>(interval);
    }

    void
    setSwapInterval(i32 interval) noexcept override
    {
        POMDOG_ASSERT(openGLContext_ != nil);
        const GLint value = static_cast<GLint>(interval);
        [openGLContext_ setValues:&value forParameter:NSOpenGLContextParameterSwapInterval];
    }
};

} // namespace

OpenGLContextCocoa::OpenGLContextCocoa() = default;

OpenGLContextCocoa::~OpenGLContextCocoa() = default;

std::tuple<std::shared_ptr<OpenGLContextCocoa>, std::unique_ptr<Error>>
OpenGLContextCocoa::create(const gpu::PresentationParameters& presentationParameters) noexcept
{
    auto ctx = std::make_shared<OpenGLContextCocoaImpl>();
    if (auto err = ctx->initialize(presentationParameters); err != nullptr) {
        return std::make_tuple(nullptr, std::move(err));
    }
    return std::make_tuple(std::move(ctx), nullptr);
}

} // namespace pomdog::detail::cocoa
