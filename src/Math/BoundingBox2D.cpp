// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/BoundingBox2D.hpp"
#include "Pomdog/Math/BoundingCircle.hpp"
#include "Pomdog/Math/ContainmentType.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {

constexpr int BoundingBox2D::CornerCount;

BoundingBox2D::BoundingBox2D(const Vector2& minIn, const Vector2& maxIn)
    : Min(minIn)
    , Max(maxIn)
{
}

bool BoundingBox2D::operator==(const BoundingBox2D& box) const noexcept
{
    return this->Max == box.Max
        && this->Min == box.Min;
}

bool BoundingBox2D::operator!=(const BoundingBox2D& box) const noexcept
{
    return this->Max != box.Max
        || this->Min != box.Min;
}

ContainmentType BoundingBox2D::Contains(const Vector2& point) const
{
    if (point.X < this->Min.X ||
        point.Y < this->Min.Y ||
        point.X > this->Max.X ||
        point.Y > this->Max.Y) {
        return ContainmentType::Disjoint;
    }
    if (point.X == this->Min.X ||
        point.Y == this->Min.Y ||
        point.X == this->Max.X ||
        point.Y == this->Max.Y) {
        return ContainmentType::Intersects;
    }
    return ContainmentType::Contains;
}

ContainmentType BoundingBox2D::Contains(const BoundingBox2D& box) const
{
    if ((this->Min.X > box.Max.X || this->Max.X < box.Min.X) ||
        (this->Min.Y > box.Max.Y || this->Max.Y < box.Min.Y)) {
        return ContainmentType::Disjoint;
    }
    if ((this->Min.X <= box.Min.X && box.Max.X <= this->Max.X) &&
        (this->Min.Y <= box.Min.Y && box.Max.Y <= this->Max.Y)) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

ContainmentType BoundingBox2D::Contains(const BoundingCircle& circle) const
{
    auto clamped = Vector2::Clamp(circle.Center, this->Min, this->Max);
    auto distanceSquared = Vector2::DistanceSquared(circle.Center, clamped);

    if (distanceSquared > circle.Radius * circle.Radius) {
        return ContainmentType::Disjoint;
    }
    if ((circle.Radius <= circle.Center.X - this->Min.X) &&
        (circle.Radius <= circle.Center.Y - this->Min.Y) &&
        (circle.Radius <= this->Max.X - circle.Center.X) &&
        (circle.Radius <= this->Max.Y - circle.Center.Y)) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

bool BoundingBox2D::Intersects(const BoundingBox2D& box) const
{
    return (this->Max.X >= box.Min.X && this->Min.X <= box.Max.X)
        && (this->Max.Y >= box.Min.Y && this->Min.Y <= box.Max.Y);
}

bool BoundingBox2D::Intersects(const BoundingCircle& circle) const
{
    auto clamped = Vector2::Clamp(circle.Center, this->Min, this->Max);
    auto distanceSquared = Vector2::DistanceSquared(circle.Center, clamped);
    return distanceSquared <= circle.Radius * circle.Radius;
}

std::array<Vector2, BoundingBox2D::CornerCount> BoundingBox2D::GetCorners() const noexcept
{
    return std::array<Vector2, BoundingBox2D::CornerCount> {{
        Vector2{this->Min.X, this->Max.Y},
        Vector2{this->Max.X, this->Max.Y},
        Vector2{this->Max.X, this->Min.Y},
        Vector2{this->Min.X, this->Min.Y},
    }};
}

} // namespace Pomdog
