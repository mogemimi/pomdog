// Copyright mogemimi. Distributed under the MIT license.

#pragma once

#include "pomdog/basic/conditional_compilation.hpp"
#include "pomdog/basic/export.hpp"
#include "pomdog/math/forward_declarations.hpp"
#include "pomdog/math/matrix4x4.hpp"
#include "pomdog/math/plane.hpp"
#include "pomdog/math/vector3.hpp"

POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_BEGIN
#include <array>
#include <optional>
POMDOG_SUPPRESS_WARNINGS_GENERATED_BY_STD_HEADERS_END

namespace Pomdog {

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

    std::array<Plane, PlaneCount> planes;
    std::array<Vector3, CornerCount> corners;
    Matrix4x4 matrix;

public:
    BoundingFrustum();

    explicit BoundingFrustum(const Matrix4x4& matrix);

    [[nodiscard]] const Matrix4x4& GetMatrix() const noexcept;

    void SetMatrix(const Matrix4x4& matrix) noexcept;

    [[nodiscard]] const Plane& GetNear() const noexcept;
    [[nodiscard]] const Plane& GetFar() const noexcept;
    [[nodiscard]] const Plane& GetLeft() const noexcept;
    [[nodiscard]] const Plane& GetRight() const noexcept;
    [[nodiscard]] const Plane& GetTop() const noexcept;
    [[nodiscard]] const Plane& GetBottom() const noexcept;

    [[nodiscard]] const std::array<Vector3, CornerCount>&
    GetCorners() const noexcept;

    [[nodiscard]] ContainmentType
    Contains(const Vector3& point) const noexcept;

    [[nodiscard]] ContainmentType
    Contains(const BoundingBox& box) const noexcept;

    [[nodiscard]] ContainmentType
    Contains(const BoundingFrustum& frustum) const noexcept;

    [[nodiscard]] ContainmentType
    Contains(const BoundingSphere& sphere) const noexcept;

    [[nodiscard]] bool
    Intersects(const BoundingBox& box) const noexcept;

    [[nodiscard]] bool
    Intersects(const BoundingFrustum& frustum) const noexcept;

    [[nodiscard]] bool
    Intersects(const BoundingSphere& sphere) const noexcept;

    [[nodiscard]] PlaneIntersectionType
    Intersects(const Plane& plane) const noexcept;

    [[nodiscard]] std::optional<float>
    Intersects(const Ray& ray) const noexcept;

private:
    void CreatePlanes();
    void CreateCorners();
};

} // namespace Pomdog
