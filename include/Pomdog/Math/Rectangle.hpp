// Copyright (c) 2013-2015 mogemimi. Distributed under the MIT license.

#pragma once

#include "Point2D.hpp"
#include "Pomdog/Basic/Export.hpp"
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
class POMDOG_EXPORT Rectangle {
public:
    std::int32_t X, Y;
    std::int32_t Width, Height;

public:
    Rectangle() noexcept = default;

    Rectangle(std::int32_t x, std::int32_t y,
        std::int32_t width, std::int32_t height) noexcept;

    Rectangle(Point2D const& position,
        std::int32_t width, std::int32_t height) noexcept;

    bool operator==(Rectangle const&) const noexcept;
    bool operator!=(Rectangle const&) const noexcept;

    std::int32_t GetBottom() const noexcept;

    std::int32_t GetRight() const noexcept;

    std::int32_t GetTop() const noexcept;

    std::int32_t GetLeft() const noexcept;

    Point2D GetCenter() const;

    Point2D GetLocation() const;

    void SetLocation(Point2D const& position);

    void Inflate(std::int32_t horizontalAmount, std::int32_t verticalAmount);

    void Offset(std::int32_t offsetX, std::int32_t offsetY);

    void Offset(Point2D const& offset);

    bool Contains(int x, int y) const noexcept;

    bool Contains(Point2D const& point) const noexcept;

    bool Contains(Rectangle const& rectangle) const;

    bool Intersects(Rectangle const& rectangle) const;
};

} // namespace Pomdog
