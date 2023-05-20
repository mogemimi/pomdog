// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/ray.h"
#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/math/bounding_box.h"
#include "pomdog/math/bounding_frustum.h"
#include "pomdog/math/bounding_sphere.h"
#include "pomdog/math/plane.h"
#include "pomdog/utility/assert.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <algorithm>
#include <cmath>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

Ray::Ray(const Vector3& positionIn, const Vector3& directionIn)
    : position(positionIn)
    , direction(directionIn)
{
}

std::optional<float> Ray::Intersects(const BoundingBox& box) const
{
    using T = float;

    constexpr auto PositiveInfinity = std::numeric_limits<T>::max();
    constexpr auto NegativeInfinity = std::numeric_limits<T>::lowest();
    constexpr auto Epsilon = std::numeric_limits<T>::epsilon();

    auto& ray = *this;

    auto tNear = NegativeInfinity;
    auto tFar = PositiveInfinity;

    if (std::abs(ray.direction.x) < Epsilon) {
        if ((ray.position.x < box.min.x) || (ray.position.x > box.max.x)) {
            return std::nullopt;
        }
    }
    else {
        POMDOG_ASSERT(ray.direction.x != 0);
        auto t1 = (box.min.x - ray.position.x) / ray.direction.x;
        auto t2 = (box.max.x - ray.position.x) / ray.direction.x;

        if (t1 > t2) {
            std::swap(t1, t2);
        }

        POMDOG_ASSERT(tNear <= t1);
        POMDOG_ASSERT(tFar >= t2);
        tNear = t1;
        tFar = t2;

        if (tNear > tFar || tFar < 0) {
            return std::nullopt;
        }
    }

    if (std::abs(ray.direction.y) < Epsilon) {
        if ((ray.position.y < box.min.y) || (ray.position.y > box.max.y)) {
            return std::nullopt;
        }
    }
    else {
        POMDOG_ASSERT(ray.direction.y != 0);
        auto t1 = (box.min.y - ray.position.y) / ray.direction.y;
        auto t2 = (box.max.y - ray.position.y) / ray.direction.y;

        if (t1 > t2) {
            std::swap(t1, t2);
        }
        if (tNear < t1) {
            tNear = t1;
        }
        if (tFar > t2) {
            tFar = t2;
        }
        if (tNear > tFar || tFar < 0) {
            return std::nullopt;
        }
    }

    if (std::abs(ray.direction.z) < Epsilon) {
        if ((ray.position.z < box.min.z) || (ray.position.z > box.max.z)) {
            return std::nullopt;
        }
    }
    else {
        POMDOG_ASSERT(ray.direction.z != 0);
        auto t1 = (box.min.z - ray.position.z) / ray.direction.z;
        auto t2 = (box.max.z - ray.position.z) / ray.direction.z;

        if (t1 > t2) {
            std::swap(t1, t2);
        }
        if (tNear < t1) {
            tNear = t1;
        }
        if (tFar > t2) {
            tFar = t2;
        }
        if (tNear > tFar || tFar < 0) {
            return std::nullopt;
        }
    }

    POMDOG_ASSERT(tNear <= tFar && tFar >= 0);
    return tNear;
}

std::optional<float> Ray::Intersects(const BoundingFrustum& frustum) const
{
    return frustum.Intersects(*this);
}

std::optional<float> Ray::Intersects(const BoundingSphere& sphere) const
{
    const auto toSphere = sphere.center - position;
    const auto toSphereLengthSquared = math::LengthSquared(toSphere);
    const auto sphereRadiusSquared = sphere.radius * sphere.radius;

    if (toSphereLengthSquared < sphereRadiusSquared) {
        return 0.0f;
    }

    const auto distance = math::Dot(direction, toSphere);
    if (distance < 0) {
        return std::nullopt;
    }

    const auto discriminant = sphereRadiusSquared + distance * distance - toSphereLengthSquared;
    if (discriminant < 0) {
        return std::nullopt;
    }
    return std::max(distance - std::sqrt(discriminant), 0.0f);
}

std::optional<float> Ray::Intersects(const Plane& plane) const
{
    constexpr auto Epsilon = 1e-6f;

    const auto denom = math::Dot(plane.normal, direction);
    if (std::abs(denom) < Epsilon) {
        return std::nullopt;
    }

    const auto dot = math::Dot(plane.normal, position) + plane.distance;
    const auto distance = -dot / denom;
    if (distance < 0.0f) {
        return std::nullopt;
    }
    return distance;
}

} // namespace pomdog
