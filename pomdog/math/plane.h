// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/export.h"
#include "pomdog/math/forward_declarations.h"
#include "pomdog/math/vector3.h"

namespace pomdog {

/// Plane is a hyperplane in 3D space.
class POMDOG_EXPORT Plane final {
public:
    /// The normal vector of the plane.
    Vector3 normal;

    /// The distance from the origin to the point that the normal of the plane starts on.
    float distance;

public:
    Plane() noexcept;

    // NOTE: The `normal` must be a normalzied vector.
    Plane(const Vector3& normal, float distance) noexcept;

    // NOTE: You should only use the vectors of a triangle in a left-handed coordinate system.
    Plane(const Vector3& point0, const Vector3& point1, const Vector3& point2) noexcept;

    void Normalize() noexcept;

    static Plane Normalize(const Plane& plane) noexcept;

    float Dot(const Vector4& vec) const noexcept;

    float DotCoordinate(const Vector3& vec) const noexcept;

    float DotNormal(const Vector3& vec) const noexcept;

    float GetDistanceToPoint(const Vector3& point) const noexcept;

    PlaneIntersectionType Intersects(const Vector3& point) const noexcept;

    PlaneIntersectionType Intersects(const BoundingBox& box) const noexcept;

    PlaneIntersectionType Intersects(const BoundingFrustum& frustum) const;

    PlaneIntersectionType Intersects(const BoundingSphere& sphere) const noexcept;

    static Plane Transform(const Plane& plane, const Matrix4x4& matrix);

    static Plane CreateFromPointNormal(const Vector3& point, const Vector3& normal);
};

} // namespace pomdog
