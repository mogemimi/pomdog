// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_frustum.h"
#include "pomdog/math/bounding_box.h"
#include "pomdog/math/bounding_sphere.h"
#include "pomdog/math/containment_type.h"
#include "pomdog/math/plane.h"
#include "pomdog/math/plane_intersection_type.h"
#include "pomdog/math/ray.h"
#include "pomdog/utility/assert.h"

namespace pomdog {
namespace {

namespace PlaneIndex {
constexpr i32 Near = 0;
constexpr i32 Far = 1;
constexpr i32 Left = 2;
constexpr i32 Right = 3;
constexpr i32 Top = 4;
constexpr i32 Bottom = 5;
} // namespace PlaneIndex

namespace CornerIndex {
constexpr i32 NearLeftTop = 0;
constexpr i32 NearRightTop = 1;
constexpr i32 NearRightBottom = 2;
constexpr i32 NearLeftBottom = 3;
constexpr i32 FarLeftTop = 4;
constexpr i32 FarRightTop = 5;
constexpr i32 FarRightBottom = 6;
constexpr i32 FarLeftBottom = 7;
} // namespace CornerIndex

constexpr auto NLT = CornerIndex::NearLeftTop;
constexpr auto NRT = CornerIndex::NearRightTop;
constexpr auto NRB = CornerIndex::NearRightBottom;
constexpr auto NLB = CornerIndex::NearLeftBottom;
constexpr auto FLT = CornerIndex::FarLeftTop;
constexpr auto FRT = CornerIndex::FarRightTop;
constexpr auto FRB = CornerIndex::FarRightBottom;
constexpr auto FLB = CornerIndex::FarLeftBottom;

constexpr bool CreatePlanesBeforeCorners = true;
constexpr bool FastContains = true;

void makePlane(Plane& plane, f32 x, f32 y, f32 z, f32 d)
{
    plane.normal.x = x;
    plane.normal.y = y;
    plane.normal.z = z;
    plane.distance = d;
}

[[nodiscard]] Vector3
computeIntersectionPoint(const Plane& a, const Plane& b, const Plane& c)
{
    const auto cross = math::cross(b.normal, c.normal);
    const auto denom = -1.0f * math::dot(a.normal, cross);
    const auto v1 = a.distance * math::cross(b.normal, c.normal);
    const auto v2 = b.distance * math::cross(c.normal, a.normal);
    const auto v3 = c.distance * math::cross(a.normal, b.normal);

    Vector3 result = (v1 + v2 + v3) / denom;
    return result;
}

[[maybe_unused]]
void createPlanes(
    std::array<Plane, BoundingFrustum::PlaneCount>& planes,
    const Matrix4x4& matrix)
{
    makePlane(
        planes[PlaneIndex::Near],
        -matrix(0, 2),
        -matrix(1, 2),
        -matrix(2, 2),
        -matrix(3, 2));

    makePlane(
        planes[PlaneIndex::Far],
        -matrix(0, 3) + matrix(0, 2),
        -matrix(1, 3) + matrix(1, 2),
        -matrix(2, 3) + matrix(2, 2),
        -matrix(3, 3) + matrix(3, 2));

    makePlane(
        planes[PlaneIndex::Left],
        -matrix(0, 3) - matrix(0, 0),
        -matrix(1, 3) - matrix(1, 0),
        -matrix(2, 3) - matrix(2, 0),
        -matrix(3, 3) - matrix(3, 0));

    makePlane(
        planes[PlaneIndex::Right],
        -matrix(0, 3) + matrix(0, 0),
        -matrix(1, 3) + matrix(1, 0),
        -matrix(2, 3) + matrix(2, 0),
        -matrix(3, 3) + matrix(3, 0));

    makePlane(
        planes[PlaneIndex::Top],
        -matrix(0, 3) + matrix(0, 1),
        -matrix(1, 3) + matrix(1, 1),
        -matrix(2, 3) + matrix(2, 1),
        -matrix(3, 3) + matrix(3, 1));

    makePlane(
        planes[PlaneIndex::Bottom],
        -matrix(0, 3) - matrix(0, 1),
        -matrix(1, 3) - matrix(1, 1),
        -matrix(2, 3) - matrix(2, 1),
        -matrix(3, 3) - matrix(3, 1));

    for (auto& plane : planes) {
        plane.normalize();
    }
}

[[maybe_unused]]
void createPlanes(
    std::array<Plane, BoundingFrustum::PlaneCount>& planes,
    const std::array<Vector3, BoundingFrustum::CornerCount>& corners)
{
    // NOTE: Left-handed coordinate system
    //
    //   NLT
    //    │\.
    //    │  \.
    //    │  ───────► Normal
    //    │      \.
    //    └─────────.
    //   NLB        NRB
    //
    // Normal == Normalize(Cross(NLT - NLB, NRB - NLT))

    planes[PlaneIndex::Near] = Plane{corners[NLB], corners[NLT], corners[NRB]};
    planes[PlaneIndex::Far] = Plane{corners[FRB], corners[FRT], corners[FLB]};
    planes[PlaneIndex::Left] = Plane{corners[FLB], corners[FLT], corners[NLB]};
    planes[PlaneIndex::Right] = Plane{corners[NRB], corners[NRT], corners[FRB]};
    planes[PlaneIndex::Top] = Plane{corners[NLT], corners[FLT], corners[NRT]};
    planes[PlaneIndex::Bottom] = Plane{corners[FLB], corners[NLB], corners[FRB]};
}

[[maybe_unused]]
void createCorners(
    std::array<Vector3, BoundingFrustum::CornerCount>& corners,
    const std::array<Plane, BoundingFrustum::PlaneCount>& planes)
{
    constexpr auto N = PlaneIndex::Near;
    constexpr auto F = PlaneIndex::Far;
    constexpr auto L = PlaneIndex::Left;
    constexpr auto R = PlaneIndex::Right;
    constexpr auto T = PlaneIndex::Top;
    constexpr auto B = PlaneIndex::Bottom;

    corners[NLT] = computeIntersectionPoint(planes[N], planes[L], planes[T]);
    corners[NRT] = computeIntersectionPoint(planes[N], planes[R], planes[T]);
    corners[NRB] = computeIntersectionPoint(planes[N], planes[R], planes[B]);
    corners[NLB] = computeIntersectionPoint(planes[N], planes[L], planes[B]);
    corners[FLT] = computeIntersectionPoint(planes[F], planes[L], planes[T]);
    corners[FRT] = computeIntersectionPoint(planes[F], planes[R], planes[T]);
    corners[FRB] = computeIntersectionPoint(planes[F], planes[R], planes[B]);
    corners[FLB] = computeIntersectionPoint(planes[F], planes[L], planes[B]);
}

[[maybe_unused]]
void createCorners(
    std::array<Vector3, BoundingFrustum::CornerCount>& corners,
    const Matrix4x4& matrix)
{
    constexpr auto xMin = -1.0f;
    constexpr auto xMax = +1.0f;
    constexpr auto yMin = -1.0f;
    constexpr auto yMax = +1.0f;
    constexpr auto zMin = -1.0f;
    constexpr auto zMax = +1.0f;

    // NOTE: Near clip
    corners[NLT] = Vector3{xMin, yMax, zMin};
    corners[NRT] = Vector3{xMax, yMax, zMin};
    corners[NRB] = Vector3{xMax, yMin, zMin};
    corners[NLB] = Vector3{xMin, yMin, zMin};

    // NOTE: Far clip
    corners[FLT] = Vector3{xMin, yMax, zMax};
    corners[FRT] = Vector3{xMax, yMax, zMax};
    corners[FRB] = Vector3{xMax, yMin, zMax};
    corners[FLB] = Vector3{xMin, yMin, zMax};

    auto inverseMatrix = math::invert(matrix);
    for (auto& corner : corners) {
        corner = math::transform(corner, inverseMatrix);
    }
}

} // namespace

BoundingFrustum::BoundingFrustum() = default;

BoundingFrustum::BoundingFrustum(const Matrix4x4& matrixIn)
{
    matrix_ = matrixIn;

    if constexpr (CreatePlanesBeforeCorners) {
        createPlanes(planes_, matrix_);
        createCorners(corners_, planes_);
    }
    else {
        createCorners(corners_, matrix_);
        createPlanes(planes_, corners_);
    }
}

const Matrix4x4& BoundingFrustum::getMatrix() const noexcept
{
    return matrix_;
}

void BoundingFrustum::setMatrix(const Matrix4x4& matrixIn) noexcept
{
    matrix_ = matrixIn;

    if constexpr (CreatePlanesBeforeCorners) {
        createPlanes(planes_, matrix_);
        createCorners(corners_, planes_);
    }
    else {
        createCorners(corners_, matrix_);
        createPlanes(planes_, corners_);
    }
}

const Plane& BoundingFrustum::getNear() const noexcept
{
    return planes_[PlaneIndex::Near];
}

const Plane& BoundingFrustum::getFar() const noexcept
{
    return planes_[PlaneIndex::Far];
}

const Plane& BoundingFrustum::getLeft() const noexcept
{
    return planes_[PlaneIndex::Left];
}

const Plane& BoundingFrustum::getRight() const noexcept
{
    return planes_[PlaneIndex::Right];
}

const Plane& BoundingFrustum::getTop() const noexcept
{
    return planes_[PlaneIndex::Top];
}

const Plane& BoundingFrustum::getBottom() const noexcept
{
    return planes_[PlaneIndex::Bottom];
}

const std::array<Vector3, BoundingFrustum::CornerCount>&
BoundingFrustum::getCorners() const noexcept
{
    return corners_;
}

ContainmentType
BoundingFrustum::contains(const Vector3& point) const noexcept
{
    if constexpr (FastContains) {
        // NOTE: fast mode
        for (auto& plane : planes_) {
            const auto distance = plane.dotCoordinate(point);
            if (distance > 0.0f) {
                return ContainmentType::Disjoint;
            }
        }
        return ContainmentType::Contains;
    }
    else {
        bool intersects = false;
        for (auto& plane : planes_) {
            const auto distance = plane.dotCoordinate(point);
            if (distance > 0.0f) {
                return ContainmentType::Disjoint;
            }
            if (distance == 0.0f) {
                intersects = true;
            }
        }
        if (intersects) {
            return ContainmentType::Intersects;
        }
        return ContainmentType::Contains;
    }
}

ContainmentType
BoundingFrustum::contains(const BoundingBox& box) const noexcept
{
    bool intersects = false;
    for (auto& plane : planes_) {
        const auto planeIntersectionType = plane.intersects(box);
        if (planeIntersectionType == PlaneIntersectionType::Front) {
            return ContainmentType::Disjoint;
        }
        if (planeIntersectionType == PlaneIntersectionType::Intersecting) {
            intersects = true;
        }
    }
    if (intersects) {
        return ContainmentType::Intersects;
    }
    return ContainmentType::Contains;
}

ContainmentType
BoundingFrustum::contains(const BoundingFrustum& frustum) const noexcept
{
    bool intersects = false;
    for (auto& plane : planes_) {
        const auto planeIntersectionType = plane.intersects(frustum);
        if (planeIntersectionType == PlaneIntersectionType::Front) {
            return ContainmentType::Disjoint;
        }
        if (planeIntersectionType == PlaneIntersectionType::Intersecting) {
            intersects = true;
        }
    }
    if (intersects) {
        return ContainmentType::Intersects;
    }
    return ContainmentType::Contains;
}

ContainmentType
BoundingFrustum::contains(const BoundingSphere& sphere) const noexcept
{
    bool intersects = false;
    for (auto& plane : planes_) {
        const auto planeIntersectionType = plane.intersects(sphere);
        if (planeIntersectionType == PlaneIntersectionType::Front) {
            return ContainmentType::Disjoint;
        }
        if (planeIntersectionType == PlaneIntersectionType::Intersecting) {
            intersects = true;
        }
    }
    if (intersects) {
        return ContainmentType::Intersects;
    }
    return ContainmentType::Contains;
}

bool BoundingFrustum::intersects(const BoundingBox& box) const noexcept
{
    for (auto& plane : planes_) {
        if (plane.intersects(box) == PlaneIntersectionType::Front) {
            return false;
        }
    }
    return true;
}

bool BoundingFrustum::intersects(const BoundingFrustum& frustum) const noexcept
{
    for (auto& plane : planes_) {
        const auto planeIntersectionType = plane.intersects(frustum);
        if (planeIntersectionType == PlaneIntersectionType::Front) {
            return false;
        }
    }
    return true;
}

bool BoundingFrustum::intersects(const BoundingSphere& sphere) const noexcept
{
    for (auto& plane : planes_) {
        const auto planeIntersectionType = plane.intersects(sphere);
        if (planeIntersectionType == PlaneIntersectionType::Front) {
            return false;
        }
    }
    return true;
}

PlaneIntersectionType
BoundingFrustum::intersects(const Plane& plane) const noexcept
{
    auto result = plane.intersects(corners_.front());
    for (u32 i = 1; i < corners_.size(); i++) {
        if (result != plane.intersects(corners_[i])) {
            result = PlaneIntersectionType::Intersecting;
        }
    }
    return result;
}

std::optional<f32>
BoundingFrustum::intersects(const Ray& ray) const noexcept
{
    std::array<std::optional<f32>, PlaneCount> distances;
    for (i32 i = 0; i < PlaneCount; i++) {
        auto& plane = planes_[i];
        const auto d = ray.intersects(plane);
        const auto distanceToRayPosition = plane.dotCoordinate(ray.position);
        if ((distanceToRayPosition < 0.0f) && !d) {
            return std::nullopt;
        }
        distances[i] = d;
        POMDOG_ASSERT(!distances[i] || *distances[i] >= 0.0f);
    }

    auto d = distances.front();
    POMDOG_ASSERT(!d || *d >= 0.0f);
    for (i32 i = 1; i < PlaneCount; i++) {
        if (!d) {
            d = distances[i];
            POMDOG_ASSERT(!d || *d >= 0.0f);
            continue;
        }
        if (distances[i]) {
            POMDOG_ASSERT(d);
            d = std::min(*distances[i], *d);
            POMDOG_ASSERT(d && *d >= 0.0f);
        }
    }
    POMDOG_ASSERT(!d || *d >= 0.0f);
    return d;
}

} // namespace pomdog
