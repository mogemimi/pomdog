//
//  Copyright (C) 2013-2015 mogemimi.
//  Distributed under the MIT License. See LICENSE.md or
//  http://enginetrouble.net/pomdog/license for details.
//

#include "Pomdog/Math/Rectangle.hpp"
#include "Pomdog/Math/ContainmentType.hpp"
#include "Pomdog/Utility/Assert.hpp"

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
//-----------------------------------------------------------------------
ContainmentType Rectangle::Contains(Point2D const& point) const
{
	if (point.X < Left()
		|| point.Y < Top()
		|| point.X > Right()
		|| point.Y < Bottom())
	{
		return ContainmentType::Disjoint;
	}

	if (point.X == Left()
		|| point.Y == Top()
		|| point.X == Right()
		|| point.Y == Bottom())
	{
		return ContainmentType::Intersects;
	}

	return ContainmentType::Contains;
}
//-----------------------------------------------------------------------
ContainmentType Rectangle::Contains(Rectangle const& rect) const
{
	if (rect.Right() < Left()
		|| rect.Left() > Right()
		|| rect.Bottom() < Top()
		|| rect.Top() > Bottom())
	{
		return ContainmentType::Disjoint;
	}

	if (rect.Left() >= Left()
		&& rect.Right() <= Right()
		&& rect.Top() >= Top()
		&& rect.Bottom() <= Bottom())
	{
		return ContainmentType::Contains;
	}

	return ContainmentType::Intersects;
}
//-----------------------------------------------------------------------
bool Rectangle::Intersects(Point2D const& point) const
{
	return point.X >= X && point.X <= (X + Width)
		&& point.Y >= Y && point.Y <= (Y + Height);
}
//-----------------------------------------------------------------------
bool Rectangle::Intersects(Rectangle const& rect) const
{
	POMDOG_ASSERT(Left() <= Right());
	POMDOG_ASSERT(Top() <= Bottom());
	POMDOG_ASSERT(rect.Left() <= rect.Right());

	return Left() > rect.Right()
		|| Right() < rect.Left()
		|| Top() > rect.Bottom()
		|| Bottom() < rect.Top();
}

}// namespace Pomdog
