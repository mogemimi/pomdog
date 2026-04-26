// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/vector3.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <functional>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class BoundingBox;
class Plane;
class Ray;
enum class ContainmentType : u8;
enum class PlaneIntersectionType : u8;
} // namespace pomdog

namespace pomdog {

/// BoundingSphere is a bounding sphere in 3D space.
class POMDOG_EXPORT BoundingSphere final {
public:
    /// The center point of the bounding sphere.
    Vector3 center;

    /// The radius of the bounding sphere.
    f32 radius;

public:
    BoundingSphere() noexcept = default;

    /// Constructs a bounding sphere with the specified center and radius.
    BoundingSphere(const Vector3& center, f32 radius) noexcept;

    /// Determines whether the bounding sphere contains the specified point.
    [[nodiscard]] ContainmentType
    contains(const Vector3& point) const noexcept;

    /// Determines whether the bounding sphere contains the specified box.
    [[nodiscard]] ContainmentType
    contains(const BoundingBox& box) const noexcept;

    /// Determines whether the bounding sphere contains the specified sphere.
    [[nodiscard]] ContainmentType
    contains(const BoundingSphere& sphere) const noexcept;

    /// Determines whether the bounding sphere intersects the specified box.
    [[nodiscard]] bool
    intersects(const BoundingBox& box) const noexcept;

    /// Determines whether the bounding sphere intersects the specified sphere.
    [[nodiscard]] bool
    intersects(const BoundingSphere& sphere) const noexcept;

    /// Determines whether the bounding sphere intersects the specified plane.
    [[nodiscard]] PlaneIntersectionType
    intersects(const Plane& plane) const noexcept;

    /// Returns the distance along the ray at which it intersects the sphere, or nullopt if no intersection.
    [[nodiscard]] std::optional<f32>
    intersects(const Ray& ray) const noexcept;

    /// Creates a bounding sphere that contains the specified points.
    [[nodiscard]] static BoundingSphere
    createFromPoints(const Vector3* points, i32 pointCount) noexcept;

    /// Creates a bounding sphere that contains the specified points.
    [[nodiscard]] static BoundingSphere
    createFromPoints(std::function<Vector3(i32 index)> points, i32 pointCount) noexcept;
};

} // namespace pomdog
