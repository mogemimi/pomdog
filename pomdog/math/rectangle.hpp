// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/math/point2d.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cstdint>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

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
    std::int32_t X, Y;
    std::int32_t Width, Height;

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

    [[nodiscard]] std::int32_t GetBottom() const noexcept;

    [[nodiscard]] std::int32_t GetRight() const noexcept;

    [[nodiscard]] std::int32_t GetTop() const noexcept;

    [[nodiscard]] std::int32_t GetLeft() const noexcept;

    [[nodiscard]] Point2D GetCenter() const;

    [[nodiscard]] Point2D GetLocation() const;

    void SetLocation(const Point2D& position);

    void Inflate(std::int32_t horizontalAmount, std::int32_t verticalAmount);

    void Offset(std::int32_t offsetX, std::int32_t offsetY);

    void Offset(const Point2D& offset);

    [[nodiscard]] bool Contains(int x, int y) const noexcept;

    [[nodiscard]] bool Contains(const Point2D& point) const noexcept;

    [[nodiscard]] bool Contains(const Rectangle& rectangle) const;

    [[nodiscard]] bool Intersects(const Rectangle& rectangle) const;
};

} // namespace Pomdog
