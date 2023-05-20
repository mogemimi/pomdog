// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_box2d.h"
#include "pomdog/math/bounding_circle.h"
#include "pomdog/math/containment_type.h"
#include "pomdog/utility/assert.h"

namespace pomdog {

BoundingBox2D::BoundingBox2D(const Vector2& minIn, const Vector2& maxIn)
    : min(minIn)
    , max(maxIn)
{
}

ContainmentType BoundingBox2D::Contains(const Vector2& point) const
{
    if ((point.x < min.x) ||
        (point.y < min.y) ||
        (point.x > max.x) ||
        (point.y > max.y)) {
        return ContainmentType::Disjoint;
    }
    if ((point.x == min.x) ||
        (point.y == min.y) ||
        (point.x == max.x) ||
        (point.y == max.y)) {
        return ContainmentType::Intersects;
    }
    return ContainmentType::Contains;
}

ContainmentType BoundingBox2D::Contains(const BoundingBox2D& box) const
{
    if ((min.x > box.max.x || max.x < box.min.x) ||
        (min.y > box.max.y || max.y < box.min.y)) {
        return ContainmentType::Disjoint;
    }
    if ((min.x <= box.min.x && box.max.x <= max.x) &&
        (min.y <= box.min.y && box.max.y <= max.y)) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

ContainmentType BoundingBox2D::Contains(const BoundingCircle& circle) const
{
    auto clamped = math::Clamp(circle.center, min, max);
    auto distanceSquared = math::DistanceSquared(circle.center, clamped);

    if (distanceSquared > circle.radius * circle.radius) {
        return ContainmentType::Disjoint;
    }
    if ((circle.radius <= circle.center.x - min.x) &&
        (circle.radius <= circle.center.y - min.y) &&
        (circle.radius <= max.x - circle.center.x) &&
        (circle.radius <= max.y - circle.center.y)) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

bool BoundingBox2D::Intersects(const BoundingBox2D& box) const
{
    return (max.x >= box.min.x && min.x <= box.max.x) &&
           (max.y >= box.min.y && min.y <= box.max.y);
}

bool BoundingBox2D::Intersects(const BoundingCircle& circle) const
{
    const auto clamped = math::Clamp(circle.center, min, max);
    const auto distanceSquared = math::DistanceSquared(circle.center, clamped);
    return distanceSquared <= circle.radius * circle.radius;
}

std::array<Vector2, BoundingBox2D::CornerCount> BoundingBox2D::GetCorners() const noexcept
{
    return std::array<Vector2, BoundingBox2D::CornerCount>{{
        Vector2{min.x, max.y},
        Vector2{max.x, max.y},
        Vector2{max.x, min.y},
        Vector2{min.x, min.y},
    }};
}

} // namespace pomdog
