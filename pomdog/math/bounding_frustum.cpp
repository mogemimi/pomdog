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
constexpr int Near = 0;
constexpr int Far = 1;
constexpr int Left = 2;
constexpr int Right = 3;
constexpr int Top = 4;
constexpr int Bottom = 5;
} // namespace PlaneIndex

namespace CornerIndex {
constexpr int NearLeftTop = 0;
constexpr int NearRightTop = 1;
constexpr int NearRightBottom = 2;
constexpr int NearLeftBottom = 3;
constexpr int FarLeftTop = 4;
constexpr int FarRightTop = 5;
constexpr int FarRightBottom = 6;
constexpr int FarLeftBottom = 7;
} // namespace CornerIndex

//#define POMDOG_CREATE_PLANES_BEFORE_CORNERS 0
#define POMDOG_CREATE_PLANES_BEFORE_CORNERS 1

//#define POMDOG_BOUNDING_FRUSTUM_FAST_CONTAINS 0
#define POMDOG_BOUNDING_FRUSTUM_FAST_CONTAINS 1

} // namespace

BoundingFrustum::BoundingFrustum() = default;

BoundingFrustum::BoundingFrustum(const Matrix4x4& matrixIn)
{
    matrix_ = matrixIn;

#if POMDOG_CREATE_PLANES_BEFORE_CORNERS
    createPlanes();
    createCorners();
#else
    createCorners();
    createPlanes();
#endif
}

const Matrix4x4& BoundingFrustum::getMatrix() const noexcept
{
    return matrix_;
}

void BoundingFrustum::setMatrix(const Matrix4x4& matrixIn) noexcept
{
    matrix_ = matrixIn;

#if POMDOG_CREATE_PLANES_BEFORE_CORNERS
    createPlanes();
    createCorners();
#else
    createCorners();
    createPlanes();
#endif
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

namespace {

#if POMDOG_CREATE_PLANES_BEFORE_CORNERS

void makePlane(Plane& plane, float x, float y, float z, float d)
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

#endif

constexpr auto NLT = CornerIndex::NearLeftTop;
constexpr auto NRT = CornerIndex::NearRightTop;
constexpr auto NRB = CornerIndex::NearRightBottom;
constexpr auto NLB = CornerIndex::NearLeftBottom;
constexpr auto FLT = CornerIndex::FarLeftTop;
constexpr auto FRT = CornerIndex::FarRightTop;
constexpr auto FRB = CornerIndex::FarRightBottom;
constexpr auto FLB = CornerIndex::FarLeftBottom;

} // namespace

void BoundingFrustum::createPlanes()
{
#if POMDOG_CREATE_PLANES_BEFORE_CORNERS
    makePlane(
        planes_[PlaneIndex::Near],
        -matrix_(0, 2),
        -matrix_(1, 2),
        -matrix_(2, 2),
        -matrix_(3, 2));

    makePlane(
        planes_[PlaneIndex::Far],
        -matrix_(0, 3) + matrix_(0, 2),
        -matrix_(1, 3) + matrix_(1, 2),
        -matrix_(2, 3) + matrix_(2, 2),
        -matrix_(3, 3) + matrix_(3, 2));

    makePlane(
        planes_[PlaneIndex::Left],
        -matrix_(0, 3) - matrix_(0, 0),
        -matrix_(1, 3) - matrix_(1, 0),
        -matrix_(2, 3) - matrix_(2, 0),
        -matrix_(3, 3) - matrix_(3, 0));

    makePlane(
        planes_[PlaneIndex::Right],
        -matrix_(0, 3) + matrix_(0, 0),
        -matrix_(1, 3) + matrix_(1, 0),
        -matrix_(2, 3) + matrix_(2, 0),
        -matrix_(3, 3) + matrix_(3, 0));

    makePlane(
        planes_[PlaneIndex::Top],
        -matrix_(0, 3) + matrix_(0, 1),
        -matrix_(1, 3) + matrix_(1, 1),
        -matrix_(2, 3) + matrix_(2, 1),
        -matrix_(3, 3) + matrix_(3, 1));

    makePlane(
        planes_[PlaneIndex::Bottom],
        -matrix_(0, 3) - matrix_(0, 1),
        -matrix_(1, 3) - matrix_(1, 1),
        -matrix_(2, 3) - matrix_(2, 1),
        -matrix_(3, 3) - matrix_(3, 1));

    for (auto& plane : planes_) {
        plane.normalize();
    }
#else
    // NOTE: Left-handed coordinate system
    //
    //   NLT
    //    |\.
    //    |  \.
    //    |  =======> Normal
    //    |      \.
    //    |------- \.
    //   NLB        NRB
    //
    // Normal == Normalize(Cross(NLT - NLB, NRB - NLT))

    planes_[PlaneIndex::Near] = Plane{corners_[NLB], corners_[NLT], corners_[NRB]};
    planes_[PlaneIndex::Far] = Plane{corners_[FRB], corners_[FRT], corners_[FLB]};
    planes_[PlaneIndex::Left] = Plane{corners_[FLB], corners_[FLT], corners_[NLB]};
    planes_[PlaneIndex::Right] = Plane{corners_[NRB], corners_[NRT], corners_[FRB]};
    planes_[PlaneIndex::Top] = Plane{corners_[NLT], corners_[FLT], corners_[NRT]};
    planes_[PlaneIndex::Bottom] = Plane{corners_[FLB], corners_[NLB], corners_[FRB]};
#endif
}

void BoundingFrustum::createCorners()
{
#if POMDOG_CREATE_PLANES_BEFORE_CORNERS
    constexpr auto N = PlaneIndex::Near;
    constexpr auto F = PlaneIndex::Far;
    constexpr auto L = PlaneIndex::Left;
    constexpr auto R = PlaneIndex::Right;
    constexpr auto T = PlaneIndex::Top;
    constexpr auto B = PlaneIndex::Bottom;

    corners_[NLT] = computeIntersectionPoint(planes_[N], planes_[L], planes_[T]);
    corners_[NRT] = computeIntersectionPoint(planes_[N], planes_[R], planes_[T]);
    corners_[NRB] = computeIntersectionPoint(planes_[N], planes_[R], planes_[B]);
    corners_[NLB] = computeIntersectionPoint(planes_[N], planes_[L], planes_[B]);
    corners_[FLT] = computeIntersectionPoint(planes_[F], planes_[L], planes_[T]);
    corners_[FRT] = computeIntersectionPoint(planes_[F], planes_[R], planes_[T]);
    corners_[FRB] = computeIntersectionPoint(planes_[F], planes_[R], planes_[B]);
    corners_[FLB] = computeIntersectionPoint(planes_[F], planes_[L], planes_[B]);
#else
    constexpr auto xMin = -1.0f;
    constexpr auto xMax = +1.0f;
    constexpr auto yMin = -1.0f;
    constexpr auto yMax = +1.0f;
    constexpr auto zMin = -1.0f;
    constexpr auto zMax = +1.0f;

    // NOTE: Near clip
    corners_[NLT] = Vector3{xMin, yMax, zMin};
    corners_[NRT] = Vector3{xMax, yMax, zMin};
    corners_[NRB] = Vector3{xMax, yMin, zMin};
    corners_[NLB] = Vector3{xMin, yMin, zMin};

    // NOTE: Far clip
    corners_[FLT] = Vector3{xMin, yMax, zMax};
    corners_[FRT] = Vector3{xMax, yMax, zMax};
    corners_[FRB] = Vector3{xMax, yMin, zMax};
    corners_[FLB] = Vector3{xMin, yMin, zMax};

    auto inverseMatrix = math::invert(matrix_);
    for (auto& corner : corners_) {
        corner = math::transform(corner, inverseMatrix);
    }
#endif
}

ContainmentType
BoundingFrustum::contains(const Vector3& point) const noexcept
{
#if POMDOG_BOUNDING_FRUSTUM_FAST_CONTAINS
    // NOTE: fast mode
    for (auto& plane : planes_) {
        const auto distance = plane.dotCoordinate(point);
        if (distance > 0.0f) {
            return ContainmentType::Disjoint;
        }
    }
    return ContainmentType::Contains;
#else
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
#endif
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
    for (std::size_t i = 1; i < corners_.size(); ++i) {
        if (result != plane.intersects(corners_[i])) {
            result = PlaneIntersectionType::Intersecting;
        }
    }
    return result;
}

std::optional<float>
BoundingFrustum::intersects(const Ray& ray) const noexcept
{
    std::array<std::optional<float>, PlaneCount> distances;
    for (int i = 0; i < PlaneCount; ++i) {
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
    for (int i = 1; i < PlaneCount; ++i) {
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
