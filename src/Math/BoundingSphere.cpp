// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Math/BoundingSphere.hpp"
#include "Pomdog/Math/ContainmentType.hpp"
#include "Pomdog/Math/BoundingBox.hpp"
#include "Pomdog/Math/Ray.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {

BoundingSphere::BoundingSphere(Vector3 const& center, float radius)
    : Center(center)
    , Radius(radius)
{
    POMDOG_ASSERT(radius >= 0);
}
//-----------------------------------------------------------------------
bool BoundingSphere::operator==(BoundingSphere const& sphere) const noexcept
{
    return this->Radius == sphere.Radius
        && this->Center == sphere.Center;
}
//-----------------------------------------------------------------------
bool BoundingSphere::operator!=(BoundingSphere const& sphere) const noexcept
{
    return this->Radius != sphere.Radius
        || this->Center != sphere.Center;
}
//-----------------------------------------------------------------------
ContainmentType BoundingSphere::Contains(Vector3 const& point) const
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
//-----------------------------------------------------------------------
ContainmentType BoundingSphere::Contains(BoundingBox const& box) const
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
//-----------------------------------------------------------------------
ContainmentType BoundingSphere::Contains(BoundingSphere const& sphere) const
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
//-----------------------------------------------------------------------
bool BoundingSphere::Intersects(BoundingBox const& box) const
{
    return box.Intersects(*this);
}
//-----------------------------------------------------------------------
bool BoundingSphere::Intersects(BoundingSphere const& sphere) const
{
    auto distance = Vector3::Distance(this->Center, sphere.Center);
    return distance <= this->Radius + sphere.Radius;
}
//-----------------------------------------------------------------------
Optional<float> BoundingSphere::Intersects(Ray const& ray) const
{
    return ray.Intersects(*this);
}

} // namespace Pomdog
