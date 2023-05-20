// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_circle.h"
#include "pomdog/math/bounding_box2d.h"
#include "pomdog/math/containment_type.h"
#include "pomdog/utility/assert.h"

namespace pomdog {

BoundingCircle::BoundingCircle(const Vector2& centerIn, float radiusIn)
    : center(centerIn)
    , radius(radiusIn)
{
    POMDOG_ASSERT(radiusIn >= 0);
}

ContainmentType BoundingCircle::Contains(const Vector2& point) const
{
    const auto distanceSquared = math::DistanceSquared(point, center);
    const auto radiusSquared = radius * radius;
    if (distanceSquared > radiusSquared) {
        return ContainmentType::Disjoint;
    }
    if (distanceSquared < radiusSquared) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

ContainmentType BoundingCircle::Contains(const BoundingBox2D& box) const
{
    bool inside = true;
    for (auto& corner : box.GetCorners()) {
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

ContainmentType BoundingCircle::Contains(const BoundingCircle& circle) const
{
    const auto distance = math::Distance(center, circle.center);
    if (distance > radius + circle.radius) {
        return ContainmentType::Disjoint;
    }
    if (distance + circle.radius < radius) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

bool BoundingCircle::Intersects(const BoundingBox2D& box) const
{
    return box.Intersects(*this);
}

bool BoundingCircle::Intersects(const BoundingCircle& circle) const
{
    const auto distance = math::Distance(center, circle.center);
    return distance <= radius + circle.radius;
}

} // namespace pomdog
