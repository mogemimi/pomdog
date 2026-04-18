// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/vector3.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class BoundingSphere;
class Ray;
class Plane;
enum class ContainmentType : u8;
enum class PlaneIntersectionType : u8;
} // namespace pomdog

namespace pomdog {

/// BoundingBox is an axis-aligned bounding box in 3D space.
class POMDOG_EXPORT BoundingBox final {
public:
    Vector3 min;
    Vector3 max;

    static constexpr i32 CornerCount = 8;

public:
    BoundingBox() noexcept;

    BoundingBox(const Vector3& min, const Vector3& max) noexcept;

    /// Returns true if the specified point is inside the bounding box.
    [[nodiscard]] bool
    contains(const Vector3& point) const noexcept;

    /// Determines whether the bounding box contains the specified box.
    [[nodiscard]] ContainmentType
    contains(const BoundingBox& box) const noexcept;

    /// Determines whether the bounding box contains the specified sphere.
    [[nodiscard]] ContainmentType
    contains(const BoundingSphere& sphere) const noexcept;

    /// Determines whether the bounding box intersects the specified box.
    [[nodiscard]] bool
    intersects(const BoundingBox& box) const noexcept;

    /// Determines whether the bounding box intersects the specified sphere.
    [[nodiscard]] bool
    intersects(const BoundingSphere& sphere) const noexcept;

    /// Determines whether the bounding box intersects the specified plane.
    [[nodiscard]] PlaneIntersectionType
    intersects(const Plane& plane) const noexcept;

    /// Returns the distance along the ray at which it intersects the box, or nullopt if no intersection.
    [[nodiscard]] std::optional<f32>
    intersects(const Ray& ray) const noexcept;

    /// Returns the corners of the bounding box.
    [[nodiscard]] std::array<Vector3, CornerCount>
    getCorners() const noexcept;
};

} // namespace pomdog
