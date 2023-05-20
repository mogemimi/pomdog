// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_box.h"
#include "pomdog/math/bounding_sphere.h"
#include "pomdog/math/containment_type.h"
#include "pomdog/math/plane.h"
#include "pomdog/math/ray.h"
#include "pomdog/utility/assert.h"

namespace pomdog {

BoundingBox::BoundingBox(const Vector3& minIn, const Vector3& maxIn)
    : min(minIn)
    , max(maxIn)
{
}

ContainmentType BoundingBox::Contains(const Vector3& point) const
{
    if ((point.x < min.x) ||
        (point.y < min.y) ||
        (point.z < min.z) ||
        (point.x > max.x) ||
        (point.y > max.y) ||
        (point.z > max.z)) {
        return ContainmentType::Disjoint;
    }
    if ((point.x == min.x) ||
        (point.y == min.y) ||
        (point.z == min.z) ||
        (point.x == max.x) ||
        (point.y == max.y) ||
        (point.z == max.z)) {
        return ContainmentType::Intersects;
    }
    return ContainmentType::Contains;
}

ContainmentType BoundingBox::Contains(const BoundingBox& box) const
{
    if ((min.x > box.max.x || max.x < box.min.x) ||
        (min.y > box.max.y || max.y < box.min.y) ||
        (min.z > box.max.z || max.z < box.min.z)) {
        return ContainmentType::Disjoint;
    }
    if ((min.x <= box.min.x && box.max.x <= max.x) &&
        (min.y <= box.min.y && box.max.y <= max.y) &&
        (min.z <= box.min.z && box.max.z <= max.z)) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

ContainmentType BoundingBox::Contains(const BoundingSphere& sphere) const
{
    const auto clamped = math::Clamp(sphere.center, min, max);
    const auto distanceSquared = math::DistanceSquared(sphere.center, clamped);

    if (distanceSquared > sphere.radius * sphere.radius) {
        return ContainmentType::Disjoint;
    }
    if ((sphere.radius <= sphere.center.x - min.x) &&
        (sphere.radius <= sphere.center.y - min.y) &&
        (sphere.radius <= sphere.center.z - min.z) &&
        (sphere.radius <= max.x - sphere.center.x) &&
        (sphere.radius <= max.y - sphere.center.y) &&
        (sphere.radius <= max.z - sphere.center.z)) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

bool BoundingBox::Intersects(const BoundingBox& box) const
{
    return (max.x >= box.min.x && min.x <= box.max.x) &&
           (max.y >= box.min.y && min.y <= box.max.y) &&
           (max.z >= box.min.z && min.z <= box.max.z);
}

bool BoundingBox::Intersects(const BoundingSphere& sphere) const
{
    const auto clamped = math::Clamp(sphere.center, min, max);
    const auto distanceSquared = math::DistanceSquared(sphere.center, clamped);
    return distanceSquared <= sphere.radius * sphere.radius;
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
    return std::array<Vector3, BoundingBox::CornerCount>{{
        Vector3{min.x, max.y, max.z},
        Vector3{max.x, max.y, max.z},
        Vector3{max.x, min.y, max.z},
        Vector3{min.x, min.y, max.z},
        Vector3{min.x, max.y, min.z},
        Vector3{max.x, max.y, min.z},
        Vector3{max.x, min.y, min.z},
        Vector3{min.x, min.y, min.z},
    }};
}

} // namespace pomdog
