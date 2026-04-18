// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_circle_xz.h"
#include "pomdog/math/bounding_box_xz.h"
#include "pomdog/math/containment_type.h"
#include "pomdog/utility/assert.h"

namespace pomdog {

BoundingCircleXZ::BoundingCircleXZ() noexcept = default;

BoundingCircleXZ::BoundingCircleXZ(const VectorXZ& centerIn, f32 radiusIn) noexcept
    : center(centerIn)
    , radius(radiusIn)
{
    POMDOG_ASSERT(radiusIn >= 0);
}

ContainmentType
BoundingCircleXZ::contains(const VectorXZ& point) const noexcept
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
BoundingCircleXZ::contains(const BoundingBoxXZ& box) const noexcept
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
BoundingCircleXZ::contains(const BoundingCircleXZ& circle) const noexcept
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

bool BoundingCircleXZ::intersects(const BoundingBoxXZ& box) const noexcept
{
    return box.intersects(*this);
}

bool BoundingCircleXZ::intersects(const BoundingCircleXZ& circle) const noexcept
{
    const auto distance = math::distance(center, circle.center);
    return distance <= radius + circle.radius;
}

} // namespace pomdog
