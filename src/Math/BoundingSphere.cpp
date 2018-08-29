// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/BoundingSphere.hpp"
#include "Pomdog/Math/BoundingBox.hpp"
#include "Pomdog/Math/ContainmentType.hpp"
#include "Pomdog/Math/Plane.hpp"
#include "Pomdog/Math/Ray.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {

BoundingSphere::BoundingSphere(const Vector3& center, float radius)
    : Center(center)
    , Radius(radius)
{
    POMDOG_ASSERT(radius >= 0);
}

bool BoundingSphere::operator==(const BoundingSphere& sphere) const noexcept
{
    return this->Radius == sphere.Radius
        && this->Center == sphere.Center;
}

bool BoundingSphere::operator!=(const BoundingSphere& sphere) const noexcept
{
    return this->Radius != sphere.Radius
        || this->Center != sphere.Center;
}

ContainmentType BoundingSphere::Contains(const Vector3& point) const
{
    auto distanceSquared = Vector3::DistanceSquared(point, Center);
    auto radiusSquared = Radius * Radius;
    if (distanceSquared > radiusSquared) {
        return ContainmentType::Disjoint;
    }
    else if (distanceSquared < radiusSquared) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

ContainmentType BoundingSphere::Contains(const BoundingBox& box) const
{
    bool inside = true;
    for (auto & corner : box.GetCorners()) {
        if (this->Contains(corner) == ContainmentType::Disjoint) {
            inside = false;
            break;
        }
    }
    if (inside) {
        return ContainmentType::Contains;
    }
    if (this->Intersects(box)) {
        return ContainmentType::Intersects;
    }
    return ContainmentType::Disjoint;
}

ContainmentType BoundingSphere::Contains(const BoundingSphere& sphere) const
{
    auto distance = Vector3::Distance(this->Center, sphere.Center);
    if (distance > this->Radius + sphere.Radius) {
        return ContainmentType::Disjoint;
    }
    if (distance + sphere.Radius < this->Radius) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

bool BoundingSphere::Intersects(const BoundingBox& box) const
{
    return box.Intersects(*this);
}

bool BoundingSphere::Intersects(const BoundingSphere& sphere) const
{
    auto distance = Vector3::Distance(this->Center, sphere.Center);
    return distance <= this->Radius + sphere.Radius;
}

PlaneIntersectionType BoundingSphere::Intersects(const Plane& plane) const
{
    return plane.Intersects(*this);
}

std::optional<float> BoundingSphere::Intersects(const Ray& ray) const
{
    return ray.Intersects(*this);
}

} // namespace Pomdog
