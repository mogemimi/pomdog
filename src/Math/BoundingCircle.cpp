// Copyright (c) 2013-2015 mogemimi.
// Distributed under the MIT license. See LICENSE.md file for details.

#include "Pomdog/Math/BoundingCircle.hpp"
#include "Pomdog/Math/ContainmentType.hpp"
#include "Pomdog/Math/BoundingBox2D.hpp"
#include "Pomdog/Utility/Assert.hpp"

namespace Pomdog {
//-----------------------------------------------------------------------
BoundingCircle::BoundingCircle(Vector2 const& center, float radius)
    : Center(center)
    , Radius(radius)
{
    POMDOG_ASSERT(radius >= 0);
}
//-----------------------------------------------------------------------
bool BoundingCircle::operator==(BoundingCircle const& circle) const noexcept
{
    return this->Radius == circle.Radius
        && this->Center == circle.Center;
}
//-----------------------------------------------------------------------
bool BoundingCircle::operator!=(BoundingCircle const& circle) const noexcept
{
    return this->Radius != circle.Radius
        || this->Center != circle.Center;
}
//-----------------------------------------------------------------------
ContainmentType BoundingCircle::Contains(Vector2 const& point) const
{
    auto distanceSquared = Vector2::DistanceSquared(point, Center);
    auto radiusSquared = Radius * Radius;
    if (distanceSquared > radiusSquared) {
        return ContainmentType::Disjoint;
    }
    if (distanceSquared < radiusSquared) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}
//-----------------------------------------------------------------------
ContainmentType BoundingCircle::Contains(BoundingBox2D const& box) const
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
ContainmentType BoundingCircle::Contains(BoundingCircle const& circle) const
{
    auto distance = Vector2::Distance(this->Center, circle.Center);
    if (distance > this->Radius + circle.Radius) {
        return ContainmentType::Disjoint;
    }
    if (distance + circle.Radius < this->Radius) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}
//-----------------------------------------------------------------------
bool BoundingCircle::Intersects(BoundingBox2D const& box) const
{
    return box.Intersects(*this);
}
//-----------------------------------------------------------------------
bool BoundingCircle::Intersects(BoundingCircle const& circle) const
{
    auto distance = Vector2::Distance(this->Center, circle.Center);
    return distance <= this->Radius + circle.Radius;
}
//-----------------------------------------------------------------------
} // namespace Pomdog
