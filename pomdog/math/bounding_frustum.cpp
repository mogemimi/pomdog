// Copyright mogemimi. Distributed under the MIT license.

#include "pomdog/math/bounding_frustum.hpp"
#include "pomdog/math/bounding_box.hpp"
#include "pomdog/math/bounding_sphere.hpp"
#include "pomdog/math/containment_type.hpp"
#include "pomdog/math/plane.hpp"
#include "pomdog/math/plane_intersection_type.hpp"
#include "pomdog/math/ray.hpp"
#include "pomdog/utility/assert.hpp"

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

} // namespace

BoundingFrustum::BoundingFrustum() = default;

BoundingFrustum::BoundingFrustum(const Matrix4x4& matrixIn)
{
    this->matrix = matrixIn;

#if POMDOG_CREATE_PLANES_BEFORE_CORNERS
    this->CreatePlanes();
    this->CreateCorners();
#else
    this->CreateCorners();
    this->CreatePlanes();
#endif
}

const Matrix4x4& BoundingFrustum::GetMatrix() const noexcept
{
    return matrix;
}

void BoundingFrustum::SetMatrix(const Matrix4x4& matrixIn) noexcept
{
    this->matrix = matrixIn;

#if POMDOG_CREATE_PLANES_BEFORE_CORNERS
    this->CreatePlanes();
    this->CreateCorners();
#else
    this->CreateCorners();
    this->CreatePlanes();
#endif
}

const Plane& BoundingFrustum::GetNear() const noexcept
{
    return planes[PlaneIndex::Near];
}

const Plane& BoundingFrustum::GetFar() const noexcept
{
    return planes[PlaneIndex::Far];
}

const Plane& BoundingFrustum::GetLeft() const noexcept
{
    return planes[PlaneIndex::Left];
}

const Plane& BoundingFrustum::GetRight() const noexcept
{
    return planes[PlaneIndex::Right];
}

const Plane& BoundingFrustum::GetTop() const noexcept
{
    return planes[PlaneIndex::Top];
}

const Plane& BoundingFrustum::GetBottom() const noexcept
{
    return planes[PlaneIndex::Bottom];
}

const std::array<Vector3, BoundingFrustum::CornerCount>& BoundingFrustum::GetCorners() const noexcept
{
    return corners;
}

namespace {

#if POMDOG_CREATE_PLANES_BEFORE_CORNERS

void MakePlane(Plane& plane, float x, float y, float z, float d)
{
    plane.Normal.X = x;
    plane.Normal.Y = y;
    plane.Normal.Z = z;
    plane.Distance = d;
}

Vector3 ComputeIntersectionPoint(const Plane& a, const Plane& b, const Plane& c)
{
    const auto cross = math::Cross(b.Normal, c.Normal);
    const auto denom = -1.0f * math::Dot(a.Normal, cross);
    const auto v1 = a.Distance * math::Cross(b.Normal, c.Normal);
    const auto v2 = b.Distance * math::Cross(c.Normal, a.Normal);
    const auto v3 = c.Distance * math::Cross(a.Normal, b.Normal);

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

void BoundingFrustum::CreatePlanes()
{
#if POMDOG_CREATE_PLANES_BEFORE_CORNERS
    MakePlane(
        planes[PlaneIndex::Near],
        -matrix(0, 2),
        -matrix(1, 2),
        -matrix(2, 2),
        -matrix(3, 2));

    MakePlane(
        planes[PlaneIndex::Far],
        -matrix(0, 3) + matrix(0, 2),
        -matrix(1, 3) + matrix(1, 2),
        -matrix(2, 3) + matrix(2, 2),
        -matrix(3, 3) + matrix(3, 2));

    MakePlane(
        planes[PlaneIndex::Left],
        -matrix(0, 3) - matrix(0, 0),
        -matrix(1, 3) - matrix(1, 0),
        -matrix(2, 3) - matrix(2, 0),
        -matrix(3, 3) - matrix(3, 0));

    MakePlane(
        planes[PlaneIndex::Right],
        -matrix(0, 3) + matrix(0, 0),
        -matrix(1, 3) + matrix(1, 0),
        -matrix(2, 3) + matrix(2, 0),
        -matrix(3, 3) + matrix(3, 0));

    MakePlane(
        planes[PlaneIndex::Top],
        -matrix(0, 3) + matrix(0, 1),
        -matrix(1, 3) + matrix(1, 1),
        -matrix(2, 3) + matrix(2, 1),
        -matrix(3, 3) + matrix(3, 1));

    MakePlane(
        planes[PlaneIndex::Bottom],
        -matrix(0, 3) - matrix(0, 1),
        -matrix(1, 3) - matrix(1, 1),
        -matrix(2, 3) - matrix(2, 1),
        -matrix(3, 3) - matrix(3, 1));

    for (auto& plane : planes) {
        plane.Normalize();
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

    planes[PlaneIndex::Near] = Plane{corners[NLB], corners[NLT], corners[NRB]};
    planes[PlaneIndex::Far] = Plane{corners[FRB], corners[FRT], corners[FLB]};
    planes[PlaneIndex::Left] = Plane{corners[FLB], corners[FLT], corners[NLB]};
    planes[PlaneIndex::Right] = Plane{corners[NRB], corners[NRT], corners[FRB]};
    planes[PlaneIndex::Top] = Plane{corners[NLT], corners[FLT], corners[NRT]};
    planes[PlaneIndex::Bottom] = Plane{corners[FLB], corners[NLB], corners[FRB]};
#endif
}

void BoundingFrustum::CreateCorners()
{
#if POMDOG_CREATE_PLANES_BEFORE_CORNERS
    constexpr auto N = PlaneIndex::Near;
    constexpr auto F = PlaneIndex::Far;
    constexpr auto L = PlaneIndex::Left;
    constexpr auto R = PlaneIndex::Right;
    constexpr auto T = PlaneIndex::Top;
    constexpr auto B = PlaneIndex::Bottom;

    corners[NLT] = ComputeIntersectionPoint(planes[N], planes[L], planes[T]);
    corners[NRT] = ComputeIntersectionPoint(planes[N], planes[R], planes[T]);
    corners[NRB] = ComputeIntersectionPoint(planes[N], planes[R], planes[B]);
    corners[NLB] = ComputeIntersectionPoint(planes[N], planes[L], planes[B]);
    corners[FLT] = ComputeIntersectionPoint(planes[F], planes[L], planes[T]);
    corners[FRT] = ComputeIntersectionPoint(planes[F], planes[R], planes[T]);
    corners[FRB] = ComputeIntersectionPoint(planes[F], planes[R], planes[B]);
    corners[FLB] = ComputeIntersectionPoint(planes[F], planes[L], planes[B]);
#else
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

    auto inverseMatrix = Matrix4x4::Invert(matrix);
    for (auto& corner : corners) {
        corner = Vector3::Transform(corner, inverseMatrix);
    }
#endif
}

ContainmentType BoundingFrustum::Contains(const Vector3& point) const noexcept
{
#if 1
    // NOTE: fast mode
    for (auto& plane : planes) {
        const auto distance = plane.DotCoordinate(point);
        if (distance > 0.0f) {
            return ContainmentType::Disjoint;
        }
    }
    return ContainmentType::Contains;
#else
    bool intersects = false;
    for (auto& plane : planes) {
        const auto distance = plane.DotCoordinate(point);
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

ContainmentType BoundingFrustum::Contains(const BoundingBox& box) const noexcept
{
    bool intersects = false;
    for (auto& plane : planes) {
        const auto planeIntersectionType = plane.Intersects(box);
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

ContainmentType BoundingFrustum::Contains(const BoundingFrustum& frustum) const noexcept
{
    bool intersects = false;
    for (auto& plane : planes) {
        const auto planeIntersectionType = plane.Intersects(frustum);
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

ContainmentType BoundingFrustum::Contains(const BoundingSphere& sphere) const noexcept
{
    bool intersects = false;
    for (auto& plane : planes) {
        const auto planeIntersectionType = plane.Intersects(sphere);
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

bool BoundingFrustum::Intersects(const BoundingBox& box) const noexcept
{
    for (auto& plane : planes) {
        if (plane.Intersects(box) == PlaneIntersectionType::Front) {
            return false;
        }
    }
    return true;
}

bool BoundingFrustum::Intersects(const BoundingFrustum& frustum) const noexcept
{
    for (auto& plane : planes) {
        const auto planeIntersectionType = plane.Intersects(frustum);
        if (planeIntersectionType == PlaneIntersectionType::Front) {
            return false;
        }
    }
    return true;
}

bool BoundingFrustum::Intersects(const BoundingSphere& sphere) const noexcept
{
    for (auto& plane : planes) {
        const auto planeIntersectionType = plane.Intersects(sphere);
        if (planeIntersectionType == PlaneIntersectionType::Front) {
            return false;
        }
    }
    return true;
}

PlaneIntersectionType BoundingFrustum::Intersects(const Plane& plane) const noexcept
{
    auto result = plane.Intersects(corners.front());
    for (std::size_t i = 1; i < corners.size(); ++i) {
        if (result != plane.Intersects(corners[i])) {
            result = PlaneIntersectionType::Intersecting;
        }
    }
    return result;
}

std::optional<float> BoundingFrustum::Intersects(const Ray& ray) const noexcept
{
    std::array<std::optional<float>, PlaneCount> distances;
    for (int i = 0; i < PlaneCount; ++i) {
        auto& plane = planes[i];
        const auto d = ray.Intersects(plane);
        const auto distanceToRayPosition = plane.DotCoordinate(ray.Position);
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
