// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#include "Pomdog/Math/Ray.hpp"
#include "Pomdog/Math/BoundingBox.hpp"
#include "Pomdog/Math/BoundingFrustum.hpp"
#include "Pomdog/Math/BoundingSphere.hpp"
#include "Pomdog/Math/Plane.hpp"
#include "Pomdog/Utility/Assert.hpp"
#include <algorithm>
#include <cmath>

namespace Pomdog {

Ray::Ray(const Vector3& positionIn, const Vector3& directionIn)
    : Position(positionIn)
    , Direction(directionIn)
{
}

bool Ray::operator==(const Ray& ray) const noexcept
{
    return this->Position == ray.Position
        && this->Direction == ray.Direction;
}

bool Ray::operator!=(const Ray& ray) const noexcept
{
    return this->Position != ray.Position
        || this->Direction != ray.Direction;
}

std::optional<float> Ray::Intersects(const BoundingBox& box) const
{
    typedef float T;

    constexpr auto PositiveInfinity = std::numeric_limits<T>::max();
    constexpr auto NegativeInfinity = std::numeric_limits<T>::lowest();
    constexpr auto Epsilon = std::numeric_limits<T>::epsilon();

    auto & ray = *this;

    auto tNear = NegativeInfinity;
    auto tFar = PositiveInfinity;

    if (std::abs(ray.Direction.X) < Epsilon) {
        if (ray.Position.X < box.Min.X || ray.Position.X > box.Max.X) {
            return std::nullopt;
        }
    }
    else {
        POMDOG_ASSERT(ray.Direction.X != 0);
        auto t1 = (box.Min.X - ray.Position.X) / ray.Direction.X;
        auto t2 = (box.Max.X - ray.Position.X) / ray.Direction.X;

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

    if (std::abs(ray.Direction.Y) < Epsilon) {
        if (ray.Position.Y < box.Min.Y || ray.Position.Y > box.Max.Y) {
            return std::nullopt;
        }
    }
    else {
        POMDOG_ASSERT(ray.Direction.Y != 0);
        auto t1 = (box.Min.Y - ray.Position.Y) / ray.Direction.Y;
        auto t2 = (box.Max.Y - ray.Position.Y) / ray.Direction.Y;

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

    if (std::abs(ray.Direction.Z) < Epsilon) {
        if (ray.Position.Z < box.Min.Z || ray.Position.Z > box.Max.Z) {
            return std::nullopt;
        }
    }
    else {
        POMDOG_ASSERT(ray.Direction.Z != 0);
        auto t1 = (box.Min.Z - ray.Position.Z) / ray.Direction.Z;
        auto t2 = (box.Max.Z - ray.Position.Z) / ray.Direction.Z;

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
    const auto toSphere = sphere.Center - this->Position;
    const auto toSphereLengthSquared = toSphere.LengthSquared();
    const auto sphereRadiusSquared = sphere.Radius * sphere.Radius;

    if (toSphereLengthSquared < sphereRadiusSquared) {
        return 0.0f;
    }

    const auto distance = Vector3::Dot(this->Direction, toSphere);
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

    const auto denom = Vector3::Dot(plane.Normal, Direction);
    if (std::abs(denom) < Epsilon) {
        return std::nullopt;
    }

    const auto dot = Vector3::Dot(plane.Normal, Position) + plane.Distance;
    const auto distance = -dot / denom;
    if (distance < 0.0f) {
        return std::nullopt;
    }
    return distance;
}

} // namespace Pomdog
