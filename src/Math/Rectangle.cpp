// Copyright (c) 2013-2016 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {

Rectangle::Rectangle(std::int32_t x, std::int32_t y,
    std::int32_t width, std::int32_t height) noexcept
    : X(x)
    , Y(y)
    , Width(width)
    , Height(height)
{}
//-----------------------------------------------------------------------
Rectangle::Rectangle(Point2D const& position,
    std::int32_t width, std::int32_t height) noexcept
    : X(position.X)
    , Y(position.Y)
    , Width(width)
    , Height(height)
{}
//-----------------------------------------------------------------------
bool Rectangle::operator==(Rectangle const& other) const noexcept
{
    return (X == other.X && Y == other.Y
        && Width == other.Width && Height == other.Height);
}
//-----------------------------------------------------------------------
bool Rectangle::operator!=(Rectangle const& other) const noexcept
{
    return (X != other.X || Y != other.Y
        || Width != other.Width || Height != other.Height);
}
//-----------------------------------------------------------------------
std::int32_t Rectangle::GetBottom() const noexcept
{
    return Y + Height;
}
//-----------------------------------------------------------------------
std::int32_t Rectangle::GetRight() const noexcept
{
    return X + Width;
}
//-----------------------------------------------------------------------
std::int32_t Rectangle::GetTop() const noexcept
{
    return Y;
}
//-----------------------------------------------------------------------
std::int32_t Rectangle::GetLeft() const noexcept
{
    return X;
}
//-----------------------------------------------------------------------
Point2D Rectangle::GetCenter() const
{
    return {X + Width / 2, Y + Height / 2};
}
//-----------------------------------------------------------------------
Point2D Rectangle::GetLocation() const
{
    return {X, Y};
}
//-----------------------------------------------------------------------
void Rectangle::SetLocation(Point2D const& position)
{
    this->X = position.X;
    this->Y = position.Y;
}
//-----------------------------------------------------------------------
void Rectangle::Inflate(std::int32_t horizontalAmount, std::int32_t verticalAmount)
{
    this->X -= horizontalAmount;
    this->Y -= verticalAmount;
    this->Width += horizontalAmount * 2;
    this->Height += verticalAmount * 2;
}
//-----------------------------------------------------------------------
void Rectangle::Offset(std::int32_t offsetX, std::int32_t offsetY)
{
    this->X += offsetX;
    this->Y += offsetY;
}
//-----------------------------------------------------------------------
void Rectangle::Offset(Point2D const& offset)
{
    X += offset.X;
    Y += offset.Y;
}
//-----------------------------------------------------------------------
bool Rectangle::Contains(int pointX, int pointY) const noexcept
{
    return pointX >= X && pointX <= (X + Width)
        && pointY >= Y && pointY <= (Y + Height);
}
//-----------------------------------------------------------------------
bool Rectangle::Contains(Point2D const& point) const noexcept
{
    return this->Contains(point.X, point.Y);
}
//-----------------------------------------------------------------------
bool Rectangle::Contains(Rectangle const& rect) const
{
    return (rect.GetLeft() >= GetLeft()
        && rect.GetRight() <= GetRight()
        && rect.GetTop() >= GetTop()
        && rect.GetBottom() <= GetBottom());
}
//-----------------------------------------------------------------------
bool Rectangle::Intersects(Rectangle const& rect) const
{
    POMDOG_ASSERT(GetLeft() <= GetRight());
    POMDOG_ASSERT(GetTop() <= GetBottom());
    POMDOG_ASSERT(rect.GetLeft() <= rect.GetRight());

    return (GetLeft() < rect.GetRight()
        && GetRight() > rect.GetLeft()
        && GetTop() < rect.GetBottom()
        && GetBottom() > rect.GetTop());
}

} // namespace Pomdog
