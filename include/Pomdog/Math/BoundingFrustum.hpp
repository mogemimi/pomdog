// Copyright (c) 2013-2018 mogemimi. Distributed under the MIT license.

#pragma once

#include "Pomdog/Basic/Export.hpp"
#include "Pomdog/Math/Matrix4x4.hpp"
#include "Pomdog/Math/Plane.hpp"
#include "Pomdog/Math/Vector3.hpp"
#include "Pomdog/Math/detail/ForwardDeclarations.hpp"
#include <array>
#include <optional>

namespace Pomdog {

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

    const Matrix4x4& GetMatrix() const noexcept;

    void SetMatrix(const Matrix4x4& matrix) noexcept;

    const Plane& GetNear() const noexcept;
    const Plane& GetFar() const noexcept;
    const Plane& GetLeft() const noexcept;
    const Plane& GetRight() const noexcept;
    const Plane& GetTop() const noexcept;
    const Plane& GetBottom() const noexcept;

    const std::array<Vector3, CornerCount>& GetCorners() const noexcept;

    ContainmentType Contains(const Vector3& point) const noexcept;

    ContainmentType Contains(const BoundingBox& box) const noexcept;

    ContainmentType Contains(const BoundingFrustum& frustum) const noexcept;

    ContainmentType Contains(const BoundingSphere& sphere) const noexcept;

    bool Intersects(const BoundingBox& box) const noexcept;

    bool Intersects(const BoundingFrustum& frustum) const noexcept;

    bool Intersects(const BoundingSphere& sphere) const noexcept;

    PlaneIntersectionType Intersects(const Plane& plane) const noexcept;

    std::optional<float> Intersects(const Ray& ray) const noexcept;

private:
    void CreatePlanes();
    void CreateCorners();
};

} // namespace Pomdog
