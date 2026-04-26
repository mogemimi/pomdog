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
    /// Creates a new Plane object.
    Plane() noexcept;

    /// Constructs a plane from a normal vector and a distance.
    ///
    /// NOTE: The `normal` must be a normalized vector.
    Plane(const Vector3& normal, f32 distance) noexcept;

    /// Constructs a plane from three points.
    ///
    /// NOTE: You should only use the vectors of a triangle in a left-handed coordinate system.
    Plane(const Vector3& point0, const Vector3& point1, const Vector3& point2) noexcept;

    /// Normalizes the plane.
    void normalize() noexcept;

    /// Returns a normalized plane.
    [[nodiscard]] static Plane
    normalize(const Plane& plane) noexcept;

    /// Calculates and returns the dot product of the plane and the specified vector.
    [[nodiscard]] f32
    dot(const Vector4& vec) const noexcept;

    /// Calculates and returns the dot product of the plane and the specified coordinate.
    [[nodiscard]] f32
    dotCoordinate(const Vector3& vec) const noexcept;

    /// Calculates and returns the dot product of the plane normal and the specified vector.
    [[nodiscard]] f32
    dotNormal(const Vector3& vec) const noexcept;

    /// Returns the distance from the plane to the specified point.
    [[nodiscard]] f32
    getDistanceToPoint(const Vector3& point) const noexcept;

    /// Determines whether the plane intersects the specified point.
    [[nodiscard]] PlaneIntersectionType
    intersects(const Vector3& point) const noexcept;

    /// Determines whether the plane intersects the specified box.
    [[nodiscard]] PlaneIntersectionType
    intersects(const BoundingBox& box) const noexcept;

    /// Determines whether the plane intersects the specified frustum.
    [[nodiscard]] PlaneIntersectionType
    intersects(const BoundingFrustum& frustum) const;

    /// Determines whether the plane intersects the specified sphere.
    [[nodiscard]] PlaneIntersectionType
    intersects(const BoundingSphere& sphere) const noexcept;

    /// Returns a transformed plane by the specified matrix.
    [[nodiscard]] static Plane
    transform(const Plane& plane, const Matrix4x4& matrix);

    /// Creates a plane from a point and a normal vector.
    [[nodiscard]] static Plane
    createFromPointNormal(const Vector3& point, const Vector3& normal);
};

} // namespace pomdog
