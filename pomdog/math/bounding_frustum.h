// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.h"
#include "pomdog/basic/export.h"
#include "pomdog/math/forward_declarations.h"
#include "pomdog/math/matrix4x4.h"
#include "pomdog/math/plane.h"
#include "pomdog/math/vector3.h"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace pomdog {

/// BoundingFrustum is a bounding frustum in 3D space.
///
/// @code
/// Frustum image:
///
///                 FarPlane
///              f0__________ f1
///      TopPlane /|        /|
///              / |       / |
///             /  |      /  |
///            /   |     /   |    RightPlane
/// LeftPlane /  f3|_ _ /_ _ |f2
///          /____/____/    /
///        n0|  /      |n1/
///          |/________|/  BottomPlane
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
/// @endcode
class POMDOG_EXPORT BoundingFrustum final {
private:
    static constexpr int CornerCount = 8;
    static constexpr int PlaneCount = 6;

    std::array<Plane, PlaneCount> planes_;
    std::array<Vector3, CornerCount> corners_;
    Matrix4x4 matrix_;

public:
    BoundingFrustum();

    explicit BoundingFrustum(const Matrix4x4& matrix);

    [[nodiscard]] const Matrix4x4&
    getMatrix() const noexcept;

    void setMatrix(const Matrix4x4& matrix) noexcept;

    [[nodiscard]] const Plane&
    getNear() const noexcept;

    [[nodiscard]] const Plane&
    getFar() const noexcept;

    [[nodiscard]] const Plane&
    getLeft() const noexcept;

    [[nodiscard]] const Plane&
    getRight() const noexcept;

    [[nodiscard]] const Plane&
    getTop() const noexcept;

    [[nodiscard]] const Plane&
    getBottom() const noexcept;

    [[nodiscard]] const std::array<Vector3, CornerCount>&
    getCorners() const noexcept;

    [[nodiscard]] ContainmentType
    contains(const Vector3& point) const noexcept;

    [[nodiscard]] ContainmentType
    contains(const BoundingBox& box) const noexcept;

    [[nodiscard]] ContainmentType
    contains(const BoundingFrustum& frustum) const noexcept;

    [[nodiscard]] ContainmentType
    contains(const BoundingSphere& sphere) const noexcept;

    [[nodiscard]] bool
    intersects(const BoundingBox& box) const noexcept;

    [[nodiscard]] bool
    intersects(const BoundingFrustum& frustum) const noexcept;

    [[nodiscard]] bool
    intersects(const BoundingSphere& sphere) const noexcept;

    [[nodiscard]] PlaneIntersectionType
    intersects(const Plane& plane) const noexcept;

    [[nodiscard]] std::optional<float>
    intersects(const Ray& ray) const noexcept;

private:
    void createPlanes();
    void createCorners();
};

} // namespace pomdog
