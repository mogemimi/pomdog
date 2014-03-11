//
//  Copyright (C) 2013-2014 mogemimi.
//
//  Distributed under the MIT License.
//  See accompanying file LICENSE.md or copy at
//  http://enginetrouble.net/pomdog/LICENSE.md for details.
//

#include <Pomdog/Math/Rectangle.hpp>
#include <Pomdog/Utility/Assert.hpp>

namespace Pomdog {

Rectangle::Rectangle(std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height)
	: X(x)
	, Y(y)
	, Width(width)
	, Height(height)
{}
//-----------------------------------------------------------------------
Rectangle::Rectangle(Point2D const& position, std::int32_t width, std::int32_t height)
	: X(position.X)
	, Y(position.Y)
	, Width(width)
	, Height(height)
{}
//-----------------------------------------------------------------------
bool Rectangle::operator==(Rectangle const& other) const
{
	return (X == other.X && Y == other.Y &&
		Width  == other.Width && Height == other.Height);
}
//-----------------------------------------------------------------------
bool Rectangle::operator!=(Rectangle const& other) const
{
	return (X != other.X || Y != other.Y ||
		Width  != other.Width || Height != other.Height);
}
//-----------------------------------------------------------------------
std::int32_t Rectangle::Bottom() const
{
	return Y + Height;
}
//-----------------------------------------------------------------------
std::int32_t Rectangle::Right() const
{
	return X + Width;
}
//-----------------------------------------------------------------------
std::int32_t Rectangle::Top() const
{
	return Y;
}
//-----------------------------------------------------------------------
std::int32_t Rectangle::Left() const
{
	return X;
}
//-----------------------------------------------------------------------
Point2D Rectangle::Center() const
{
	return Point2D(X + Width / 2, Y + Height / 2);
}
//-----------------------------------------------------------------------
Point2D Rectangle::Location() const
{
	return {X, Y};
}
//-----------------------------------------------------------------------
void Rectangle::Location(Point2D const& position)
{
	this->X = position.X;
	this->Y = position.Y;
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

}// namespace Pomdog
