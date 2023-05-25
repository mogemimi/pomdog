// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/rectangle.h"
#include "pomdog/utility/assert.h"

namespace pomdog {

Rectangle::Rectangle(std::int32_t xIn, std::int32_t yIn, std::int32_t widthIn, std::int32_t heightIn) noexcept
    : x(xIn)
    , y(yIn)
    , width(widthIn)
    , height(heightIn)
{
}

Rectangle::Rectangle(const Point2D& position, std::int32_t widthIn, std::int32_t heightIn) noexcept
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

std::int32_t Rectangle::getBottom() const noexcept
{
    return y + height;
}

std::int32_t Rectangle::getRight() const noexcept
{
    return x + width;
}

std::int32_t Rectangle::getTop() const noexcept
{
    return y;
}

std::int32_t Rectangle::getLeft() const noexcept
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

void Rectangle::inflate(std::int32_t horizontalAmount, std::int32_t verticalAmount)
{
    x -= horizontalAmount;
    y -= verticalAmount;
    width += horizontalAmount * 2;
    height += verticalAmount * 2;
}

void Rectangle::offset(std::int32_t offsetX, std::int32_t offsetY)
{
    x += offsetX;
    y += offsetY;
}

void Rectangle::offset(const Point2D& offset)
{
    x += offset.x;
    y += offset.y;
}

bool Rectangle::contains(int pointX, int pointY) const noexcept
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

bool Rectangle::contains(const Rectangle& rect) const
{
    return rect.getLeft() >= getLeft() &&
           rect.getRight() <= getRight() &&
           rect.getTop() >= getTop() &&
           rect.getBottom() <= getBottom();
}

bool Rectangle::intersects(const Rectangle& rect) const
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
