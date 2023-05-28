// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/gl4/opengl_prerequisites.h"
#include "pomdog/gpu/gl4/texture2d_gl4.h"
#include "pomdog/gpu/pixel_format.h"
#include "pomdog/gpu/render_target2d.h"
#include "pomdog/utility/errors.h"
#include "pomdog/utility/tagged.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog::gpu::detail::gl4 {

using RenderBuffer2DGL4 = pomdog::detail::Tagged<GLuint, RenderTarget2D>;

class RenderTarget2DGL4 final : public RenderTarget2D {
public:
    ~RenderTarget2DGL4() override;

    [[nodiscard]] std::unique_ptr<Error>
    initialize(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        std::int32_t levelCount,
        PixelFormat format,
        std::int32_t multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    std::int32_t getWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t getHeight() const noexcept override;

    /// Gets the mipmap level.
    std::int32_t getLevelCount() const noexcept override;

    /// Gets the format of the pixel data in the render target.
    PixelFormat getFormat() const noexcept override;

    /// Gets the size of the texture resource.
    Rectangle getBounds() const noexcept override;

    /// Copies the pixel data from texture to memory.
    void getData(void* result, std::size_t offsetInBytes, std::size_t sizeInBytes) const override;

    void bindToFramebuffer(GLuint frameBuffer, GLenum attachmentPoint);

    void unbindFromFramebuffer(GLuint frameBuffer, GLenum attachmentPoint);

    /// Gets the handle of the native texture resource.
    [[nodiscard]] Texture2DObjectGL4
    getTextureHandle() const noexcept;

private:
    Texture2DGL4 texture_;
    std::int32_t pixelWidth_ = 0;
    std::int32_t pixelHeight_ = 0;
    std::int32_t levelCount_ = 0;
    PixelFormat format_ = PixelFormat::A8_UNorm;
    bool generateMipmap_ = false;
    bool multiSampleEnabled_ = false;
};

} // namespace pomdog::gpu::detail::gl4
