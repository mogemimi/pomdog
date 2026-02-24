// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/depth_stencil_buffer.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/gpu/gl4/texture2d_gl4.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/tagged.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {

class DepthStencilBufferGL4 final : public DepthStencilBuffer {
private:
    GLuint renderBuffer_ = 0;
    i32 pixelWidth_ = 0;
    i32 pixelHeight_ = 0;
    PixelFormat depthStencilFormat_ = PixelFormat::Invalid;
    bool multiSampleEnabled_ = false;
    bool hasStencil_ = false;

public:
    ~DepthStencilBufferGL4() override;

    /// Initializes the depth stencil buffer.
    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        i32 pixelWidth,
        i32 pixelHeight,
        PixelFormat depthStencilFormat,
        i32 multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] i32
    getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] i32
    getHeight() const noexcept override;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    [[nodiscard]] PixelFormat
    getFormat() const noexcept override;

    /// Gets the size of the texture resource.
    [[nodiscard]] Rectangle
    getBounds() const noexcept override;

    /// Binds the depth stencil buffer to the frame buffer.
    void bindToFramebuffer(GLuint frameBuffer) noexcept;
};

} // namespace pomdog::gpu::detail::gl4
