// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/vector3.h"

namespace pomdog {
class Vector4;
class Matrix4x4;
class BoundingBox;
class BoundingFrustum;
class BoundingSphere;
enum class PlaneIntersectionType : u8;
} // namespace pomdog

namespace pomdog {

/// Plane is a hyperplane in 3D space.
class POMDOG_EXPORT Plane final {
public:
    /// The normal vector of the plane.
    Vector3 normal;

    /// The distance from the origin to the point that the normal of the plane starts on.
    f32 distance;

public:
    Plane() noexcept;

    // NOTE: The `normal` must be a normalzied vector.
    Plane(const Vector3& normal, f32 distance) noexcept;

    // NOTE: You should only use the vectors of a triangle in a left-handed coordinate system.
    Plane(const Vector3& point0, const Vector3& point1, const Vector3& point2) noexcept;

    void normalize() noexcept;

    [[nodiscard]] static Plane
    normalize(const Plane& plane) noexcept;

    [[nodiscard]] f32
    dot(const Vector4& vec) const noexcept;

    [[nodiscard]] f32
    dotCoordinate(const Vector3& vec) const noexcept;

    [[nodiscard]] f32
    dotNormal(const Vector3& vec) const noexcept;

    [[nodiscard]] f32
    getDistanceToPoint(const Vector3& point) const noexcept;

    [[nodiscard]] PlaneIntersectionType
    intersects(const Vector3& point) const noexcept;

    [[nodiscard]] PlaneIntersectionType
    intersects(const BoundingBox& box) const noexcept;

    [[nodiscard]] PlaneIntersectionType
    intersects(const BoundingFrustum& frustum) const;

    [[nodiscard]] PlaneIntersectionType
    intersects(const BoundingSphere& sphere) const noexcept;

    [[nodiscard]] static Plane
    transform(const Plane& plane, const Matrix4x4& matrix);

    [[nodiscard]] static Plane
    createFromPointNormal(const Vector3& point, const Vector3& normal);
};

} // namespace pomdog
