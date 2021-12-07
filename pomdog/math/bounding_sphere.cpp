// Copyright (c) 2013-2021 mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_sphere.hpp"
#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/math/bounding_box.hpp"
#include "pomdog/math/containment_type.hpp"
#include "pomdog/math/plane.hpp"
#include "pomdog/math/ray.hpp"
#include "pomdog/utility/assert.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

BoundingSphere::BoundingSphere(const Vector3& center, float radius)
    : Center(center)
    , Radius(radius)
{
    POMDOG_ASSERT(radius >= 0);
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

BoundingSphere
BoundingSphere::CreateFromPoints(const Vector3* points, std::size_t pointCount) noexcept
{
    POMDOG_ASSERT(points != nullptr);
    POMDOG_ASSERT(pointCount > 0);
    auto accessor = [&](std::size_t i) -> Vector3 { return points[i]; };
    return CreateFromPoints(std::move(accessor), pointCount);
}

BoundingSphere
BoundingSphere::CreateFromPoints(std::function<Vector3(std::size_t)> points, std::size_t pointCount) noexcept
{
    POMDOG_ASSERT(points != nullptr);
    POMDOG_ASSERT(pointCount > 0);

    // NOTE: Compute bounding sphere using Jack Ritter's algorithm.
    std::size_t maxX = 0;
    std::size_t maxY = 0;
    std::size_t maxZ = 0;
    std::size_t minX = 0;
    std::size_t minY = 0;
    std::size_t minZ = 0;

    for (std::size_t i = 0; i < pointCount; i++) {
        const auto& p = points(i);
        if (p.X < points(minX).X) {
            minX = i;
        }
        if (p.X > points(maxX).X) {
            maxX = i;
        }
        if (p.Y < points(minY).Y) {
            minY = i;
        }
        if (p.Y > points(maxY).Y) {
            maxY = i;
        }
        if (p.Z < points(minZ).Z) {
            minZ = i;
        }
        if (p.Z > points(maxZ).Z) {
            maxZ = i;
        }
    }

    const auto distX = Vector3::DistanceSquared(points(maxX), points(minX));
    const auto distY = Vector3::DistanceSquared(points(maxY), points(minY));
    const auto distZ = Vector3::DistanceSquared(points(maxZ), points(minZ));

    std::size_t max = maxX;
    std::size_t min = minX;
    if (distY > distX && distY > distZ) {
        max = maxY;
        min = minY;
    }
    else if (distZ > distX && distZ > distY) {
        max = maxZ;
        min = minZ;
    }

    auto center = (points(max) + points(min)) * 0.5f;
    auto radius = Vector3::Distance(points(max), center);
    auto radiusSq = radius * radius;

    // NOTE: Compute strict bounding sphere.
    for (std::size_t i = 0; i < pointCount; i++) {
        const auto p = points(i);
        const auto diff = p - center;
        const auto distanceSq = diff.LengthSquared();
        if (distanceSq > radiusSq) {
            const auto distance = std::sqrt(distanceSq);
            const auto direction = diff / distance;
            const auto g = center - radius * direction;
            center = (g + p) * 0.5f;
            radius = Vector3::Distance(p, center);
            radiusSq = radius * radius;
        }
    }

    BoundingSphere sphere;
    sphere.Center = center;
    sphere.Radius = radius;
    return sphere;
}

} // namespace Pomdog
