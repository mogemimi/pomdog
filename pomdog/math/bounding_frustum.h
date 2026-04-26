// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/basic/types.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/plane.h"
#include "pomdog/math/vector3.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {
class BoundingBox;
class BoundingSphere;
class Ray;
enum class ContainmentType : u8;
enum class PlaneIntersectionType : u8;
} // namespace pomdog

namespace pomdog {

/// BoundingFrustum is a bounding frustum in 3D space.
///
/// ```
/// Frustum image:
///
///                 FarPlane
///              f0__________ f1
///      TopPlane /│        /│
///              / │       / │
///             /  │      /  │
///            /   │     /   │    RightPlane
/// LeftPlane /  f3│_ _ /_ _ │f2
///          /____/____/    /
///        n0│  /      │n1/
///          │/________│/  BottomPlane
///         n3        n2
///          NearPlane
///
/// Planes:
///     (0) Near   = [n0, n1, n2, n3]
///     (1) Far    = [f0, f1, f2, f3]
///     (2) Left   = [n0, n3, f3, f0]
///     (3) Right  = [n1, n2, f2, f1]
///     (4) Top    = [n0, n1, f1, f0]
///     (5) Bottom = [n2, f2, f3, n3]
/// ```
class POMDOG_EXPORT BoundingFrustum final {
public:
    /// The number of corners in the bounding frustum.
    static constexpr i32 CornerCount = 8;

    /// The number of planes in the bounding frustum.
    static constexpr i32 PlaneCount = 6;

private:
    std::array<Plane, PlaneCount> planes_;
    std::array<Vector3, CornerCount> corners_;
    Matrix4x4 matrix_;

public:
    BoundingFrustum();

    /// Constructs a bounding frustum from the specified matrix.
    explicit BoundingFrustum(const Matrix4x4& matrix);

    /// Returns the matrix of the bounding frustum.
    [[nodiscard]] const Matrix4x4&
    getMatrix() const noexcept;

    /// Sets the matrix of the bounding frustum.
    void setMatrix(const Matrix4x4& matrix) noexcept;

    /// Returns the near plane of the bounding frustum.
    [[nodiscard]] const Plane&
    getNear() const noexcept;

    /// Returns the far plane of the bounding frustum.
    [[nodiscard]] const Plane&
    getFar() const noexcept;

    /// Returns the left plane of the bounding frustum.
    [[nodiscard]] const Plane&
    getLeft() const noexcept;

    /// Returns the right plane of the bounding frustum.
    [[nodiscard]] const Plane&
    getRight() const noexcept;

    /// Returns the top plane of the bounding frustum.
    [[nodiscard]] const Plane&
    getTop() const noexcept;

    /// Returns the bottom plane of the bounding frustum.
    [[nodiscard]] const Plane&
    getBottom() const noexcept;

    /// Returns the corners of the bounding frustum.
    [[nodiscard]] const std::array<Vector3, CornerCount>&
    getCorners() const noexcept;

    /// Determines whether the bounding frustum contains the specified point.
    [[nodiscard]] ContainmentType
    contains(const Vector3& point) const noexcept;

    /// Determines whether the bounding frustum contains the specified box.
    [[nodiscard]] ContainmentType
    contains(const BoundingBox& box) const noexcept;

    /// Determines whether the bounding frustum contains the specified frustum.
    [[nodiscard]] ContainmentType
    contains(const BoundingFrustum& frustum) const noexcept;

    /// Determines whether the bounding frustum contains the specified sphere.
    [[nodiscard]] ContainmentType
    contains(const BoundingSphere& sphere) const noexcept;

    /// Determines whether the bounding frustum intersects the specified box.
    [[nodiscard]] bool
    intersects(const BoundingBox& box) const noexcept;

    /// Determines whether the bounding frustum intersects the specified frustum.
    [[nodiscard]] bool
    intersects(const BoundingFrustum& frustum) const noexcept;

    /// Determines whether the bounding frustum intersects the specified sphere.
    [[nodiscard]] bool
    intersects(const BoundingSphere& sphere) const noexcept;

    /// Determines whether the bounding frustum intersects the specified plane.
    [[nodiscard]] PlaneIntersectionType
    intersects(const Plane& plane) const noexcept;

    /// Returns the distance along the ray at which it intersects the frustum, or nullopt if no intersection.
    [[nodiscard]] std::optional<f32>
    intersects(const Ray& ray) const noexcept;
};

} // namespace pomdog
