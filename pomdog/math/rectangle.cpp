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

std::int32_t Rectangle::GetBottom() const noexcept
{
    return y + height;
}

std::int32_t Rectangle::GetRight() const noexcept
{
    return x + width;
}

std::int32_t Rectangle::GetTop() const noexcept
{
    return y;
}

std::int32_t Rectangle::GetLeft() const noexcept
{
    return x;
}

Point2D Rectangle::GetCenter() const
{
    return {x + width / 2, y + height / 2};
}

Point2D Rectangle::GetLocation() const
{
    return {x, y};
}

void Rectangle::SetLocation(const Point2D& position)
{
    x = position.x;
    y = position.y;
}

void Rectangle::Inflate(std::int32_t horizontalAmount, std::int32_t verticalAmount)
{
    x -= horizontalAmount;
    y -= verticalAmount;
    width += horizontalAmount * 2;
    height += verticalAmount * 2;
}

void Rectangle::Offset(std::int32_t offsetX, std::int32_t offsetY)
{
    x += offsetX;
    y += offsetY;
}

void Rectangle::Offset(const Point2D& offset)
{
    x += offset.x;
    y += offset.y;
}

bool Rectangle::Contains(int pointX, int pointY) const noexcept
{
    return pointX >= x &&
           pointX <= (x + width) &&
           pointY >= y &&
           pointY <= (y + height);
}

bool Rectangle::Contains(const Point2D& point) const noexcept
{
    return Contains(point.x, point.y);
}

bool Rectangle::Contains(const Rectangle& rect) const
{
    return rect.GetLeft() >= GetLeft() &&
           rect.GetRight() <= GetRight() &&
           rect.GetTop() >= GetTop() &&
           rect.GetBottom() <= GetBottom();
}

bool Rectangle::Intersects(const Rectangle& rect) const
{
    POMDOG_ASSERT(GetLeft() <= GetRight());
    POMDOG_ASSERT(GetTop() <= GetBottom());
    POMDOG_ASSERT(rect.GetLeft() <= rect.GetRight());

    return GetLeft() < rect.GetRight() &&
           GetRight() > rect.GetLeft() &&
           GetTop() < rect.GetBottom() &&
           GetBottom() > rect.GetTop();
}

} // namespace pomdog
