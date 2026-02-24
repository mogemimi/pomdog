// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/gpu/forward_declarations.h"
#include "pomdog/gpu/texture.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <memory>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class Rectangle;
} // namespace pomdog

namespace pomdog::gpu {

class POMDOG_EXPORT DepthStencilBuffer : public Texture {
public:
    DepthStencilBuffer() noexcept;
    DepthStencilBuffer(const DepthStencilBuffer&) = delete;
    DepthStencilBuffer& operator=(const DepthStencilBuffer&) = delete;

    virtual ~DepthStencilBuffer() override;

    /// Gets the width of the texture data, in pixels.
    [[nodiscard]] virtual i32
    getWidth() const noexcept = 0;

    /// Gets the height of the texture data, in pixels.
    [[nodiscard]] virtual i32
    getHeight() const noexcept = 0;

    /// Gets the format of the pixel data in the depth-stencil buffer.
    [[nodiscard]] virtual PixelFormat
    getFormat() const noexcept = 0;

    /// Gets the size of the texture resource.
    [[nodiscard]] virtual Rectangle
    getBounds() const noexcept = 0;
};

} // namespace pomdog::gpu
