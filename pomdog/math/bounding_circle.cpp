// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_circle.h"
#include "pomdog/math/bounding_box2d.h"
#include "pomdog/math/containment_type.h"
#include "pomdog/utility/assert.h"

namespace pomdog {

BoundingCircle::BoundingCircle(const Vector2& centerIn, float radiusIn) noexcept
    : center(centerIn)
    , radius(radiusIn)
{
    POMDOG_ASSERT(radiusIn >= 0);
}

ContainmentType
BoundingCircle::contains(const Vector2& point) const noexcept
{
    const auto distanceSquared = math::distanceSquared(point, center);
    const auto radiusSquared = radius * radius;
    if (distanceSquared > radiusSquared) {
        return ContainmentType::Disjoint;
    }
    if (distanceSquared < radiusSquared) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

ContainmentType
BoundingCircle::contains(const BoundingBox2D& box) const noexcept
{
    bool inside = true;
    for (auto& corner : box.getCorners()) {
        if (contains(corner) == ContainmentType::Disjoint) {
            inside = false;
            break;
        }
    }
    if (inside) {
        return ContainmentType::Contains;
    }
    if (intersects(box)) {
        return ContainmentType::Intersects;
    }
    return ContainmentType::Disjoint;
}

ContainmentType
BoundingCircle::contains(const BoundingCircle& circle) const noexcept
{
    const auto distance = math::distance(center, circle.center);
    if (distance > radius + circle.radius) {
        return ContainmentType::Disjoint;
    }
    if (distance + circle.radius < radius) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

bool BoundingCircle::intersects(const BoundingBox2D& box) const noexcept
{
    return box.intersects(*this);
}

bool BoundingCircle::intersects(const BoundingCircle& circle) const noexcept
{
    const auto distance = math::distance(center, circle.center);
    return distance <= radius + circle.radius;
}

} // namespace pomdog
