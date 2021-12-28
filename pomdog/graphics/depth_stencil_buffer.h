// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/graphics/forward_declarations.h"
#include "pomdog/graphics/texture.h"
#include "pomdog/math/forward_declarations.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

class POMDOG_EXPORT DepthStencilBuffer : public Texture {
public:
    DepthStencilBuffer() noexcept;
    DepthStencilBuffer(const DepthStencilBuffer&) = delete;
    DepthStencilBuffer& operator=(const DepthStencilBuffer&) = delete;

    virtual ~DepthStencilBuffer() override;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] virtual std::int32_t GetWidth() const noexcept = 0;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] virtual std::int32_t GetHeight() const noexcept = 0;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    [[nodiscard]] virtual SurfaceFormat GetFormat() const noexcept = 0;

    /// Gets the size of the texture resource.
    [[nodiscard]] virtual Rectangle GetBounds() const noexcept = 0;
};

} // namespace pomdog
