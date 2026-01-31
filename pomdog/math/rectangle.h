// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/point2d.h"

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
    i32 x;
    i32 y;
    i32 width;
    i32 height;

public:
    Rectangle() noexcept = default;

    Rectangle(
        i32 x,
        i32 y,
        i32 width,
        i32 height) noexcept;

    Rectangle(
        const Point2D& position,
        i32 width,
        i32 height) noexcept;

    [[nodiscard]] bool operator==(const Rectangle&) const noexcept;
    [[nodiscard]] bool operator!=(const Rectangle&) const noexcept;

    [[nodiscard]] i32
    getBottom() const noexcept;

    [[nodiscard]] i32
    getRight() const noexcept;

    [[nodiscard]] i32
    getTop() const noexcept;

    [[nodiscard]] i32
    getLeft() const noexcept;

    [[nodiscard]] Point2D
    getCenter() const;

    [[nodiscard]] Point2D
    getLocation() const;

    void setLocation(const Point2D& position);

    void inflate(i32 horizontalAmount, i32 verticalAmount);

    void offset(i32 offsetX, i32 offsetY);

    void offset(const Point2D& offset);

    [[nodiscard]] bool
    contains(i32 x, i32 y) const noexcept;

    [[nodiscard]] bool
    contains(const Point2D& point) const noexcept;

    [[nodiscard]] bool
    contains(const Rectangle& rectangle) const noexcept;

    [[nodiscard]] bool
    intersects(const Rectangle& rectangle) const noexcept;
};

} // namespace pomdog
