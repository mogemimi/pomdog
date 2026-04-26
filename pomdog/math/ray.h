// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/vector3.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class BoundingBox;
class BoundingFrustum;
class BoundingSphere;
class Plane;
} // namespace pomdog

namespace pomdog {

/// Ray represents a ray with an origin point and a direction vector in 3D space.
class POMDOG_EXPORT Ray final {
public:
    /// The origin point of the ray.
    Vector3 position;

    /// The direction vector of the ray.
    Vector3 direction;

public:
    /// Creates a new Ray object.
    Ray() noexcept;

    /// Constructs a ray from the specified origin point and direction vector.
    Ray(const Vector3& position, const Vector3& direction) noexcept;

    /// Returns the distance along the ray at which it intersects the box, or nullopt if no intersection.
    [[nodiscard]] std::optional<f32>
    intersects(const BoundingBox& box) const;

    /// Returns the distance along the ray at which it intersects the frustum, or nullopt if no intersection.
    [[nodiscard]] std::optional<f32>
    intersects(const BoundingFrustum& frustum) const;

    /// Returns the distance along the ray at which it intersects the sphere, or nullopt if no intersection.
    [[nodiscard]] std::optional<f32>
    intersects(const BoundingSphere& sphere) const;

    /// Returns the distance along the ray at which it intersects the plane, or nullopt if no intersection.
    [[nodiscard]] std::optional<f32>
    intersects(const Plane& plane) const;
};

} // namespace pomdog
