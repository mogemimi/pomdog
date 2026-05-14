// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/color.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <span>
#include <vector>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Image is a 2D array of pixels stored as Color values.
class POMDOG_EXPORT Image final {
private:
    std::vector<Color> data_ = {};
    i32 width_ = 0;
    i32 height_ = 0;

public:
    /// Creates an image with `width` and `height` in pixels.
    Image(i32 width, i32 height);

    Image() = delete;

    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    /// Moves an image from another Image object.
    Image(Image&&);

    /// Moves an image from another Image object.
    Image& operator=(Image&&);

    /// Returns the width of the image, in pixels.
    [[nodiscard]] i32
    getWidth() const noexcept;

    /// Returns the height of the image, in pixels.
    [[nodiscard]] i32
    getHeight() const noexcept;

    /// Returns a view of the image pixels as a contiguous array.
    ///
    /// The returned span contains `width * height` pixels in row-major order.
    [[nodiscard]] std::span<const Color>
    getData() const noexcept;

    /// Replaces the image data with `pixelData`.
    ///
    /// `pixelData` must contain `width * height` pixels.
    void
    setData(std::span<const Color> pixelData);

    /// Replaces the image data with `pixelData` without copying.
    ///
    /// `pixelData` must contain `width * height` pixels.
    void
    setData(std::vector<Color>&& pixelData);

    /// Returns the pixel at the given `x` and `y` coordinates.
    [[nodiscard]] Color
    getPixel(i32 x, i32 y) const;

    /// Sets the pixel at the given `x` and `y` coordinates to `color`.
    void
    setPixel(i32 x, i32 y, const Color& color);

    /// Fills the entire image with `color`.
    void
    fill(const Color& color);

    /// Converts the image data to use premultiplied alpha.
    void
    premultiplyAlpha();
};

} // namespace pomdog
