// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/depth_stencil_buffer.h"
#include "pomdog/gpu/forward_declarations.h"
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
public:
    ~DepthStencilBufferGL4() override;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        PixelFormat depthStencilFormat,
        std::int32_t multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const noexcept override;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    PixelFormat GetFormat() const noexcept override;

    /// Gets the size of the texture resource.
    Rectangle GetBounds() const noexcept override;

    void BindToFramebuffer(GLuint frameBuffer) noexcept;

private:
    GLuint renderBuffer = 0;
    std::int32_t pixelWidth = 0;
    std::int32_t pixelHeight = 0;
    PixelFormat depthStencilFormat = PixelFormat::Invalid;
    bool multiSampleEnabled = false;
    bool hasStencil = false;
};

} // namespace pomdog::gpu::detail::gl4
