// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#ifndef POMDOG_RECTANGLE_4B874E7C_HPP
#define POMDOG_RECTANGLE_4B874E7C_HPP

#include "Point2D.hpp"
#include "Pomdog/Basic/Export.hpp"
#include <cstdint>

namespace Pomdog {

enum class ContainmentType: std::uint8_t;

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
    // Constructors:
    Rectangle() noexcept = default;

    Rectangle(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height) noexcept;

    Rectangle(Point2D const& position, std::int32_t width, std::int32_t height);

    // Binary operators:
    bool operator==(Rectangle const&) const;
    bool operator!=(Rectangle const&) const;

    std::int32_t Bottom() const;

    std::int32_t Right() const;

    std::int32_t Top() const;

    std::int32_t Left() const;

    Point2D Center() const;

    Point2D Location() const;

    void Location(Point2D const& position);

    void Offset(std::int32_t offsetX, std::int32_t offsetY);

    void Offset(Point2D const& offset);

    ContainmentType Contains(Point2D const& point) const;

    ContainmentType Contains(Rectangle const& rectangle) const;

    bool Intersects(Point2D const& point) const;

    bool Intersects(Rectangle const& rectangle) const;
};

}// namespace Pomdog

#endif // POMDOG_RECTANGLE_4B874E7C_HPP
