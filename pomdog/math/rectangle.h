// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/math/point2d.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// Rectangle is a rectangle in 2D space.
///
///@code
///            width
///   _______________________
///  |* <--(x, y)            |
///  |                       |
///  |           *           | height
///  |         Center        |
///  |                       | Right = x + width
///  |_______________________|
///
///     Bottom = y + height
///@endcode
class POMDOG_EXPORT Rectangle final {
public:
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;

public:
    Rectangle() noexcept = default;

    Rectangle(
        std::int32_t x,
        std::int32_t y,
        std::int32_t width,
        std::int32_t height) noexcept;

    Rectangle(
        const Point2D& position,
        std::int32_t width,
        std::int32_t height) noexcept;

    [[nodiscard]] bool operator==(const Rectangle&) const noexcept;
    [[nodiscard]] bool operator!=(const Rectangle&) const noexcept;

    [[nodiscard]] std::int32_t
    getBottom() const noexcept;

    [[nodiscard]] std::int32_t
    getRight() const noexcept;

    [[nodiscard]] std::int32_t
    getTop() const noexcept;

    [[nodiscard]] std::int32_t
    getLeft() const noexcept;

    [[nodiscard]] Point2D
    getCenter() const;

    [[nodiscard]] Point2D
    getLocation() const;

    void setLocation(const Point2D& position);

    void inflate(std::int32_t horizontalAmount, std::int32_t verticalAmount);

    void offset(std::int32_t offsetX, std::int32_t offsetY);

    void offset(const Point2D& offset);

    [[nodiscard]] bool
    contains(int x, int y) const noexcept;

    [[nodiscard]] bool
    contains(const Point2D& point) const noexcept;

    [[nodiscard]] bool
    contains(const Rectangle& rectangle) const;

    [[nodiscard]] bool
    intersects(const Rectangle& rectangle) const;
};

} // namespace pomdog
