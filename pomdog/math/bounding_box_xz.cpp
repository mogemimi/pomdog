// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_box_xz.h"
#include "pomdog/math/bounding_circle_xz.h"
#include "pomdog/math/containment_type.h"
#include "pomdog/utility/assert.h"

namespace pomdog {

BoundingBoxXZ::BoundingBoxXZ() noexcept = default;

BoundingBoxXZ::BoundingBoxXZ(const VectorXZ& minIn, const VectorXZ& maxIn) noexcept
    : min(minIn)
    , max(maxIn)
{
}

bool BoundingBoxXZ::contains(const VectorXZ& point) const noexcept
{
    if ((point.x < min.x) ||
        (point.z < min.z) ||
        (point.x > max.x) ||
        (point.z > max.z)) {
        return false;
    }
    return true;
}

ContainmentType
BoundingBoxXZ::contains(const BoundingBoxXZ& box) const noexcept
{
    if ((min.x > box.max.x || max.x < box.min.x) ||
        (min.z > box.max.z || max.z < box.min.z)) {
        return ContainmentType::Disjoint;
    }
    if ((min.x <= box.min.x && box.max.x <= max.x) &&
        (min.z <= box.min.z && box.max.z <= max.z)) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

ContainmentType
BoundingBoxXZ::contains(const BoundingCircleXZ& circle) const noexcept
{
    const auto clamped = math::clamp(circle.center, min, max);
    const auto distanceSquared = math::distanceSquared(circle.center, clamped);

    if (distanceSquared > circle.radius * circle.radius) {
        return ContainmentType::Disjoint;
    }
    if ((circle.radius <= circle.center.x - min.x) &&
        (circle.radius <= circle.center.z - min.z) &&
        (circle.radius <= max.x - circle.center.x) &&
        (circle.radius <= max.z - circle.center.z)) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

bool BoundingBoxXZ::intersects(const BoundingBoxXZ& box) const noexcept
{
    return (max.x >= box.min.x && min.x <= box.max.x) &&
           (max.z >= box.min.z && min.z <= box.max.z);
}

bool BoundingBoxXZ::intersects(const BoundingCircleXZ& circle) const noexcept
{
    const auto clamped = math::clamp(circle.center, min, max);
    const auto distanceSquared = math::distanceSquared(circle.center, clamped);
    return distanceSquared <= circle.radius * circle.radius;
}

std::array<VectorXZ, BoundingBoxXZ::CornerCount>
BoundingBoxXZ::getCorners() const noexcept
{
    return std::array<VectorXZ, BoundingBoxXZ::CornerCount>{{
        VectorXZ{min.x, max.z},
        VectorXZ{max.x, max.z},
        VectorXZ{max.x, min.z},
        VectorXZ{min.x, min.z},
    }};
}

} // namespace pomdog
