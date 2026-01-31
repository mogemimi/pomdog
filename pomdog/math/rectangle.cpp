// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/rectangle.h"
#include "pomdog/utility/assert.h"

namespace pomdog {

Rectangle::Rectangle(i32 xIn, i32 yIn, i32 widthIn, i32 heightIn) noexcept
    : x(xIn)
    , y(yIn)
    , width(widthIn)
    , height(heightIn)
{
}

Rectangle::Rectangle(const Point2D& position, i32 widthIn, i32 heightIn) noexcept
    : x(position.x)
    , y(position.y)
    , width(widthIn)
    , height(heightIn)
{
}

bool Rectangle::operator==(const Rectangle& other) const noexcept
{
    return x == other.x &&
           y == other.y &&
           width == other.width &&
           height == other.height;
}

bool Rectangle::operator!=(const Rectangle& other) const noexcept
{
    return x != other.x ||
           y != other.y ||
           width != other.width ||
           height != other.height;
}

i32 Rectangle::getBottom() const noexcept
{
    return y + height;
}

i32 Rectangle::getRight() const noexcept
{
    return x + width;
}

i32 Rectangle::getTop() const noexcept
{
    return y;
}

i32 Rectangle::getLeft() const noexcept
{
    return x;
}

Point2D Rectangle::getCenter() const
{
    return {x + width / 2, y + height / 2};
}

Point2D Rectangle::getLocation() const
{
    return {x, y};
}

void Rectangle::setLocation(const Point2D& position)
{
    x = position.x;
    y = position.y;
}

void Rectangle::inflate(i32 horizontalAmount, i32 verticalAmount)
{
    x -= horizontalAmount;
    y -= verticalAmount;
    width += horizontalAmount * 2;
    height += verticalAmount * 2;
}

void Rectangle::offset(i32 offsetX, i32 offsetY)
{
    x += offsetX;
    y += offsetY;
}

void Rectangle::offset(const Point2D& offset)
{
    x += offset.x;
    y += offset.y;
}

bool Rectangle::contains(i32 pointX, i32 pointY) const noexcept
{
    return pointX >= x &&
           pointX <= (x + width) &&
           pointY >= y &&
           pointY <= (y + height);
}

bool Rectangle::contains(const Point2D& point) const noexcept
{
    return contains(point.x, point.y);
}

bool Rectangle::contains(const Rectangle& rect) const noexcept
{
    return rect.getLeft() >= getLeft() &&
           rect.getRight() <= getRight() &&
           rect.getTop() >= getTop() &&
           rect.getBottom() <= getBottom();
}

bool Rectangle::intersects(const Rectangle& rect) const noexcept
{
    POMDOG_ASSERT(getLeft() <= getRight());
    POMDOG_ASSERT(getTop() <= getBottom());
    POMDOG_ASSERT(rect.getLeft() <= rect.getRight());

    return getLeft() < rect.getRight() &&
           getRight() > rect.getLeft() &&
           getTop() < rect.getBottom() &&
           getBottom() > rect.getTop();
}

} // namespace pomdog
