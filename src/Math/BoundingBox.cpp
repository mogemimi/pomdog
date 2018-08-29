// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/BoundingBox.hpp"
#include "Pomdog/Math/BoundingSphere.hpp"
#include "Pomdog/Math/ContainmentType.hpp"
#include "Pomdog/Math/Plane.hpp"
#include "Pomdog/Math/Ray.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {

constexpr int BoundingBox::CornerCount;

BoundingBox::BoundingBox(const Vector3& minIn, const Vector3& maxIn)
    : Min(minIn)
    , Max(maxIn)
{
}

bool BoundingBox::operator==(const BoundingBox& box) const noexcept
{
    return this->Max == box.Max
        && this->Min == box.Min;
}

bool BoundingBox::operator!=(const BoundingBox& box) const noexcept
{
    return this->Max != box.Max
        || this->Min != box.Min;
}

ContainmentType BoundingBox::Contains(const Vector3& point) const
{
    if (point.X < this->Min.X ||
        point.Y < this->Min.Y ||
        point.Z < this->Min.Z ||
        point.X > this->Max.X ||
        point.Y > this->Max.Y ||
        point.Z > this->Max.Z) {
        return ContainmentType::Disjoint;
    }
    if (point.X == this->Min.X ||
        point.Y == this->Min.Y ||
        point.Z == this->Min.Z ||
        point.X == this->Max.X ||
        point.Y == this->Max.Y ||
        point.Z == this->Max.Z) {
        return ContainmentType::Intersects;
    }
    return ContainmentType::Contains;
}

ContainmentType BoundingBox::Contains(const BoundingBox& box) const
{
    if ((this->Min.X > box.Max.X || this->Max.X < box.Min.X) ||
        (this->Min.Y > box.Max.Y || this->Max.Y < box.Min.Y) ||
        (this->Min.Z > box.Max.Z || this->Max.Z < box.Min.Z)) {
        return ContainmentType::Disjoint;
    }
    if ((this->Min.X <= box.Min.X && box.Max.X <= this->Max.X) &&
        (this->Min.Y <= box.Min.Y && box.Max.Y <= this->Max.Y) &&
        (this->Min.Z <= box.Min.Z && box.Max.Z <= this->Max.Z)) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

ContainmentType BoundingBox::Contains(const BoundingSphere& sphere) const
{
    auto clamped = Vector3::Clamp(sphere.Center, this->Min, this->Max);
    auto distanceSquared = Vector3::DistanceSquared(sphere.Center, clamped);

    if (distanceSquared > sphere.Radius * sphere.Radius) {
        return ContainmentType::Disjoint;
    }
    if ((sphere.Radius <= sphere.Center.X - this->Min.X) &&
        (sphere.Radius <= sphere.Center.Y - this->Min.Y) &&
        (sphere.Radius <= sphere.Center.Z - this->Min.Z) &&
        (sphere.Radius <= this->Max.X - sphere.Center.X) &&
        (sphere.Radius <= this->Max.Y - sphere.Center.Y) &&
        (sphere.Radius <= this->Max.Z - sphere.Center.Z)) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

bool BoundingBox::Intersects(const BoundingBox& box) const
{
    return (this->Max.X >= box.Min.X && this->Min.X <= box.Max.X)
        && (this->Max.Y >= box.Min.Y && this->Min.Y <= box.Max.Y)
        && (this->Max.Z >= box.Min.Z && this->Min.Z <= box.Max.Z);
}

bool BoundingBox::Intersects(const BoundingSphere& sphere) const
{
    auto clamped = Vector3::Clamp(sphere.Center, this->Min, this->Max);
    auto distanceSquared = Vector3::DistanceSquared(sphere.Center, clamped);
    return distanceSquared <= sphere.Radius * sphere.Radius;
}

PlaneIntersectionType BoundingBox::Intersects(const Plane& plane) const
{
    return plane.Intersects(*this);
}

std::optional<float> BoundingBox::Intersects(const Ray& ray) const
{
    return ray.Intersects(*this);
}

std::array<Vector3, BoundingBox::CornerCount> BoundingBox::GetCorners() const noexcept
{
    return std::array<Vector3, BoundingBox::CornerCount> {{
        Vector3{this->Min.X, this->Max.Y, this->Max.Z},
        Vector3{this->Max.X, this->Max.Y, this->Max.Z},
        Vector3{this->Max.X, this->Min.Y, this->Max.Z},
        Vector3{this->Min.X, this->Min.Y, this->Max.Z},
        Vector3{this->Min.X, this->Max.Y, this->Min.Z},
        Vector3{this->Max.X, this->Max.Y, this->Min.Z},
        Vector3{this->Max.X, this->Min.Y, this->Min.Z},
        Vector3{this->Min.X, this->Min.Y, this->Min.Z},
    }};
}

} // namespace Pomdog
