// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/rect2d.h"
#include "pomdog/utility/assert.h"

namespace pomdog {

Rect2D::Rect2D(i32 xIn, i32 yIn, i32 widthIn, i32 heightIn) noexcept
    : x(xIn)
    , y(yIn)
    , width(widthIn)
    , height(heightIn)
{
}

Rect2D::Rect2D(const Point2D& position, i32 widthIn, i32 heightIn) noexcept
    : x(position.x)
    , y(position.y)
    , width(widthIn)
    , height(heightIn)
{
}

bool Rect2D::operator==(const Rect2D& other) const noexcept
{
    return x == other.x &&
           y == other.y &&
           width == other.width &&
           height == other.height;
}

bool Rect2D::operator!=(const Rect2D& other) const noexcept
{
    return x != other.x ||
           y != other.y ||
           width != other.width ||
           height != other.height;
}

i32 Rect2D::minX() const noexcept
{
    return x;
}

i32 Rect2D::maxX() const noexcept
{
    return x + width;
}

i32 Rect2D::minY() const noexcept
{
    return y;
}

i32 Rect2D::maxY() const noexcept
{
    return y + height;
}

Point2D Rect2D::getCenter() const
{
    return {x + width / 2, y + height / 2};
}

Point2D Rect2D::getLocation() const
{
    return {x, y};
}

void Rect2D::setLocation(const Point2D& position)
{
    x = position.x;
    y = position.y;
}

void Rect2D::inflate(i32 horizontalAmount, i32 verticalAmount)
{
    x -= horizontalAmount;
    y -= verticalAmount;
    width += horizontalAmount * 2;
    height += verticalAmount * 2;
}

void Rect2D::offset(i32 offsetX, i32 offsetY)
{
    x += offsetX;
    y += offsetY;
}

void Rect2D::offset(const Point2D& offset)
{
    x += offset.x;
    y += offset.y;
}

bool Rect2D::contains(i32 pointX, i32 pointY) const noexcept
{
    return pointX >= x &&
           pointX <= (x + width) &&
           pointY >= y &&
           pointY <= (y + height);
}

bool Rect2D::contains(const Point2D& point) const noexcept
{
    return contains(point.x, point.y);
}

bool Rect2D::contains(const Rect2D& rect) const noexcept
{
    return rect.minX() >= minX() &&
           rect.maxX() <= maxX() &&
           rect.minY() >= minY() &&
           rect.maxY() <= maxY();
}

bool Rect2D::intersects(const Rect2D& rect) const noexcept
{
    POMDOG_ASSERT(minX() <= maxX());
    POMDOG_ASSERT(minY() <= maxY());
    POMDOG_ASSERT(rect.minX() <= rect.maxX());

    return minX() < rect.maxX() &&
           maxX() > rect.minX() &&
           minY() < rect.maxY() &&
           maxY() > rect.minY();
}

} // namespace pomdog
