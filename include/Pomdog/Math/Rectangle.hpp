// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Point2D.hpp"
#include <cstdint>

namespace Pomdog {

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

    bool operator==(const Rectangle&) const noexcept;
    bool operator!=(const Rectangle&) const noexcept;

    std::int32_t GetBottom() const noexcept;

    std::int32_t GetRight() const noexcept;

    std::int32_t GetTop() const noexcept;

    std::int32_t GetLeft() const noexcept;

    Point2D GetCenter() const;

    Point2D GetLocation() const;

    void SetLocation(const Point2D& position);

    void Inflate(std::int32_t horizontalAmount, std::int32_t verticalAmount);

    void Offset(std::int32_t offsetX, std::int32_t offsetY);

    void Offset(const Point2D& offset);

    bool Contains(int x, int y) const noexcept;

    bool Contains(const Point2D& point) const noexcept;

    bool Contains(const Rectangle& rectangle) const;

    bool Intersects(const Rectangle& rectangle) const;
};

} // namespace Pomdog
