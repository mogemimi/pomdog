// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/graphics/depth_stencil_buffer.hpp"
#include "pomdog/graphics/forward_declarations.hpp"
#include "pomdog/graphics/gl4/opengl_prerequisites.hpp"
#include "pomdog/graphics/gl4/texture2d_gl4.hpp"
#include "pomdog/graphics/surface_format.hpp"
#include "pomdog/utility/errors.hpp"
#include "pomdog/utility/tagged.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog::Detail::GL4 {

class DepthStencilBufferGL4 final : public DepthStencilBuffer {
public:
    ~DepthStencilBufferGL4() override;

    [[nodiscard]] std::unique_ptr<Error>
    Initialize(
        std::int32_t pixelWidth,
        std::int32_t pixelHeight,
        SurfaceFormat depthStencilFormat,
        std::int32_t multiSampleCount) noexcept;

    /// Gets the width of the texture data, in pixels.
    std::int32_t GetWidth() const noexcept override;

    /// Gets the height of the texture data, in pixels.
    std::int32_t GetHeight() const noexcept override;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    SurfaceFormat GetFormat() const noexcept override;

    /// Gets the size of the texture resource.
    Rectangle GetBounds() const noexcept override;

    void BindToFramebuffer(GLuint frameBuffer) noexcept;

private:
    GLuint renderBuffer = 0;
    std::int32_t pixelWidth = 0;
    std::int32_t pixelHeight = 0;
    SurfaceFormat depthStencilFormat = SurfaceFormat::Invalid;
    bool multiSampleEnabled = false;
    bool hasStencil = false;
};

} // namespace Pomdog::Detail::GL4
