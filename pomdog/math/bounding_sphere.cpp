// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_sphere.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/bounding_box.h"
#include "pomdog/math/containment_type.h"
#include "pomdog/math/plane.h"
#include "pomdog/math/ray.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

BoundingSphere::BoundingSphere(const Vector3& centerIn, float radiusIn) noexcept
    : center(centerIn)
    , radius(radiusIn)
{
    POMDOG_ASSERT(radiusIn >= 0);
}

ContainmentType
BoundingSphere::contains(const Vector3& point) const noexcept
{
    const auto distanceSquared = math::distanceSquared(point, center);
    const auto radiusSquared = radius * radius;
    if (distanceSquared > radiusSquared) {
        return ContainmentType::Disjoint;
    }
    else if (distanceSquared < radiusSquared) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

ContainmentType
BoundingSphere::contains(const BoundingBox& box) const noexcept
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
BoundingSphere::contains(const BoundingSphere& sphere) const noexcept
{
    const auto distance = math::distance(center, sphere.center);
    if (distance > radius + sphere.radius) {
        return ContainmentType::Disjoint;
    }
    if (distance + sphere.radius < radius) {
        return ContainmentType::Contains;
    }
    return ContainmentType::Intersects;
}

bool BoundingSphere::intersects(const BoundingBox& box) const noexcept
{
    return box.intersects(*this);
}

bool BoundingSphere::intersects(const BoundingSphere& sphere) const noexcept
{
    const auto distance = math::distance(center, sphere.center);
    return distance <= radius + sphere.radius;
}

PlaneIntersectionType
BoundingSphere::intersects(const Plane& plane) const noexcept
{
    return plane.intersects(*this);
}

std::optional<float>
BoundingSphere::intersects(const Ray& ray) const noexcept
{
    return ray.intersects(*this);
}

BoundingSphere
BoundingSphere::createFromPoints(const Vector3* points, i32 pointCount) noexcept
{
    POMDOG_ASSERT(points != nullptr);
    POMDOG_ASSERT(pointCount > 0);
    auto accessor = [&](i32 i) -> Vector3 { return points[i]; };
    return createFromPoints(std::move(accessor), pointCount);
}

BoundingSphere
BoundingSphere::createFromPoints(std::function<Vector3(i32 index)> points, i32 pointCount) noexcept
{
    POMDOG_ASSERT(points != nullptr);
    POMDOG_ASSERT(pointCount > 0);

    // NOTE: Compute bounding sphere using Jack Ritter's algorithm.
    i32 maxX = 0;
    i32 maxY = 0;
    i32 maxZ = 0;
    i32 minX = 0;
    i32 minY = 0;
    i32 minZ = 0;

    for (i32 i = 0; i < pointCount; i++) {
        const auto& p = points(i);
        if (p.x < points(minX).x) {
            minX = i;
        }
        if (p.x > points(maxX).x) {
            maxX = i;
        }
        if (p.y < points(minY).y) {
            minY = i;
        }
        if (p.y > points(maxY).y) {
            maxY = i;
        }
        if (p.z < points(minZ).z) {
            minZ = i;
        }
        if (p.z > points(maxZ).z) {
            maxZ = i;
        }
    }

    const auto distX = math::distanceSquared(points(maxX), points(minX));
    const auto distY = math::distanceSquared(points(maxY), points(minY));
    const auto distZ = math::distanceSquared(points(maxZ), points(minZ));

    i32 max = maxX;
    i32 min = minX;
    if (distY > distX && distY > distZ) {
        max = maxY;
        min = minY;
    }
    else if (distZ > distX && distZ > distY) {
        max = maxZ;
        min = minZ;
    }

    auto center = (points(max) + points(min)) * 0.5f;
    auto radius = math::distance(points(max), center);
    auto radiusSq = radius * radius;

    // NOTE: Compute strict bounding sphere.
    for (i32 i = 0; i < pointCount; i++) {
        const auto p = points(i);
        const auto diff = p - center;
        const auto distanceSq = math::lengthSquared(diff);
        if (distanceSq > radiusSq) {
            const auto distance = std::sqrt(distanceSq);
            const auto direction = diff / distance;
            const auto g = center - radius * direction;
            center = (g + p) * 0.5f;
            radius = math::distance(p, center);
            radiusSq = radius * radius;
        }
    }

    BoundingSphere sphere;
    sphere.center = center;
    sphere.radius = radius;
    return sphere;
}

} // namespace pomdog
